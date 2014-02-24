#pragma once

#include "../network/tcpio.h"

typedef void (*FRpcCallBack)(void* inbuf, u32 inbuflen, void* outbuf, u32 outbuflen, u32 remoteIp);



class FiRpcBase  {
private:
        static const u32 s_modid = 434;
        static const u32 s_maxTaskIdSize = 40;
        static const u32 s_invalidIp = -1;

        struct RpcBaseHeader {
                char taskid[s_maxTaskIdSize];
                u32 inbuflen;
                u32 outbuflen;

                RpcBaseHeader(){
                        memset(taskid, 0, s_maxTaskIdSize);
                }
        };
public:
        FiRpcBase(u32 port, NetAddressBook* book = NULL);

        ~FiRpcBase();

protected:
        bool execute(u32 ip, const string& taskid, void* inbuf, u32 inbuflen, void* outbuf, u32 outbuflen, u32 localip = s_invalidIp);

        void bind(const string& taskid, FRpcCallBack callback);

        CTcpIo* getNetIo(){
                return &m_net;
        }

        u32 getPort(){
                return m_port;
        }

        NetAddressBook* getIpBook(){
                return m_book;
        };

        u32 getModId(){
                return s_modid;
        }

private:
        u32 getDefaultIp();

        static bool __stdcall receiveFunc(MemItemPtr sheetData,MemItemPtr soktData, CTCPSocket *psokt, void *userData);

        bool receive(MemItemPtr sheetData,MemItemPtr soktData, CTCPSocket *psokt, void *userData);

private:
        CTcpIo m_net;
        u32 m_port;
        u32 m_defaultLocalIp;
        NetAddressBook* m_book; 
        map<string, FRpcCallBack> m_taskidToCb;

        static CCritSec s_lock;
        static map<u32, FiRpcBase*> s_portToRpc;
};

class FiRpcSvr : public FiRpcBase{
public:
        FiRpcSvr(u32 port, NetAddressBook* book = NULL);

        ~FiRpcSvr(){}

        void bind(const string& taskid, FRpcCallBack callback);
private:
};

class FiRpcClt : public FiRpcBase{
public:
        FiRpcClt(u32 port, NetAddressBook* book = NULL);

        ~FiRpcClt(){}

        bool execute(u32 ip, const string& taskid, void* inbuf, u32 inbuflen, void* outbuf, u32 outbuflen);

        void set_defaultTime(u32 in_sendTimeOut, u32 in_retryFindSockTimeOut, u32 in_sendInterval, u32 in_replyWaitTimeOut);
};

class FiRpcSvrClt: public FiRpcBase{
public:
        FiRpcSvrClt(u32 port, NetAddressBook* book = NULL);

        ~FiRpcSvrClt(){}

        bool execute(u32 ip, const string& taskid, void* inbuf, u32 inbuflen, void* outbuf, u32 outbuflen);

        void bind(const string& taskid, FRpcCallBack callback);
};
