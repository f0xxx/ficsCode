///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  hunter code ([hunter.f0x] remeber my brothers) email:hunterlhy@163.com /////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "fiOsdIF.h"
#include "fiOsd.h"
#include "../ficsNetSyn/ficsNetSyn.h"

bool gIsScanMode = false;

OsdInitHelper::OsdInitHelper(bool isScanMode)
{
	if (isScanMode)
	{
		evt.SetMode(TRUE);
		InitObjIoFactory();
		evt.Set();
	}
	else
	{
		evt.SetMode(TRUE);
		OsdCfg::GetOsdCfg();
		ficsLocIF::GetFicsLocIF();
		ObjIoPool::GetObjIoPool();
		ObjStream::GetObjStream();
		NetWapper::GetNetWapper();
		LocWapper::GetLocWapper();
		InitNetSyn();
		zkDataBase::GetZkDataBase().AddCSNodeWatch(NodeChangeWatcher);
		zkDataBase::GetZkDataBase().AddSessionWatch(SessionWatcher);
		InitObjIoFactory();

		NodeSpaceCollecter::GetNodeSpaceCollecter();
		NodeStatusObserver::GetNodeStatusObserver();
		InitNodeStatus();

		evt.Set();
	}
//  RegisterNotifyStatusFuc(OsdNotifyStatusCallBack);
}

void InitOsdLayer(bool isScanMode,bool isUseFiState)
{
	gIsScanMode = isScanMode;
	gIsUseFiState = isUseFiState;
	OsdInitHelper::GetOsdInitHelper();
}

void InitNetWapper()
{
	NetWapper::GetNetWapper();
}

void RegisterNotifyStatusFuc(NotifyStatusCallBack f)
{
	FaultHandler::GetFaultHandler().RegisterFuc(f);
}

