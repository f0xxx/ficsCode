
#include "networker.h"

CCritSec FiRpcBase::s_lock;
map<u32, FiRpcBase*> FiRpcBase::s_portToRpc;

FiRpcBase::FiRpcBase( u32 port, NetAddressBook* book /*= NULL*/ )
{
        m_port = port;
        m_book = book;
        m_defaultLocalIp = getDefaultIp();

        s_lock.Lock();
        assert(s_portToRpc.find(port) == s_portToRpc.end());
        s_portToRpc[port] = this;
        s_lock.Unlock();

        m_net.RegisterModule(s_modid, NULL, receiveFunc, NULL);
}

FiRpcBase::~FiRpcBase()
{
        s_lock.Lock();
        s_portToRpc.erase(m_port);
        s_lock.Unlock();
}

bool FiRpcBase::execute( u32 ip, const string& taskid, void* inbuf, u32 inbuflen, void* outbuf, u32 outbuflen, u32 localip )
{
        localip = s_invalidIp == localip ? m_defaultLocalIp : localip;
        if (s_invalidIp == localip){
                return false;
        }

        call_cxt cxt;
        assert(taskid.size() <= s_maxTaskIdSize);
        RpcBaseHeader head;
        memcpy(head.taskid, taskid.c_str(), taskid.size());
        head.inbuflen = inbuflen;
        head.outbuflen = outbuflen;
        cxt.set_call(getModId(), (char*)&head, sizeof(head), localip, ip, (char*)inbuf, inbuflen);
        int ret = m_net.Call(cxt);
        if (0 == ret){
                assert( cxt.get_reply_data()->GetBufferSize() == outbuflen );
                memcpy(outbuf, cxt.get_reply_data()->GetBufferPtr(), outbuflen);
                return true;
        }
        return false;
}

void FiRpcBase::bind( const string& taskid, FRpcCallBack callback )
{
        assert(m_taskidToCb.find(taskid) == m_taskidToCb.end());
        m_taskidToCb[taskid] = callback;
}

bool FiRpcBase::receive( MemItemPtr sheetData,MemItemPtr soktData, CTCPSocket *psokt, void *userData )
{
        if (soktData->GetBufferSize() < sizeof(RpcBaseHeader)){
                return false;
        }
        RpcBaseHeader* head = (RpcBaseHeader*)soktData->GetBufferPtr();
        map<string, FRpcCallBack>::iterator it = m_taskidToCb.find(head->taskid);
        if (it == m_taskidToCb.end()){
                return false;
        }

        FRpcCallBack cb = it->second;
        void* inbuf = NULL, *outbuf = NULL;
        if (0 != head->inbuflen){
                inbuf = (char*)head + sizeof(*head);
        }
        if (0 != head->outbuflen){
                outbuf = new char[head->outbuflen];
        }

        cb(inbuf, head->inbuflen, outbuf, head->outbuflen, psokt->m_dwRemoteIP);
        
        m_net.Reply(sheetData, (char*)outbuf, head->outbuflen, NULL, 0);

        delete[] outbuf;

        return true;
}

u32 FiRpcBase::getDefaultIp()
{
        NetAddressBook* book = NULL == m_book ? NetAddressBook::getGlobalAddressBook() : m_book;  
        if (NULL == book){
                return s_invalidIp;
        }

        vector<u32> myListen;
        vector< pair<u32, u32> > peer;
        book->getAllAddress(myListen, peer);
        if (0 == peer.size()){
                return s_invalidIp;
        }
        m_defaultLocalIp = peer[0].first;

        return m_defaultLocalIp;
}

bool FiRpcBase::receiveFunc( MemItemPtr sheetData,MemItemPtr soktData, CTCPSocket *psokt, void *userData )
{
        s_lock.Lock();
        map<u32, FiRpcBase*>::iterator it = s_portToRpc.find(psokt->m_dwListenPort);
        assert(it != s_portToRpc.end());
        bool br = it->second->receive(sheetData, soktData, psokt, userData);
        s_lock.Unlock();
        return br;
}


FiRpcSvr::FiRpcSvr( u32 port, NetAddressBook* book /*= NULL*/ ) : FiRpcBase(port, book)
{
        bool br = getNetIo()->StartServer(getPort(), getIpBook());
        assert(br);
}

void FiRpcSvr::bind( const string& taskid, FRpcCallBack callback )
{
        FiRpcBase::bind(taskid, callback);
}

FiRpcClt::FiRpcClt( u32 port, NetAddressBook* book /*= NULL*/ ) : FiRpcBase(port, book)
{
        bool br = getNetIo()->StartClient(getPort(), getIpBook());
        assert(br);
}

bool FiRpcClt::execute( u32 ip, const string& taskid, void* inbuf, u32 inbuflen, void* outbuf, u32 outbuflen )
{
        return FiRpcBase::execute(ip, taskid, inbuf, inbuflen, outbuf, outbuflen);
}

void FiRpcClt::set_defaultTime( u32 in_sendTimeOut, u32 in_retryFindSockTimeOut, u32 in_sendInterval, u32 in_replyWaitTimeOut )
{
        getNetIo()->set_defaultTime(in_sendTimeOut, in_retryFindSockTimeOut, in_sendInterval, in_replyWaitTimeOut);
}

FiRpcSvrClt::FiRpcSvrClt( u32 port, NetAddressBook* book /*= NULL*/ ) : FiRpcBase(port, book)
{
        bool br = getNetIo()->Start(getPort(), getIpBook());
        assert(br);
}

bool FiRpcSvrClt::execute( u32 ip, const string& taskid, void* inbuf, u32 inbuflen, void* outbuf, u32 outbuflen )
{
        return FiRpcBase::execute(ip, taskid, inbuf, inbuflen, outbuf, outbuflen);
}

void FiRpcSvrClt::bind( const string& taskid, FRpcCallBack callback )
{
        FiRpcBase::bind(taskid, callback);
}


