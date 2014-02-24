///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  hunter code ([hunter.f0x] remeber my brothers) email:hunterlhy@163.com /////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "fiOsd.h"

bool gIsUseFiState = false;

FiErr IoFileLog::AddFileLogElement(ObjID& fid, u32 order, s32 nodeID, u32 offset, s32 mainNode, s32 bakNode)
{
    FiErr ret;
    IoFileLogElement e;
    e.fileID = fid;
    e.order = order;
    e.offset = offset;
    e.nodeID = nodeID;
    e.mainNode = mainNode;
    e.bakNode = bakNode;

    RELEASE_ASSERT(nodeID >= 0);

    lock.Lock();
    if (logMMap.empty())
    {
        e.markNum = 0;
        ret = ficsLocIF::GetFicsLocIF().GetFileIF(fileLogPath)->WriteData((char *)&e, sizeof(IoFileLogElement), sizeof(IoFileLogElement) * e.markNum);
        if (ret.IsSuccess())
        {
            OSD_LOG_ERR("===AddFileLogElement=== fileID[%s],order[%d],nodeID[%d],offset[%d],mainNode[%d],bakNode[%d],markNum[%d]",  \
                        e.fileID.GetIDStr().c_str(), e.order, e.nodeID, e.offset, e.mainNode, e.bakNode, e.markNum);
            logMMap.insert(ifleMMap::value_type(iflecKey(&e), iflecData(&e)));
        }
    } else
    {
        if (logMMap.find(iflecKey(&e)) == logMMap.end())
        {
            it = logMMap.find(iflecKey());
            if (it != logMMap.end())
            {
                e.markNum = it->second.markNum;
                ret = ficsLocIF::GetFicsLocIF().GetFileIF(fileLogPath)->WriteData((char *)&e, sizeof(IoFileLogElement), sizeof(IoFileLogElement) * e.markNum);
                if (ret.IsSuccess())
                {
                    logMMap.erase(it);
                    OSD_LOG_ERR("===AddFileLogElement=== fileID[%s],order[%d],nodeID[%d],offset[%d],mainNode[%d],bakNode[%d],markNum[%d]",  \
                                e.fileID.GetIDStr().c_str(), e.order, e.nodeID, e.offset, e.mainNode, e.bakNode, e.markNum);
                    logMMap.insert(ifleMMap::value_type(iflecKey(&e), iflecData(&e)));
                }
            } else
            {
                e.markNum = logMMap.size();
                ret = ficsLocIF::GetFicsLocIF().GetFileIF(fileLogPath)->WriteData((char *)&e, sizeof(IoFileLogElement), sizeof(IoFileLogElement) * e.markNum);
                if (ret.IsSuccess())
                {
                    OSD_LOG_ERR("===AddFileLogElement=== fileID[%s],order[%d],nodeID[%d],offset[%d],mainNode[%d],bakNode[%d],markNum[%d]",  \
                                e.fileID.GetIDStr().c_str(), e.order, e.nodeID, e.offset, e.mainNode, e.bakNode, e.markNum);
                    logMMap.insert(ifleMMap::value_type(iflecKey(&e), iflecData(&e)));
                }
            }
        }
    }
    PrintElement();
    lock.Unlock();

    if (ret.IsErr())
    {
        NodeStatusObserver::GetNodeStatusObserver().NodeErrSet(OsdCfg::GetOsdCfg().GetLocNodeID());
    }

    return ret;
}


FiErr IoFileLog::DelFileLogElement(IoFileLogElement& d)
{
    FiErr ret;

    lock.Lock();
    if (logMMap.empty())
    {
        lock.Unlock();
        return ret;
    }

    it = logMMap.find(iflecKey(&d));
    if(it == logMMap.end())
    {
        lock.Unlock();
        return ret;
    }

    IoFileLogElement e;
    e.fileID.ClearID();
    e.order = 0;
    e.offset = WRITETYPE;
    e.nodeID = 0;
    e.mainNode = 0;
    e.bakNode = 0;
    e.markNum = it->second.markNum;
    ret = ficsLocIF::GetFicsLocIF().GetFileIF(fileLogPath)->WriteData((char *)&e, sizeof(IoFileLogElement), sizeof(IoFileLogElement) * e.markNum);
    if (ret.IsSuccess())
    {
        OSD_LOG_ERR("===DelFileLogElement=== fileID[%s],order[%d],nodeID[%d],markNum[%d]", d.fileID.GetIDStr().c_str(), d.order, d.nodeID, d.markNum);
        logMMap.erase(it);
        logMMap.insert(ifleMMap::value_type(iflecKey(&e), iflecData(&e)));
    }
    PrintElement();
    lock.Unlock();

    if (ret.IsErr())
    {
        NodeStatusObserver::GetNodeStatusObserver().NodeErrSet(OsdCfg::GetOsdCfg().GetLocNodeID());
    }

    return ret;
}

FaultHandler::FaultHandler()
{
    faultHandlerThread.SetDefaultCallBack(FaultHandlerFunc);
    faultHandlerThread.SetMaxThreadNum(1);
    faultHandlerThread.SetMaxThreadNumPerTask(1);
    faultHandlerThread.Run();

    evt.SetMode(TRUE);
    if(pthread_create(&doRecoverThreadId, NULL, DoRecoverThread, &evt) != 0)
    {
        RELEASE_ASSERT_FASLE;
    }
}

