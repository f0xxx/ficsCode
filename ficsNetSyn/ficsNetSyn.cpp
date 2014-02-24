#include "ficsNetSyn.h"
#include "../fiOsd/osdCfg.h"
#undef OSD_DEBUG_NONET			//close ficsNetSyn rand test
string liveNodePath("/fiCSLiveNode");

string zkServerIP("127.0.0.1:2181");
unsigned int heartbeat = 1000;

struct SortLiveNode
{
	inline bool operator()(string node1, string node2)
	{
		unsigned int preStrLen = liveNodePath.length()+1;
		return (str2u32(node1.c_str() + preStrLen) < str2u32(node2.c_str() + preStrLen));
	}
};

void csNodeStatusNotify(void *par)
{ 
	static zkDataBase& zkdb = zkDataBase::GetZkDataBase();
	static vector<string> liveNodeAryNew,intersectionAry,addNodeAry,delNodeAry;
	static vector<string>::iterator itS;
	static vector<string> &liveNodeAry = zkdb.GetLiveNodeAry();
	static OsdLock &liveNodeAryLock = zkdb.GetLiveNodeAryLock();
	static vector<evtContext> &notifiedAry = zkdb.GetNotifiedAry();
	static unsigned int preStrLenT = liveNodePath.length()+1;
	static u32 time = 500;

	bool ret = false;
	do
	{
		liveNodeAryLock.Lock();
		liveNodeAryNew.clear();
		if(zkdb.GetNodeChildren(liveNodeAryNew,liveNodePath,csNodeStatusNotify).IsSuccess())
		{
			time = 500;
			ret = true;
		}
		else
		{
			liveNodeAryLock.Unlock();
			FiEvent evtTp;
			evtTp.Wait(time);
			time = time > 3000 ? time : time * 2;
		}

	}while(!ret);

	sort(liveNodeAry.begin(),liveNodeAry.end(),SortLiveNode());
	sort(liveNodeAryNew.begin(),liveNodeAryNew.end(),SortLiveNode());
	intersectionAry.clear();
	set_intersection(liveNodeAry.begin(), liveNodeAry.end(), 	\
					 liveNodeAryNew.begin(), liveNodeAryNew.end(),	\
					 inserter(intersectionAry, intersectionAry.begin()),	\
					 SortLiveNode());

	delNodeAry.clear();
	set_difference( liveNodeAry.begin(), liveNodeAry.end(), 	\
					intersectionAry.begin(), intersectionAry.end(),	\
					inserter(delNodeAry, delNodeAry.begin()),	\
					SortLiveNode());

	addNodeAry.clear();
	set_difference( liveNodeAryNew.begin(), liveNodeAryNew.end(), 	\
					intersectionAry.begin(), intersectionAry.end(),	\
					inserter(addNodeAry, addNodeAry.begin()),	\
					SortLiveNode());

	liveNodeAry.clear();
	if(liveNodeAryNew.size() > 0)
	{
		liveNodeAry.assign(liveNodeAryNew.begin(),liveNodeAryNew.end());
	}
	liveNodeAryLock.Unlock();

	for (ECITR it = notifiedAry.begin();it != notifiedAry.end();++it)
	{
		for (itS = delNodeAry.begin();itS != delNodeAry.end();++itS)
		{
			nodeChangeInfo nci(it->par,true,str2u32(itS->c_str()+preStrLenT));
			(it->fuc)(&nci);
		}

		for (itS = addNodeAry.begin();itS != addNodeAry.end();++itS)
		{
			nodeChangeInfo nci(it->par,false,str2u32(itS->c_str()+preStrLenT));
			(it->fuc)(&nci);
		}
	}
}

void listener::eventReceived(const ZKEventSource & source,const ZKWatcherEvent & event)
{
    assert(event.getContext() != 0);
	evtContext *ec = (evtContext*)(event.getContext());

	if (ec->fuc != NULL)
	{
		(*(ec->fuc))(ec->par);
		NETSYN_LOG_DBG("===eventReceived=== ReleaseEC [%p]",ec);
		evtContextPool::GetEvtContextPool().ReleaseEC(ec);
	}
}

