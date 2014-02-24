/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <algorithm>
#include <iostream>

#include "blockingqueue.h"
#include "thread.h"
#include "zkadapter.h"
#include "../ficsNetSyn.h"
#include "../../fiOsd/osdCfg.h"

using namespace std;
using namespace zk;

DEFINE_LOGGER( LOG, "zookeeper.adapter" )
DEFINE_LOGGER( ZK_LOG, "zookeeper.core" )

/**
 * \brief A helper class to initialize ZK logging.
 */
class InitZooKeeperLogging
{
  public:
    InitZooKeeperLogging() {
//        if (ZK_LOG->isDebugEnabled()
//#ifdef LOG4CXX_TRACE
//            || ZK_LOG->isTraceEnabled()
//#endif
//            )
//        {
//            zoo_set_debug_level( ZOO_LOG_LEVEL_DEBUG );
//        } else if (ZK_LOG->isInfoEnabled()) {
//            zoo_set_debug_level( ZOO_LOG_LEVEL_INFO );
//        } else if (ZK_LOG->isWarnEnabled()) {
//            zoo_set_debug_level( ZOO_LOG_LEVEL_WARN );
//        } else {
//            zoo_set_debug_level( ZOO_LOG_LEVEL_ERROR );
//        }
	}
};

using namespace std;

namespace zk
{

/**
 * \brief This class provides logic for checking if a request can be retried.
 */
class RetryHandler
{
  public:
    RetryHandler(const ZooKeeperConfig &zkConfig)
        : m_zkConfig(zkConfig)
    {
        if (zkConfig.getAutoReconnect()) {
            retries = 2;
        } else {
            retries = 0;
        }
    }
        
    /**
     * \brief Attempts to fix a side effect of the given RC.
     * 
     * @param rc the ZK error code
     * @return whether the error code has been handled and the caller should 
     *         retry an operation the caused this error
     */
    bool handleRC(int rc)
    {
        ADAPTER_TRACE( LOG, "handleRC" );

        //check if the given error code is recoverable
        if (!retryOnError(rc)) {
            return false;
        }
        ADAPTER_LOG_TRACE( LOG, "RC: %d, retries left: %d", rc, retries );
        if (retries-- > 0) {
            return true;
        } else {
            return false;
        }
    }
        
  private:
    /**
     * The ZK config.
     */
    const ZooKeeperConfig &m_zkConfig;
        
    /**
     * The number of outstanding retries.
     */
    int retries;    
        