FiErr MetaIF::WriteMeta(ObjInfoHandle &obj, void *buf, u32 &len, u64 offset)
{
	FiErr ret;
	FiEvent evt;
	RELEASE_ASSERT(!obj.GetFileID().IsNull());
	if (0 == len)
	{
		OSD_LOG_ERR("===WriteMeta=== failed...(0 == len)");
		return ret;
	}

WriteMetaBegin:
	NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Lock();
	if (!NodeStatusObserver::GetNodeStatusObserver().IsLocNodeOK())
	{
		ret = FErr(MOsd_LocNodeIsNotOK);
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		OSD_LOG_ERR("===WriteMeta=== failed...ret[%s],fileID[%s],buf[%p],len[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),buf,len,offset);
		return ret;
	}

	if (OsdCfg::GetOsdCfg().GetLocNodeID() != obj.GetMetaMainNode() && 	\
		!NodeStatusObserver::GetNodeStatusObserver().FindNotOkNodeID(obj.GetMetaMainNode()))
	{
		ret = FErr(MOsd_MainNodeIsOK);
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		OSD_LOG_ERR("===WriteMeta=== failed...ret[%s],fileID[%s],buf[%p],len[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),buf,len,offset);
		return ret;
	}

	if(NetWapper::GetNetWapper().ProcessingNetIoAryFind(obj.GetFileID(),len,offset,Meta,&evt))
	{
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		evt.Wait();
		goto WriteMetaBegin;
	}

	if (ObjStream::GetObjStream().WriteIoFind(obj.GetFileID(),len,offset,Meta,&evt))
	{
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		evt.Wait();
		goto WriteMetaBegin;
	}

	BaseObjIo *metaWriteIo = ObjIoFactory::GetObjIoFactory().CreateObj(MetaWrite);
	metaWriteIo->InitObjIo((char *)&obj,(char *)buf,len,offset);

	FaultHandler::GetFaultHandler().GetOimLock().Lock();
	if(FaultHandler::GetFaultHandler().RecoverIoFind(obj.GetFileID(),len,offset,Meta,&evt))
	{
		metaWriteIo->DelObjIo(metaWriteIo);
		FaultHandler::GetFaultHandler().GetOimLock().Unlock();
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		evt.Wait();
		goto WriteMetaBegin;
	}
	ObjStream::GetObjStream().WriteIoAdd(metaWriteIo);
	FaultHandler::GetFaultHandler().GetOimLock().Unlock();
	NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();

	ObjStream::GetObjStream().PushIo(metaWriteIo);
	ret = metaWriteIo->GetRetStatus();
	ObjStream::GetObjStream().WriteIoDel(metaWriteIo);
	metaWriteIo->DelObjIo(metaWriteIo);

	if (ret.IsErr())
	{
		ret.SetCustomErr(ErrNodeAgencyStop);
		OSD_LOG_ERR("===WriteMeta=== failed...ret[%s],fileID[%s],buf[%p],len[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),buf,len,offset);
	}
	OSD_LOG_DBG("===WriteMeta=== done...ret[%s],fileID[%s],buf[%p],len[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),buf,len,offset);
	return ret;
}

FiErr MetaIF::ScanningReadMeta(ObjInfoHandle &obj, void *buf, u32 &len, u64 offset)
{
	FiErr ret;
	RELEASE_ASSERT(!obj.GetFileID().IsNull());
	if (0 == len)
	{
		OSD_LOG_ERR("===ScanningReadMeta=== failed...(0 == len)");
		return ret;
	}

	BaseObjIo *metaReadIo = ObjIoFactory::GetObjIoFactory().CreateObj(MetaRead);
	metaReadIo->InitObjIo((char *)&obj,(char *)buf,len,offset);
	ObjStream::GetObjStream().PushIo(metaReadIo);
	len = metaReadIo->GetRetLength();
	ret = metaReadIo->GetRetStatus();
	metaReadIo->DelObjIo(metaReadIo);
	if (ret.IsErr())
	{
		ret.SetCustomErr(ErrNodeAgencyStop);
		OSD_LOG_ERR("===ScanningReadMeta=== failed...ret[%s],fileID[%s],buf[%p],len[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),buf,len,offset);
	}
	OSD_LOG_DBG("===ScanningReadMeta=== done...ret[%s],fileID[%s],buf[%p],len[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),buf,len,offset);
	return ret;
}

FiErr MetaIF::ReadMeta(ObjInfoHandle &obj, void *buf, u32 &len, u64 offset)
{
	FiErr ret;
	RELEASE_ASSERT(!obj.GetFileID().IsNull());
	if (0 == len)
	{
		OSD_LOG_ERR("===ReadMeta=== failed...(0 == len)");
		return ret;
	}

	NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Lock();
	if (!NodeStatusObserver::GetNodeStatusObserver().IsLocNodeOK())
	{
		ret = FErr(MOsd_LocNodeIsNotOK);
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		OSD_LOG_ERR("===ReadMeta=== failed...ret[%s],fileID[%s],buf[%p],len[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),buf,len,offset);
		return ret;
	}
	NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();

	BaseObjIo *metaReadIo = ObjIoFactory::GetObjIoFactory().CreateObj(MetaRead);
	metaReadIo->InitObjIo((char *)&obj,(char *)buf,len,offset);
	ObjStream::GetObjStream().PushIo(metaReadIo);
	len = metaReadIo->GetRetLength();
	ret = metaReadIo->GetRetStatus();
	metaReadIo->DelObjIo(metaReadIo);
	if (ret.IsErr())
	{
		ret.SetCustomErr(ErrNodeAgencyStop);
		OSD_LOG_ERR("===ReadMeta=== failed...ret[%s],fileID[%s],buf[%p],len[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),buf,len,offset);
	}
	OSD_LOG_DBG("===ReadMeta=== done...ret[%s],fileID[%s],buf[%p],len[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),buf,len,offset);
	return ret;
}

FiErr MetaIF::DelMeta(ObjInfoHandle &obj)
{
	FiErr ret;
	RELEASE_ASSERT(!obj.GetFileID().IsNull());

	NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Lock();
	if (!NodeStatusObserver::GetNodeStatusObserver().IsLocNodeOK())
	{
		ret = FErr(MOsd_LocNodeIsNotOK);
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		OSD_LOG_ERR("===DelMeta=== failed...ret[%s],fileID[%s]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str());
		return ret;
	}
	NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();


	BaseObjIo *metaDelIo = ObjIoFactory::GetObjIoFactory().CreateObj(MetaDel);
	metaDelIo->InitObjIo((char *)&obj);
	ObjStream::GetObjStream().PushIo(metaDelIo);
	ret = metaDelIo->GetRetStatus();
	metaDelIo->DelObjIo(metaDelIo);
	if (ret.IsErr())
	{
		ret.SetCustomErr(ErrNodeAgencyStop);
		OSD_LOG_ERR("===DelMeta=== failed...ret[%s],fileID[%s]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str());
	}
	OSD_LOG_DBG("===DelMeta=== done...ret[%s],fileID[%s]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str());
	return ret;
}

FiErr MetaIF::AllocMetalObjEx(ObjInfoHandle &obj,s32 mainNode,RddcPolicy rp)
{
	FiErr ret;
//	if (!NodeStatusObserver::GetNodeStatusObserver().IsLocNodeOK())
//	{
//		ret = FErr(MOsd_LocNodeIsNotOK);
//		return ret;
//	}

	ObjID fid;
	fid.GenerateID();
	ObjInfoSequence bis;

	bis.metaNode[0] = mainNode;
	bis.metaNode[1] = OsdCfg::GetOsdCfg().GetMetaBakNodeIDEx(mainNode);

	bis.DataRP = (FiDefault == rp)?OsdCfg::GetOsdCfg().GetDataRP():rp;
	bis.dataNodeNum = OsdCfg::GetOsdCfg().GetDataNodeHashNum();

	for (u32 i = 0;i < bis.dataNodeNum;++i)
	{
		bis.dataNode[i] = NodeSpaceCollecter::GetNodeSpaceCollecter().AllocOneNode(false,bis.dataNode,i);
	}

	ObjInfoHandle objRet(fid,(char *)&bis,sizeof(ObjInfoSequence));
	obj = objRet;

	return ret;
}

FiErr MetaIF::AllocMetalObj(ObjInfoHandle &obj,bool isLocalAlloc,RddcPolicy rp)
{
	FiErr ret;
//	if (!NodeStatusObserver::GetNodeStatusObserver().IsLocNodeOK())		//always suc
//	{
//		ret = FErr(MOsd_LocNodeIsNotOK);
//		return ret;
//	}

	ObjID fid;
	fid.GenerateID();
	ObjInfoSequence bis;

	if (isLocalAlloc)
	{
		bis.metaNode[0] = OsdCfg::GetOsdCfg().GetLocNodeID();
		bis.metaNode[1] = OsdCfg::GetOsdCfg().GetMetaBakNodeID();
	}
	else
	{
		bis.metaNode[0] = NodeSpaceCollecter::GetNodeSpaceCollecter().AllocOneNode(true);
		bis.metaNode[1] = OsdCfg::GetOsdCfg().GetMetaBakNodeIDEx(bis.metaNode[0]);
	}

	bis.DataRP = (FiDefault == rp)?OsdCfg::GetOsdCfg().GetDataRP():rp;
	bis.dataNodeNum = OsdCfg::GetOsdCfg().GetDataNodeHashNum();

	for (u32 i = 0;i < bis.dataNodeNum;++i)
	{
		bis.dataNode[i] = NodeSpaceCollecter::GetNodeSpaceCollecter().AllocOneNode(false,bis.dataNode,i);
	}

	ObjInfoHandle objRet(fid,(char *)&bis,sizeof(ObjInfoSequence));
	obj = objRet;

	return ret;
}

FiErr MetaIF::TruncateMeta(ObjInfoHandle &obj, u64 offset, u64 oriSize)
{
	FiErr ret;
	RELEASE_ASSERT(!obj.GetFileID().IsNull());

TruncateMetaBegin:
	NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Lock();
	if (!NodeStatusObserver::GetNodeStatusObserver().IsLocNodeOK())
	{
		ret = FErr(MOsd_LocNodeIsNotOK);
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		OSD_LOG_ERR("===TruncateMeta=== failed...ret[%s],fileID[%s],oriSize[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),oriSize,offset);
		return ret;
	}

	if (OsdCfg::GetOsdCfg().GetLocNodeID() != obj.GetMetaMainNode() && 	\
		!NodeStatusObserver::GetNodeStatusObserver().FindNotOkNodeID(obj.GetMetaMainNode()))
	{
		ret = FErr(MOsd_MainNodeIsOK);
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		OSD_LOG_ERR("===TruncateMeta=== failed...ret[%s],fileID[%s],oriSize[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),oriSize,offset);
		return ret;
	}

	if (oriSize > offset)
	{
		FiEvent evt;
		if(NetWapper::GetNetWapper().ProcessingNetIoAryFind(obj.GetFileID(),oriSize-offset,offset,Meta,&evt))
		{
			NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
			evt.Wait();
			goto TruncateMetaBegin;
		}

		if (ObjStream::GetObjStream().WriteIoFind(obj.GetFileID(),oriSize-offset,offset,Meta,&evt))
		{
			NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
			evt.Wait();
			goto TruncateMetaBegin;
		}
		
		BaseObjIo *metaTruncateIo = ObjIoFactory::GetObjIoFactory().CreateObj(MetaTruncate);
		metaTruncateIo->InitObjIo((char *)&obj,NULL,0,offset);

		FaultHandler::GetFaultHandler().GetOimLock().Lock();
		if(FaultHandler::GetFaultHandler().RecoverIoFind(obj.GetFileID(),oriSize-offset,offset,Meta,&evt))
		{
			metaTruncateIo->DelObjIo(metaTruncateIo);
			FaultHandler::GetFaultHandler().GetOimLock().Unlock();
			NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
			evt.Wait();
			goto TruncateMetaBegin;
		}
		ObjStream::GetObjStream().WriteIoAdd(metaTruncateIo);
		FaultHandler::GetFaultHandler().GetOimLock().Unlock();
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();

		ObjStream::GetObjStream().PushIo(metaTruncateIo);
		ret = metaTruncateIo->GetRetStatus();
		ObjStream::GetObjStream().WriteIoDel(metaTruncateIo);
		metaTruncateIo->DelObjIo(metaTruncateIo);
	}
	else
	{
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
	}

	if (ret.IsErr())
	{
		ret.SetCustomErr(ErrNodeAgencyStop);
		OSD_LOG_ERR("===TruncateMeta=== failed...ret[%s],fileID[%s],oriSize[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),oriSize,offset);
	}
	OSD_LOG_DBG("===TruncateMeta=== done...ret[%s],fileID[%s],oriSize[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),oriSize,offset);
	return ret;
}

FiErr DataIF::WriteData(ObjInfoHandle &obj, void *buf, u32 &len, u64 offset)
{
	FiErr ret;
	FiEvent evt;
	RELEASE_ASSERT(!obj.GetFileID().IsNull());
	if (0 == len)
	{
		OSD_LOG_ERR("===WriteData=== failed...(0 == len)");
		return ret;
	}

WriteDataBegin:
	NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Lock();
	if (!NodeStatusObserver::GetNodeStatusObserver().IsLocNodeOK())
	{
		ret = FErr(MOsd_LocNodeIsNotOK);
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		OSD_LOG_ERR("===WriteData=== failed...ret[%s],fileID[%s],buf[%p],len[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),buf,len,offset);
		return ret;
	}

	if (OsdCfg::GetOsdCfg().GetLocNodeID() != obj.GetRev1() && 	\
		!NodeStatusObserver::GetNodeStatusObserver().FindNotOkNodeID(obj.GetRev1()))
	{
		ret = FErr(MOsd_MainNodeIsOK);
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		OSD_LOG_ERR("===WriteData=== failed...ret[%s],fileID[%s],buf[%p],len[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),buf,len,offset);
		return ret;
	}

	if(NetWapper::GetNetWapper().ProcessingNetIoAryFind(obj.GetFileID(),len,offset,Data,&evt))
	{
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		evt.Wait();
		goto WriteDataBegin;
	}

	if (ObjStream::GetObjStream().WriteIoFind(obj.GetFileID(),len,offset,Data,&evt))
	{
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		evt.Wait();
		goto WriteDataBegin;
	}


	BaseObjIo *dataWriteIo = ObjIoFactory::GetObjIoFactory().CreateObj(DataWrite);
	dataWriteIo->InitObjIo((char *)&obj,(char *)buf,len,offset);

	FaultHandler::GetFaultHandler().GetOimLock().Lock();
	if(FaultHandler::GetFaultHandler().RecoverIoFind(obj.GetFileID(),len,offset,Data,&evt))
	{
		dataWriteIo->DelObjIo(dataWriteIo);
		FaultHandler::GetFaultHandler().GetOimLock().Unlock();
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		evt.Wait();
		goto WriteDataBegin;
	}
	ObjStream::GetObjStream().WriteIoAdd(dataWriteIo);
	FaultHandler::GetFaultHandler().GetOimLock().Unlock();

	NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();

	ObjStream::GetObjStream().PushIo(dataWriteIo);
	ret = dataWriteIo->GetRetStatus();
	
	ObjStream::GetObjStream().WriteIoDel(dataWriteIo);
	dataWriteIo->DelObjIo(dataWriteIo);
	if (ret.IsErr())
	{
		ret.SetCustomErr(ErrNodeAgencyStop);
		OSD_LOG_ERR("===WriteData=== failed...ret[%s],fileID[%s],buf[%p],len[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),buf,len,offset);
	}
	OSD_LOG_DBG("===WriteData=== done...ret[%s],fileID[%s],buf[%p],len[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),buf,len,offset);
	return ret;
}

FiErr DataIF::ReadData(ObjInfoHandle &obj, void *buf, u32 &len, u64 offset)
{
	FiErr ret;
	RELEASE_ASSERT(!obj.GetFileID().IsNull());
	if (0 == len)
	{
		OSD_LOG_ERR("===ReadData=== failed...(0 == len)");
		return ret;
	}

	NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Lock();
	if (!NodeStatusObserver::GetNodeStatusObserver().IsLocNodeOK())
	{
		ret = FErr(MOsd_LocNodeIsNotOK);
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		OSD_LOG_ERR("===ReadData=== failed...ret[%s],fileID[%s],buf[%p],len[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),buf,len,offset);
		return ret;
	}
	NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();

	memset(buf,0,len);
	BaseObjIo *dataReadIo = ObjIoFactory::GetObjIoFactory().CreateObj(DataRead);
	dataReadIo->InitObjIo((char *)&obj,(char *)buf,len,offset);
	ObjStream::GetObjStream().PushIo(dataReadIo);
//	len = dataReadIo->GetRetLength();	//client filtered,all ok
	ret = dataReadIo->GetRetStatus();
	dataReadIo->DelObjIo(dataReadIo);
	if (ret.IsErr())
	{
		ret.SetCustomErr(ErrNodeAgencyStop);
		OSD_LOG_ERR("===ReadData=== failed...ret[%s],fileID[%s],buf[%p],len[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),buf,len,offset);
	}
	OSD_LOG_ERR("===ReadData=== done...ret[%s],fileID[%s],buf[%p],len[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),buf,len,offset);
	return ret;
}

FiErr DataIF::TruncateData(ObjInfoHandle &obj, u64 offset, u64 oriSize)
{
	FiErr ret;
	RELEASE_ASSERT(!obj.GetFileID().IsNull());

TruncateDataBegin:
	NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Lock();
	if (!NodeStatusObserver::GetNodeStatusObserver().IsLocNodeOK())
	{
		ret = FErr(MOsd_LocNodeIsNotOK);
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		OSD_LOG_ERR("===TruncateData=== failed...ret[%s],fileID[%s],oriSize[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),oriSize,offset);
		return ret;
	}

	if (OsdCfg::GetOsdCfg().GetLocNodeID() != obj.GetMetaMainNode() && 	\
		!NodeStatusObserver::GetNodeStatusObserver().FindNotOkNodeID(obj.GetMetaMainNode()))
	{
		ret = FErr(MOsd_MainNodeIsOK);
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		OSD_LOG_ERR("===TruncateData=== failed...ret[%s],fileID[%s],oriSize[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),oriSize,offset);
		return ret;
	}

	u64 TruncateGrainSize = (u64)OsdCfg::GetOsdCfg().GetDataSliceSize()*(u64)obj.GetDataNodeNum();
	offset += (TruncateGrainSize - (offset%TruncateGrainSize));

	if (oriSize > offset)
	{
		BaseObjIo *dataTruncateIo = ObjIoFactory::GetObjIoFactory().CreateObj(DataTruncate);
		dataTruncateIo->InitObjIo((char *)&obj,NULL,oriSize,offset);
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();

		ObjStream::GetObjStream().PushIo(dataTruncateIo);
		ret = dataTruncateIo->GetRetStatus();
		dataTruncateIo->DelObjIo(dataTruncateIo);
	}
	else
	{
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
	}

	if (ret.IsErr())
	{
		OSD_LOG_ERR("===TruncateData=== failed...ret[%s],fileID[%s],oriSize[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),oriSize,offset);
	}
	OSD_LOG_ERR("===TruncateData=== done...ret[%s],fileID[%s],oriSize[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),oriSize,offset);
}

FiErr DataIF::DistributeTruncateData(char *ioPar)
{
	FiErr ret;
	BaseObjIo *io = (BaseObjIo *)ioPar;

DistributeTruncateDataBegin:
	NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Lock();
	if (!NodeStatusObserver::GetNodeStatusObserver().IsLocNodeOK())
	{
		ret = FErr(MOsd_LocNodeIsNotOK);
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		OSD_LOG_ERR("===DistributeTruncateData=== failed...ret[%s],fileID[%s],oriSize[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),io->GetFileID().GetIDStr().c_str(),io->GetLength(),io->GetOffset());
		return ret;
	}

	FiEvent evt;
	if(NetWapper::GetNetWapper().ProcessingNetIoAryFind(io->GetFileID(),io->GetLength()-io->GetOffset(),io->GetOffset(),Data,&evt))
	{
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		evt.Wait();
		goto DistributeTruncateDataBegin;
	}

	if (ObjStream::GetObjStream().WriteIoFind(io->GetFileID(),io->GetLength()-io->GetOffset(),io->GetOffset(),Data,&evt))
	{
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		evt.Wait();
		goto DistributeTruncateDataBegin;
	}

	BaseObjIo *dataDistributeTruncateIo = ObjIoFactory::GetObjIoFactory().CreateObj(DataDistributeTruncate);
	dataDistributeTruncateIo->InitObjIo((char *)io);

	FaultHandler::GetFaultHandler().GetOimLock().Lock();
	if(FaultHandler::GetFaultHandler().RecoverIoFind(io->GetFileID(),io->GetLength()-io->GetOffset(),io->GetOffset(),Data,&evt))
	{
		dataDistributeTruncateIo->DelObjIo(dataDistributeTruncateIo);
		FaultHandler::GetFaultHandler().GetOimLock().Unlock();
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		evt.Wait();
		goto DistributeTruncateDataBegin;
	}
	ObjStream::GetObjStream().WriteIoAdd(dataDistributeTruncateIo);
	FaultHandler::GetFaultHandler().GetOimLock().Unlock();

	NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();

	ObjStream::GetObjStream().PushIo(dataDistributeTruncateIo);
	ret = dataDistributeTruncateIo->GetRetStatus();
	
	ObjStream::GetObjStream().WriteIoDel(dataDistributeTruncateIo);
	dataDistributeTruncateIo->DelObjIo(dataDistributeTruncateIo);

	if (ret.IsErr())
	{
		OSD_LOG_ERR("===DistributeTruncateData=== failed...ret[%s],fileID[%s],oriSize[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),io->GetFileID().GetIDStr().c_str(),io->GetLength(),io->GetOffset());
	}
	OSD_LOG_ERR("===DistributeTruncateData=== done...ret[%s],fileID[%s],oriSize[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),io->GetFileID().GetIDStr().c_str(),io->GetLength(),io->GetOffset());

	return ret;
}

FiErr JournalIF::WriteJournal(ObjInfoHandle &obj, void *buf, u32 &len, u64 offset)
{
	FiErr ret;
	FiEvent evt;
	RELEASE_ASSERT(!obj.GetFileID().IsNull());
	if (0 == len)
	{
		OSD_LOG_ERR("===WriteJournal=== failed...(0 == len)");
		return ret;
	}

WriteMetaBegin:
	NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Lock();
	if (!NodeStatusObserver::GetNodeStatusObserver().IsLocNodeOK())
	{
		ret = FErr(MOsd_LocNodeIsNotOK);
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		OSD_LOG_ERR("===WriteJournal=== failed...ret[%s],fileID[%s],buf[%p],len[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),buf,len,offset);
		return ret;
	}

	if (OsdCfg::GetOsdCfg().GetLocNodeID() != obj.GetMetaMainNode() && 	\
		!NodeStatusObserver::GetNodeStatusObserver().FindNotOkNodeID(obj.GetMetaMainNode()))
	{
		ret = FErr(MOsd_MainNodeIsOK);
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		OSD_LOG_ERR("===WriteJournal=== failed...ret[%s],fileID[%s],buf[%p],len[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),buf,len,offset);
		return ret;
	}

	if(NetWapper::GetNetWapper().ProcessingNetIoAryFind(obj.GetFileID(),len,offset,Meta,&evt))
	{
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		evt.Wait();
		goto WriteMetaBegin;
	}

	if (ObjStream::GetObjStream().WriteIoFind(obj.GetFileID(),len,offset,Meta,&evt))
	{
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		evt.Wait();
		goto WriteMetaBegin;
	}

	BaseObjIo *journalWriteIo = ObjIoFactory::GetObjIoFactory().CreateObj(JournalWrite);
	journalWriteIo->InitObjIo((char *)&obj,(char *)buf,len,offset);

	FaultHandler::GetFaultHandler().GetOimLock().Lock();
	if(FaultHandler::GetFaultHandler().RecoverIoFind(obj.GetFileID(),len,offset,Meta,&evt))
	{
		journalWriteIo->DelObjIo(journalWriteIo);
		FaultHandler::GetFaultHandler().GetOimLock().Unlock();
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		evt.Wait();
		goto WriteMetaBegin;
	}
	ObjStream::GetObjStream().WriteIoAdd(journalWriteIo);
	FaultHandler::GetFaultHandler().GetOimLock().Unlock();

	NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();

	ObjStream::GetObjStream().PushIo(journalWriteIo);
	ret = journalWriteIo->GetRetStatus();
	ObjStream::GetObjStream().WriteIoDel(journalWriteIo);
	journalWriteIo->DelObjIo(journalWriteIo);

	if (ret.IsErr())
	{
		ret.SetCustomErr(ErrNodeAgencyStop);
		OSD_LOG_ERR("===WriteJournal=== failed...ret[%s],fileID[%s],buf[%p],len[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),buf,len,offset);
	}
	OSD_LOG_ERR("===WriteJournal=== done...ret[%s],fileID[%s],buf[%p],len[0x%x],offset[0x%llx]",ret.StrFiText().c_str(),obj.GetFileID().GetIDStr().c_str(),buf,len,offset);

	return ret;
}

FiErr JournalIF::ReadJournal(ObjInfoHandle &obj, void *buf, u32 &len, u64 offset)
{
	MetaIF mif;
	return mif.ReadMeta(obj,buf,len,offset);
}

FiErr JournalIF::ScanningReadJournal(ObjInfoHandle &obj, void *buf, u32 &len, u64 offset)
{
	MetaIF mif;
	return mif.ScanningReadMeta(obj,buf,len,offset);
}

FiErr JournalIF::DelJournal(ObjInfoHandle &obj)
{
	MetaIF mif;
	return mif.DelMeta(obj);
}

FiErr JournalIF::AllocJournalObj(ObjInfoHandle &obj)
{
	MetaIF mif;
	return mif.AllocMetalObj(obj,true);
}

FiErr JournalIF::TruncateJournal(ObjInfoHandle &obj, u64 len, u64 oriSize)
{
	RELEASE_ASSERT_FASLE;
	MetaIF mif;
	return mif.TruncateMeta(obj,len,oriSize);
}


FiErr FiOsdFormat::StandardFormat(CNodeServersInfo* pNSsInfo, char* &outBuffer, size_t &bufSize)
{
	NodeSpaceCollecter::InitAllNodeSpace();
	NodeStatusObserver::InitNodesStatus();
	ficsLocIF::GetFicsLocIF().RemoveDir(OsdCfg::GetOsdCfg().GetMetaRootPath(),false);
	ficsLocIF::GetFicsLocIF().RemoveDir(OsdCfg::GetOsdCfg().GetDataRootPath(),false);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//example:
//void test()
//{
//	JournalIF jif;
//	ObjID fid;
//	unsigned int len = 0;
//	ObjInfoHandle oih(fid,(char*)0,0);
//	jif.WriteJournal(oih,0,len,0);
//}



