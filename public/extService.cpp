
#include "extService.h"
#include "netIpBook.h"
#include <iostream>

using namespace std;
#if defined(__linux) 
#include "../fistate/fstate.h"
#endif

ExtSvrServiceRemote::ExtSvrServiceRemote() : FiRpcSvr(gExtSvrServicePort, ServerIpBook::GetInstance())
{

}


ExtSvrServiceOnSvr::ExtSvrServiceOnSvr() : FiRpcClt(gExtSvrServicePort, ServerIpBook::GetInstance())
{
       set_defaultTime(s_sendTimeOut, s_retryFindSockTimeOut, s_sendInterval, s_replyWaitTimeOut);
}

ExtSvrServiceOnSvr::~ExtSvrServiceOnSvr()
{

}

void InitExtSvrService()
{
        ExtSvrServiceRemote *remote = ExtSvrServiceRemote::GetInstance();
#if defined(__linux) 
        remote->bind(_IsNodeHlty, isNodeHlty);
#endif
        remote->bind(_testrpc, testrpc);
}

#if defined(__linux) 
void isNodeHlty(void* inbuf, u32 inbuflen, void* outbuf, u32 outbuflen, u32 remoteIp){
        assert(outbuflen == sizeof(FiErrData) && outbuf != NULL);
        FiErr err;
        FiErrData *data = (FiErrData*)outbuf;
        if (0 == inbuflen){
                FStateTool::GetInstance()->IsHisMajorHealthy(FStateToolNS::GetCurNodeNo(), &err);
        }else{
                assert(sizeof(u32) == inbuflen); 
                FStateTool::GetInstance()->IsHisMajorHealthy(*(u32*)inbuf, &err);
        }
        *data = err;
}
#endif

void  testrpc(void* inbuf, u32 inbuflen, void* outbuf, u32 outbuflen, u32 remoteIp){
        assert(inbuflen == sizeof(int) && outbuflen == sizeof(int));
        int input = *(int*)inbuf;
        *(int*)outbuf = input*4;
        cout << "***************input:"<< input << ", output:" <<  *(int*)outbuf << ",**********************"<< endl;
}