zhandle_t* zkDataBase::GetZkHandle() 
{
	zhandle_t *zh = NULL;

ReGetZkHandle:
	try
	{
		zh = zka->getZkHandle();
	}
	catch (ZooKeeperException& zke)
	{
		NETSYN_LOG_ERR("===GetZkHandle=== exception,%s---and ReGetZkHandle after 18ms", zke.what());
		err = zke.getZKErrorCode();
		FiEvent evtTp;
		evtTp.Wait(18);
		zka->reconnect();
		goto ReGetZkHandle;
//  	ret = FErrEx(MNetSyn_ZKFailed,zke.getZKErrorCode());
	}

	return zh;
}

FiErr zkDataBase::ReConnect()
{
	FiErr ret;

#ifdef OSD_DEBUG_NONET
	if(rand() % 9 == 0)
	{
//		ret = FErr(MNetSyn_ZKFailed);
		ret = FErrEx(MNetSyn_ZKFailed, -1);
		return ret;
	}
#endif

	try
	{
		zka->reconnect();
	}
	catch (ZooKeeperException& zke)
	{
		NETSYN_LOG_ERR("===ReConnect=== exception,%s", zke.what());
		err = zke.getZKErrorCode();
		ret = FErrEx(MNetSyn_ZKFailed,zke.getZKErrorCode());
	}
	return ret;
}

void zkDataBase::DisConnect() 
{ 
#ifdef OSD_DEBUG_NONET
	if(rand() % 9 == 0)
	{
		return;
	}
#endif
	zka->disconnect();
}

FiErr zkDataBase::CreateNode(const string& path, const char *value, int valueLength, int flags, bool createAncestors)
{
	FiErr ret;

#ifdef OSD_DEBUG_NONET
	if(rand() % 9 == 0)
	{
//		ret = FErrEx(MNetSyn_ZKFailed,ZNODEEXISTS);
		ret = FErrEx(MNetSyn_ZKFailed, -1);
		return ret;
	}
#endif

	try
	{
		if(!zka->createNode(path, value, valueLength, flags, createAncestors))
		{
			ret = FErrEx(MNetSyn_ZKFailed,ZNODEEXISTS);
			NETSYN_LOG_ERR("===CreateNode=== %s <failed>", path.c_str());
		}
	}
	catch (ZooKeeperException& zke)
	{
		NETSYN_LOG_ERR("===CreateNode=== exception,%s", zke.what());
		err = zke.getZKErrorCode();
		ret = FErrEx(MNetSyn_ZKFailed,zke.getZKErrorCode());
	}
	return ret;
}

FiErr zkDataBase::CreateSequenceNode(const string& path, int64_t &retId, int flags, const char *value, int valueLength, bool createAncestors)
{
	FiErr ret;

#ifdef OSD_DEBUG_NONET
	if(rand() % 9 == 0)
	{
//		ret = FErr(MNetSyn_ZKFailed);
		ret = FErrEx(MNetSyn_ZKFailed, -1);
		return ret;
	}
#endif

	try
	{
		retId = zka->createSequence(path, value, valueLength, flags, createAncestors);
		if (-1 == retId) 
		{
			ret = FErr(MNetSyn_ZKFailed);
		}
	}
	catch (ZooKeeperException& zke)
	{
		NETSYN_LOG_ERR("===CreateSequenceNode=== exception,%s", zke.what());
		err = zke.getZKErrorCode();
		ret = FErrEx(MNetSyn_ZKFailed,zke.getZKErrorCode());
	}
	return ret;
}

FiErr zkDataBase::CreateSequenceNodeEx(const string& path, string &retPath, int flags, const char *value, int valueLength, bool createAncestors)
{
	FiErr ret;

#ifdef OSD_DEBUG_NONET
	if(rand() % 9 == 0)
	{
//		ret = FErr(MNetSyn_ZKFailed);
		ret = FErrEx(MNetSyn_ZKFailed, -1);
		return ret;
	}
#endif

	try
	{
		if (!zka->createSequenceEx(path, retPath, value, valueLength, flags, createAncestors)) 
		{
			ret = FErr(MNetSyn_ZKFailed);
			NETSYN_LOG_ERR("===CreateSequenceNodeEx=== %s <failed>", retPath.c_str());
		}
		else 
		{
			NETSYN_LOG_ERR("===CreateSequenceNodeEx=== %s suc", retPath.c_str());
		}
	}
	catch (ZooKeeperException& zke)
	{
		NETSYN_LOG_ERR("===CreateSequenceNodeEx=== exception,%s", zke.what());
		err = zke.getZKErrorCode();
		ret = FErrEx(MNetSyn_ZKFailed,zke.getZKErrorCode());
	}
	return ret;
}