bool FaultHandler::DoRecoverDone()
{
    bool reCoverDone = false;
    if (OsdCfg::GetOsdCfg().IsInstallMetaSvr())
    {
        IoFileLog *metaIoRecoverLog = LocWapper::GetLocWapper().GetMetaIoRecoverLog();
        IoFileLog *dataIoRecoverLog = LocWapper::GetLocWapper().GetDataIoRecoverLog();
        reCoverDone = (metaIoRecoverLog->IsFileLogNull() && dataIoRecoverLog->IsFileLogNull());
    }
    else
    {
        IoFileLog *dataIoRecoverLog = LocWapper::GetLocWapper().GetDataIoRecoverLog();
        reCoverDone = dataIoRecoverLog->IsFileLogNull();
    }
    return reCoverDone;
}

void FaultHandler::ClearRecoverLog()
{
    if (OsdCfg::GetOsdCfg().IsInstallMetaSvr())
    {
        IoFileLog *metaIoRecoverLog = LocWapper::GetLocWapper().GetMetaIoRecoverLog();
        IoFileLog *dataIoRecoverLog = LocWapper::GetLocWapper().GetDataIoRecoverLog();
        if (!ficsLocIF::GetFicsLocIF().RemoveFile(metaIoRecoverLog->GetFileLogPath()))
        {
            OSD_LOG_ERR("===ClearRecoverLog=== failed [%s]", metaIoRecoverLog->GetFileLogPath().c_str());
        }
        if (!ficsLocIF::GetFicsLocIF().RemoveFile(dataIoRecoverLog->GetFileLogPath()))
        {
            OSD_LOG_ERR("===ClearRecoverLog=== failed [%s]", dataIoRecoverLog->GetFileLogPath().c_str());
        }
        metaIoRecoverLog->ClearFileLogMMap();
        dataIoRecoverLog->ClearFileLogMMap();
    } else
    {
        IoFileLog *dataIoRecoverLog = LocWapper::GetLocWapper().GetDataIoRecoverLog();
        if (!ficsLocIF::GetFicsLocIF().RemoveFile(dataIoRecoverLog->GetFileLogPath()))
        {
            OSD_LOG_ERR("===ClearRecoverLog=== failed [%s]", dataIoRecoverLog->GetFileLogPath().c_str());
        }
        dataIoRecoverLog->ClearFileLogMMap();
    }
}

void FaultHandler::DoFullRecover()
{
    //del full recover log dir
    string fullRecoverLogPath;
    if (OsdCfg::GetOsdCfg().IsInstallMetaSvr())
    {
        fullRecoverLogPath = OsdCfg::GetOsdCfg().GetMetaRootPath()+string("MetaIoFullRecoverLogDir");
        ficsLocIF::GetFicsLocIF().RemoveDir(fullRecoverLogPath,false);
        fullRecoverLogPath = OsdCfg::GetOsdCfg().GetMetaRootPath()+string("DataIoFullRecoverLogDir");
        ficsLocIF::GetFicsLocIF().RemoveDir(fullRecoverLogPath,false);
    }
    else
    {
        fullRecoverLogPath = OsdCfg::GetOsdCfg().GetMetaRootPath()+string("DataIoFullRecoverLogDir");
        ficsLocIF::GetFicsLocIF().RemoveDir(fullRecoverLogPath,false);
    }
    //enum all dir&write log

    //run status
}

void FaultHandler::FaultHandlerFunc(u64 taskId, void* par)
{
    FaultHandler &fh = FaultHandler::GetFaultHandler();
    OsdInternalStatus ois = (OsdInternalStatus)((u64)par);
    switch (ois) 
    {
    case StartDoRecoverType:
        OSD_LOG_ERR("===FaultHandlerFunc=== StartDoRecover");
        fh.StartDoRecover();
        break;
    case StopDoRecoverType:
        OSD_LOG_ERR("===FaultHandlerFunc=== StopDoRecover");
        fh.StopDoRecover();
        break;
    case DoFullRecoverType:
        OSD_LOG_ERR("===FaultHandlerFunc=== DoFullRecover");
        fh.DoFullRecover();
        break;
    case OsdOK:
        OSD_LOG_ERR("===FaultHandlerFunc=== SetOsdLayerStatus[OsdLayerOK]");
        fh.SetOsdLayerStatus(OsdLayerOK);
        break;
    case OsdErr:
        OSD_LOG_ERR("===FaultHandlerFunc=== SetOsdLayerStatus[OsdLayerErr]");
        fh.SetOsdLayerStatus(OsdLayerErr);
        break;
    default:
        RELEASE_ASSERT_FASLE;
    }
}

void ConsistencyHandler::DoConsistency()
{
    memset(chBuf,0,chBufLen);

    DoConsistencySub(Meta,LocWapper::GetLocWapper().GetJournalIoConsistencyLog(),chBuf,chBufLen);
    DoConsistencySub(Data,LocWapper::GetLocWapper().GetDataIoConsistencyLog(),chBuf,chBufLen);
}

