///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  hunter code ([hunter.f0x] remeber my brothers) email:hunterlhy@yahoo.cn /////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "objIo.h"
#include "fiOsd.h"

void InitObjIoFactory()
{
	ObjIoFactory &oif = ObjIoFactory::GetObjIoFactory();

	oif.Register(JournalWrite,(ObjIoCreateFuc)(&JournalWriteObjIo::CreateObjIo));
	
	oif.Register(MetaRead,(ObjIoCreateFuc)(&MetaReadObjIo::CreateObjIo));
	oif.Register(MetaWrite,(ObjIoCreateFuc)(&MetaWriteObjIo::CreateObjIo));
	oif.Register(MetaTruncate,(ObjIoCreateFuc)(&MetaTruncateObjIo::CreateObjIo));
	oif.Register(MetaDel,(ObjIoCreateFuc)(&MetaDelObjIo::CreateObjIo));

//  oif.Register(JournalRead,(ObjIoCreateFuc)(&JournalReadObjIo::CreateObjIo));
//  oif.Register(JournalWrite,(ObjIoCreateFuc)(&JournalReadObjIo::CreateObjIo));
//  oif.Register(JournalDel,(ObjIoCreateFuc)(&JournalReadObjIo::CreateObjIo));
//  oif.Register(JournalTruncate,(ObjIoCreateFuc)(&JournalReadObjIo::CreateObjIo));

	oif.Register(DataRead,(ObjIoCreateFuc)(&DataReadObjIo::CreateObjIo));
	oif.Register(DataWrite,(ObjIoCreateFuc)(&DataWriteObjIo::CreateObjIo));
	oif.Register(DataTruncate,(ObjIoCreateFuc)(&DataTruncateObjIo::CreateObjIo));
	oif.Register(DataDistributeTruncate,(ObjIoCreateFuc)(&DataDistributeTruncateObjIo::CreateObjIo));
//	oif.Register(DataDel,(ObjIoCreateFuc)(&DataDelObjIo::CreateObjIo));

	oif.Register(NetRead,(ObjIoCreateFuc)(&NetReadObjIo::CreateObjIo));
	oif.Register(NetWrite,(ObjIoCreateFuc)(&NetWriteObjIo::CreateObjIo));
	oif.Register(NetTruncate,(ObjIoCreateFuc)(&NetTruncateObjIo::CreateObjIo));
	oif.Register(NetDel,(ObjIoCreateFuc)(&NetDelObjIo::CreateObjIo));

	oif.Register(RedirectRead,(ObjIoCreateFuc)(&RedirectReadObjIo::CreateObjIo));
	oif.Register(RedirectWrite,(ObjIoCreateFuc)(&RedirectWriteObjIo::CreateObjIo));
	oif.Register(RedirectTruncate,(ObjIoCreateFuc)(&RedirectTruncateObjIo::CreateObjIo));
	oif.Register(RedirectDel,(ObjIoCreateFuc)(&RedirectDelObjIo::CreateObjIo));

	oif.Register(RecoverWrite,(ObjIoCreateFuc)(&RecoverWriteObjIo::CreateObjIo));

	oif.Register(ConsistencyRead,(ObjIoCreateFuc)(&ConsistencyReadObjIo::CreateObjIo));
	oif.Register(NetConsistencyRead,(ObjIoCreateFuc)(&NetConsistencyReadObjIo::CreateObjIo));
	oif.Register(ConsistencyTruncate,(ObjIoCreateFuc)(&ConsistencyTruncateObjIo::CreateObjIo));
}

void objmemcpy(void *decPar,void *srcPar,s32 bufLen)
{
	RELEASE_ASSERT(sizeof(BaseObjIo) == bufLen);
	BaseObjIo *dec = (BaseObjIo *)decPar;
	BaseObjIo *src = (BaseObjIo *)srcPar;

	dec->version = src->version;
	dec->fileID = src->fileID;
	dec->dataType = src->dataType;
	dec->srcOt = src->srcOt;
	dec->ot = src->ot;
	dec->ret = src->ret;
	dec->nodeNum = src->nodeNum;
	memcpy(dec->nodeInfo,src->nodeInfo,sizeof(s32)*DATA_NODE_MAX_NUM);
	dec->srcNode = src->srcNode;
	dec->dstNode = src->dstNode;
	dec->buf = src->buf;
	dec->length = src->length;
	dec->offset = src->offset;
	dec->retLength = src->retLength;
	dec->objIoNum = src->objIoNum;
	memcpy(dec->objIoAry,src->objIoAry,sizeof(IoPiece)*2);
}

void MergTruncateRecoverOp(IoFileLog *ioRecoverLog,BaseObjIo *io,u32 sliceSize)
{
	vector<IoFileLogElement> ifleAry;
	ioRecoverLog->GetFileLogAryWithFileID(ifleAry,io->GetFileID(),true);
	if (ifleAry.size() < 2)
	{
		IoFileLogElement e = *ifleAry.begin();
		for (vector<IoFileLogElement>::iterator itTp = ifleAry.begin() + 1;itTp != ifleAry.end();++itTp)
		{
			if ((itTp->order*sliceSize + itTp->offset) <= (e.order + e.offset))
			{
				OSD_LOG_ERR("===MergTruncateRecoverOp=== DelFileLogElement fileID[%s],order[%d]",e.fileID.GetIDStr().c_str(),e.order);
				ioRecoverLog->DelFileLogElement(e);
				e = *itTp;
			}
		}
	}
}

bool FindTruncateRecoverOp(IoFileLog *ioRecoverLog,BaseObjIo *io,u32 sliceSize)
{
	bool ret = false;
	vector<IoFileLogElement> ifleAry;
	ioRecoverLog->GetFileLogAryWithFileID(ifleAry,io->GetFileID(),true);
	if (!ifleAry.empty())
	{
		for (vector<IoFileLogElement>::iterator itTp = ifleAry.begin();itTp != ifleAry.end();++itTp)
		{
			if ((io->GetOffset()+io->GetLength())>(itTp->order*sliceSize+itTp->offset))
			{
				ret = true;
				break;
			}
		}
	}

	return ret;
}

