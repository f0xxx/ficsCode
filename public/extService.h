#pragma  once

#include "networker.h"

static const int gExtSvrServicePort = 9944;

class ExtSvrServiceRemote : public FiRpcSvr, public FiSingleton<ExtSvrServiceRemote> {
        friend class FiSingleton<ExtSvrServiceRemote>;
public:
        ~ExtSvrServiceRemote(){}
private:
        ExtSvrServiceRemote();
};

class ExtSvrServiceOnSvr : public FiRpcClt, public FiSingleton<ExtSvrServiceOnSvr>{
        friend class FiSingleton<ExtSvrServiceOnSvr>;
public:
        ~ExtSvrServiceOnSvr();

private:
        ExtSvrServiceOnSvr();

private:
        static const u32 s_sendTimeOut = 2000;
        static const u32 s_retryFindSockTimeOut = 1000 ;
        static const u32 s_sendInterval = 1000;
        static const u32 s_replyWaitTimeOut = 1000;
};

void  testrpc(void* inbuf, u32 inbuflen, void* outbuf, u32 outbuflen, u32 remoteIp);
void  isNodeHlty(void* inbuf, u32 inbuflen, void* outbuf, u32 outbuflen, u32 remoteIp);

#define _testrpc "testrpc"
#define _IsNodeHlty "isNodeHlty"
void InitExtSvrService();