void ConsistencyHandler::DoConsistencySub(ObjDataType dataType,IoFileLog *ioConsistencyLog,char *buf,u32 bufLen)
{
    static IoFileLogElement e;
    static s32 readLen = 0;
    static s32 retLen = 0;
    static s64 offset = 0;
    static FiErr ret;
    while (ioConsistencyLog->GetFileLogElement(e))
    {
        if (WRITETYPE == e.offset)
        {
            readLen = bufLen;
            ObjInfoHandle obj(e.fileID,e.mainNode,e.bakNode);
            offset = (s64)e.order * ((dataType == Meta)?(s64)OsdCfg::GetOsdCfg().GetMetaSliceSize():(s64)OsdCfg::GetOsdCfg().GetDataSliceSize());
            BaseObjIo *readIo = (dataType == Meta)?     \
                                ObjIoFactory::GetObjIoFactory().CreateObj(MetaRead):    \
                                ObjIoFactory::GetObjIoFactory().CreateObj(DataRead);
            BaseObjIo *writeIo = (dataType == Meta)?    \
                                ObjIoFactory::GetObjIoFactory().CreateObj(MetaWrite):   \
                                ObjIoFactory::GetObjIoFactory().CreateObj(DataWrite);

            OSD_LOG_ERR("===DoConsistencySub=== WRITETYPE fileID[%s],order[%d]",e.fileID.GetIDStr().c_str(),e.order);

            do
            {
                readIo->InitObjIo((char *)&obj,(char *)buf,readLen,offset);
                readIo->SetDstNode(e.nodeID);
                ret = LocWapper::GetLocWapper().SendIo(readIo,buf,readLen,offset,ConsistencyRead);
                retLen = readIo->GetRetLength();
                ret = readIo->GetRetStatus();
                if (ret.IsErr())
                {
                    break;
                }

                if(retLen > 0)
                {
                    writeIo->InitObjIo((char *)&obj,(char *)buf,retLen,offset);
                    writeIo->SetDstNode(e.nodeID);
                    ret = LocWapper::GetLocWapper().Write(writeIo);
                    if (ret.IsErr())
                    {
                        break;
                    }

                    offset += retLen;
                }

            }while(retLen);

            if (ret.IsSuccess())
            {
                OSD_LOG_ERR("===DoConsistencySub=== DelFileLogElement WRITETYPE fileID[%s],order[%d]",e.fileID.GetIDStr().c_str(),e.order);
                ioConsistencyLog->DelFileLogElement(e);
            }
            writeIo->DelObjIo(writeIo);
            readIo->DelObjIo(readIo);
        }
        else
        {
            ObjInfoHandle obj(e.fileID,e.mainNode,e.bakNode);
            u32 fileSliceSize = (dataType == Meta)?OsdCfg::GetOsdCfg().GetMetaSliceSize():OsdCfg::GetOsdCfg().GetDataSliceSize();
            offset = e.order*fileSliceSize + e.offset;
            BaseObjIo *truncateIo = (dataType == Meta)?     \
                                ObjIoFactory::GetObjIoFactory().CreateObj(MetaTruncate):    \
                                ObjIoFactory::GetObjIoFactory().CreateObj(DataTruncate);
            OSD_LOG_ERR("===DoConsistencySub=== TRUNCATETYPE fileID[%s],offset[%d]",e.fileID.GetIDStr().c_str(),offset);
            truncateIo->InitObjIo((char *)&obj,NULL,0,offset);
            truncateIo->SetDstNode(e.nodeID);
            ret = LocWapper::GetLocWapper().Truncate(truncateIo);
            if (ret.IsSuccess())
            {
                ret = LocWapper::GetLocWapper().SendIo(truncateIo,NULL,0,truncateIo->GetOffset(),ConsistencyTruncate);
                if (ret.IsSuccess())
                {
                    OSD_LOG_ERR("===DoConsistencySub=== DelFileLogElement TRUNCATETYPE fileID[%s],offset[%d]",e.fileID.GetIDStr().c_str(),offset);
                    ioConsistencyLog->DelFileLogElement(e);
                }
            }
            truncateIo->DelObjIo(truncateIo);
        }
    }
}