const char* GetObjIoTypeStr(u32 num)
{
	static const char *ioTypeStrAry[] =
	{
	"MetaRead",
	"MetaWrite",
	"MetaDel",
	"MetaTruncate",

	"JournalWrite",

	"DataRead",
	"DataWrite",
	"DataDel",
	"DataTruncate",
	"DataDistributeTruncate",

	"NetRead",
	"NetWrite",
	"NetDel",
	"NetTruncate",

	"RedirectRead",
	"RedirectWrite",
	"RedirectDel",
	"RedirectTruncate",

	"RecoverWrite",
	"RecoverTruncate",

	"ConsistencyRead",
	"NetConsistencyRead",
	"ConsistencyTruncate"
	};
	RELEASE_ASSERT(ConsistencyTruncate == sizeof(ioTypeStrAry)/sizeof(const char*)-1);
	return ioTypeStrAry[num];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  JournalWriteObjIo  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void JournalWriteObjIo::InitObjIo(char *headBuf, char *bufPar, u64 lenPar, u64 offsetPar)
{	
	ObjInfoHandle *oih = (ObjInfoHandle *)headBuf;
	dataType = Meta;
	version = OsdCfg::GetOsdCfg().GetVersion();
	fileID = oih->GetFileID();
	srcOt = MetaWrite;
	ot = MetaWrite;
	srcNode = OsdCfg::GetOsdCfg().GetLocNodeID();
	dstNode = -1;
	buf = bufPar;
	length = lenPar;
	offset = offsetPar;

	RELEASE_ASSERT(length <= OsdCfg::GetOsdCfg().GetMetaSliceSize());
	RELEASE_ASSERT(srcNode == oih->GetMetaMainNode()||srcNode == oih->GetMetaBakNode());

	rp = FiRAID1;
	dstNode = (srcNode == oih->GetMetaMainNode())?oih->GetMetaBakNode():oih->GetMetaMainNode();
	nodeNum = 2;
	nodeInfo[0] = oih->GetMetaMainNode();
	nodeInfo[1] = oih->GetMetaBakNode();

	IoDivide iod = IoDivide(OsdCfg::GetOsdCfg().GetMetaSliceSize(),buf,length,offset);
	vector<IoPiece> &ioPieces = iod.GetIoPieces();
	RELEASE_ASSERT(ioPieces.size() <= 2);

	objIoNum = ioPieces.size();
	objIoAry[0] = ioPieces[0];

	if (objIoNum == 2)
	{
		objIoAry[1] = ioPieces[1];
	}
}

void JournalWriteObjIo::ObjStreamProcessObjIo()
{
	FiEvent evt;
	FiErr locRet;
	FiErr netRet;
	static IoFileLogElement e[2];

	IoFileLog *jcl = LocWapper::GetLocWapper().GetJournalIoConsistencyLog();
	locRet = jcl->AddFileLogElement(GetFileID(),GetObjIoAry()[0].order,GetDstNode(),WRITETYPE,GetMainNode(),GetBakNode());
	if (locRet.IsErr())
	{
		SetRetStatus(locRet);
		return;
	}

	e[0].fileID = GetFileID();
	e[0].order = GetObjIoAry()[0].order;
	e[0].offset = WRITETYPE;
	e[0].nodeID =  GetDstNode();

	OSD_LOG_ERR("===JournalWriteObjIo::ObjStreamProcessObjIo=== AddFileLogElement JournalIoConsistencyLog fileID[%s],order[%d]",e[0].fileID.GetIDStr().c_str(),e[0].order);

	if (GetObjIoNum() == 2)
	{
		locRet = jcl->AddFileLogElement(GetFileID(),GetObjIoAry()[1].order,GetDstNode(),WRITETYPE,GetMainNode(),GetBakNode());
		if (locRet.IsErr())
		{
			OSD_LOG_ERR("===JournalWriteObjIo::ObjStreamProcessObjIo=== DelFileLogElement JournalIoConsistencyLog fileID[%s],order[%d]",e[0].fileID.GetIDStr().c_str(),e[0].order);
			jcl->DelFileLogElement(e[0]);
			SetRetStatus(locRet);
			return;
		}

		e[1].fileID = GetFileID();
		e[1].order = GetObjIoAry()[1].order;
		e[1].offset = WRITETYPE;
		e[1].nodeID = GetDstNode();

		OSD_LOG_ERR("===JournalWriteObjIo::ObjStreamProcessObjIo=== AddFileLogElement JournalIoConsistencyLog fileID[%s],order[%d]",e[1].fileID.GetIDStr().c_str(),e[1].order);
	}

	NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Lock();
	if (FindTruncateRecoverOp(LocWapper::GetLocWapper().GetMetaIoRecoverLog(),this,OsdCfg::GetOsdCfg().GetMetaSliceSize()))
	{
		netRet = LogForRecover();
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		SetRetStatus(netRet);
		if (netRet.IsSuccess())
		{
			locRet = LocWapper::GetLocWapper().Write(this);
			SetRetStatus(locRet);
		}
	}
	else
	{
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		NetWapper::GetNetWapper().PushNetIo(this, evt);
		locRet = LocWapper::GetLocWapper().Write(this);
		evt.Wait();
		netRet = GetRetStatus();	//netRet的含义包括：网络返回失败后记录恢复日志是否成功。
		if (netRet.IsSuccess())
		{
			SetRetStatus(locRet);
		}
	}

	if (GetRetStatus().IsSuccess())
	{
		OSD_LOG_ERR("===JournalWriteObjIo::ObjStreamProcessObjIo=== DelFileLogElement JournalIoConsistencyLog fileID[%s],order[%d]",e[0].fileID.GetIDStr().c_str(),e[0].order);
		jcl->DelFileLogElement(e[0]);
		if (GetObjIoNum() == 2)
		{
			OSD_LOG_ERR("===JournalWriteObjIo::ObjStreamProcessObjIo=== DelFileLogElement JournalIoConsistencyLog fileID[%s],order[%d]",e[1].fileID.GetIDStr().c_str(),e[1].order);
			jcl->DelFileLogElement(e[1]);
		}
	}
}

void JournalWriteObjIo::ProcessNetReplyBuf(char* bufPar)
{
	BaseObjIo *netIo = (BaseObjIo *)bufPar;
	ret = netIo->GetRetStatus();
}

FiErr JournalWriteObjIo::LogForRecover()
{
	FiErr ret;
	IoFileLog *ioRecoverLog = LocWapper::GetLocWapper().GetMetaIoRecoverLog();
	for (u32 i = 0;i<GetObjIoNum();++i)
	{
		ret = ioRecoverLog->AddFileLogElement(fileID,objIoAry[i].order,dstNode,WRITETYPE,GetMainNode(),GetBakNode());
		if (ret.IsErr())
		{
			return ret;
		}
		OSD_LOG_ERR("===JournalWriteObjIo::LogForRecover=== AddFileLogElement fileID[%s],order[%d]",fileID.GetIDStr().c_str(),GetObjIoAry()[i].order);
	}
	return ret;
}

string JournalWriteObjIo::GetStorPath(u32 no)
{
	char idStr[25] = {0};
	s32 locNodeID = OsdCfg::GetOsdCfg().GetLocNodeID();
	if (GetMainNode() == locNodeID)
	{
		sprintf(idStr,"/rddcNode_%d",GetBakNode());
	}
	else if(GetBakNode() == locNodeID)
	{
		sprintf(idStr,"/rddcNode_%d",GetMainNode());
	}
	else
	{
		if (GetSrcNode() == GetMainNode())
		{
			sprintf(idStr,"/rddcNode_%d",GetBakNode());
		}
		else if(GetSrcNode() == GetBakNode())
		{
			sprintf(idStr,"/rddcNode_%d",GetMainNode());
		}
		else
		{
			RELEASE_ASSERT_FASLE;
		}
	}

	char noStr[25] = {0};
	if (IsMetaType())
	{
		sprintf(noStr,"/meta%d",no);
		RELEASE_ASSERT(no < OsdCfg::GetOsdCfg().GetMetaMaxFileNo());
	}
	else
	{
		sprintf(noStr,"/%d_data%d",GetMainNode(),no);
		RELEASE_ASSERT(no < OsdCfg::GetOsdCfg().GetDataMaxFileNo());
	}
	return GetStorDirPath(IsMetaType(),fileID)+idStr+noStr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  MetaReadObjIo  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MetaReadObjIo::InitObjIo(char *headBuf, char *bufPar, u64 lenPar, u64 offsetPar)
{
	ObjInfoHandle *oih = (ObjInfoHandle *)headBuf;
	dataType = Meta;
	version = OsdCfg::GetOsdCfg().GetVersion();
	fileID = oih->GetFileID();
	srcOt = MetaRead;
	ot = MetaRead;
	srcNode = OsdCfg::GetOsdCfg().GetLocNodeID();
	dstNode = -1;
	buf = bufPar;
	length = lenPar;
	offset = offsetPar;

	retLength = 0;

	RELEASE_ASSERT(length <= OsdCfg::GetOsdCfg().GetMetaSliceSize());
	RELEASE_ASSERT(srcNode == oih->GetMetaMainNode()||srcNode == oih->GetMetaBakNode());

	rp = FiRAID1;
	dstNode = (srcNode == oih->GetMetaMainNode())?srcNode == oih->GetMetaBakNode():srcNode == oih->GetMetaMainNode();
	nodeNum = 2;
	nodeInfo[0] = oih->GetMetaMainNode();
	nodeInfo[1] = oih->GetMetaBakNode();

	IoDivide iod = IoDivide(OsdCfg::GetOsdCfg().GetMetaSliceSize(),buf,length,offset);
	vector<IoPiece> &ioPieces = iod.GetIoPieces();
	RELEASE_ASSERT(ioPieces.size() <= 2);

	objIoNum = ioPieces.size();
	objIoAry[0] = ioPieces[0];

	if (objIoNum == 2)
	{
		objIoAry[1] = ioPieces[1];
	}
}

void MetaReadObjIo::ObjStreamProcessObjIo()
{
	SetRetStatus(LocWapper::GetLocWapper().Read(this));
}

void MetaReadObjIo::ProcessNetReplyBuf(char* bufPar)
{
	RELEASE_ASSERT_FASLE;
//  BaseObjIo *netIo = (BaseObjIo *)bufPar;
//  ret = netIo->GetRetStatus();
//  if (netIo->GetRetLength() > 0)
//  {
// 		AddRetLength(netIo->GetRetLength());
//  	objmemcpy(buf,bufPar+sizeof(BaseObjIo),retLength);
//  }
}

string MetaReadObjIo::GetStorPath(u32 no)
{
	char idStr[25] = {0};
	s32 locNodeID = OsdCfg::GetOsdCfg().GetLocNodeID();
	if (GetMainNode() == locNodeID)
	{
		sprintf(idStr,"/rddcNode_%d",GetBakNode());
	}
	else if(GetBakNode() == locNodeID)
	{
		sprintf(idStr,"/rddcNode_%d",GetMainNode());
	}
	else
	{
		if (GetSrcNode() == GetMainNode())
		{
			sprintf(idStr,"/rddcNode_%d",GetBakNode());
		}
		else if(GetSrcNode() == GetBakNode())
		{
			sprintf(idStr,"/rddcNode_%d",GetMainNode());
		}
		else
		{
			RELEASE_ASSERT_FASLE;
		}
	}

	char noStr[25] = {0};
	if (IsMetaType())
	{
		sprintf(noStr,"/meta%d",no);
		RELEASE_ASSERT(no < OsdCfg::GetOsdCfg().GetMetaMaxFileNo());
	}
	else
	{
		sprintf(noStr,"/%d_data%d",GetMainNode(),no);
		RELEASE_ASSERT(no < OsdCfg::GetOsdCfg().GetDataMaxFileNo());
	}
	return GetStorDirPath(IsMetaType(),fileID)+idStr+noStr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  MetaWriteObjIo  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MetaWriteObjIo::InitObjIo(char *headBuf, char *bufPar, u64 lenPar, u64 offsetPar)
{	
	ObjInfoHandle *oih = (ObjInfoHandle *)headBuf;
	dataType = Meta;
	version = OsdCfg::GetOsdCfg().GetVersion();
	fileID = oih->GetFileID();
	srcOt = MetaWrite;
	ot = MetaWrite;
	srcNode = OsdCfg::GetOsdCfg().GetLocNodeID();
	dstNode = -1;
	buf = bufPar;
	length = lenPar;
	offset = offsetPar;

	RELEASE_ASSERT(length <= OsdCfg::GetOsdCfg().GetMetaSliceSize());
	RELEASE_ASSERT(srcNode == oih->GetMetaMainNode()||srcNode == oih->GetMetaBakNode());

	rp = FiRAID1;
	dstNode = (srcNode == oih->GetMetaMainNode())?oih->GetMetaBakNode():oih->GetMetaMainNode();
	nodeNum = 2;
	nodeInfo[0] = oih->GetMetaMainNode();
	nodeInfo[1] = oih->GetMetaBakNode();

	IoDivide iod = IoDivide(OsdCfg::GetOsdCfg().GetMetaSliceSize(),buf,length,offset);
	vector<IoPiece> &ioPieces = iod.GetIoPieces();
	RELEASE_ASSERT(ioPieces.size() <= 2);

	objIoNum = ioPieces.size();
	objIoAry[0] = ioPieces[0];

	if (objIoNum == 2)
	{
		objIoAry[1] = ioPieces[1];
	}
}

void MetaWriteObjIo::ObjStreamProcessObjIo()
{
	FiEvent evt;
	FiErr locRet;
	FiErr netRet;

	NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Lock();
	if (FindTruncateRecoverOp(LocWapper::GetLocWapper().GetMetaIoRecoverLog(),this,OsdCfg::GetOsdCfg().GetMetaSliceSize()))
	{
		netRet = LogForRecover();
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		SetRetStatus(netRet);
		if (netRet.IsSuccess())
		{
			locRet = LocWapper::GetLocWapper().Write(this);
			SetRetStatus(locRet);
		}
	}
	else
	{
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		NetWapper::GetNetWapper().PushNetIo(this, evt);
		locRet = LocWapper::GetLocWapper().Write(this);
		evt.Wait();
		netRet = GetRetStatus();	//netRet的含义包括：网络返回失败后记录恢复日志是否成功。
		if (netRet.IsSuccess())
		{
			SetRetStatus(locRet);
		}
	}
}

void MetaWriteObjIo::ProcessNetReplyBuf(char* bufPar)
{
	BaseObjIo *netIo = (BaseObjIo *)bufPar;
	ret = netIo->GetRetStatus();
}

FiErr MetaWriteObjIo::LogForRecover()
{
	FiErr ret;
	IoFileLog *ioRecoverLog = LocWapper::GetLocWapper().GetMetaIoRecoverLog();
	for (u32 i = 0;i<GetObjIoNum();++i)
	{
		ret = ioRecoverLog->AddFileLogElement(fileID,objIoAry[i].order,dstNode,WRITETYPE,GetMainNode(),GetBakNode());
		if (ret.IsErr())
		{
			return ret;
		}
		OSD_LOG_ERR("===MetaWriteObjIo::LogForRecover=== AddFileLogElement fileID[%s],order[%d]",fileID.GetIDStr().c_str(),GetObjIoAry()[i].order);
	}
	return ret;
}

string MetaWriteObjIo::GetStorPath(u32 no)
{
	char idStr[25] = {0};
	s32 locNodeID = OsdCfg::GetOsdCfg().GetLocNodeID();
	if (GetMainNode() == locNodeID)
	{
		sprintf(idStr,"/rddcNode_%d",GetBakNode());
	}
	else if(GetBakNode() == locNodeID)
	{
		sprintf(idStr,"/rddcNode_%d",GetMainNode());
	}
	else
	{
		if (GetSrcNode() == GetMainNode())
		{
			sprintf(idStr,"/rddcNode_%d",GetBakNode());
		}
		else if(GetSrcNode() == GetBakNode())
		{
			sprintf(idStr,"/rddcNode_%d",GetMainNode());
		}
		else
		{
			RELEASE_ASSERT_FASLE;
		}
	}

	char noStr[25] = {0};
	if (IsMetaType())
	{
		sprintf(noStr,"/meta%d",no);
		RELEASE_ASSERT(no < OsdCfg::GetOsdCfg().GetMetaMaxFileNo());
	}
	else
	{
		sprintf(noStr,"/%d_data%d",GetMainNode(),no);
		RELEASE_ASSERT(no < OsdCfg::GetOsdCfg().GetDataMaxFileNo());
	}
	return GetStorDirPath(IsMetaType(),fileID)+idStr+noStr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  MetaTruncateObjIo  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MetaTruncateObjIo::InitObjIo(char *headBuf, char *bufPar, u64 lenPar, u64 offsetPar)
{
	ObjInfoHandle *oih = (ObjInfoHandle *)headBuf;
	dataType = Meta;
	version = OsdCfg::GetOsdCfg().GetVersion();
	fileID = oih->GetFileID();
	srcOt = MetaTruncate;
	ot = MetaTruncate;
	srcNode = OsdCfg::GetOsdCfg().GetLocNodeID();
	dstNode = -1;
	buf = NULL;
	length = lenPar;	//oriSize
	offset = offsetPar;

//	RELEASE_ASSERT(srcNode == oih->GetMetaMainNode()||srcNode == oih->GetMetaBakNode());

	rp = FiRAID1;
	dstNode = (srcNode == oih->GetMetaMainNode())?oih->GetMetaBakNode():oih->GetMetaMainNode();
	nodeNum = 2;
	nodeInfo[0] = oih->GetMetaMainNode();
	nodeInfo[1] = oih->GetMetaBakNode();

	objIoNum = 0;
}

void MetaTruncateObjIo::ObjStreamProcessObjIo()
{
	FiEvent evt;
	FiErr locRet;

	NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Lock();
	if (FindTruncateRecoverOp(LocWapper::GetLocWapper().GetMetaIoRecoverLog(),this,OsdCfg::GetOsdCfg().GetMetaSliceSize()))
	{
		locRet = LogForRecover();
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		SetRetStatus(locRet);
		if (locRet.IsSuccess())
		{
			locRet = LocWapper::GetLocWapper().Truncate(this);
			SetRetStatus(locRet);
		}
	}
	else
	{
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		locRet = LocWapper::GetLocWapper().Truncate(this);
		SetRetStatus(locRet);
		if (locRet.IsSuccess())
		{
			NetWapper::GetNetWapper().PushNetIo(this, evt);
			evt.Wait();
		}
	}
}

void MetaTruncateObjIo::ProcessNetReplyBuf(char* bufPar)
{
	BaseObjIo *netIo = (BaseObjIo *)bufPar;
	ret = netIo->GetRetStatus();
}

FiErr MetaTruncateObjIo::LogForRecover()
{
	FiErr ret;
	IoFileLog *ioRecoverLog = LocWapper::GetLocWapper().GetMetaIoRecoverLog();

	u32 fileSliceSize = OsdCfg::GetOsdCfg().GetMetaSliceSize();
	u32 truncateNo = (u32)(offset/(u64)fileSliceSize);
	u32 truncateSize = (u32)(offset%(u64)fileSliceSize);
	ret = ioRecoverLog->AddFileLogElement(fileID,truncateNo,dstNode,truncateSize);
	if (ret.IsSuccess())
	{
		OSD_LOG_ERR("===MetaTruncateObjIo::LogForRecover=== AddFileLogElement fileID[%s],order[%d],offset[%lld]",fileID.GetIDStr().c_str(),truncateNo,offset);
		MergTruncateRecoverOp(ioRecoverLog,this,fileSliceSize);
	}
	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  MetaDelObjIo  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MetaDelObjIo::InitObjIo(char *headBuf, char *bufPar, u64 lenPar, u64 offsetPar)
{	
	ObjInfoHandle *oih = (ObjInfoHandle *)headBuf;
	dataType = Meta;
	version = OsdCfg::GetOsdCfg().GetVersion();
	fileID = oih->GetFileID();
	srcOt = MetaDel;
	ot = MetaDel;
	srcNode = OsdCfg::GetOsdCfg().GetLocNodeID();
	dstNode = -1;
	buf = NULL;
	length = 0;
	offset = 0;

	RELEASE_ASSERT(srcNode == oih->GetMetaMainNode()||srcNode == oih->GetMetaBakNode());

	rp = FiRAID1;
	dstNode = (srcNode == oih->GetMetaMainNode())?oih->GetMetaBakNode():oih->GetMetaMainNode();
	nodeNum = OsdCfg::GetOsdCfg().GetDataNodeHashNum();
	RELEASE_ASSERT(nodeNum < DATA_NODE_MAX_NUM - 1);
	nodeInfo[0] = oih->GetMetaMainNode();
	nodeInfo[1] = oih->GetMetaBakNode();
	memcpy(&nodeInfo[2],oih->GetDataNodeAry(),sizeof(s32)*nodeNum);

	objIoNum = 0;
}

void MetaDelObjIo::ObjStreamProcessObjIo()
{
	FiEvent evt;
	FiErr locRet;
	FiErr netRet;
	static IoFileLogElement e;
	
	IoFileLog *jcl = LocWapper::GetLocWapper().GetJournalIoConsistencyLog();
	locRet = jcl->AddFileLogElement(GetFileID(),-1,GetDstNode(),DELTYPE,GetMainNode(),GetBakNode());
	if (locRet.IsErr())
	{
		SetRetStatus(locRet);
		return;
	}

	NetWapper::GetNetWapper().PushNetIo(this, evt);
	IoCmdLogElement cmd(io->GetFileID(), nodeInfo, nodeNum);
	locRet = cmdLog->AddCmdLogElement(cmd);
	evt.Wait();
	netRet = GetRetStatus();	//netRet的含义包括：网络返回失败后记录恢复日志是否成功。
	if (netRet.IsSuccess())
	{
		SetRetStatus(locRet);
	}

	if (GetRetStatus().IsSuccess())
	{
		OSD_LOG_ERR("===MetaDelObjIo::ObjStreamProcessObjIo=== DelFileLogElement JournalIoConsistencyLog fileID[%s],order[-1]", e.fileID.GetIDStr().c_str());
		jcl->DelFileLogElement(e);
	}
}

//void MetaDelObjIo::ProcessNetReplyBuf(char* bufPar)
//{
//	RELEASE_ASSERT_FASLE;
//}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  DataReadObjIo  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DataReadObjIo::InitObjIo(char *headBuf, char *bufPar, u64 lenPar, u64 offsetPar)
{
	ObjInfoHandle *oih = (ObjInfoHandle *)headBuf;
	dataType = Data;
	version = OsdCfg::GetOsdCfg().GetVersion();
	fileID = oih->GetFileID();
	srcOt = DataRead;
	ot = DataRead;
	srcNode = OsdCfg::GetOsdCfg().GetLocNodeID();
	dstNode = -1;
	buf = bufPar;
	length = lenPar;
	offset = offsetPar;

	retLength = 0;

	u32 dss = OsdCfg::GetOsdCfg().GetDataSliceSize();
	RELEASE_ASSERT(length <= dss);
	RELEASE_ASSERT((offset % dss) + length <= dss);

	RELEASE_ASSERT(srcNode ==  oih->GetRev1() || srcNode ==  oih->GetRev2());

	rp = FiRAID1;
	dstNode = (srcNode == oih->GetRev1()) ? oih->GetRev2() : oih->GetRev1(); 
	nodeNum = 2;
	nodeInfo[0] = oih->GetRev1();
	nodeInfo[1] = oih->GetRev2();

	IoDivide iod = IoDivide(OsdCfg::GetOsdCfg().GetDataSliceSize(),buf,length,offset);
	vector<IoPiece> &ioPieces = iod.GetIoPieces();
	RELEASE_ASSERT(ioPieces.size() <= 1);

	objIoNum = 1;
	objIoAry[0] = ioPieces[0];
}

void DataReadObjIo::ObjStreamProcessObjIo()
{
	SetRetStatus(LocWapper::GetLocWapper().Read(this));
}

void DataReadObjIo::ProcessNetReplyBuf(char* bufPar)
{
	RELEASE_ASSERT_FASLE;
}

string DataReadObjIo::GetStorPath(u32 no)
{
	char idStr[25] = {0};
	s32 locNodeID = OsdCfg::GetOsdCfg().GetLocNodeID();
	if (GetMainNode() == locNodeID)
	{
		sprintf(idStr,"/rddcNode_%d",GetBakNode());
	}
	else if(GetBakNode() == locNodeID)
	{
		sprintf(idStr,"/rddcNode_%d",GetMainNode());
	}
	else
	{
		if (GetSrcNode() == GetMainNode())
		{
			sprintf(idStr,"/rddcNode_%d",GetBakNode());
		}
		else if(GetSrcNode() == GetBakNode())
		{
			sprintf(idStr,"/rddcNode_%d",GetMainNode());
		}
		else
		{
			RELEASE_ASSERT_FASLE;
		}
	}

	char noStr[25] = {0};
	if (IsMetaType())
	{
		sprintf(noStr,"/meta%d",no);
		RELEASE_ASSERT(no < OsdCfg::GetOsdCfg().GetMetaMaxFileNo());
	}
	else
	{
		sprintf(noStr,"/%d_data%d",GetMainNode(),no);
		RELEASE_ASSERT(no < OsdCfg::GetOsdCfg().GetDataMaxFileNo());
	}
	return GetStorDirPath(IsMetaType(),fileID)+idStr+noStr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  DataWriteObjIo  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DataWriteObjIo::InitObjIo(char *headBuf, char *bufPar, u64 lenPar, u64 offsetPar)
{	
	ObjInfoHandle *oih = (ObjInfoHandle *)headBuf;
	dataType = Data;
	version = OsdCfg::GetOsdCfg().GetVersion();
	fileID = oih->GetFileID();
	srcOt = DataWrite;
	ot = DataWrite;
	srcNode = OsdCfg::GetOsdCfg().GetLocNodeID();
	dstNode = -1;
	buf = bufPar;
	length = lenPar;
	offset = offsetPar;

	RELEASE_ASSERT(srcNode ==  oih->GetRev1() || srcNode ==  oih->GetRev2());

	rp = FiRAID1;
	dstNode = (srcNode == oih->GetRev1()) ? oih->GetRev2() : oih->GetRev1(); 
	nodeNum = 2;
	nodeInfo[0] = oih->GetRev1();
	nodeInfo[1] = oih->GetRev2();

	IoDivide iod = IoDivide(OsdCfg::GetOsdCfg().GetDataSliceSize(),buf,length,offset);
	vector<IoPiece> &ioPieces = iod.GetIoPieces();
	RELEASE_ASSERT(ioPieces.size() <= 1);

	objIoNum = 1;
	objIoAry[0] = ioPieces[0];
}

void DataWriteObjIo::ObjStreamProcessObjIo()
{
	FiEvent evt;
	FiErr locRet;
	FiErr netRet;
	static IoFileLogElement e;

	IoFileLog *dcl = LocWapper::GetLocWapper().GetDataIoConsistencyLog();
	locRet = dcl->AddFileLogElement(GetFileID(),GetObjIoAry()[0].order,GetDstNode(),WRITETYPE,GetMainNode(),GetBakNode());
	if (locRet.IsErr())
	{
		SetRetStatus(locRet);
		return;
	}
	OSD_LOG_ERR("===DataWriteObjIo::ObjStreamProcessObjIo=== AddFileLogElement fileID[%s],order[%d]",GetFileID().GetIDStr().c_str(),GetObjIoAry()[0].order);

	e.fileID = GetFileID();
	e.order = GetObjIoAry()[0].order;
	e.offset = WRITETYPE;
	e.nodeID = GetDstNode();

	NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Lock();
	if (FindTruncateRecoverOp(LocWapper::GetLocWapper().GetDataIoRecoverLog(),this,OsdCfg::GetOsdCfg().GetDataSliceSize()))
	{
		netRet = LogForRecover();
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		SetRetStatus(netRet);
		if (netRet.IsSuccess())
		{
			locRet = LocWapper::GetLocWapper().Write(this);
			SetRetStatus(locRet);
		}
	}
	else
	{
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		NetWapper::GetNetWapper().PushNetIo(this, evt);
		locRet = LocWapper::GetLocWapper().Write(this);
		evt.Wait();
		netRet = GetRetStatus();	//netRet的含义包括：网络返回失败后记录恢复日志是否成功。
		if (netRet.IsSuccess())
		{
			SetRetStatus(locRet);
		}
	}

	if (GetRetStatus().IsSuccess())
	{
		OSD_LOG_ERR("===DataWriteObjIo::ObjStreamProcessObjIo=== DelFileLogElement DataIoConsistencyLog fileID[%s],order[%d]",e.fileID.GetIDStr().c_str(),e.order);
		dcl->DelFileLogElement(e);
	}
}

void DataWriteObjIo::ProcessNetReplyBuf(char* bufPar)
{
	BaseObjIo *netIo = (BaseObjIo *)bufPar;
	ret = netIo->GetRetStatus();
}

FiErr DataWriteObjIo::LogForRecover()
{
	FiErr ret;
	IoFileLog *ioRecoverLog = LocWapper::GetLocWapper().GetDataIoRecoverLog();
	for (u32 i = 0;i<GetObjIoNum();++i)
	{
		ret = ioRecoverLog->AddFileLogElement(fileID,objIoAry[i].order,dstNode,WRITETYPE,GetMainNode(),GetBakNode());
		if (ret.IsErr())
		{
			return ret;
		}
		OSD_LOG_ERR("===DataWriteObjIo::LogForRecover=== AddFileLogElement fileID[%s],order[%d]",fileID.GetIDStr().c_str(),GetObjIoAry()[i].order);
	}
	return ret;
}

string DataWriteObjIo::GetStorPath(u32 no)
{
	char idStr[25] = {0};
	s32 locNodeID = OsdCfg::GetOsdCfg().GetLocNodeID();
	if (GetMainNode() == locNodeID)
	{
		sprintf(idStr,"/rddcNode_%d",GetBakNode());
	}
	else if(GetBakNode() == locNodeID)
	{
		sprintf(idStr,"/rddcNode_%d",GetMainNode());
	}
	else
	{
		if (GetSrcNode() == GetMainNode())
		{
			sprintf(idStr,"/rddcNode_%d",GetBakNode());
		}
		else if(GetSrcNode() == GetBakNode())
		{
			sprintf(idStr,"/rddcNode_%d",GetMainNode());
		}
		else
		{
			RELEASE_ASSERT_FASLE;
		}
	}

	char noStr[25] = {0};
	if (IsMetaType())
	{
		sprintf(noStr,"/meta%d",no);
		RELEASE_ASSERT(no < OsdCfg::GetOsdCfg().GetMetaMaxFileNo());
	}
	else
	{
		sprintf(noStr,"/%d_data%d",GetMainNode(),no);
		RELEASE_ASSERT(no < OsdCfg::GetOsdCfg().GetDataMaxFileNo());
	}
	return GetStorDirPath(IsMetaType(),fileID)+idStr+noStr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  DataTruncateObjIo  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DataTruncateObjIo::InitObjIo(char *headBuf, char *bufPar, u64 lenPar, u64 offsetPar)
{	
	ObjInfoHandle *oih = (ObjInfoHandle *)headBuf;
	dataType = Data;
	version = OsdCfg::GetOsdCfg().GetVersion();
	fileID = oih->GetFileID();
	srcOt = DataTruncate;
	ot = DataTruncate;
	srcNode = OsdCfg::GetOsdCfg().GetLocNodeID();
	dstNode = -1;
	buf = NULL;
	length = lenPar;	//oriSize
	offset = offsetPar;

//	RELEASE_ASSERT(srcNode == oih->GetMetaMainNode()||srcNode == oih->GetMetaBakNode());

	rp = FiRAID1;
//	dstNode = (srcNode == oih->GetMetaMainNode())?oih->GetMetaBakNode():oih->GetMetaMainNode();
	nodeNum = OsdCfg::GetOsdCfg().GetDataNodeHashNum();
	RELEASE_ASSERT(nodeNum < DATA_NODE_MAX_NUM - 1);
	nodeInfo[0] = -1;
	nodeInfo[1] = -1;
	memcpy(&nodeInfo[2],oih->GetDataNodeAry(),sizeof(s32)*nodeNum);

	objIoNum = 0;
}

void DataTruncateObjIo::ObjStreamProcessObjIo()
{
	FiEvent evt;
	for (s32 i = 2;i<nodeNum+2;++i)
	{
		SetDstNode(nodeInfo[i]);
		NetWapper::GetNetWapper().PushNetIo(this, evt);
		evt.Wait();
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  DataDistributeTruncateObjIo  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void DataDistributeTruncateObjIo::InitObjIo(char *headBuf, char *bufPar, u64 lenPar, u64 offsetPar)
{	
	objmemcpy(this,headBuf,sizeof(BaseObjIo));
	srcNode = OsdCfg::GetOsdCfg().GetLocNodeID();
	nodeInfo[0] = srcNode;

	s32 i = 2;
	for (;i<nodeNum+2;++i)
	{
		if (srcNode == nodeInfo[i])
		{
			dstNode = nodeInfo[(i+1==nodeNum+2)?2:i+1];
			nodeInfo[1] = dstNode;
			break;
		}
	}
	RELEASE_ASSERT(i<nodeNum+2);
}

void DataDistributeTruncateObjIo::ObjStreamProcessObjIo()
{
	FiEvent evt;
	FiErr locRet;
	static IoFileLogElement e;

	IoFileLog *dcl = LocWapper::GetLocWapper().GetDataIoConsistencyLog();
	u32 fileSliceSize = OsdCfg::GetOsdCfg().GetDataSliceSize();
	u32 truncateNo = (u32)(offset/(u64)fileSliceSize);
	u32 truncateSize = (u32)(offset%(u64)fileSliceSize);
	locRet = dcl->AddFileLogElement(GetFileID(),truncateNo,GetDstNode(),truncateSize);
	if (locRet.IsErr())
	{
		SetRetStatus(locRet);
		return;
	}
	OSD_LOG_ERR("===DataDistributeTruncateObjIo::ObjStreamProcessObjIo=== AddFileLogElement fileID[%s],order[%d]",GetFileID().GetIDStr().c_str(),truncateNo);

	e.fileID = GetFileID();
	e.order = truncateNo;
	e.offset = truncateSize;
	e.nodeID = GetDstNode();

	NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Lock();
	if (FindTruncateRecoverOp(LocWapper::GetLocWapper().GetDataIoRecoverLog(),this,OsdCfg::GetOsdCfg().GetDataSliceSize()))
	{
		locRet = LogForRecover();
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		SetRetStatus(locRet);
		if (locRet.IsSuccess())
		{
			locRet = LocWapper::GetLocWapper().Truncate(this);
			SetRetStatus(locRet);
		}
	}
	else
	{
		NodeStatusObserver::GetNodeStatusObserver().GetNodesStatusLock().Unlock();
		locRet = LocWapper::GetLocWapper().Truncate(this);
		SetRetStatus(locRet);
		if (locRet.IsSuccess())
		{
			NetWapper::GetNetWapper().PushNetIo(this, evt);
			evt.Wait();
		}
	}

	if (GetRetStatus().IsSuccess())
	{
		OSD_LOG_ERR("===DataDistributeTruncateObjIo::ObjStreamProcessObjIo=== DelFileLogElement DataIoConsistencyLog fileID[%s],order[%d]",e.fileID.GetIDStr().c_str(),e.order);
		dcl->DelFileLogElement(e);
	}
}

void DataDistributeTruncateObjIo::ProcessNetReplyBuf(char* bufPar)
{
	BaseObjIo *netIo = (BaseObjIo *)bufPar;
	ret = netIo->GetRetStatus();
}

FiErr DataDistributeTruncateObjIo::LogForRecover()
{
	FiErr ret;
	IoFileLog *ioRecoverLog = LocWapper::GetLocWapper().GetDataIoRecoverLog();

	u32 fileSliceSize = OsdCfg::GetOsdCfg().GetDataSliceSize();
	u32 truncateNo = (u32)(offset/(u64)fileSliceSize);
	u32 truncateSize = (u32)(offset%(u64)fileSliceSize);
	ret = ioRecoverLog->AddFileLogElement(fileID,truncateNo,dstNode,truncateSize);
	if (ret.IsSuccess())
	{
		OSD_LOG_ERR("===DataDistributeTruncateObjIo::LogForRecover=== AddFileLogElement fileID[%s],order[%d]",fileID.GetIDStr().c_str(),truncateNo);
		MergTruncateRecoverOp(ioRecoverLog,this,fileSliceSize);
	}
	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  NetReadObjIo  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetReadObjIo::InitObjIo(char *headBuf, char *bufPar, u64 lenPar, u64 offsetPar)
{
	objmemcpy(this,headBuf,sizeof(BaseObjIo));
	buf = new char[length];
	objIoAry[0].buf = buf;
	objIoAry[1].buf = (objIoNum == 1) ? NULL : objIoAry[0].buf + objIoAry[0].length;
	retLength = 0;
}

void NetReadObjIo::ObjStreamProcessObjIo()
{
	SetRetStatus(LocWapper::GetLocWapper().Read(this));
}

string NetReadObjIo::GetStorPath(u32 no)
{
	char idStr[25] = {0};
	s32 locNodeID = OsdCfg::GetOsdCfg().GetLocNodeID();
	if (GetMainNode() == locNodeID)
	{
		sprintf(idStr,"/rddcNode_%d",GetBakNode());
	}
	else if(GetBakNode() == locNodeID)
	{
		sprintf(idStr,"/rddcNode_%d",GetMainNode());
	}
	else
	{
		if (GetSrcNode() == GetMainNode())
		{
			sprintf(idStr,"/rddcNode_%d",GetBakNode());
		}
		else if(GetSrcNode() == GetBakNode())
		{
			sprintf(idStr,"/rddcNode_%d",GetMainNode());
		}
		else
		{
			RELEASE_ASSERT_FASLE;
		}
	}

	char noStr[25] = {0};
	if (IsMetaType())
	{
		sprintf(noStr,"/meta%d",no);
		RELEASE_ASSERT(no < OsdCfg::GetOsdCfg().GetMetaMaxFileNo());
	}
	else
	{
		sprintf(noStr,"/%d_data%d",GetMainNode(),no);
		RELEASE_ASSERT(no < OsdCfg::GetOsdCfg().GetDataMaxFileNo());
	}
	return GetStorDirPath(IsMetaType(),fileID)+idStr+noStr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  NetWriteObjIo  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetWriteObjIo::InitObjIo(char *headBuf, char *bufPar, u64 lenPar, u64 offsetPar)
{
	objmemcpy(this,headBuf,sizeof(BaseObjIo));
	buf = headBuf + sizeof(BaseObjIo);
	objIoAry[0].buf = buf;
	objIoAry[1].buf = (objIoNum == 1) ? NULL : objIoAry[0].buf + objIoAry[0].length;
}

void NetWriteObjIo::ObjStreamProcessObjIo()
{
	SetRetStatus(LocWapper::GetLocWapper().Write(this));
}

string NetWriteObjIo::GetStorPath(u32 no)
{
	char idStr[25] = {0};
	s32 locNodeID = OsdCfg::GetOsdCfg().GetLocNodeID();
	if (GetMainNode() == locNodeID)
	{
		sprintf(idStr,"/rddcNode_%d",GetBakNode());
	}
	else if(GetBakNode() == locNodeID)
	{
		sprintf(idStr,"/rddcNode_%d",GetMainNode());
	}
	else
	{
		if (GetSrcNode() == GetMainNode())
		{
			sprintf(idStr,"/rddcNode_%d",GetBakNode());
		}
		else if(GetSrcNode() == GetBakNode())
		{
			sprintf(idStr,"/rddcNode_%d",GetMainNode());
		}
		else
		{
			RELEASE_ASSERT_FASLE;
		}
	}

	char noStr[25] = {0};
	if (IsMetaType())
	{
		sprintf(noStr,"/meta%d",no);
		RELEASE_ASSERT(no < OsdCfg::GetOsdCfg().GetMetaMaxFileNo());
	}
	else
	{
		sprintf(noStr,"/%d_data%d",GetMainNode(),no);
		RELEASE_ASSERT(no < OsdCfg::GetOsdCfg().GetDataMaxFileNo());
	}
	return GetStorDirPath(IsMetaType(),fileID)+idStr+noStr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  NetTruncateObjIo  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetTruncateObjIo::InitObjIo(char *headBuf, char *bufPar, u64 lenPar, u64 offsetPar)
{
	objmemcpy(this,headBuf,sizeof(BaseObjIo));
}

void NetTruncateObjIo::ObjStreamProcessObjIo()
{
	SetRetStatus(LocWapper::GetLocWapper().Truncate(this));
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  NetDelObjIo  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetDelObjIo::InitObjIo(char *headBuf, char *bufPar, u64 lenPar, u64 offsetPar)
{
	objmemcpy(this,headBuf,sizeof(BaseObjIo));
}

void NetDelObjIo::ObjStreamProcessObjIo()
{
	SetRetStatus(LocWapper::GetLocWapper().Delete(this));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  RedirectReadObjIo  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RedirectReadObjIo::InitObjIo(char *headBuf, char *bufPar, u64 lenPar, u64 offsetPar)
{
	objmemcpy(this,headBuf,sizeof(BaseObjIo));

	buf = bufPar;
	length = lenPar;
	offset = offsetPar;
	srcNode = OsdCfg::GetOsdCfg().GetLocNodeID();

	retLength = 0;

	objIoNum = 1;
	objIoAry[0].buf = buf;
	objIoAry[0].length = length;
	objIoAry[0].offset = offset;
}

void RedirectReadObjIo::ProcessNetReplyBuf(char* bufPar)
{
	BaseObjIo *netIo = (BaseObjIo *)bufPar;
	ret = netIo->GetRetStatus();
	retLength = netIo->GetRetLength();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  RedirectWriteObjIo  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RedirectWriteObjIo::InitObjIo(char *headBuf, char *bufPar, u64 lenPar, u64 offsetPar)
{
	objmemcpy(this,headBuf,sizeof(BaseObjIo));

	buf = bufPar;
	length = lenPar;
	offset = offsetPar;
	srcNode = OsdCfg::GetOsdCfg().GetLocNodeID();

//  preAllocNode = NodeSpaceCollecter::GetNodeSpaceCollecter().AllocOneNode(IsMetaType(),nodeInfo,nodeNum);
//  dstNode = preAllocNode;
	
	objIoNum = 1;
	objIoAry[0].buf = buf;
	objIoAry[0].length = length;
	objIoAry[0].offset = offset;
}

void RedirectWriteObjIo::ProcessNetReplyBuf(char* bufPar)
{
	BaseObjIo *netIo = (BaseObjIo *)bufPar;
	ret = netIo->GetRetStatus();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  RedirectTruncateObjIo  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RedirectTruncateObjIo::InitObjIo(char *headBuf, char *bufPar, u64 lenPar, u64 offsetPar)
{
	objmemcpy(this,headBuf,sizeof(BaseObjIo));
	srcNode = OsdCfg::GetOsdCfg().GetLocNodeID();
}

void RedirectTruncateObjIo::ProcessNetReplyBuf(char* bufPar)
{
	BaseObjIo *netIo = (BaseObjIo *)bufPar;
	ret = netIo->GetRetStatus();
}

FiErr RedirectTruncateObjIo::LogForRecover()
{
	FiErr ret;
	IoFileLog *ioRecoverLog = (IsMetaType())?LocWapper::GetLocWapper().GetMetaIoRecoverLog():LocWapper::GetLocWapper().GetDataIoRecoverLog();
	u32 fileSliceSize = IsMetaType()?OsdCfg::GetOsdCfg().GetMetaSliceSize():OsdCfg::GetOsdCfg().GetDataSliceSize();
	u32 truncateNo = (u32)(offset/(u64)fileSliceSize);
	u32 truncateSize = (u32)(offset%(u64)fileSliceSize);
	ret = ioRecoverLog->AddFileLogElement(fileID,truncateNo,dstNode,truncateSize);
	if (ret.IsErr())
	{
		return ret;
	}
	OSD_LOG_ERR("===RedirectTruncateObjIo::LogForRecover=== %s AddFileLogElement fileID[%s],order[%d],offset[%lld]",(IsMetaType())?"Meta":"Data",fileID.GetIDStr().c_str(),truncateNo,offset);
	return ret;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  RedirectDelObjIo  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RedirectDelObjIo::InitObjIo(char *headBuf, char *bufPar, u64 lenPar, u64 offsetPar)
{
	IoFileLogElement *e = (IoFileLogElement *)headBuf;

	dataType = (lenPar == MetaDel)?Meta:Data;
	version = OsdCfg::GetOsdCfg().GetVersion();
	fileID = e->fileID;
	srcOt = RedirectDel;
	ot = RedirectDel;
	srcNode = OsdCfg::GetOsdCfg().GetLocNodeID();
	dstNode = e->nodeID;

	buf = NULL;
	length = 0;
	offset = 0;
}

void RedirectDelObjIo::ProcessNetReplyBuf(char* bufPar)
{
	BaseObjIo *netIo = (BaseObjIo *)bufPar;
	ret = netIo->GetRetStatus();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  RecoverWriteObjIo  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RecoverWriteObjIo::InitObjIo(char *headBuf, char *bufPar, u64 lenPar, u64 offsetPar)
{
	objmemcpy(this,headBuf,sizeof(BaseObjIo));

	buf = bufPar;
	length = lenPar;
	offset = offsetPar;
	srcNode = OsdCfg::GetOsdCfg().GetLocNodeID();

//  preAllocNode = NodeSpaceCollecter::GetNodeSpaceCollecter().AllocOneNode(IsMetaType(),nodeInfo,nodeNum);
//  dstNode = preAllocNode;
	
	objIoNum = 1;
	objIoAry[0].buf = buf;
	objIoAry[0].length = length;
	objIoAry[0].offset = offset;
}

void RecoverWriteObjIo::ProcessNetReplyBuf(char* bufPar)
{
	BaseObjIo *netIo = (BaseObjIo *)bufPar;
	ret = netIo->GetRetStatus();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  RecoverTruncateObjIo  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RecoverTruncateObjIo::InitObjIo(char *headBuf, char *bufPar, u64 lenPar, u64 offsetPar)
{
	objmemcpy(this,headBuf,sizeof(BaseObjIo));
	srcNode = OsdCfg::GetOsdCfg().GetLocNodeID();
}

void RecoverTruncateObjIo::ProcessNetReplyBuf(char* bufPar)
{
	BaseObjIo *netIo = (BaseObjIo *)bufPar;
	ret = netIo->GetRetStatus();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  ConsistencyReadObjIo  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ConsistencyReadObjIo::InitObjIo(char *headBuf, char *bufPar, u64 lenPar, u64 offsetPar)
{
	objmemcpy(this,headBuf,sizeof(BaseObjIo));

	srcOt = ConsistencyRead;
	ot = ConsistencyRead;

	buf = bufPar;
	length = lenPar;
	offset = offsetPar;

	retLength = 0;

	objIoNum = 1;
	objIoAry[0].buf = buf;
	objIoAry[0].length = length;
	objIoAry[0].offset = offset;
}

//void ConsistencyReadObjIo::ObjStreamProcessObjIo()
//{
//	FiErr ret;
//	s32 nodeID = -1;
//	IoFileLog *ioRecoverLog = (IsMetaType())?LocWapper::GetLocWapper().GetMetaIoRecoverLog():LocWapper::GetLocWapper().GetDataIoRecoverLog();
//	if(ioRecoverLog->IsFileInIoFileLog(GetFileID(),GetObjIoAry()[0].order,nodeID))
//	{
//		if(GetSrcNode() == nodeID)
//		{
//			SetRetStatus(ret);
//			return;
//		}
//	}
//	ret = LocWapper::GetLocWapper().Read(this);
//	SetRetStatus(ret);
//}

void ConsistencyReadObjIo::ProcessNetReplyBuf(char* bufPar)
{
	BaseObjIo *netIo = (BaseObjIo *)bufPar;
	ret = netIo->GetRetStatus();
	retLength = netIo->GetRetLength();
}

bool ConsistencyReadObjIo::IsSetDoRecover()
{
	return NodeStatusObserver::GetNodeStatusObserver().CheckOsdVersion();
}

FiErr ConsistencyReadObjIo::LogForRecover()
{
	FiErr ret;
	IoFileLog *ioRecoverLog = IsMetaType()?LocWapper::GetLocWapper().GetMetaIoRecoverLog():LocWapper::GetLocWapper().GetDataIoRecoverLog();
	for (u32 i = 0;i<GetObjIoNum();++i)
	{
		ret = ioRecoverLog->AddFileLogElement(fileID,objIoAry[i].order,dstNode,WRITETYPE,GetMainNode(),GetBakNode());
		if (ret.IsErr())
		{
			return ret;
		}
		OSD_LOG_ERR("===ConsistencyReadObjIo::LogForRecover=== AddFileLogElement fileID[%s],order[%d]",fileID.GetIDStr().c_str(),GetObjIoAry()[i].order);
	}
	return ret;
}

string ConsistencyReadObjIo::GetStorPath(u32 no)
{
	char idStr[25] = {0};
	s32 locNodeID = OsdCfg::GetOsdCfg().GetLocNodeID();
	if (GetMainNode() == locNodeID)
	{
		sprintf(idStr,"/rddcNode_%d",GetBakNode());
	}
	else if(GetBakNode() == locNodeID)
	{
		sprintf(idStr,"/rddcNode_%d",GetMainNode());
	}
	else
	{
		if (GetSrcNode() == GetMainNode())
		{
			sprintf(idStr,"/rddcNode_%d",GetBakNode());
		}
		else if(GetSrcNode() == GetBakNode())
		{
			sprintf(idStr,"/rddcNode_%d",GetMainNode());
		}
		else
		{
			RELEASE_ASSERT_FASLE;
		}
	}

	char noStr[25] = {0};
	if (IsMetaType())
	{
		sprintf(noStr,"/meta%d",no);
		RELEASE_ASSERT(no < OsdCfg::GetOsdCfg().GetMetaMaxFileNo());
	}
	else
	{
		sprintf(noStr,"/%d_data%d",GetMainNode(),no);
		RELEASE_ASSERT(no < OsdCfg::GetOsdCfg().GetDataMaxFileNo());
	}
	return GetStorDirPath(IsMetaType(),fileID)+idStr+noStr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  NetConsistencyReadObjIo  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void NetConsistencyReadObjIo::InitObjIo(char *headBuf, char *bufPar, u64 lenPar, u64 offsetPar)
{
	objmemcpy(this,headBuf,sizeof(BaseObjIo));
	buf = new char[length];
	objIoAry[0].buf = buf;
	objIoAry[1].buf = (objIoNum == 1) ? NULL : objIoAry[0].buf + objIoAry[0].length;
	retLength = 0;
}

void NetConsistencyReadObjIo::ObjStreamProcessObjIo()
{
	FiErr ret;
	s32 nodeID = -1;
	IoFileLog *ioRecoverLog = (IsMetaType())?LocWapper::GetLocWapper().GetMetaIoRecoverLog():LocWapper::GetLocWapper().GetDataIoRecoverLog();
	if(ioRecoverLog->IsFileInIoFileLog(GetFileID(),GetObjIoAry()[0].order,nodeID))
	{
		if(GetSrcNode() == nodeID)
		{
			SetRetStatus(ret);
			return;
		}
	}
	ret = LocWapper::GetLocWapper().Read(this);
	SetRetStatus(ret);
}

string NetConsistencyReadObjIo::GetStorPath(u32 no)
{
	char idStr[25] = {0};
	s32 locNodeID = OsdCfg::GetOsdCfg().GetLocNodeID();
	if (GetMainNode() == locNodeID)
	{
		sprintf(idStr,"/rddcNode_%d",GetBakNode());
	}
	else if(GetBakNode() == locNodeID)
	{
		sprintf(idStr,"/rddcNode_%d",GetMainNode());
	}
	else
	{
		if (GetSrcNode() == GetMainNode())
		{
			sprintf(idStr,"/rddcNode_%d",GetBakNode());
		}
		else if(GetSrcNode() == GetBakNode())
		{
			sprintf(idStr,"/rddcNode_%d",GetMainNode());
		}
		else
		{
			RELEASE_ASSERT_FASLE;
		}
	}

	char noStr[25] = {0};
	if (IsMetaType())
	{
		sprintf(noStr,"/meta%d",no);
		RELEASE_ASSERT(no < OsdCfg::GetOsdCfg().GetMetaMaxFileNo());
	}
	else
	{
		sprintf(noStr,"/%d_data%d",GetMainNode(),no);
		RELEASE_ASSERT(no < OsdCfg::GetOsdCfg().GetDataMaxFileNo());
	}
	return GetStorDirPath(IsMetaType(),fileID)+idStr+noStr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  ConsistencyTruncateObjIo  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ConsistencyTruncateObjIo::InitObjIo(char *headBuf, char *bufPar, u64 lenPar, u64 offsetPar)
{
	objmemcpy(this,headBuf,sizeof(BaseObjIo));
	srcOt = ConsistencyTruncate;
	ot = ConsistencyTruncate;
}

void ConsistencyTruncateObjIo::ObjStreamProcessObjIo()
{
	if (NodeStatusObserver::GetNodeStatusObserver().CheckOsdVersion())
	{
		SetRetStatus(LocWapper::GetLocWapper().Truncate(this));
	}
	else
	{
		FiErr ret;
		if (IsMetaType())
		{
			IoFileLog *ioRecoverLog = LocWapper::GetLocWapper().GetMetaIoRecoverLog();
			u32 fileSliceSize = OsdCfg::GetOsdCfg().GetMetaSliceSize();
			u32 truncateNoBegin = (u32)(offset/(u64)fileSliceSize);
			u32 truncateNoEnd = (u32)(length/(u64)fileSliceSize);
			RELEASE_ASSERT(truncateNoEnd >= truncateNoBegin);

			vector<NodeAndTruncateNo> ntAry;
			ret = ficsLocIF::GetFicsLocIF().GetTruncateRecoverAry(OsdCfg::GetOsdCfg().GetMetaRootPath()+fileID.GetIDStr(),	\
																  ntAry,truncateNoBegin,truncateNoEnd);
			if (ret.IsErr())
			{
				SetRetStatus(ret);
				return;
			}
			if (!ntAry.empty())
			{
				ret = NodeStatusObserver::GetNodeStatusObserver().NodeDoRecoverSetEx(GetSrcNode(),ioRecoverLog,GetFileID(),ntAry);
			}
		}
		else
		{
			IoFileLog *ioRecoverLog = LocWapper::GetLocWapper().GetDataIoRecoverLog();
			u32 fileSliceSize = OsdCfg::GetOsdCfg().GetDataSliceSize();
			u32 truncateNoBegin = (u32)(offset/(u64)fileSliceSize);
			u32 truncateNoEnd = (u32)(length/(u64)fileSliceSize);
			RELEASE_ASSERT(truncateNoEnd >= truncateNoBegin);

			vector<NodeAndTruncateNo> ntAry;
			ret = ficsLocIF::GetFicsLocIF().GetTruncateRecoverAry(OsdCfg::GetOsdCfg().GetDataRootPath()+fileID.GetIDStr(),	\
																  ntAry,truncateNoBegin,truncateNoEnd);
			if (ret.IsErr())
			{
				SetRetStatus(ret);
				return;
			}
			if (!ntAry.empty())
			{
				ret = NodeStatusObserver::GetNodeStatusObserver().NodeDoRecoverSetEx(GetSrcNode(),ioRecoverLog,GetFileID(),ntAry);
			}
		}

		if (ret.IsErr())
		{
			SetRetStatus(ret);
		}
	}
}

void ConsistencyTruncateObjIo::ProcessNetReplyBuf(char* bufPar)
{
	BaseObjIo *netIo = (BaseObjIo *)bufPar;
	ret = netIo->GetRetStatus();
}

bool ConsistencyTruncateObjIo::IsSetDoRecover()
{
	return NodeStatusObserver::GetNodeStatusObserver().CheckOsdVersion();
}

FiErr ConsistencyTruncateObjIo::LogForRecover()
{
	FiErr ret;
	IoFileLog *ioRecoverLog = (IsMetaType())?LocWapper::GetLocWapper().GetMetaIoRecoverLog():LocWapper::GetLocWapper().GetDataIoRecoverLog();
	u32 fileSliceSize = IsMetaType()?OsdCfg::GetOsdCfg().GetMetaSliceSize():OsdCfg::GetOsdCfg().GetDataSliceSize();
	u32 truncateNo = (u32)(offset/(u64)fileSliceSize);
	u32 truncateSize = (u32)(offset%(u64)fileSliceSize);
	ret = ioRecoverLog->AddFileLogElement(fileID,truncateNo,dstNode,truncateSize);
	if (ret.IsErr())
	{
		return ret;
	}
	OSD_LOG_ERR("===ConsistencyTruncateObjIo::LogForRecover=== %s AddFileLogElement fileID[%s],order[%d],offset[%lld]",(IsMetaType())?"Meta":"Data",fileID.GetIDStr().c_str(),truncateNo,offset);
	return ret;
}