    /**
     * Checks whether the given error entitles this adapter
     * to retry the previous operation.
     * 
     * @param zkErrorCode one of the ZK error code
     */
    static bool retryOnError(int zkErrorCode)
    {
        return (zkErrorCode == ZCONNECTIONLOSS ||
                zkErrorCode == ZOPERATIONTIMEOUT);
    }
};
    
    
//the implementation of the global ZK event watcher
void zkWatcher(zhandle_t *zh, int type, int state, const char *path,
               void *watcherCtx)
{
    ADAPTER_TRACE( LOG, "zkWatcher" );

    //a workaround for buggy ZK API
    string sPath = 
        (path == NULL || 
         state == ZOO_SESSION_EVENT || 
         state == ZOO_NOTWATCHING_EVENT)
        ? "" 
        : string(path);
    ADAPTER_LOG_INFO( LOG,
              "Received a ZK event - type: %d, state: %d, path: '%s'",
              type, state, sPath.c_str() );
    ZooKeeperAdapter *zka = (ZooKeeperAdapter *)zoo_get_context(zh);
    if (zka != NULL) {
        zka->enqueueEvent( type, state, sPath );
    } else {
        ADAPTER_LOG_ERROR( LOG,
                   "Skipping ZK event (type: %d, state: %d, path: '%s'), "
                   "because ZK passed no context",
                   type, state, sPath.c_str() );
    }
}



// =======================================================================

ZooKeeperAdapter::ZooKeeperAdapter(ZooKeeperConfig config, 
                                   ZKEventListener *listener,
                                   bool establishConnection) 
    throw(ZooKeeperException)
    : m_zkConfig(config),
      mp_zkHandle(NULL), 
      m_terminating(false),
      m_connected(false),
      m_state(AS_DISCONNECTED),
      isReconnect(true)
{
    ADAPTER_TRACE( LOG, "ZooKeeperAdapter" );

    resetRemainingConnectTimeout();
    
    //enforce setting up appropriate ZK log level
    static InitZooKeeperLogging INIT_ZK_LOGGING;
    
    if (listener != NULL) {
        addListener(listener);
    }

    //start the event dispatcher thread
    m_eventDispatcher.Create( *this, &ZooKeeperAdapter::processEvents );

    //start the user event dispatcher thread
    m_userEventDispatcher.Create( *this, &ZooKeeperAdapter::processUserEvents );
    
    //optionally establish the connection
    if (establishConnection) {
        reconnect();
    }
}

ZooKeeperAdapter::~ZooKeeperAdapter()
{
    ADAPTER_TRACE( LOG, "~ZooKeeperAdapter" );

    try {
        disconnect();
    } catch (std::exception &e) {
        ADAPTER_LOG_ERROR( LOG, 
                   "An exception while disconnecting from ZK: %s",
                   e.what() );
    }
    m_terminating = true;
    m_userEventDispatcher.Join();
    m_eventDispatcher.Join();
}

void
ZooKeeperAdapter::validatePath(const string &path) throw(ZooKeeperException)
{
    ADAPTER_TRACE( LOG, "validatePath" );
    
    if (path.find( "/" ) != 0) {
        throw ZooKeeperException( string("Node path must start with '/' but"
                                         "it was '") +
                                  path +
                                  "'" );
    }
    if (path.length() > 1) {
        if (path.rfind( "/" ) == path.length() - 1) {
            throw ZooKeeperException( string("Node path must not end with "
                                             "'/' but it was '") +
                                      path +
                                      "'" );
        }
        if (path.find( "//" ) != string::npos) {
            throw ZooKeeperException( string("Node path must not contain "
                                             "'//' but it was '") +
                                      path +
                                      "'" ); 
        }
    }
}

void
ZooKeeperAdapter::disconnect()
{
    ADAPTER_TRACE( LOG, "disconnect" );
    ADAPTER_LOG_TRACE( LOG, "mp_zkHandle: %p, state %d", mp_zkHandle, m_state );

    m_stateLock.lock();
    if (mp_zkHandle != NULL) {
        zookeeper_close( mp_zkHandle );
        mp_zkHandle = NULL;
        setState( AS_DISCONNECTED );
    }
    m_stateLock.unlock();
}

void
ZooKeeperAdapter::reconnect() throw(ZooKeeperException)
{
    ADAPTER_TRACE( LOG, "reconnect" );
    
    m_stateLock.lock();
    if (!isReconnect)
    {
        m_stateLock.unlock();
        throw ZooKeeperException("reconnect closed");
    }
    //clear the connection state
    disconnect();
    
    //establish a new connection to ZooKeeper
    mp_zkHandle = zookeeper_init( OsdCfg::GetOsdCfg().GetZKIP().c_str(), 
                                  zkWatcher, 
                                  m_zkConfig.getLeaseTimeout(),
                                  NULL, this, 0);
    resetRemainingConnectTimeout();
    if (mp_zkHandle != NULL) {
        setState( AS_CONNECTING );
        m_stateLock.unlock();
    } else {
        m_stateLock.unlock();
        throw ZooKeeperException( 
            string("Unable to connect to ZK running at '") +
                    m_zkConfig.getHosts() + "'" );
    }
    
    ADAPTER_LOG_DEBUG( LOG, "mp_zkHandle: %p, state %d", mp_zkHandle, m_state ); 
}

void 
ZooKeeperAdapter::LogContext(const Listener2Context &context)
{
	for (Listener2Context::const_iterator i = context.begin();i != context.end();++i) 
	{
		NETSYN_LOG_ERR("===LogContext=== listener[%p],fuc[%p]",i->first,((evtContext *)i->second)->fuc);
	}
}

void
ZooKeeperAdapter::handleEvent(int type, int state, const string &path)
{
    ADAPTER_TRACE( LOG, "handleEvent" );
    ADAPTER_LOG_TRACE( LOG, 
               "type: %d, state %d, path: %s",
               type, state, path.c_str() );
	NETSYN_LOG_ERR("===handleEvent=== type: %d, state %d, path: %s", type, state, path.c_str());
    Listener2Context context, context2;
    //ignore internal ZK events
    if (type != ZOO_SESSION_EVENT && type != ZOO_NOTWATCHING_EVENT) {
        m_zkContextsMutex.Acquire();
        //check if the user context is available
        if (type == ZOO_CHANGED_EVENT || type == ZOO_DELETED_EVENT) {
            //we may have two types of interest here, 
            //in this case lets try to notify twice
            context = findAndRemoveListenerContext( GET_NODE_DATA, path );
			NETSYN_LOG_ERR("===handleEvent=== findAndRemoveListenerContext GET_NODE_DATA path[%s]",path.c_str());
			if (!context.empty())
			{
				LogContext(context);
			}
            context2 = findAndRemoveListenerContext( NODE_EXISTS, path );
			NETSYN_LOG_ERR("===handleEvent=== findAndRemoveListenerContext NODE_EXISTS path[%s]",path.c_str());
			if (!context2.empty())
			{
				LogContext(context);
			}
            if (context.empty()) {
                //make sure that the 2nd context is NULL and
                // assign it to the 1st one
                context = context2;
                context2.clear();
            }
        } else if (type == ZOO_CHILD_EVENT) {
            context = findAndRemoveListenerContext( GET_NODE_CHILDREN, path );
			NETSYN_LOG_ERR("===handleEvent=== findAndRemoveListenerContext GET_NODE_CHILDREN path[%s]",path.c_str());
			if (!context.empty())
			{
				LogContext(context);
			}
        } else if (type == ZOO_CREATED_EVENT) {
            context = findAndRemoveListenerContext( NODE_EXISTS, path );
			NETSYN_LOG_ERR("===handleEvent=== findAndRemoveListenerContext NODE_EXISTS path[%s]",path.c_str());
			if (!context.empty())
			{
				LogContext(context);
			}
        }
        m_zkContextsMutex.Release();
    }
    
    handleEvent( type, state, path, context );
    if (!context2.empty()) {
        handleEvent( type, state, path, context2 );
    }
}

void
ZooKeeperAdapter::handleEvent(int type,
                              int state,
                              const string &path,
                              const Listener2Context &listeners)
{
    ADAPTER_TRACE( LOG, "handleEvents" );

    if (listeners.empty()) {
        //propagate with empty context
        ZKWatcherEvent event(type, state, path);
        fireEvent( event );
    } else {
        for (Listener2Context::const_iterator i = listeners.begin();
             i != listeners.end();
             ++i) {
            ZKWatcherEvent event(type, state, path, i->second);
            if (i->first != NULL) {
                fireEvent( i->first, event );
            } else {
                fireEvent( event );
            }
        }
    }
}

void 
ZooKeeperAdapter::enqueueEvent(int type, int state, const string &path)
{
    ADAPTER_TRACE( LOG, "enqueueEvents" );

    m_events.put( ZKWatcherEvent( type, state, path ) );
}

void
ZooKeeperAdapter::processEvents()
{
    ADAPTER_TRACE( LOG, "processEvents" );

    while (!m_terminating) {
        bool timedOut = false;
        ZKWatcherEvent source = m_events.take( 100, &timedOut );
        if (!timedOut) {
            if (source.getType() == ZOO_SESSION_EVENT) {
                ADAPTER_LOG_INFO( LOG,
                          "Received SESSION event, state: %d. Adapter state: %d",
                          source.getState(), m_state );
                m_stateLock.lock();
                if (source.getState() == ZOO_CONNECTED_STATE) {
					m_zkContextsMutex.Acquire();
					evtContextPool::GetEvtContextPool().ResetEC();
					m_zkContextsMutex.Release();
					m_connected = true;
                    resetRemainingConnectTimeout();
                    setState( AS_CONNECTED );
                } else if (source.getState() == ZOO_CONNECTING_STATE) {
                    m_connected = false;
                    setState( AS_CONNECTING );
                } else if (source.getState() == ZOO_EXPIRED_SESSION_STATE) {
                    ADAPTER_LOG_INFO( LOG, "Received EXPIRED_SESSION event" );
                    setState( AS_SESSION_EXPIRED );
                }
                m_stateLock.unlock();
            }
            m_userEvents.put( source );
        }
    }
}

void
ZooKeeperAdapter::processUserEvents()
{
    ADAPTER_TRACE( LOG, "processUserEvents" );

    while (!m_terminating) {
        bool timedOut = false;
        ZKWatcherEvent source = m_userEvents.take( 100, &timedOut );
        if (!timedOut) {
            try {
                handleEvent( source.getType(),
                             source.getState(),
                             source.getPath() );
            } catch (std::exception &e) {
                ADAPTER_LOG_ERROR( LOG, 
                           "Unable to process event (type: %d, state: %d, "
                                   "path: %s), because of exception: %s",
                           source.getType(),
                           source.getState(),
                           source.getPath().c_str(),
                           e.what() );
            }
        }
    }
}

void 
ZooKeeperAdapter::registerContext(WatchableMethod method,
                                  const string &path,
                                  ZKEventListener *listener,
                                  ContextType context)
{
    ADAPTER_TRACE( LOG, "registerContext" );

	evtContext *ec = (evtContext *)m_zkContexts[method][path][listener];
	if (NULL != ec && NULL != ec->fuc)
	{
        NETSYN_LOG_DBG("===registerContext=== ReleaseEC [%p]", ec);
		evtContextPool::GetEvtContextPool().ReleaseEC(ec);
	}
    m_zkContexts[method][path][listener] = context;
}

ZooKeeperAdapter::Listener2Context
ZooKeeperAdapter::findAndRemoveListenerContext(WatchableMethod method,
                                               const string &path)
{
    ADAPTER_TRACE( LOG, "findAndRemoveListenerContext" );

    Listener2Context listeners;
    Path2Listener2Context::iterator elem = m_zkContexts[method].find( path );
    if (elem != m_zkContexts[method].end()) {
        listeners = elem->second;
        m_zkContexts[method].erase( elem );
    } 
    return listeners;
}

void 
ZooKeeperAdapter::setState(AdapterState newState)
{
    ADAPTER_TRACE( LOG, "setState" );    
    if (newState != m_state) {
        ADAPTER_LOG_INFO( LOG, "Adapter state transition: %d -> %d", m_state, newState );
        m_state = newState;
        m_stateLock.notify();
    } else {
        ADAPTER_LOG_TRACE( LOG, "New state same as the current: %d", newState );
    }
}


//TODO move this code to verifyConnection so reconnect()
//is called from one place only
void
ZooKeeperAdapter::waitUntilConnected() 
  throw(ZooKeeperException)
{
    ADAPTER_TRACE( LOG, "waitUntilConnected" );    
    long long int timeout = getRemainingConnectTimeout();
    ADAPTER_LOG_INFO( LOG,
              "Waiting up to %lld ms until a connection to ZK is established",
              timeout );
    bool connected;
    if (timeout > 0) {
        long long int toWait = timeout;
        while (m_state != AS_CONNECTED && toWait > 0) {
            //check if session expired and reconnect if so
            if (m_state == AS_SESSION_EXPIRED) {
                ADAPTER_LOG_INFO( LOG,
                        "Reconnecting because the current session has expired" );
                reconnect();
            }
            struct timeval now;
            gettimeofday( &now, NULL );
            int64_t milliSecs = -(now.tv_sec * 1000LL + now.tv_usec / 1000);
            ADAPTER_LOG_TRACE( LOG, "About to wait %lld ms", toWait );
            m_stateLock.wait( toWait );
            gettimeofday( &now, NULL );
            milliSecs += now.tv_sec * 1000LL + now.tv_usec / 1000;
            toWait -= milliSecs;
        }
        waitedForConnect( timeout - toWait );
        ADAPTER_LOG_INFO( LOG, "Waited %lld ms", timeout - toWait );
    }
    connected = (m_state == AS_CONNECTED);
    if (!connected) {
        if (timeout > 0) {
            ADAPTER_LOG_WARN( LOG, "Timed out while waiting for connection to ZK" );
            throw ZooKeeperException("Timed out while waiting for "
                                    "connection to ZK");
        } else {
            ADAPTER_LOG_ERROR( LOG, "Global timeout expired and still not connected to ZK" );
            throw ZooKeeperException("Global timeout expired and still not "
                                     "connected to ZK");
        }
    }
    ADAPTER_LOG_INFO( LOG, "Connected!" );
}

void
ZooKeeperAdapter::setReconnectStatus(bool par)
{
    ADAPTER_TRACE( LOG, "closeReconnect" );
    m_stateLock.lock();
    isReconnect = par;
    m_stateLock.unlock();
}

void
ZooKeeperAdapter::verifyConnection() throw(ZooKeeperException)
{
    ADAPTER_TRACE( LOG, "verifyConnection" );

    m_stateLock.lock();
    if (!isReconnect)
    {
        m_stateLock.unlock();
        return;
    }

    try {
        if (m_state == AS_DISCONNECTED) {
            throw ZooKeeperException("Disconnected from ZK. " \
                "Please use reconnect() before attempting to use any ZK API");
        } else if (m_state != AS_CONNECTED) {
            ADAPTER_LOG_TRACE( LOG, "Checking if need to reconnect..." );
            //we are not connected, so check if connection in progress...
            if (m_state != AS_CONNECTING) {
                ADAPTER_LOG_TRACE( LOG, 
                           "yes. Checking if allowed to auto-reconnect..." );
                //...not in progres, so check if we can reconnect
                if (!m_zkConfig.getAutoReconnect()) {
                    //...too bad, disallowed :(
                    ADAPTER_LOG_TRACE( LOG, "no. Sorry." );
                    throw ZooKeeperException("ZK connection is down and "
                                             "auto-reconnect is not allowed");
                } else {
                    ADAPTER_LOG_TRACE( LOG, "...yes. About to reconnect" );
                }
                //...we are good to retry the connection
                reconnect();
            } else {
                ADAPTER_LOG_TRACE( LOG, "...no, already in CONNECTING state" );
            }               
            //wait until the connection is established
            waitUntilConnected(); 
        }
    } catch (ZooKeeperException &e) {
        m_stateLock.unlock();
        throw e;
    }
    m_stateLock.unlock();
}

bool
ZooKeeperAdapter::createNode(const string &path, 
                             const char *value, 
                             int valueLength,
                             int flags, 
                             bool createAncestors,
                             string &returnPath) 
    throw(ZooKeeperException) 
{
    ADAPTER_TRACE( LOG, "createNode (internal)" );
    validatePath( path );
    
    const int MAX_PATH_LENGTH = 1024;
    char realPath[MAX_PATH_LENGTH];
    realPath[0] = 0;
    
    int rc;
    RetryHandler rh(m_zkConfig);
    do {
        verifyConnection();
        rc = zoo_create( mp_zkHandle, 
                         path.c_str(), 
                         value,
                         valueLength,
                         &ZOO_OPEN_ACL_UNSAFE,
                         flags,
                         realPath,
                         MAX_PATH_LENGTH );
    } while (rc != ZOK && rh.handleRC(rc));
    if (rc != ZOK) {
        if (rc == ZNODEEXISTS) {
            //the node already exists
            ADAPTER_LOG_WARN( LOG, "Error %d for %s", rc, path.c_str() );
            return false;
        } else if (rc == ZNONODE && createAncestors) {
            ADAPTER_LOG_WARN( LOG, "Error %d for %s", rc, path.c_str() );
            //one of the ancestors doesn't exist so lets start from the root 
            //and make sure the whole path exists, creating missing nodes if
            //necessary
            for (string::size_type pos = 1; pos != string::npos; ) {
                pos = path.find( "/", pos );
                if (pos != string::npos) {
                    try {
                        createNode( path.substr( 0, pos ), "", 0, 0, true );
                    } catch (ZooKeeperException &e) {
                        throw ZooKeeperException( string("Unable to create "
                                                         "node ") + 
                                                  path, 
                                                  rc );
                    }
                    pos++;
                } else {
                    //no more path components
                    return createNode( path, value , valueLength , flags, false, returnPath );
                }
            }
        }
        ADAPTER_LOG_ERROR( LOG,"Error %d for %s", rc, path.c_str() );
        throw ZooKeeperException( string("Unable to create node ") +
                                  path,
                                  rc );
    } else {
        ADAPTER_LOG_INFO( LOG, "%s has been created", realPath );
        returnPath = string( realPath );
        return true;
    }
}

bool
ZooKeeperAdapter::createNode(const string &path,
                             const char *value,
                             int valueLength, 
                             int flags,
                             bool createAncestors) 
        throw(ZooKeeperException) 
{
    ADAPTER_TRACE( LOG, "createNode" );

    string createdPath;
    return createNode( path, value , valueLength , flags, createAncestors, createdPath );
}

int64_t
ZooKeeperAdapter::createSequence(const string &path,
                                 const char *value,
                                 int valueLength,
                                 int flags,
                                 bool createAncestors) 
    throw(ZooKeeperException)
{
    ADAPTER_TRACE( LOG, "createSequence" );

    string createdPath;    
    bool result = createNode( path,
                              value,
                              valueLength,
                              flags | ZOO_SEQUENCE,
                              createAncestors,
                              createdPath );
    if (!result) {
        return -1;
    } else {
        //extract sequence number from the returned path
        if (createdPath.find( path ) != 0) {
            throw ZooKeeperException( string("Expecting returned path '") +
                                      createdPath + 
                                      "' to start with '" +
                                      path +
                                      "'" );
        }
        string seqSuffix =
            createdPath.substr( path.length(), 
                                createdPath.length() - path.length() );
        char *ptr = NULL;
        int64_t seq = strtol( seqSuffix.c_str(), &ptr, 10 );
        if (ptr != NULL && *ptr != '\0') {
            throw ZooKeeperException( string("Expecting a number but got ") +
                                      seqSuffix );
        }
        return seq;
    }
}

bool
ZooKeeperAdapter::createSequenceEx(const string &path,
								 string &createdPath,
                                 const char *value,
                                 int valueLength,
                                 int flags,
                                 bool createAncestors) 
    throw(ZooKeeperException)
{
    ADAPTER_TRACE( LOG, "createSequenceEx" );

    bool result = createNode( path,
                              value,
                              valueLength,
                              flags | ZOO_SEQUENCE,
                              createAncestors,
                              createdPath );
    if (!result) 
	{
        return false;
    } 
	return true;
}


bool
ZooKeeperAdapter::deleteNode(const string &path,
                             bool recursive,
                             int version)
    throw(ZooKeeperException)
{
    ADAPTER_TRACE( LOG, "deleteNode" );

    validatePath( path );
        
    int rc;
    RetryHandler rh(m_zkConfig);
    do {
        verifyConnection();
        rc = zoo_delete( mp_zkHandle, path.c_str(), version );
    } while (rc != ZOK && rh.handleRC(rc));
    if (rc != ZOK) {
        if (rc == ZNONODE) {
            ADAPTER_LOG_WARN( LOG, "Error %d for %s", rc, path.c_str() );
            return false;
        }
        if (rc == ZNOTEMPTY && recursive) {
            ADAPTER_LOG_WARN( LOG, "Error %d for %s", rc, path.c_str() );
            //get all children and delete them recursively...
            vector<string> nodeList;
            getNodeChildren( nodeList, path );
            for (vector<string>::const_iterator i = nodeList.begin();
                 i != nodeList.end();
                 ++i) {
                deleteNode( *i, true );
            }
            //...and finally attempt to delete the node again
            return deleteNode( path, false ); 
        }
        ADAPTER_LOG_ERROR( LOG, "Error %d for %s", rc, path.c_str() );
        throw ZooKeeperException( string("Unable to delete node ") + path,
                                  rc );
    } else {
        ADAPTER_LOG_INFO( LOG, "%s has been deleted", path.c_str() );
        return true;
    }
}

bool
ZooKeeperAdapter::nodeExists(const string &path,
                             ZKEventListener *listener,
                             void *context, Stat *stat)
    throw(ZooKeeperException)
{
    ADAPTER_TRACE( LOG, "nodeExists" );

    validatePath( path );

    struct Stat tmpStat;
    if (stat == NULL) {
        stat = &tmpStat;
    }
    memset( stat, 0, sizeof(Stat) );

    int rc;
    RetryHandler rh(m_zkConfig);
    do {
        verifyConnection();
        if (context != NULL) {    
            m_zkContextsMutex.Acquire();
            rc = zoo_exists( mp_zkHandle,
                             path.c_str(),
                             (listener != NULL ? 1 : 0),
                             stat );
            if (rc == ZOK || rc == ZNONODE) {
				NETSYN_LOG_ERR("===nodeExists=== registerContext NODE_EXISTS path[%s],listener[%p],context[%p],fuc[%p]",path.c_str(),&listener,context,((evtContext *)context)->fuc);
                registerContext( NODE_EXISTS, path, listener, context );
            }
            m_zkContextsMutex.Release();
        } else {
            rc = zoo_exists( mp_zkHandle,
                             path.c_str(),
                             (listener != NULL ? 1 : 0),
                             stat );
        }
    } while (rc != ZOK && rh.handleRC(rc));
    if (rc != ZOK) {
        if (rc == ZNONODE) {
            ADAPTER_LOG_TRACE( LOG, "Node %s does not exist", path.c_str() );
            return false;
        }
        ADAPTER_LOG_ERROR( LOG, "Error %d for %s", rc, path.c_str() );
        throw ZooKeeperException(
                 string("Unable to check existence of node ") + path,
                 rc );
    } else {
        return true;        
    }
}

//bool
//ZooKeeperAdapter::nodeExistsEx(const string &path,
//                             watcher_fn listener,
//                             void *context, Stat *stat)
//    throw(ZooKeeperException)
//{
//    ADAPTER_TRACE( LOG, "nodeExistsEx" );
//
//    validatePath( path );
//
//    struct Stat tmpStat;
//    if (stat == NULL) {
//        stat = &tmpStat;
//    }
//    memset( stat, 0, sizeof(Stat) );
//
//    int rc;
//    RetryHandler rh(m_zkConfig);
//    do {
//        verifyConnection();
//        if (context != NULL) {
//            m_zkContextsMutex.Acquire();
//            rc = zoo_wexists( mp_zkHandle,
//                             path.c_str(),
//                             listener,
//                             context,
//                             stat );
//            NETSYN_LOG_ERR("===zoo_wexists=== called path[%s]", path.c_str());
////          if (rc == ZOK || rc == ZNONODE) {
////          }
//            m_zkContextsMutex.Release();
//        } else {
//            rc = zoo_wexists( mp_zkHandle,
//                             path.c_str(),
//                              listener,
//                              context,
//                              stat );
//            NETSYN_LOG_ERR("===zoo_wexists=== called path[%s]", path.c_str());
//        }
//    } while (rc != ZOK && rh.handleRC(rc));
//    if (rc != ZOK) {
//        if (rc == ZNONODE) {
//            ADAPTER_LOG_TRACE( LOG, "Node %s does not exist", path.c_str() );
//            return false;
//        }
//        ADAPTER_LOG_ERROR( LOG, "Error %d for %s", rc, path.c_str() );
//        throw ZooKeeperException(
//                 string("Unable to check existence of node ") + path,
//                 rc );
//    } else {
//        return true;
//    }
//}

void flushDone(int rc, const char *value, const void *data)
{
    FiEvent *evt = (FiEvent *)data;
    evt->Set();
}

void
ZooKeeperAdapter::getNodeChildren(vector<string> &nodeList,
                                  const string &path, 
                                  ZKEventListener *listener,
                                  void *context)
    throw (ZooKeeperException)
{
    ADAPTER_TRACE( LOG, "getNodeChildren" );

    validatePath( path );
    
    String_vector children;
    memset( &children, 0, sizeof(children) );

    int rc;
    RetryHandler rh(m_zkConfig);
    FiEvent evt;
    do {
        verifyConnection();
        if (context != NULL) {
            m_zkContextsMutex.Acquire();
            rc = zoo_async( mp_zkHandle, 
                          path.c_str(),
                          flushDone,
                          &evt);
			if (rc == ZOK)
			{
                evt.Wait();
				rc = zoo_get_children( mp_zkHandle,
									   path.c_str(), 
									   (listener != NULL ? 1 : 0), 
									   &children );
				if (rc == ZOK) {
					NETSYN_LOG_ERR("===getNodeChildren=== registerContext GET_NODE_CHILDREN path[%s],listener[%p],context[%p],fuc[%p]",path.c_str(),&listener,context,((evtContext *)context)->fuc);
					registerContext( GET_NODE_CHILDREN, path, listener, context );
				}
			}
            m_zkContextsMutex.Release();
        } else {
            rc = zoo_async( mp_zkHandle, 
                          path.c_str(),
                          flushDone,
                          &evt);
			if (rc == ZOK)
			{
                evt.Wait();
				rc = zoo_get_children( mp_zkHandle,
									   path.c_str(), 
									   (listener != NULL ? 1 : 0),
									   &children );
			}
        }
    } while (rc != ZOK && rh.handleRC(rc));
    if (rc != ZOK) {
        ADAPTER_LOG_ERROR( LOG, "Error %d for %s", rc, path.c_str() );
        throw ZooKeeperException( string("Unable to get children of node ") +
                                  path, 
                                  rc );
    } else {
        for (int i = 0; i < children.count; ++i) {
            //convert each child's path from relative to absolute 
            string absPath(path);
            if (path != "/") {
                absPath.append( "/" );
            } 
            absPath.append( children.data[i] ); 
            nodeList.push_back( absPath );
        }
        //make sure the order is always deterministic
        sort( nodeList.begin(), nodeList.end() );
    }
}

void
ZooKeeperAdapter::getNodeChildrenEx(String_vector *children,
                                  const string &path, 
                                  ZKEventListener *listener,
                                  void *context)
    throw (ZooKeeperException)
{
    ADAPTER_TRACE( LOG, "getNodeChildren" );

    validatePath( path );
    
    memset( children, 0, sizeof(children) );

    int rc;
    RetryHandler rh(m_zkConfig);
    FiEvent evt;
    do {
        verifyConnection();
        if (context != NULL) {
            m_zkContextsMutex.Acquire();
            rc = zoo_async( mp_zkHandle, 
                          path.c_str(),
                          flushDone,
                          &evt);
			if (rc == ZOK)
			{
                evt.Wait();
				rc = zoo_get_children( mp_zkHandle,
									   path.c_str(), 
									   (listener != NULL ? 1 : 0), 
									   children );
				if (rc == ZOK) {
					NETSYN_LOG_ERR("===getNodeChildrenEx=== registerContext GET_NODE_CHILDREN path[%s],listener[%p],context[%p],fuc[%p]",path.c_str(),&listener,context,((evtContext *)context)->fuc);
					registerContext( GET_NODE_CHILDREN, path, listener, context );
				}
			}
            m_zkContextsMutex.Release();
        } else {
            rc = zoo_async( mp_zkHandle, 
                          path.c_str(),
                          flushDone,
                          &evt);
			if (rc == ZOK)
			{
                evt.Wait();
				rc = zoo_get_children( mp_zkHandle,
									   path.c_str(), 
									   (listener != NULL ? 1 : 0),
									   children );
			}
        }
    } while (rc != ZOK && rh.handleRC(rc));
    if (rc != ZOK) {
        ADAPTER_LOG_ERROR( LOG, "Error %d for %s", rc, path.c_str() );
        throw ZooKeeperException( string("Unable to get children of node ") +
                                  path, 
                                  rc );
    }
}

void
ZooKeeperAdapter::getNodeData(const string &path,
                              char *value,
                              int &valueLength,
                              ZKEventListener *listener,
                              void *context, Stat *stat)
    throw(ZooKeeperException)
{
    ADAPTER_TRACE( LOG, "getNodeData" );

    validatePath( path );
   
//  const int MAX_DATA_LENGTH = 128 * 1024;
//  char buffer[MAX_DATA_LENGTH];
//  memset( buffer, 0, MAX_DATA_LENGTH );
    struct Stat tmpStat;
    if (stat == NULL) {
        stat = &tmpStat;
    }
    memset( stat, 0, sizeof(Stat) );
    
    int rc;
//  int len;
    RetryHandler rh(m_zkConfig);
    FiEvent evt;
    do {
        verifyConnection();
//      len = MAX_DATA_LENGTH - 1;
        if (context != NULL) {
            m_zkContextsMutex.Acquire();

            rc = zoo_async( mp_zkHandle, 
                          path.c_str(),
                          flushDone,
                          &evt);
			if (rc == ZOK)
			{
                evt.Wait();
                rc = zoo_get( mp_zkHandle, 
                              path.c_str(),
                              (listener != NULL ? 1 : 0),
                              value, &valueLength, stat );
                if (rc == ZOK) {
					NETSYN_LOG_ERR("===getNodeData=== registerContext GET_NODE_DATA path[%s],listener[%p],context[%p],fuc[%p]",path.c_str(),&listener,context,((evtContext *)context)->fuc);
                    registerContext( GET_NODE_DATA, path, listener, context );
                }
			}
            m_zkContextsMutex.Release();
        } else {
            rc = zoo_async( mp_zkHandle, 
                          path.c_str(),
                          flushDone,
                          &evt);
			if (rc == ZOK)
			{
                evt.Wait();
                rc = zoo_get( mp_zkHandle,
                              path.c_str(),
                              (listener != NULL ? 1 : 0),
                              value, &valueLength, stat );
            }
        }
    } while (rc != ZOK && rh.handleRC(rc));

    if (rc != ZOK) {
        ADAPTER_LOG_ERROR( LOG, "Error %d for %s", rc, path.c_str() );
        throw ZooKeeperException( 
            string("Unable to get data of node ") + path, rc 
        );
    } 
//  else {
//      valueLength =len;
//      return string( buffer, buffer + len );
//  }
}

void
ZooKeeperAdapter::setNodeData(const string &path,
                              const char *value,
                              int valueLength,
                              int version)
    throw(ZooKeeperException)
{
    ADAPTER_TRACE( LOG, "setNodeData" );

    validatePath( path );

    int rc;
    RetryHandler rh(m_zkConfig);
    do {
        verifyConnection();
        rc = zoo_set( mp_zkHandle,
                      path.c_str(),
                      value,
                      valueLength,
                      version);
    } while (rc != ZOK && rh.handleRC(rc));
    if (rc != ZOK) {
        ADAPTER_LOG_ERROR( LOG, "Error %d for %s", rc, path.c_str() );
        throw ZooKeeperException( string("Unable to set data for node ") +
                                  path,
                                  rc );
    }
}

}   /* end of 'namespace zk' */