void FaultHandler::DoRecover(ObjDataType dataType,FiEvent *evt,char *buf,u32 bufLen)
{
    static IoFileLogElement e;
    static string sliceFilePath;
    static s32 readLen = 0;
    static s32 retLen = 0;
    static s64 offset = 0;
    static FiErr ret;
    static FiEvent mutexEvt;
    IoFileLog *ioRecoverLog = (dataType == Meta)?   \
                            LocWapper::GetLocWapper().GetMetaIoRecoverLog():        \
                            LocWapper::GetLocWapper().GetDataIoRecoverLog();

    while (ioRecoverLog->GetFileLogElement(e))
    {
        evt->Wait();
        if (WRITETYPE == e.offset)
        {
            readLen = bufLen;
            ObjInfoHandle obj(e.fileID,e.mainNode,e.bakNode);
            offset = (s64)e.order * ((dataType == Meta)?(s64)OsdCfg::GetOsdCfg().GetMetaSliceSize():(s64)OsdCfg::GetOsdCfg().GetDataSliceSize());
            BaseObjIo *readIo = (dataType == Meta)?     \
                                ObjIoFactory::GetObjIoFactory().CreateObj(MetaRead):    \
                                ObjIoFactory::GetObjIoFactory().CreateObj(DataRead);
            BaseObjIo *writeIo = (dataType == Meta)?    \
                                ObjIoFactory::GetObjIoFactory().CreateObj(MetaWrite):   \
                                ObjIoFactory::GetObjIoFactory().CreateObj(DataWrite);

            OSD_LOG_ERR("===DoRecover=== WRITETYPE fileID[%s],order[%d]",e.fileID.GetIDStr().c_str(),e.order);

            do
            {
                readIo->InitObjIo((char *)&obj,(char *)buf,readLen,offset);

            WriteIoFindRedo:
                oimLock.Lock();
                if (ObjStream::GetObjStream().WriteIoFind(readIo->GetFileID(),readIo->GetLength(),readIo->GetOffset(),readIo->GetDataType(),&mutexEvt))
                {
                    oimLock.Unlock();
                    mutexEvt.Wait();
                    goto WriteIoFindRedo;
                }
                oim.IoAdd(readIo);
                oimLock.Unlock();

                ObjStream::GetObjStream().PushIo(readIo);
                retLen = readIo->GetRetLength();
                ret = readIo->GetRetStatus();
                if (ret.IsErr())
                {
                    oim.IoDel(readIo);
                    break;
                }

                if(retLen > 0)
                {
                    writeIo->InitObjIo((char *)&obj,(char *)buf,retLen,offset);
                    writeIo->SetDstNode(e.nodeID);
                    ret = LocWapper::GetLocWapper().SendIo(writeIo,buf,retLen,offset,RecoverWrite);
                    if (ret.IsErr())
                    {
                        oim.IoDel(readIo);
                        break;
                    }

                    offset += retLen;
                }
                oim.IoDel(readIo);

            }while(retLen);

            if (ret.IsSuccess())
            {
                OSD_LOG_ERR("===DoRecover=== DelFileLogElement WRITETYPE fileID[%s],order[%d]",e.fileID.GetIDStr().c_str(),e.order);
                ioRecoverLog->DelFileLogElement(e);
            }
            writeIo->DelObjIo(writeIo);
            readIo->DelObjIo(readIo);
        }
        else
        {
            ObjInfoHandle obj(e.fileID,e.mainNode,e.bakNode);
            u32 fileSliceSize = (dataType == Meta)?OsdCfg::GetOsdCfg().GetMetaSliceSize():OsdCfg::GetOsdCfg().GetDataSliceSize();
            offset = e.order*fileSliceSize + e.offset;
            BaseObjIo *truncateIo = (dataType == Meta)?     \
                                ObjIoFactory::GetObjIoFactory().CreateObj(MetaTruncate):    \
                                ObjIoFactory::GetObjIoFactory().CreateObj(DataTruncate);
            OSD_LOG_ERR("===DoRecover=== TRUNCATETYPE fileID[%s],offset[%d]",e.fileID.GetIDStr().c_str(),offset);
            truncateIo->InitObjIo((char *)&obj,NULL,0,offset);
            truncateIo->SetDstNode(e.nodeID);
            ret = LocWapper::GetLocWapper().SendIo(truncateIo,NULL,0,truncateIo->GetOffset(),RecoverTruncate);
            if (ret.IsSuccess())
            {
                OSD_LOG_ERR("===DoRecover=== DelFileLogElement TRUNCATETYPE fileID[%s],offset[%d]",e.fileID.GetIDStr().c_str(),offset);
                ioRecoverLog->DelFileLogElement(e);
            }
            truncateIo->DelObjIo(truncateIo);
        }
    }
}

void* FaultHandler::DoRecoverThread(void* par)
{
    FiEvent *evt = (FiEvent*)par;
    const s32 bufLen = 1*1024*1024;
    char *buf = new char[bufLen];
    RELEASE_ASSERT(buf != NULL);
    memset(buf,0,bufLen);

    while (1)
    {
        evt->Wait();
        OSD_LOG_ERR("===DoRecoverThread=== recover begin!!!");
        if (OsdCfg::GetOsdCfg().IsInstallMetaSvr())
        {
            FaultHandler::GetFaultHandler().DoRecover(Meta,evt,buf,bufLen);
        }

        FaultHandler::GetFaultHandler().DoRecover(Data,evt,buf,bufLen);

        if(NodeStatusObserver::GetNodeStatusObserver().NodeDoRecoverReset(NodeStatusObserver::GetNodeStatusObserver().GetRecoverRemoteNodeID()))
        {
            FaultHandler::GetFaultHandler().StopDoRecover();
            OSD_LOG_ERR("===DoRecoverThread=== recover done!!!");
        }
    }
}

FiErr NodeStatusObserver::LocStatusHandle(NodeStatusE status)
{
    if (status == ERR) 
    {
        ois = LocNodeErr;
        OSD_LOG_ERR("<<<<< LocNodeErr >>>>>");
        nodesStatusLock.Unlock();

        waitTime *= 2;
        waitTime = (waitTime > MAX_WAITTIME)?MAX_WAITTIME:waitTime;
        evt.Wait(waitTime);
        ObjStream::GetObjStream().WaitIoAryEmpty();

        ConsistencyHandler::GetConsistencyHandler().DoConsistency();

        nodesStatusLock.Lock();
        ObjStream::GetObjStream().AssertIoAryEmpty();
        return LocStatusSetRecover();
    }
    else if (status == RECOVER) 
    {
        ois = LocNodeRecover;
        OSD_LOG_ERR("<<<<< LocNodeRecover >>>>>");
        return LocStatusSetOK();
    }
    else
    {
        RELEASE_ASSERT_FASLE;
    }
}