FiErr zkDataBase::DeleteNode(const string& path, bool recursive, int version)
{
	FiErr ret;

//#ifdef OSD_DEBUG_NONET
//	if(rand() % 9 == 0)
//	{
//		ret = FErrEx(MNetSyn_ZKFailed,ZNONODE);
//		ret = FErrEx(MNetSyn_ZKFailed, -1);
//		return ret;
//	}
//#endif

	try
	{
		if(!zka->deleteNode(path, recursive, version))
		{
			ret = FErrEx(MNetSyn_ZKFailed,ZNONODE);
			NETSYN_LOG_ERR("===DeleteNode=== %s <failed>", path.c_str());
		}
		else 
		{
			NETSYN_LOG_ERR("===DeleteNode=== %s suc", path.c_str());
		}
	}
	catch (ZooKeeperException& zke)
	{
		NETSYN_LOG_ERR("===DeleteNode=== exception,%s", zke.what());
		err = zke.getZKErrorCode();
		ret = FErrEx(MNetSyn_ZKFailed,zke.getZKErrorCode());
	}
	return ret;
}

FiErr zkDataBase::NodeExists(const string& path, ZKDATABASEFUC callBackFuc, void *callBackFucPar, Stat *stat)
{
	FiErr ret;

#ifdef OSD_DEBUG_NONET
	if(rand() % 9 == 0)
	{
//		ret = FErrEx(MNetSyn_ZKFailed,ZNONODE);
		ret = FErrEx(MNetSyn_ZKFailed, -1);
		return ret;
	}
#endif

	if (NULL == callBackFuc)
	{
		try
		{
			if(!zka->nodeExists(path, NULL, NULL, stat))
			{
				ret = FErrEx(MNetSyn_ZKFailed,ZNONODE);
			}
		}
		catch (ZooKeeperException& zke)
		{
			NETSYN_LOG_ERR("===NodeExists=== exception,%s", zke.what());
			err = zke.getZKErrorCode();
			ret = FErrEx(MNetSyn_ZKFailed,zke.getZKErrorCode());
		}
		return ret;
	}

	evtContext *ec = evtContextPool::GetEvtContextPool().AllocEC(); 
	NETSYN_LOG_DBG("===NodeExists=== AllocEC [%p]", ec);
	try
	{
		ec->fuc = callBackFuc;
		ec->par = callBackFucPar;
		if(!zka->nodeExists(path, &ltn, ec, stat))
		{
			evtContextPool::GetEvtContextPool().ReleaseEC(ec); 
			NETSYN_LOG_DBG("===NodeExists=== ReleaseEC [%p]", ec);
			ret = FErrEx(MNetSyn_ZKFailed,ZNONODE);
		}
	}
	catch (ZooKeeperException& zke)
	{
		evtContextPool::GetEvtContextPool().ReleaseEC(ec); 
		NETSYN_LOG_DBG("===NodeExists=== ReleaseEC [%p]", ec);
		NETSYN_LOG_ERR("===NodeExists=== exception,%s", zke.what());
		err = zke.getZKErrorCode();
		ret = FErrEx(MNetSyn_ZKFailed,zke.getZKErrorCode());
	}
	return ret;
}

//FiErr zkDataBase::NodeExistsEx(const string& path, watcher_fn callBackFuc, void *callBackFucPar, Stat *stat)
//{
//    FiErr ret;
//    try
//    {
//        if(!zka->nodeExistsEx(path, callBackFuc,callBackFucPar, stat))
//        {
//            ret = FErrEx(MNetSyn_ZKFailed,ZNONODE);
//        }
//    }
//    catch (ZooKeeperException& zke)
//    {
//        NETSYN_LOG_ERR("%s", zke.what());
//        err = zke.getZKErrorCode();
//        ret = FErrEx(MNetSyn_ZKFailed,zke.getZKErrorCode());
//    }
//    return ret;
//}