FiErr NodeStatusObserver::NodeDoRecoverSetEx(s32 remoteNodeID,IoFileLog *ioRecoverLog,ObjID &fileID,vector<NodeAndTruncateNo> &ntAry)
{
    FiErr ret;
    bool isSet = false;
    RELEASE_ASSERT(remoteNodeID >= 0 && remoteNodeID != nodeID);

    nodesStatusLock.Lock();

    u32 locOffset = nodeID*nodeTotal+nodeID;
    u32 remoteOffset = remoteNodeID*nodeTotal+remoteNodeID;
    u32 setOffset = remoteNodeID*nodeTotal+nodeID;

    if (DORECOVER == nodesStatus[setOffset] && ERR == nodesStatus[remoteOffset])
    {
        vector<char> statusVec;
        statusVec.reserve(nodeTotal);
        for (u32 i = 0;i<nodeTotal;++i) 
        {
            statusVec.push_back(nodesStatus[i*nodeTotal+i]);
        }
        u32 nodeOkNum = count(statusVec.begin(),statusVec.end(),OK);
        if (nodeOkNum < nodeTotal-1)
        {
            ret = FErr(MOsd_OsdIsErr);
        }
		else
		{
			for (vector<NodeAndTruncateNo>::iterator it = ntAry.begin(); it != ntAry.end(); ++it)
			{
				OSD_LOG_ERR("===NodeDoRecoverSetEx=== AddFileLogElement fileID[%s],order[%d]", fileID.GetIDStr().c_str(), *it);
				ret = ioRecoverLog->AddFileLogElement(fileID, it->truncateNo, remoteNodeID, WRITETYPE, it->nodeID, nodeID);
				if (ret.IsErr())
				{
					break;
				}
			}
		}
        nodesStatusLock.Unlock();
        OSD_LOG_ERR("===NodeDoRecoverSetEx=== ret!!! DORECOVER == nodesStatus[setOffset] && ERR == nodesStatus[remoteOffset]");
        return ret;
    }
    
    OSD_LOG_ERR("===NodeDoRecoverSetEx=== GetZKLock begin");
    zkl->GetZKLock();
    ret = zkDataBase::GetZkDataBase().GetNodeData(nodesStatus,nodeTotal*nodeTotal,path,StatusWatcher,this);
    if(ret.IsSuccess())
    {
        OSD_LOG_ERR("===NodeDoRecoverSetEx=== GetNodeData %s",GetNodesStatusFMTStr());

        char statusBak = nodesStatus[remoteOffset];
        nodesStatus[remoteOffset] = ERR;
        isSet = true;
        ret = zkDataBase::GetZkDataBase().SetNodeData(path,nodesStatus,nodeTotal*nodeTotal);
        if (ret.IsErr())
        {
            nodesStatus[remoteOffset] = statusBak;
            OSD_LOG_ERR("===NodeDoRecoverSetEx=== SetNodeData <failed> remoteNodeID[%d-ERR***] locNode[%d-DORECOVER] %s",remoteNodeID,nodeID,GetNodesStatusFMTStr());
        }
        else
        {
            OSD_LOG_ERR("===NodeDoRecoverSetEx=== SetNodeData suc remoteNodeID[%d-ERR***] locNode[%d-DORECOVER] %s",remoteNodeID,nodeID,GetNodesStatusFMTStr());
            vector<char> statusVec;
            statusVec.reserve(nodeTotal);
            for (u32 i = 0;i<nodeTotal;++i) 
            {
                statusVec.push_back(nodesStatus[i*nodeTotal+i]);
            }
            u32 nodeOkNum = count(statusVec.begin(),statusVec.end(),OK);
            if (nodeOkNum < nodeTotal-1)
            {
                ret = FErr(MOsd_OsdIsErr);
            }
            else
            {
                statusBak = nodesStatus[setOffset];
                nodesStatus[setOffset] = DORECOVER;
                ret = zkDataBase::GetZkDataBase().SetNodeData(path,nodesStatus,nodeTotal*nodeTotal);
                if (ret.IsErr())
                {
                    nodesStatus[setOffset] = statusBak;
                    OSD_LOG_ERR("===NodeDoRecoverSetEx=== SetNodeData <failed> remoteNodeID[%d-ERR] locNode[%d-DORECOVER***] %s",remoteNodeID,nodeID,GetNodesStatusFMTStr());
                }
                else
                {
                    OSD_LOG_ERR("===NodeDoRecoverSetEx=== SetNodeData suc remoteNodeID[%d-ERR] locNode[%d-DORECOVER***] %s",remoteNodeID,nodeID,GetNodesStatusFMTStr());
                    for (vector<NodeAndTruncateNo>::iterator it = ntAry.begin();it != ntAry.end();++it)
                    {
                        OSD_LOG_ERR("===NodeDoRecoverSetEx=== AddFileLogElement fileID[%s],order[%d]",fileID.GetIDStr().c_str(),*it);
                        ret = ioRecoverLog->AddFileLogElement(fileID,it->truncateNo,remoteNodeID,WRITETYPE,it->nodeID,nodeID);
                        if (ret.IsErr())
                        {
                            break;
                        }
                    }
                }
            }
        }
    }

    zkl->ReleaseZKLock();
    OSD_LOG_ERR("===NodeDoRecoverSetEx=== GetZKLock end");
    nodesStatusLock.Unlock();

    if (ret.IsErr())
    {
        OSD_LOG_ERR("===NodeDoRecoverSetEx=== %s <failed> path[%s],errInfo[%s]",isSet?"SetNodeData":"GetNodeData",path.c_str(),ret.StrFiText().c_str());
    }
    return ret;
}

FiErr NodeStatusObserver::NodeDoRecoverSet(s32 remoteNodeID,BaseObjIo *io)
{
    FiErr ret;
    bool isSet = false;
    RELEASE_ASSERT(remoteNodeID >= 0 && remoteNodeID != nodeID);

    nodesStatusLock.Lock();

    u32 locOffset = nodeID*nodeTotal+nodeID;
    u32 remoteOffset = remoteNodeID*nodeTotal+remoteNodeID;
    u32 setOffset = remoteNodeID*nodeTotal+nodeID;

    if (DORECOVER == nodesStatus[setOffset] && ERR == nodesStatus[remoteOffset])
    {
        if (io->IsSetDoRecover())
        {
            vector<char> statusVec;
            statusVec.reserve(nodeTotal);
            for (u32 i = 0;i<nodeTotal;++i) 
            {
                statusVec.push_back(nodesStatus[i*nodeTotal+i]);
            }
            u32 nodeOkNum = count(statusVec.begin(),statusVec.end(),OK);
            if (nodeOkNum < nodeTotal-1)
            {
                ret = FErr(MOsd_OsdIsErr);
            }
            else
            {
                ret = io->LogForRecover();
            }
        }
        nodesStatusLock.Unlock();
        OSD_LOG_ERR("===NodeDoRecoverSet=== ret!!! DORECOVER == nodesStatus[setOffset] && ERR == nodesStatus[remoteOffset]");
        return ret;
    }

    OSD_LOG_ERR("===NodeDoRecoverSet=== GetZKLock begin");
    zkl->GetZKLock();
    ret = zkDataBase::GetZkDataBase().GetNodeData(nodesStatus,nodeTotal*nodeTotal,path,StatusWatcher,this);
    if(ret.IsSuccess())
    {
        OSD_LOG_ERR("===NodeDoRecoverSet=== GetNodeData %s",GetNodesStatusFMTStr());
//      if (OK != nodesStatus[locOffset])
//      {
//          ret = FErr(MOsd_LocNodeIsNotOK);
//          goto NodeDoRecoverSetEnd;
//      }

        char statusBak = nodesStatus[remoteOffset];
        nodesStatus[remoteOffset] = ERR;
        isSet = true;
        ret = zkDataBase::GetZkDataBase().SetNodeData(path,nodesStatus,nodeTotal*nodeTotal);
        if (ret.IsErr())
        {
            nodesStatus[remoteOffset] = statusBak;
            OSD_LOG_ERR("===NodeDoRecoverSet=== SetNodeData <failed> remoteNodeID[%d-ERR***] locNode[%d-DORECOVER] %s",remoteNodeID,nodeID,GetNodesStatusFMTStr());
        }
        else
        {
            OSD_LOG_ERR("===NodeDoRecoverSet=== SetNodeData suc remoteNodeID[%d-ERR***] locNode[%d-DORECOVER] %s",remoteNodeID,nodeID,GetNodesStatusFMTStr());

            if (io->IsSetDoRecover())
            {
                vector<char> statusVec;
                statusVec.reserve(nodeTotal);
                for (u32 i = 0;i<nodeTotal;++i) 
                {
                    statusVec.push_back(nodesStatus[i*nodeTotal+i]);
                }
                u32 nodeOkNum = count(statusVec.begin(),statusVec.end(),OK);
                if (nodeOkNum < nodeTotal-1)
                {
                    ret = FErr(MOsd_OsdIsErr);
                }
                else
                {
                    statusBak = nodesStatus[setOffset];
                    nodesStatus[setOffset] = DORECOVER;
                    ret = zkDataBase::GetZkDataBase().SetNodeData(path,nodesStatus,nodeTotal*nodeTotal);
                    if (ret.IsErr())
                    {
                        nodesStatus[setOffset] = statusBak;
                        OSD_LOG_ERR("===NodeDoRecoverSet=== SetNodeData <failed> remoteNodeID[%d-ERR] locNode[%d-DORECOVER***] %s",remoteNodeID,nodeID,GetNodesStatusFMTStr());
                    }
                    else
                    {
                        OSD_LOG_ERR("===NodeDoRecoverSet=== SetNodeData suc remoteNodeID[%d-ERR] locNode[%d-DORECOVER***] %s",remoteNodeID,nodeID,GetNodesStatusFMTStr());
                        ret = io->LogForRecover();
                    }
                }
            }
        }
    }

//NodeDoRecoverSetEnd:
    zkl->ReleaseZKLock();
    OSD_LOG_ERR("===NodeDoRecoverSet=== GetZKLock end");
    nodesStatusLock.Unlock();

    if (ret.IsErr())
    {
        OSD_LOG_ERR("===NodeDoRecoverSet=== %s <failed> path[%s],errInfo[%s]",isSet?"SetNodeData":"GetNodeData",path.c_str(),ret.StrFiText().c_str());
    }
    return ret;
}