FiErr zkDataBase::GetNodeChildren(vector<string>& children, const string& path, ZKDATABASEFUC callBackFuc, void *callBackFucPar)
{
	FiErr ret;

#ifdef OSD_DEBUG_NONET
	if(rand() % 9 == 0)
	{
//		ret = FErr(MNetSyn_ZKFailed);
		ret = FErrEx(MNetSyn_ZKFailed, -1);
		return ret;
	}
#endif

	if (NULL == callBackFuc)
	{
		try
		{
			zka->getNodeChildren(children, path, NULL, NULL);
		}
		catch (ZooKeeperException& zke)
		{
			NETSYN_LOG_ERR("===GetNodeChildren=== exception,%s", zke.what());
			err = zke.getZKErrorCode();
			ret = FErrEx(MNetSyn_ZKFailed,zke.getZKErrorCode());
		}
		return ret;
	}

	evtContext *ec = evtContextPool::GetEvtContextPool().AllocEC(); 
	NETSYN_LOG_DBG("===GetNodeChildren=== AllocEC [%p]", ec);
	try
	{
		ec->fuc = callBackFuc;
		ec->par = callBackFucPar;
		zka->getNodeChildren(children, path, &ltn, ec);
	}
	catch (ZooKeeperException& zke)
	{
		evtContextPool::GetEvtContextPool().ReleaseEC(ec); 
		NETSYN_LOG_DBG("===GetNodeChildren=== ReleaseEC [%p]", ec);
		NETSYN_LOG_ERR("===GetNodeChildren=== exception,%s", zke.what());
		err = zke.getZKErrorCode();
		ret = FErrEx(MNetSyn_ZKFailed,zke.getZKErrorCode());
	}
	return ret;
}

FiErr zkDataBase::GetNodeChildrenEx(String_vector *children, const string& path, ZKDATABASEFUC callBackFuc, void *callBackFucPar)
{
	FiErr ret;

#ifdef OSD_DEBUG_NONET
	if(rand() % 9 == 0)
	{
//		ret = FErr(MNetSyn_ZKFailed);
		ret = FErrEx(MNetSyn_ZKFailed, -1);
		return ret;
	}
#endif

	if (NULL == callBackFuc)
	{
		try
		{
			zka->getNodeChildrenEx(children, path, NULL, NULL);
		}
		catch (ZooKeeperException& zke)
		{
			NETSYN_LOG_ERR("===GetNodeChildrenEx=== exception,%s", zke.what());
			err = zke.getZKErrorCode();
			ret = FErrEx(MNetSyn_ZKFailed,zke.getZKErrorCode());
		}
		return ret;
	}

	evtContext *ec = evtContextPool::GetEvtContextPool().AllocEC(); 
	NETSYN_LOG_DBG("===GetNodeChildrenEx=== AllocEC [%p]", ec);
	try
	{
		ec->fuc = callBackFuc;
		ec->par = callBackFucPar;
		zka->getNodeChildrenEx(children, path, &ltn, ec);
	}
	catch (ZooKeeperException& zke)
	{
		evtContextPool::GetEvtContextPool().ReleaseEC(ec); 
		NETSYN_LOG_DBG("===GetNodeChildrenEx=== ReleaseEC [%p]", ec);
		NETSYN_LOG_ERR("===GetNodeChildrenEx=== exception,%s", zke.what());
		err = zke.getZKErrorCode();
		ret = FErrEx(MNetSyn_ZKFailed,zke.getZKErrorCode());
	}
	return ret;
}

FiErr zkDataBase::GetNodeData(char *value ,int valueLength ,const string& path, ZKDATABASEFUC callBackFuc, void *callBackFucPar, Stat *stat)
{
	return GetNodeDataEx(value,valueLength,path,callBackFuc,callBackFucPar,stat);
}