void NetWapper::SendNetIo(BaseObjIo *io)
{
#ifdef OSD_DEBUG_NONET
    if (NetSendTest(io))
    {
    }
#else
    io->Change2NetObjIo();
    CMemItem buff;
    buff.SetBufferSize(sizeof(BaseObjIo));
    buff.AddData((byte*)io,sizeof(BaseObjIo));

    call_cxt cxt;

    s32 netRet = 0;
    if (io->GetSrcNode() == io->GetDstNode())
    {
        cxt.set_call(NET_MOD_ID,(char*)buff.GetBufferPtr(),buff.GetDataRealSize(),io->GetSrcNodeIP(),io->GetSrcNodeIP());
        if (io->IsHaveSendBuf())
        {
            cxt.set_buf2(io->GetBuf(), io->GetLength());
        }
        netRet = net.LocalCall(cxt);
    }
    else
    {
        cxt.set_call(NET_MOD_ID,(char*)buff.GetBufferPtr(),buff.GetDataRealSize(),io->GetSrcNodeIP(),io->GetDstNodeIP());
        if (io->IsHaveSendBuf())
        {
            cxt.set_buf2(io->GetBuf(), io->GetLength());
        }
        netRet = net.Call(cxt);
    }

    if (0 == netRet)
    {
        if (io->IsHaveReplyBuf())
        {
            BaseObjIo *ioRet = (BaseObjIo *)cxt.get_reply_data()->GetBufferPtr();
            RELEASE_ASSERT(ioRet->GetRetLength()+sizeof(BaseObjIo) == cxt.get_reply_data()->GetDataRealSize());
            memcpy(io->GetBuf(),(char *)ioRet+sizeof(BaseObjIo),ioRet->GetRetLength());
        }

        OSD_LOG_DBG("===SendNetIo=== send suc,fileID[%s],buf[%p],length[%d],offset[%ldd]",io->GetFileID().GetIDStr().c_str(),io->GetBuf(),io->GetLength(),io->GetOffset());
        io->ProcessNetReplyBuf((char *)cxt.get_reply_data()->GetBufferPtr());
        if (io->GetRetStatus().IsErr())
        {
            OSD_LOG_ERR("net op failed,ioType[%d],errInfo[%s]",io->GetObjIoType(),io->GetRetStatus().StrFiText().c_str());
            FiErr ret = NodeStatusObserver::GetNodeStatusObserver().NodeDoRecoverSet(io->GetDstNode(),io);
            io->SetRetStatus(ret);
            if(ret.IsSuccess())
            {
                OSD_LOG_ERR("===NodeDoRecoverSet=== suc srcNodeIP[%d],dstNodeID[%d]",io->GetSrcNodeIP(),io->GetDstNode());
//              io->SetRetStatus(FErr(MNet_SendFailed));    net failed but io ret suc 
            }
        }
    }
#endif
    else
    {
        OSD_LOG_ERR("send buff failed");
        FiErr ret = NodeStatusObserver::GetNodeStatusObserver().NodeDoRecoverSet(io->GetDstNode(),io);
        if(ret.IsSuccess())
        {
            OSD_LOG_ERR("===NodeDoRecoverSet=== suc srcNodeID[%d],dstNodeID[%d]",io->GetSrcNode(),io->GetDstNode());
//          io->SetRetStatus(FErr(MNet_SendFailed));    net failed but io ret suc 
        }
        else
        {
            io->SetRetStatus(ret);
        }
    }
}

bool NetWapper::ReceiveNetIo(MemItemPtr sheetData,MemItemPtr soktData, CTCPSocket *psokt, void *userData)
{
    RecvIoHandlePar *rihp = new RecvIoHandlePar();
    rihp->p1 = sheetData;
    rihp->p2 = soktData;
    Tsheet *sh = (Tsheet *)sheetData->GetCurDataPtr();
    OSD_LOG_ERR("===ReceiveNetIo===...pktid[%lld],mod[%d]",sh->sheet_num,sh->reg_module_type);
    RELEASE_ASSERT(NetWapper::GetNetWapper().GetRecvIoHandleThreadPool().PushTask(0,rihp,NULL,NULL).IsSuccess());
    return true;
}

void NetWapper::ReceiveNetIoHandle(MemItemPtr sheetData,MemItemPtr soktData)
{
    BaseObjIo *ioTp = (BaseObjIo *)soktData->GetBufferPtr();
    RELEASE_ASSERT(ioTp->GetVersion() == OsdCfg::GetOsdCfg().GetVersion());

    OSD_LOG_ERR("===ReceiveNetIoHandle=== begin...pktid[%lld],mod[%d],fileID[%s],objIoType[%s],len[0x%x],offset[0x%llx]",sh->sheet_num,sh->reg_module_type,ioTp->GetFileID().GetIDStr().c_str(),GetObjIoTypeStr(ioTp->GetObjIoType()),ioTp->GetLength(),ioTp->GetOffset());

    OsdInitHelper::GetOsdInitHelper().WaitOsdInitDone();

    if (ioTp->GetObjIoType() == DataDistributeTruncate)
    {
        net.Reply(sheetData, (char *)io, sizeof(BaseObjIo), NULL, 0);
        DataIF dif;
        dif.DistributeTruncateData((char *)io);
        OSD_LOG_ERR("===ReceiveNetIoHandle=== [DataDistributeTruncate] done fileID[%s],ret[%s]",ioTp->GetFileID().GetIDStr().c_str(),ioTp->GetRetStatus().StrFiText().c_str());
        return;
    }

    BaseObjIo *io = ObjIoFactory::GetObjIoFactory().CreateObj(ioTp->GetSrcObjIoType());
	objmemcpy(io,ioTp,sizeof(BaseObjIo));

    NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Lock();
    if (io->IsCheckNodeStatus())
    {
        if (NodeStatusObserver::GetNodeStatusObserver().IsLocNodeErr())
        {
            NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
            io->SetRetStatus(FErr(MOsd_LocNodeIsErr));
            net.Reply(sheetData, (char *)io, sizeof(BaseObjIo), NULL, 0);
            OSD_LOG_ERR("===ReceiveNetIoHandle=== failed fileID[%s],ret[%s]",io->GetFileID().GetIDStr().c_str(),io->GetRetStatus().StrFiText().c_str());
            io->DelObjIo(io);
            return;
        }

        if (NodeStatusObserver::GetNodeStatusObserver().IsRemoteNodeErr() &&
            NodeStatusObserver::GetNodeStatusObserver().FindNotOkNodeID(io->GetSrcNode()))
        {
            NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
            io->SetRetStatus(FErr(MOsd_RemoteNodeIsNotOK));
            net.Reply(sheetData, (char *)io, sizeof(BaseObjIo), NULL, 0);
            OSD_LOG_ERR("===ReceiveNetIoHandle=== failed fileID[%s],ret[%s]",io->GetFileID().GetIDStr().c_str(),io->GetRetStatus().StrFiText().c_str());
            io->DelObjIo(io);
            return;
        }

        if (io->GetMainNode() == OsdCfg::GetOsdCfg().GetLocNodeID() &&  \
            NodeStatusObserver::GetNodeStatusObserver().IsLocNodeOK())
        {
            NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
            io->SetRetStatus(FErr(MOsd_MainNodeIsOK));
            net.Reply(sheetData, (char *)io, sizeof(BaseObjIo), NULL, 0);
            OSD_LOG_ERR("===ReceiveNetIoHandle=== failed fileID[%s],ret[%s]",io->GetFileID().GetIDStr().c_str(),io->GetRetStatus().StrFiText().c_str());
            io->DelObjIo(io);
            return;
        }
    }

    BaseObjIo *netIo = ObjIoFactory::GetObjIoFactory().CreateObj(io->GetObjIoType());
    io->DelObjIo(io);
    netIo->InitObjIo((char *)soktData->GetBufferPtr());

    ProcessingNetIoAryAdd(netIo);
    NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
    ObjStream::GetObjStream().PushIo(netIo);
    ProcessingNetIoAryDel(netIo);

    if (netIo->IsHaveReplyBuf())
    {
        net.Reply(sheetData, (char *)netIo, sizeof(BaseObjIo), netIo->GetBuf(), netIo->GetRetLength());
    } else
    {
        net.Reply(sheetData, (char *)netIo, sizeof(BaseObjIo), NULL, 0);
    }
    OSD_LOG_ERR("===ReceiveNetIoHandle=== done fileID[%s],ret[%s]",netIo->GetFileID().GetIDStr().c_str(),netIo->GetRetStatus().StrFiText().c_str());
    netIo->DelObjIo(netIo);
}

string GetStorDirPath(bool isMeta,ObjID &fileID)
{
    string rootPath = (isMeta)?OsdCfg::GetOsdCfg().GetMetaRootPath():OsdCfg::GetOsdCfg().GetDataRootPath();
    u32 hn = fileID.GetU32()%HashPathNum;
    char noStr[6] = { 0 };
    sprintf(noStr, "%d/", hn);
    return rootPath+noStr+fileID.GetIDStr();
}

void SessionWatcher(int state)
{
    if (state == NS_CONNECTED)
    {
        OSD_LOG_ERR("===SessionWatcher=== NS_CONNECTED");
        zkDataBase::GetZkDataBase().MarkMeAlive();
        while (!NodeStatusObserver::GetNodeStatusObserver().RefreshNodeStatusAndSet())
        {
            FiEvent evt;
            evt.Wait(2000);
        }
        NodeStatusObserver::GetNodeStatusObserver().StatusAnalyze();
        NodeSpaceCollecter::GetNodeSpaceCollecter().RefreshSpace();
    }
    else if (state == NS_DISCONNECTED)
    {
        OSD_LOG_ERR("===SessionWatcher=== NS_DISCONNECTED");
        zkDataBase::GetZkDataBase().ReConnect();
    }
    else
    {
        RELEASE_ASSERT_FASLE;
    }
}

void NodeChangeWatcher(void *par)
{
    nodeChangeInfo *nci = (nodeChangeInfo *)par;
    OSD_LOG_ERR("===NodeChangeWatcher=== nodeID[%d],state[%s]",nci->nodeID,nci->isDead?"Down":"Up");
    if (nci->isDead)
    {
        if (nci->nodeID != OsdCfg::GetOsdCfg().GetLocNodeID())
        {
            NodeStatusObserver::GetNodeStatusObserver().NodeErrSet(nci->nodeID);
        }
    }
}

void InitNodeStatus()
{
    while (!NodeStatusObserver::GetNodeStatusObserver().RefreshNodeStatusAndSet())
    {
        FiEvent evt;
        evt.Wait(2000);
    }
    NodeStatusObserver::GetNodeStatusObserver().StatusAnalyze();
}

//void OsdNotifyStatusCallBack(OsdLayerStatus s)
//{
//    NodeStatusObserver::GetNodeStatusObserver().SetOsdStatus(s);
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