FiErr zkDataBase::GetNodeDataEx(char *value ,int &valueLength ,const string& path, ZKDATABASEFUC callBackFuc, void *callBackFucPar, Stat *stat)
{
	FiErr ret;
	bool isReGet = false;

#ifdef OSD_DEBUG_NONET
	if(rand() % 9 == 0)
	{
//		ret = FErr(MNetSyn_ZKFailed);
		ret = FErrEx(MNetSyn_ZKFailed, -1);
		return ret;
	}
#endif


	RELEASE_ASSERT(value != NULL);
	if (0 == valueLength)
	{
		return ret;
	}

ReGetNodeData:
	if (NULL == callBackFuc)
	{
		try
		{
			zka->getNodeData(path, value, valueLength, NULL, NULL, stat); 
		}
		catch (ZooKeeperException& zke)
		{
			NETSYN_LOG_ERR("===GetNodeDataEx=== exception,%s", zke.what());
			err = zke.getZKErrorCode();
			if (ZCLOSING == err)
			{
				isReGet = true;
			}
			else
			{
				ret = FErrEx(MNetSyn_ZKFailed,zke.getZKErrorCode());
			}
		}
		if (isReGet)
		{
			zka->reconnect();
			isReGet = false;
			goto ReGetNodeData;
		}
		return ret;
	}

	evtContext *ec = evtContextPool::GetEvtContextPool().AllocEC();
	NETSYN_LOG_DBG("===GetNodeDataEx=== AllocEC [%p]", ec);
	try
	{
		ec->fuc = callBackFuc;
		ec->par = callBackFucPar;
		zka->getNodeData(path, value, valueLength, &ltn, ec, stat); 
	}
	catch (ZooKeeperException& zke)
	{
		evtContextPool::GetEvtContextPool().ReleaseEC(ec); 
        NETSYN_LOG_DBG("===GetNodeDataEx=== ReleaseEC [%p]", ec);
		NETSYN_LOG_ERR("===GetNodeDataEx=== exception,%s", zke.what());
		err = zke.getZKErrorCode();
		if (ZCLOSING == err)
		{
			isReGet = true;
		}
		else
		{
			ret = FErrEx(MNetSyn_ZKFailed,zke.getZKErrorCode());
		}
	}
	if (isReGet)
	{
		zka->reconnect();
		isReGet = false;
		goto ReGetNodeData;
	}
	return ret;
}

FiErr zkDataBase::SetNodeData(const string& path, const char *value = "",int valueLength = 0, int version)
{
	FiErr ret;

#ifdef OSD_DEBUG_NONET
	if(rand() % 9 == 0)
	{
//		ret = FErr(MNetSyn_ZKFailed);
		ret = FErrEx(MNetSyn_ZKFailed, -1);
		return ret;
	}
#endif

	try
	{
		zka->setNodeData(path, value, valueLength, version);
	}
	catch (ZooKeeperException& zke)
	{
		NETSYN_LOG_ERR("===SetNodeData=== exception,%s", zke.what());
		err = zke.getZKErrorCode();
		ret = FErrEx(MNetSyn_ZKFailed,zke.getZKErrorCode());
	}
	return ret;
}

bool zkDataBase::MarkMeAlive()
{
	bool ret = false;
//  if(NodeExists(liveNodePath).IsErr())
//  {
//  	RELEASE_ASSERT(CreateNode(liveNodePath).IsSuccess());
//  }

	string nodeIDPath = liveNodePath+'/'+locNodeIDStr;
	if(CreateNode(nodeIDPath,locIPStr.c_str(),locIPStr.length(),ZOO_EPHEMERAL).IsSuccess())
	{
		NETSYN_LOG_ERR("===MarkMeAlive=== CreateNode suc,path[%s]", nodeIDPath.c_str());
		do
		{
			liveNodeAryLock.Lock();
			liveNodeAry.clear();
			if(GetNodeChildren(liveNodeAry,liveNodePath,csNodeStatusNotify).IsSuccess())
			{
				liveNodeAryLock.Unlock();
				ret = true;
			}
			else
			{
				liveNodeAryLock.Unlock();
				FiEvent evtTp;
				evtTp.Wait(100);
			}

		}while(!ret);
	}
	return ret;
}

bool zkDataBase::IsLocNodeDead()
{
	bool ret = false;
	string nodeIDPath = liveNodePath+'/'+locNodeIDStr;
	return NodeExists(nodeIDPath).IsErr();
}

void zkDataBase::AddCSNodeWatch(ZKDATABASEFUC fuc, void *par)
{
	evtContext ec(fuc,par);
	notifiedAry.push_back(ec);
}

void zkDataBase::AddSessionWatch(ZKSESSIONWATCHFUC fuc)
{
	sessionNotifiedAry.push_back(fuc);
}

zkDataBase::zkDataBase(string& zkServerIP):config(zkServerIP, heartbeat),err(0)
{
	zka = new ZooKeeperAdapter(config, &asl, false);
	if (NULL == zka)
	{
		NETSYN_LOG_ERR("NULL == zka");
		RELEASE_ASSERT_FASLE;
		return;
	}
	liveNodeAry.reserve(18);
	notifiedAry.reserve(6);
	sessionNotifiedAry.reserve(6);
	char nodeIDStr[25];
	sprintf(nodeIDStr,"%d",OsdCfg::GetOsdCfg().GetLocNodeIDEx());
	locNodeIDStr = string(nodeIDStr);
	locIPStr = OsdCfg::GetOsdCfg().GetLocNodeIPStr();
}

zkDataBase::~zkDataBase()
{
	if (NULL != zka)
	{
		delete zka;
		zka = NULL;
	}
}

u32 zkLock::cnt = 0;
OsdLock zkLock::l;

zkLock::zkLock(char* path,zkr_lock_completion cpt,void *cptd):lockPath(path),completion(cpt),completionData(cptd)
{
	zkr_lock_init_cb(&mutex, NULL, (char *)lockPath.c_str(), &ZOO_OPEN_ACL_UNSAFE, completion, completionData);
}

zkLock::~zkLock()
{
	zkr_lock_destroy(&mutex);
}

void zkLock::GetZKLock()
{
//	struct timespec ts;
	u32 redoTime = 0;
	do
	{
		l.Lock();
		if (zkr_lock_isowner(&mutex))
		{
			zkDataBase::GetZkDataBase().setReconnectStatus(false);
			++cnt;
			break;
		}
		else if(zkr_lock_lock(&mutex)) 
		{
			zkDataBase::GetZkDataBase().setReconnectStatus(false);
			++cnt;
			break;
		}

		if (0 == cnt)
		{
			zkDataBase::GetZkDataBase().setReconnectStatus(true);
		}
		l.Unlock();

		NETSYN_LOG_ERR("===GetZKLock=== <failed> 1s later redo myid[%s] ownerid[%s]",mutex.id,mutex.ownerid);
		mutex.evt->Wait(1000);
		if(++redoTime>20)
		{
			redoTime = 0;
			zkDataBase::GetZkDataBase().ReConnect();
		}

//  	ts.tv_sec = 3;
//  	ts.tv_nsec = 0;
//  	nanosleep(&ts, 0);
	}
	while (true);

	l.Unlock();
}

bool zkLock::TryGetZKLock()
{
	bool ret = false;

	u32 redoTime = 0;
	do
	{
		l.Lock();
		if (zkr_lock_isowner(&mutex))
		{
			zkDataBase::GetZkDataBase().setReconnectStatus(false);
			++cnt;
			ret = true;
			break;
		}
		else if(zkr_lock_lock(&mutex)) 
		{
			zkDataBase::GetZkDataBase().setReconnectStatus(false);
			++cnt;
			ret = true;
			break;
		}

		if (0 == cnt)
		{
			zkDataBase::GetZkDataBase().setReconnectStatus(true);
		}
		l.Unlock();

		if (zkDataBase::GetZkDataBase().IsLocNodeDead())
		{
			break;
		}

		NETSYN_LOG_ERR("===GetZKLock=== <failed> 1s later redo myid[%s] ownerid[%s]",mutex.id,mutex.ownerid);
		mutex.evt->Wait(1000);
		if(++redoTime>20)
		{
			redoTime = 0;
			zkDataBase::GetZkDataBase().ReConnect();
		}

//  	ts.tv_sec = 3;
//  	ts.tv_nsec = 0;
//  	nanosleep(&ts, 0);
	}
	while (true);

	l.Unlock();

	return ret;
}

void zkLock::ReleaseZKLock()
{
	l.Lock();
	if (zkr_lock_isowner(&mutex))
	{
		if(zkr_lock_unlock(&mutex) != 0)
		{
			NETSYN_LOG_ERR("===ReleaseZKLock=== <failed> lockPath[%s]",mutex.path);
		}
	}
	if (0 == (--cnt))
	{
		zkDataBase::GetZkDataBase().setReconnectStatus(true);
	}
	l.Unlock();
}

NetSynInitHelper::NetSynInitHelper(string &zkServerIPT,unsigned int heartbeatT)
{
	zkServerIP = zkServerIPT;
	heartbeat = heartbeatT;
	zkDataBase &zkdb = zkDataBase::GetZkDataBase();
	while(zkdb.ReConnect().IsErr())
	{
		FiEvent evt;
//		NETSYN_LOG_ERR("zookeeper ReConnect failed!!!");
		evt.Wait(1000);
	}
	string nodeIDPath = liveNodePath+'/'+zkdb.GetLocNodeIDStr();
	zkdb.DeleteNode(nodeIDPath);
	zkdb.MarkMeAlive();
	return;
}

void InitNetSyn()
{
	string zkServerIPT = OsdCfg::GetOsdCfg().GetZKIP();
	s32 heartbeatT = OsdCfg::GetOsdCfg().GetHeartbeat();
	NetSynInitHelper::GetNetSynInitHelper(zkServerIPT,heartbeatT);
}

