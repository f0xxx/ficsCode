///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  hunter code ([hunter.f0x] remeber my brothers) email:hunterlhy@163.com /////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "osdCache.h"
#include "ficsLocIF.h"
#include <fstream>
using namespace std;

u32 DiskCacheNode::timeoutInValidList = 4;	//seconds
u32 DiskCacheNode::timeoutInDirtyList = 2;	//seconds
u32 DiskCacheNode::minReadCopySize = 8;	//8*512 = 4k

u32 FileCacheNode::timeout = 4;	//seconds

u32 IoFileInfo::timeout = 30;	//seconds
u32 IoFileInfo::maxReadAheadSize = 16*1024*1024;	
u32 IoFileInfo::minReadAheadSize = 256*1024;	
u32 IoFileInfo::seriesNum = 3;	//seriesNum = 1 will shutdown read ahead

bool isOpRealDevice = true;

cacheReadWrite *cache = NULL;
//OPCALLBACK ReadWriteProc = NULL;
//REPORTCALLBACK ReportProc = NULL;
//extern vector<RunlistUpUse> rlAry;
//extern lockBase *rlAryLock;
//extern VIRTUALVOLUME_ALL VirtualVol_All;

//for mas interface
extern ThreadPool *deliverTP;
DWORD WINAPI deliverThread(LPVOID par);

#define FISHAREREVSIZE (20*1024*1024)
#define MINIREADSIZE (256*1024)
CacheConfig cc = {0};
u64 frequency = 0;

//extern vector<RunlistUpUse>::iterator rluuIt;

//u32 logAll[10000] = {0};
//u32 logReadDisk[10000] = {0};
//u32 logWait[10000] = {0};
//
//static u32 logIndex = 0;
//
////INITTIME_INMEM(1);
////INITTIME_INMEM(2);
////INITTIME_INMEM(3);

#define LOGBUFSIZE (3*1024*1024)

//logPerformace
//u32 DCRDTimeSumAry[LOGBUFSIZE] = {0};
//u32 DCRDTimeSumCount = 0;
//
//u32 BeginReadGetLockAry[LOGBUFSIZE] = {0};
//u32 BeginReadGetLockCount = 0;
//u32 ReadAheadCheckAry[LOGBUFSIZE] = {0};
//u32 ReadAheadCheckCount = 0;
//u32 StartReadAheadAry[LOGBUFSIZE] = {0};
//u32 StartReadAheadCount = 0;
//u32 WaitReadAheadBackAry[LOGBUFSIZE] = {0};
//u32 WaitReadAheadBackCount = 0;
//u32 ReadDataFromFileCacheAry[LOGBUFSIZE] = {0};
//u32 ReadDataFromFileCacheCount = 0;
//u32 ReadDataFromDiskCacheOrDiskAry[LOGBUFSIZE] = {0};
//u32	ReadDataFromDiskCacheOrDiskCount = 0;
//
//u32 readDiskAry[LOGBUFSIZE] = {0};
//u32 readDiskCount = 0;
//u32 readAheadAry[LOGBUFSIZE] = {0};
//u32 readAheadCount = 0;
//
//u32 ValidCacheNotReadSizeAry[LOGBUFSIZE] = {0};
//u32 ValidCacheNotReadSizeCount = 0;
//u32 UnitReadAheadDataSizeAry[LOGBUFSIZE] = {0};
//u32 UnitReadAheadDataSizeCount = 0;
//
//u32 ioOnDiskAry[LOGBUFSIZE] = {0};
//u32 ioOnDiskCount = 0;

//u32 lastErr = 0;

char developInfo[256] = "hunter code ([hunter.f0x] remeber my brothers) email:hunterlhy@163.com";
lockUnix g_lw;

inline bool IsTimeout_Sub(u64 markTime,u32 timeout)
{
	bool ret = true;
	u64 tpTime = GetMicroSecTime();

	if(!((u64)(tpTime - markTime)>((u64)timeout*1000000)))
	{
		ret = false;
	}
	return ret;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  IoFileInfoHandle  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//SpaceAnalyse::SpaceAnalyse(SpaceNode &APar,SpaceNode &BPar):A(APar),B(BPar),isHaveSameSpace(false)
//{
//	if(A.length == 0 || B.length == 0)
//	{
//		return;
//	}
//
//	assert(A.length != 0);
//	assert(B.length != 0);
//	memset(&Same,0,sizeof(SpaceNode));
//	memset(&ADiff,0,sizeof(SpaceNode)*2);
//	memset(&BDiff,0,sizeof(SpaceNode)*2);
//
//	if(A.start == B.start && A.length == B.length)
//	{
//		isHaveSameSpace = true;
//		Same.start = A.start;
//		Same.length = A.length;
//		return;
//	}
//
//	if((A.start>=B.start && A.start<B.start+B.length)||	\
//		(A.start+A.length<=B.start+B.length && A.start+A.length>B.start)||	\
//		(A.start<B.start && A.start+A.length>B.start+B.length))
//	{
//		isHaveSameSpace = true;
//		if(A.start>=B.start && A.start<B.start+B.length)
//		{
//			Same.start = A.start;
//			Same.length = (A.start + A.length > B.start + B.length)?(B.start + B.length - A.start):A.length;
//			if(A.start>B.start)
//			{
//				BDiff[0].start = B.start;
//				BDiff[0].length = A.start - B.start;
//			}
//			if(A.start + A.length > B.start + B.length)
//			{
//				ADiff[0].start = B.start + B.length;
//				ADiff[0].length = (A.start + A.length)-(B.start + B.length);
//			}
//			else if(A.start + A.length < B.start + B.length)
//			{
//				BDiff[1].start = A.start + A.length;
//				BDiff[1].length = (B.start + B.length)-(A.start + A.length);
//			}
//		}
//		else if(A.start+A.length<=B.start+B.length && A.start+A.length>B.start)
//		{
//			Same.start = (A.start > B.start)?A.start:B.start;
//			Same.length = A.start + A.length - Same.start;
//			if(A.start+A.length<B.start+B.length)
//			{
//				BDiff[0].start = A.start+A.length;
//				BDiff[0].length = (B.start + B.length)-(A.start + A.length);
//			}
//			if(A.start > B.start)
//			{
//				BDiff[1].start = B.start;
//				BDiff[1].length = A.start - B.start;
//			}
//			else if(A.start < B.start)
//			{
//				ADiff[0].start = A.start;
//				ADiff[0].length = B.start-A.start;
//			}
//		}
//		else
//		{
//			Same.start = B.start;
//			Same.length = B.length;
//			ADiff[0].start = A.start;
//			ADiff[0].length = B.start-A.start;
//			ADiff[1].start = B.start+B.length;
//			ADiff[1].length = (A.start+A.length)-(B.start+B.length);
//		}
//	}
//}

OsdLock IoFileInfoHandle::l;

IoFileInfoHandle::IoFileInfoHandle(ObjID fileID,u32 length,u64 offset,u32 pcID):cnt(new int(1))
{
	FileReadIoLogNode frl;
	pfi = new IoFileInfo;	//can use obj pool for optimization
	RELEASE_ASSERT(pfi != NULL);
	pfi->pcId = pcID;
//	pfi->volGuid = ddion->difsdiskionode.difsdiskio.VolGuid;
//	pfi->processId = 0;
//	pfi->fileHandle = ddion->difsdiskionode.difsdiskio.dirc_iocontext.HandleIdentifier;
	pfi->fileId = fileID;

	pfi->ioDiskOffset = offset;
	pfi->ioDisklength = length;

	pfi->markUserSpeedTime = -1;
	pfi->markTime = -1;
	pfi->raStatus = NonReadAhead;
	pfi->startReadAheadPos = -1;

	pfi->readCount = 0;
	pfi->hitCount = 0;
	pfi->distance = 0;
	pfi->ioLogList.reserve(1);

	frl.oriFilelength =  pfi->ioDiskOffset;
	frl.oriFileOffset = pfi->ioDisklength;
	pfi->ioLogList.push_back(frl);
	lastfrl = frl;
}

//inline void IoFileInfoHandle::BuildIoNode(void *ddionbuf,ObjID diskId,char *ioBuf,u32 diskLength,u64 diskOffset)
//{
//	PDIFSDISKIONODE_APP ddion = (PDIFSDISKIONODE_APP)ddionbuf;
//	ddion->difsdiskionode.optype = READFILE;
//	ddion->difsdiskionode.difsdiskio.isNetIo = 1;
//
//	ddion->difsdiskionode.difsdiskio.uIoPacketNumber = (ULONG)lastfrl.ioOrder;
//	ddion->difsdiskionode.difsdiskio.dirc_iocontext.Filelength = lastfrl.oriFilelength;
//	ddion->difsdiskionode.difsdiskio.dirc_iocontext.FileOffset = lastfrl.oriFileOffset;
//
//	ddion->difsdiskionode.difsdiskio.PCID = pfi->pcId;
//	ddion->difsdiskionode.difsdiskio.dirc_iocontext.HandleIdentifier = pfi->fileHandle;
//	ddion->difsdiskionode.difsdiskio.dirc_iocontext.fileId = pfi->fileId;
//	ddion->difsdiskionode.difsdiskio.VolGuid = pfi->volGuid;
//	ddion->difsdiskionode.VolGuid = pfi->volGuid;
//
//	ddion->difsdiskionode.difsdiskio.MasHandle = diskId;
//	ddion->difsdiskionode.MasHandle = diskId;
//	ddion->difsdiskionode.difsdiskio.Buffer = (u64)ioBuf;
//	ddion->difsdiskionode.difsdiskio.Length = diskLength;
//	ddion->difsdiskionode.difsdiskio.ByteOffset = diskOffset;
//}

inline void IoFileInfoHandle::FileInfo2DiskInfo(SpaceNode &fileInfo,vector<IoDiskInfo> &diskInfoAry)
{
	IoDiskInfo di;
	di.diskId = GetDiskId();
	di.ioDiskOffset = fileInfo.start;
	di.ioDisklength = fileInfo.length;
	diskInfoAry.push_back(di);
	return;

//	//fileInfo to volInfo
//	SpaceNode &snA = fileInfo,snB;
//	SpaceNode *sameSpace = NULL;
//	vector<SpaceNode> volInfo;
//	volInfo.reserve(2);
//	u32 i = 0;
//	for(i = 0;i<pfi->fileBlockNum;++i)
//	{
//		if(fileInfo.start>=pfi->fbAry[i].fileOffset+pfi->fbAry[i].length)
//		{
//			continue;
//		}
//		if(fileInfo.start+fileInfo.length<=pfi->fbAry[i].fileOffset)
//		{
//			assert(volInfo.size()!=0);
//			break;
//		}
//
//		snB.start = pfi->fbAry[i].fileOffset;
//		snB.length = pfi->fbAry[i].length;
//		LOG_READAHEADRUN("<threadId:%lu>\t[%s]fbAry%d---fileOffset[0x%llx];length[0x%llx]\n",pthread_self(),__FUNCTION__,i,snB.start,snB.length);
//
//		SpaceAnalyse sa(snA,snB);
//		if(sa.IsHaveSameSpace())
//		{
//			sameSpace = sa.GetSameSapce();
//			LOG_READAHEADRUN("<threadId:%lu>\t[%s]start[0x%llx];fileOffset[0x%llx];volOffset[0x%llx]\n",pthread_self(),__FUNCTION__,sameSpace->start,pfi->fbAry[i].fileOffset,pfi->fbAry[i].volOffset);
//			snB.start = sameSpace->start - pfi->fbAry[i].fileOffset + pfi->fbAry[i].volOffset;
//			snB.length = sameSpace->length;
//			volInfo.push_back(snB);
//		}
//	}
//
//	if(volInfo.empty())
//	{
//		LOG_READAHEADRUN("<threadId:%lu>\t[%s]volInfo is empty&return\n",pthread_self(),__FUNCTION__);
//		return;
//	}
//
//#ifdef _f0xCacheDbg
//	{
//		int iDbg=0;
//		for(vector<SpaceNode>::iterator itDbg=volInfo.begin();itDbg!=volInfo.end();++itDbg,++iDbg)
//		{
//			LOG_READAHEADRUN("<threadId:%lu>\t[%s]volInfo%d---fileOffset[0x%llx];length[0x%llx]\n",pthread_self(),__FUNCTION__,iDbg,itDbg->start,itDbg->length);
//		}
//	}
//#endif
//
//
//	//volInfo to diskInfo
//	u64 volOffset = 0;
//	IoDiskInfo di;
//	for(i = 0;i<pfi->diskNum;++i)
//	{
//		for(snIt = volInfo.begin();snIt!=volInfo.end();++snIt)
//		{
//			if(snIt->start>=volOffset+pfi->diskAry[i].diskSize)
//			{
//				continue;
//			}
//			if(snIt->start+snIt->length<=volOffset)
//			{
//				assert(diskInfoAry.size()!=0);
//				break;
//			}
//
//			snA.start = volOffset;
//			snA.length = pfi->diskAry[i].diskSize;
//			snB.start = snIt->start;
//			snB.length = snIt->length;
//			SpaceAnalyse sa(snA,snB);
//			if(sa.IsHaveSameSpace())
//			{
//				sameSpace = sa.GetSameSapce();
//				di.diskId = pfi->diskAry[i].diskId;
//				di.ioDiskOffset = sameSpace->start - volOffset + FISHAREREVSIZE;
//				di.ioDisklength = (u32)sameSpace->length;
//				diskInfoAry.push_back(di);
//			}
//		}
//		volOffset += pfi->diskAry[i].diskSize;
//	}
//	assert(diskInfoAry.size()!=0);
//
//#ifdef _f0xCacheDbg
//	{
//		int iDbg=0;
//		for(vector<IoDiskInfo>::iterator itDbg=diskInfoAry.begin();itDbg!=diskInfoAry.end();++itDbg,++iDbg)
//		{
//			LOG_READAHEADRUN("<threadId:%lu>\t[%s]diskInfoAry%d---fileOffset[0x%llx];length[0x%x]\n",pthread_self(),__FUNCTION__,iDbg,itDbg->ioDiskOffset,itDbg->ioDisklength);
//		}
//	}
//#endif
}

inline bool IoFileInfoHandle::RHStrategy(vector<SpaceNode> &RHFileIoAry,u32 userSpeed)
{
	bool ret = false;
	SpaceNode sn;

	if(pfi->seriesNum<2 || pfi->ioLogList.size()<pfi->seriesNum)
	{
		LOG_NORMAL("<threadId:%lu>\t[%s]SeriesStrategy Out---ioLogListSize[%lu];seriesNum[%u]\n",pthread_self(),__FUNCTION__,pfi->ioLogList.size(),pfi->seriesNum);
		return ret;
	}

	switch(pfi->raStatus)
	{
	case NonReadAhead:
		{
			LOG_READAHEADRUN("<threadId:%lu>\t[%s]NonReadAhead IN\n",pthread_self(),__FUNCTION__);
			bool isStartRH = true;
			iofihIt = pfi->ioLogList.end()-pfi->seriesNum;
			pfi->distance = (iofihIt+1)->oriFileOffset - (iofihIt->oriFileOffset+(u64)iofihIt->oriFilelength);
			pfi->startReadAheadPos = iofihIt->oriFileOffset;

			for(++iofihIt;(iofihIt+1)!=pfi->ioLogList.end();++iofihIt)
			{
				if(pfi->distance != (iofihIt+1)->oriFileOffset - (iofihIt->oriFileOffset+(u64)iofihIt->oriFilelength))
				{
					LOG_READAHEADRUN("<threadId:%lu>\t[%s]distance not match\n",pthread_self(),__FUNCTION__);
					isStartRH = false;
					break;
				}
			}
			if( isStartRH && pfi->startReadAheadPos!=iofihIt->oriFileOffset)
			{
				LOG_READAHEADRUN("<threadId:%lu>\t[%s]ReadAhead Start---RHStatus[%d];distance[0x%llx]\n",pthread_self(),__FUNCTION__,pfi->raStatus,pfi->distance);
				pfi->raStatus = pfi->startReadAheadPos<iofihIt->oriFileOffset?ForwardReadAhead:BackReadAhead;
				pfi->readCount = 0;
				pfi->hitCount = 0;
				if(pfi->distance == 0)
				{
					sn.length = userSpeed;
					sn.start = (pfi->raStatus == ForwardReadAhead)?(iofihIt->oriFileOffset+iofihIt->oriFilelength):(iofihIt->oriFileOffset - userSpeed);
					RHFileIoAry.push_back(sn);
				}
				else
				{
					sn.length = iofihIt->oriFilelength;
					sn.start = (pfi->raStatus == ForwardReadAhead)?(iofihIt->oriFileOffset+iofihIt->oriFilelength+pfi->distance):(iofihIt->oriFileOffset - iofihIt->oriFilelength + pfi->distance);
					RHFileIoAry.push_back(sn);
					sn.length = iofihIt->oriFilelength;
					sn.start = (pfi->raStatus == ForwardReadAhead)?(iofihIt->oriFileOffset+iofihIt->oriFilelength+pfi->distance):(iofihIt->oriFileOffset - 2*iofihIt->oriFilelength + 2*pfi->distance);
					RHFileIoAry.push_back(sn);
				}
				ret = true;
			}
#ifdef _f0xCacheDbg
			{
				int iDbg=0;
				for(iofihIt = pfi->ioLogList.end()-pfi->seriesNum;pfi->ioLogList.size()>=pfi->seriesNum&&(iofihIt+1)!=pfi->ioLogList.end();++iofihIt,++iDbg)
				{
					LOG_PERFORMACE_DOWN("<threadId:%lu>\t[%s]ioLogListNode%d---oriFileOffset[0x%llx];oriFilelength[0x%x];distance[0x%llx]\n",pthread_self(),__FUNCTION__,iDbg,iofihIt->oriFileOffset,iofihIt->oriFilelength,(iofihIt+1)->oriFileOffset - (iofihIt->oriFileOffset+(u64)iofihIt->oriFilelength));
				}
			}
#endif
			LOG_READAHEADRUN("<threadId:%lu>\t[%s]NonReadAhead Out\n",pthread_self(),__FUNCTION__);
			break;
		}
	case ForwardReadAhead:
		{
			LOG_READAHEADRUN("<threadId:%lu>\t[%s]ForwardReadAhead IN\n",pthread_self(),__FUNCTION__);
			iofihIt = pfi->ioLogList.end()-1;
			if(iofihIt->oriFileOffset<pfi->startReadAheadPos || pfi->readCount<10?false:(pfi->hitCount*10/pfi->readCount)<3)
			{
				pfi->raStatus = NonReadAhead;
				pfi->readCount = 0;
				pfi->hitCount = 0;
				LOG_READAHEADRUN("<threadId:%lu>\t[%s]ReadAhead Stop\n",pthread_self(),__FUNCTION__);
			}
			else
			{
				if(pfi->distance == 0)
				{
					sn.length = userSpeed;
					sn.start = (pfi->raStatus == ForwardReadAhead)?(iofihIt->oriFileOffset+iofihIt->oriFilelength):(iofihIt->oriFileOffset - userSpeed);
					RHFileIoAry.push_back(sn);
				}
				else
				{
					sn.length = iofihIt->oriFilelength;
					sn.start = (pfi->raStatus == ForwardReadAhead)?(iofihIt->oriFileOffset+iofihIt->oriFilelength+pfi->distance):(iofihIt->oriFileOffset - iofihIt->oriFilelength + pfi->distance);
					RHFileIoAry.push_back(sn);
					sn.length = iofihIt->oriFilelength;
					sn.start = (pfi->raStatus == ForwardReadAhead)?(iofihIt->oriFileOffset+iofihIt->oriFilelength+pfi->distance):(iofihIt->oriFileOffset - 2*iofihIt->oriFilelength + 2*pfi->distance);
					RHFileIoAry.push_back(sn);
				}
				ret = true;
			}
			LOG_READAHEADRUN("<threadId:%lu>\t[%s]ForwardReadAhead Out\n",pthread_self(),__FUNCTION__);
			break;
		}
	case BackReadAhead:
		{
			LOG_READAHEADRUN("<threadId:%lu>\t[%s]BackReadAhead IN\n",pthread_self(),__FUNCTION__);
			iofihIt = pfi->ioLogList.end()-1;
			if(iofihIt->oriFileOffset>pfi->startReadAheadPos || pfi->readCount<10?false:(pfi->hitCount*10/pfi->readCount)<3)
			{
				pfi->raStatus = NonReadAhead;
				pfi->readCount = 0;
				pfi->hitCount = 0;
				LOG_READAHEADRUN("<threadId:%lu>\t[%s]ReadAhead Stop\n",pthread_self(),__FUNCTION__);
			}
			else
			{
				if(pfi->distance == 0)
				{
					sn.length = userSpeed;
					sn.start = (pfi->raStatus == ForwardReadAhead)?(iofihIt->oriFileOffset+iofihIt->oriFilelength):(iofihIt->oriFileOffset - userSpeed);
					RHFileIoAry.push_back(sn);
				}
				else
				{
					sn.length = iofihIt->oriFilelength;
					sn.start = (pfi->raStatus == ForwardReadAhead)?(iofihIt->oriFileOffset+iofihIt->oriFilelength+pfi->distance):(iofihIt->oriFileOffset - iofihIt->oriFilelength + pfi->distance);
					RHFileIoAry.push_back(sn);
					sn.length = iofihIt->oriFilelength;
					sn.start = (pfi->raStatus == ForwardReadAhead)?(iofihIt->oriFileOffset+iofihIt->oriFilelength+pfi->distance):(iofihIt->oriFileOffset - 2*iofihIt->oriFilelength + 2*pfi->distance);
					RHFileIoAry.push_back(sn);
					ret = true;
				}
			}
			LOG_READAHEADRUN("<threadId:%lu>\t[%s]BackReadAhead Out\n",pthread_self(),__FUNCTION__);
			break;
		}
	default:
		RELEASE_ASSERT_FASLE;
	}
	return ret;
}

inline bool IoFileInfoHandle::RandomStrategy(vector<SpaceNode> &RHFileIoAry,FileReadIoLogNode &newNode)
{
	SpaceNode sn;
	sn.length = newNode.oriFilelength*4;
	sn.start = newNode.oriFileOffset;
	//sn.start &= ~((u64)MINIREADSIZE-1);
	//if(sn.length%MINIREADSIZE != 0)
	//{
	//	sn.length &= ~(MINIREADSIZE-1);
	//	sn.length += MINIREADSIZE;
	//}

	RHFileIoAry.push_back(sn);
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  memory  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
cacheUnitMarkBits::cacheUnitMarkBits(u32 bitNum)
{
	assert(bitNum>0);
	markBitNum = bitNum;
	u32 tp=(bitNum%BITALIGN)?1:0;
	mark64Num = bitNum/BITALIGN + tp; 
	bs = new u64[mark64Num];
	assert(bs != NULL);
	ClearAllBits();
}

cacheUnitMarkBits::~cacheUnitMarkBits()
{
	if(bs != NULL)
	{	
		delete [] bs;
	}	
}

inline void cacheUnitMarkBits::ClearAllBits()
{
	memset(bs,0,mark64Num*sizeof(u64));
}

inline void cacheUnitMarkBits::SetAllBits()
{
	memset(bs,0xff,mark64Num*sizeof(u64));
	u32 tp = mark64Num*BITALIGN-markBitNum;
	bs[mark64Num-1] &= MARKCONST64>>tp;
}

inline void cacheUnitMarkBits::ClearBitRange(u32 startIndex,u32 bitLength)
{
	assert(bitLength > 0);
	assert(startIndex+bitLength < markBitNum+1);
	u32 Begin = startIndex/BITALIGN;
	u32 End = (startIndex+bitLength)/BITALIGN;
	u32 bitBeginOffset = startIndex%BITALIGN;
	u32 bitEndOffset = (startIndex+bitLength)%BITALIGN;
	u64 mark = 0;
	if(Begin == End)
	{
		mark = (MARKCONST64&(MARKCONST64>>(BITALIGN-bitLength)))<<bitBeginOffset;
		bs[Begin] &= ~mark;
	}
	else
	{
		u32 i = Begin;
		for(;i<End;++i)
		{
			mark = MARKCONST64&(MARKCONST64<<bitBeginOffset);
			bs[i] &= ~mark;
			bitBeginOffset = 0;
		}
		if(bitEndOffset != 0)
		{
			mark = MARKCONST64<<bitEndOffset;
			bs[i] &= mark;
		}
	}
}

inline void cacheUnitMarkBits::SetBitRange(u32 startIndex,u32 bitLength)
{
	assert(bitLength > 0);
	assert(startIndex+bitLength < markBitNum+1);
	u32 Begin = startIndex/BITALIGN;
	u32 End = (startIndex+bitLength)/BITALIGN;
	u32 bitBeginOffset = startIndex%BITALIGN;
	u32 bitEndOffset = (startIndex+bitLength)%BITALIGN;
	u64 mark = 0;
	if(Begin == End)
	{
		mark = (MARKCONST64&(MARKCONST64>>(BITALIGN-bitLength)))<<bitBeginOffset;
		bs[Begin] |= mark;
	}
	else
	{
		u32 i = Begin;
		for(;i<End;++i)
		{
			mark = MARKCONST64&(MARKCONST64<<bitBeginOffset);
			bs[i] |= mark;
			bitBeginOffset = 0;
		}
		if(bitEndOffset != 0)
		{
			mark = MARKCONST64<<bitEndOffset;
			bs[i] |= ~mark;
		}
	}
}

inline void cacheUnitMarkBits::ClearBit(u32 Index)
{
	assert(Index < markBitNum);
	ClearBitRange(Index,1);
}

inline void cacheUnitMarkBits::SetBit(u32 Index)
{
	assert(Index < markBitNum);
	SetBitRange(Index,1);

}

inline int cacheUnitMarkBits::GetSeriesSetBitsNum()
{
	int tp = 0;
	u32 startIndex = 0;
	u32 bitLength = 0;
	int i = 0;
	while((tp = GetSeriesSetBits(tp,GetMarkBitNum()-1,startIndex,bitLength)) != -1)
	{
		++i;
	} 
	if(bitLength != 0)
		++i;
	return i;
}

inline int cacheUnitMarkBits::GetSeriesSetBits(u32 startGetIndex,u32 endGetIndex,u32 &startIndex,u32 &bitLength)
{
	assert(startGetIndex < markBitNum);
	assert(endGetIndex < markBitNum);
	assert(startGetIndex <= endGetIndex);
	u32 intBegin = startGetIndex/BITALIGN;
	u32 intEnd = endGetIndex/BITALIGN;
	u32 bitBeginOffset = startGetIndex%BITALIGN;
	u64 mark = 1;
	startIndex = 0;
	bitLength = 0;
	for(u32 i = intBegin;i<intEnd+1;++i)
	{
		for(u32 j = bitBeginOffset ; j<BITALIGN && (j+i*BITALIGN)<=endGetIndex ; ++j)
		{
			if((mark<<j)&bs[i])
			{
				startIndex = (bitLength == 0)?j+i*BITALIGN:startIndex;
				++bitLength;
			}
			else
			{
				if(bitLength != 0)
				{
					return (startIndex+bitLength+1>endGetIndex)?-1:(startIndex+bitLength+1);
				}
			}
		}
		bitBeginOffset = 0;
	}
	return -1;
}

inline bool cacheUnitMarkBits::FindSeriesSetBits(u32 startFindIndex,u32 endFindIndex,u32 &SeriesBitsNum,u32 &startIndex,u32 &bitLength)
{
	int tp = startFindIndex;
	SeriesBitsNum = (endFindIndex - startFindIndex+1) < SeriesBitsNum ? (endFindIndex - startFindIndex+1):SeriesBitsNum;
	while((tp = GetSeriesSetBits(tp,endFindIndex,startIndex,bitLength)) != -1)
	{
		if(bitLength>=SeriesBitsNum)
		{
			return true;
		}
	}

	if(bitLength>=SeriesBitsNum)
	{
		return true;
	}

	return false;
}

inline bool cacheUnitMarkBits::IsAllMarkBitSet()
{
	u32 startIn = 0;
	u32 endIn = startIn+markBitNum-1;
	u32 startOut = 0;
	u32 endOut = 0;
	GetSeriesSetBits(startIn,endIn,startOut,endOut);
	//return (startIn == startOut && endIn == endOut);
	return (endOut == markBitNum);
}

//inline bool cacheUnitMarkBits::IsLastBitSet()
//{
//	u32 startIn = markBitNum-2;
//	u32 endIn = markBitNum-1;
//	u32 startOut = 0;
//	u32 endOut = 0;
//	GetSeriesSetBits(startIn,endIn,startOut,endOut);
//	return (startIn == startOut && endIn == endOut);
//}

bool cacheUnit::CUWriteBytes(void *buf,u32 bufSize,u32 CUOffset)
{
	bool ret = false;
	assert(mop != NULL);
	assert(buf != NULL);
	assert(bufSize > 0 && bufSize%DISKMINSECTORSIZE == 0);
	assert(CUOffset%DISKMINSECTORSIZE == 0);
	lk->GetLock();
	LOG_NORMAL("<threadId:%lu>\t[%s] buf[%p],bufSize[0x%x],cacheUnitID[%d],CUOffset[0x%x] \n",pthread_self(),__FUNCTION__,buf,bufSize,cacheUnitID,CUOffset);
	ret = mop->MPWriteBytes(buf,bufSize,cacheUnitID,CUOffset);
	lk->ReleseLock();
	return ret;
}

bool cacheUnit::CUReadBytes(void *buf,u32 bufSize,u32 CUOffset)
{
	bool ret = false;
	assert(mop != NULL);
	assert(buf != NULL);
	assert(bufSize > 0 && bufSize%DISKMINSECTORSIZE == 0);
	assert(CUOffset%DISKMINSECTORSIZE == 0);
	lk->GetLock();
	ret = mop->MPReadBytes(buf,bufSize,cacheUnitID,CUOffset);
	lk->ReleseLock();
	return ret;
}

inline char* cacheUnit::GetCacheUnitBuf()
{
	return mop->GetCacheUnitBuf(cacheUnitID);
}

inline void cacheUnit::FreeCacheUnitBuf()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  thread  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
threadUnit::threadUnit(LPTHREAD_START_ROUTINE pwtPar,void *threadPar):par(threadPar),endThread(false),pwt(NULL),islive(false)
{
	if(pwtPar == NULL)
	{
		LOG_NORMAL("<threadId:%lu>\t[%s]pwtPar == NULL\n",pthread_self(),__FUNCTION__);
		assert(0);
		return;
	}
	pwt = pwtPar;

	isActive.SetMode(TRUE);
	isEnd.SetMode(TRUE);
	int nret = fi_create_thread(&hThread, TemplateThread, this);
	assert(!nret);
}

threadUnit::~threadUnit()
{
	endThread = true;
	isActive.Set();
	if(wait_timeout == isEnd.Wait(WAITTHREADENDTIME))
	{
		isActive.Set();
		isEnd.Wait(WAITTHREADENDTIME);
		pthread_exit(NULL);
	}
}

void threadUnit::WorkThreadWrapper()
{
	while(1)
	{
		//BEGINMARK_INMEM(1);
		//BEGINMARK_INMEM(2);
		isActive.Wait();
		//ENDMARKANDLOG_INMEM(1,logWait);
		if(endThread)
		{
			break;
		}
		assert(pwt != NULL);
		(*pwt)(par);
		//ENDMARKANDLOG_INMEM(2,logAll);
	}
	isEnd.Set();
}

void threadUnit::ActiveThread()
{
	if(isActive.Set())
	{
		islive = true;
	}
	else
	{
		LOG_NORMAL("<threadId:%lu>\t[%s]	Failed!\n",pthread_self(),__FUNCTION__);
	}
}

void threadUnit::HaltThread()
{
	if(isActive.Reset())
	{
		islive = false;
	}
	else
	{
		LOG_NORMAL("<threadId:%lu>\t[%s]	Failed!\n",pthread_self(),__FUNCTION__);
	}
}


ThreadPool::ThreadPool(int threadNum,LPTHREAD_START_ROUTINE pwtPar,void *par):thrdNum(threadNum),activateThrdNum(0)
{
	assert(pwtPar != NULL && threadNum > 0);
	thrdPool.reserve(threadNum);
	pwt = pwtPar;
	for(u32 i = 0;i<thrdNum;++i)
	{
		thrdPool.push_back(new threadUnit(pwt,par));
	}

	thrdPoolLock = new lockUnix();
	assert(thrdPoolLock != NULL);
}

ThreadPool::~ThreadPool()
{
	thrdPoolLock->GetLock();
	for(u32 i = 0;i<thrdNum;++i)
	{
		delete thrdPool[i];
	}
	thrdPoolLock->ReleseLock();
	delete thrdPoolLock;
}

void ThreadPool::ActiveThreads(u32 threadNum)
{
	thrdPoolLock->GetLock();
	assert(activateThrdNum <= thrdNum);
	if(activateThrdNum == thrdNum)
	{
		thrdPoolLock->ReleseLock();
		return;
	}

	if(threadNum > thrdNum - activateThrdNum)
	{
		activateThrdNum = thrdNum;
		for_each(thrdPool.begin(), thrdPool.end(), ActiveThreadsOp());
	}
	else
	{
		u32 oatn = activateThrdNum;
		activateThrdNum += threadNum;
		for_each(thrdPool.begin() + oatn, thrdPool.begin() + activateThrdNum, ActiveThreadsOp());
	}
	thrdPoolLock->ReleseLock();
}

void ThreadPool::HaltThreads(u32 threadNum)
{
	thrdPoolLock->GetLock();
	assert(activateThrdNum <= thrdNum);
	if(activateThrdNum == 0)
	{
		thrdPoolLock->ReleseLock();
		return;
	}

	if(threadNum > activateThrdNum)
	{
		activateThrdNum = 0;
		for_each(thrdPool.begin(), thrdPool.end(), HaltThreadsOp());
	}
	else
	{
		u32 oatn = activateThrdNum;
		activateThrdNum -= threadNum;
		for_each(thrdPool.begin() + activateThrdNum, thrdPool.begin() + oatn, HaltThreadsOp());
	}
	thrdPoolLock->ReleseLock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  DiskCacheCom  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void* LazyWriteThread(LPVOID par)
{
	cacheReadWrite *pCache = (cacheReadWrite *)par;
	DiskCacheCom *pDiskCache = pCache->GetDiskCache();
	assert(pDiskCache!=NULL);
	pDiskCache->DCCFlushDirtyList(pCache->GetNextLayer(),false);

	return NULL;
}

DiskCacheNode::DiskCacheNode(cacheUnit *cuPar):cu(cuPar),isFlushing(FALSE)
{
	markTimeInValidList = 0;
	markTimeInDirtyList = 0;
	assert(cu != NULL);
	pDiskInfo = new IoDiskInfo;
	assert(pDiskInfo != NULL);
	memset(pDiskInfo,0,sizeof(IoDiskInfo));
	pDiskInfo->ioDisklength = cu->GetMemPool()->GetCacheUnitBufSize();
	bitmap = new cacheUnitMarkBits(cu->GetMemPool()->GetCacheUnitBufSize()/DISKMINSECTORSIZE);
	assert(bitmap != NULL);
	ErrRetAry.reserve(2);
	isFlushDone.SetMode(TRUE);
}

DiskCacheNode::~DiskCacheNode()
{
	if(pDiskInfo != NULL)
	{
		delete pDiskInfo;
	}
	if(bitmap != NULL)
	{
		delete bitmap;
	}
	cu = NULL;
}

inline void DiskCacheNode::GetWriteDeviceIOAry(vector<DiskIoPar> &collectWriteDeviceIO)
{
	DiskIoPar diskWriteIoPar;
	int tp = 0;
	u32 startIndex = 0;
	u32 bitLength = 0;
	while((tp = bitmap->GetSeriesSetBits(tp,bitmap->GetMarkBitNum()-1,startIndex,bitLength)) != -1)
	{
		diskWriteIoPar.diskId = pDiskInfo->diskId;
		diskWriteIoPar.ioBuf = cu->GetCacheUnitBuf()+startIndex*DISKMINSECTORSIZE;
		diskWriteIoPar.diskLength = bitLength*DISKMINSECTORSIZE;
		diskWriteIoPar.diskOffset = pDiskInfo->ioDiskOffset+startIndex*DISKMINSECTORSIZE;
		collectWriteDeviceIO.push_back(diskWriteIoPar);

		LOG_NORMAL("<threadId:%lu> diskWriteIoPar.ioBuf (%p)\n",pthread_self(), diskWriteIoPar.ioBuf );
	} 

	if(bitLength != 0)
	{
		diskWriteIoPar.diskId = pDiskInfo->diskId;
		diskWriteIoPar.ioBuf = cu->GetCacheUnitBuf()+startIndex*DISKMINSECTORSIZE;
		diskWriteIoPar.diskLength = bitLength*DISKMINSECTORSIZE;
		diskWriteIoPar.diskOffset = pDiskInfo->ioDiskOffset+startIndex*DISKMINSECTORSIZE;
		collectWriteDeviceIO.push_back(diskWriteIoPar);

		LOG_NORMAL("<threadId:%lu> diskWriteIoPar.ioBuf (%p)\n",pthread_self(), diskWriteIoPar.ioBuf );
	}
}

inline bool DiskCacheNode::DCNWriteBytes(void *buf,u32 bufSize,u32 DCNOffset,ErrRetNode &ern)
{
	bool ret = true;
	assert(bufSize+DCNOffset-1<cu->GetMemPool()->GetCacheUnitBufSize());
	for(eraIt = ErrRetAry.begin();eraIt != ErrRetAry.end();++eraIt)
	{
		if(ern.fileId == eraIt->fileId && ern.pcId == eraIt->pcId)
		{
			break;
		}
	}
	if(eraIt == ErrRetAry.end())
	{
		ErrRetAry.push_back(ern);
	}
	ret = cu->CUWriteBytes(buf,bufSize,DCNOffset);
	bitmap->SetBitRange(DCNOffset/DISKMINSECTORSIZE,bufSize/DISKMINSECTORSIZE);
#ifdef _f0xCacheDbg
	{
		assert(bitmap->GetSeriesSetBitsNum());
		LOG_READAHEADRUN("<threadId:%lu>\t[%s]dcn[%p],DCNOffset[0x%x],bufSize[0x%x],GetSeriesSetBitsNum[%d]\n",pthread_self(),__FUNCTION__,this,DCNOffset,bufSize,bitmap->GetSeriesSetBitsNum());
	}
#endif
	return ret;
}

inline bool DiskCacheNode::DCNReadBytes(void *buf,u32 bufSize,u32 DCNOffset,bool isInDirtyList,bool readRealDevice,vector<DiskIoPar> &collectReadDeviceIO)
{
	bool ret = true;
	u32 startIndex = 0,bitLength = 0;
	u32 bufStartIndex = DCNOffset/DISKMINSECTORSIZE;
	u32 bufEndIndex = (DCNOffset+bufSize-1)/DISKMINSECTORSIZE;
	u32 minRCS = isInDirtyList?1:minReadCopySize;
	char* bufLog = (char*)buf; 
	u64 diskOffsetLog = (u64)DCNOffset+pDiskInfo->ioDiskOffset;

	DiskIoPar diskReadIoPar;
	//DiskReadIoBitMapPar driobmp;
	//diskReadIoPar.bmOp.reserve(18);

	LOG_NORMAL("<threadId:%lu>\t[%s] isInDirtyList[%s]\n",pthread_self(),__FUNCTION__,isInDirtyList?"true":"false");
	assert(bufSize+DCNOffset-1<cu->GetMemPool()->GetCacheUnitBufSize());

	if(readRealDevice||	\
		(!isInDirtyList&&IsTimeoutInValidList())||	\
		!bitmap->FindSeriesSetBits(bufStartIndex,bufEndIndex,minRCS,startIndex,bitLength))
	{
		diskReadIoPar.diskId = pDiskInfo->diskId;
		diskReadIoPar.ioBuf = (char *)buf;
		diskReadIoPar.diskLength = bufSize;
		diskReadIoPar.diskOffset = (u64)DCNOffset+pDiskInfo->ioDiskOffset;
		//if(!isInDirtyList)
		//{
		//	driobmp.bitmap = bitmap;
		//	driobmp.startIndex = DCNOffset/DISKMINSECTORSIZE;
		//	driobmp.bitLength = bufSize/DISKMINSECTORSIZE;
		//	driobmp.readUnitBuf = (char *)buf;
		//	driobmp.cu = cu;
		//	diskReadIoPar.bmOp.clear();
		//	diskReadIoPar.bmOp.push_back(driobmp);
		//}
		RELEASE_ASSERT((diskReadIoPar.diskOffset-diskOffsetLog) == (diskReadIoPar.ioBuf - bufLog));
		RecordeReadRealDeviceIO(collectReadDeviceIO,diskReadIoPar);
	}
	else
	{
		u32 logBitOffset = bufStartIndex;
		int tp = startIndex+bitLength+1;	//tp = start index next find series set bits
		char *pBuf = (char *)buf;
		u32 movLength = 0;
		u64 diskOffset = 0;
		LOG_NORMAL("<threadId:%lu>\t[%s] minRCS[%d];startIndex[%d];bitLength[%d];tp[%d];pBuf[%p];movLength[0x%x];diskOffset[0x%llx]\n",	\
			pthread_self(),__FUNCTION__,minRCS,startIndex,bitLength,tp,pBuf,movLength,diskOffset);
		do 
		{
			if(bitLength>=minRCS)	// >minReadCopySize copy (default = 8--->8*512=4k)
			{
				movLength = (startIndex-logBitOffset)*DISKMINSECTORSIZE;
				if(movLength != 0)
				{
					diskOffset = pDiskInfo->ioDiskOffset+(u64)(logBitOffset*DISKMINSECTORSIZE);

					diskReadIoPar.diskId = pDiskInfo->diskId;
					diskReadIoPar.ioBuf = (char *)pBuf;
					diskReadIoPar.diskLength = movLength;
					diskReadIoPar.diskOffset = diskOffset;
					//if(!isInDirtyList)
					//{
					//	driobmp.bitmap = bitmap;
					//	driobmp.startIndex = logBitOffset;
					//	driobmp.bitLength = movLength/DISKMINSECTORSIZE;
					//	driobmp.readUnitBuf = (char *)buf;
					//	driobmp.cu = cu;
					//	diskReadIoPar.bmOp.clear();
					//	diskReadIoPar.bmOp.push_back(driobmp);
					//}
					RELEASE_ASSERT((diskReadIoPar.diskOffset-diskOffsetLog) == (diskReadIoPar.ioBuf - bufLog));
					RecordeReadRealDeviceIO(collectReadDeviceIO,diskReadIoPar);
				}
				pBuf += movLength;
				ret = cu->CUReadBytes(pBuf,bitLength*DISKMINSECTORSIZE,startIndex*DISKMINSECTORSIZE);
				assert(ret);
				logBitOffset = tp-1;
				if(logBitOffset >= bufEndIndex)
				{
					bitLength = 0;
					return ret;
				}
				pBuf += bitLength*DISKMINSECTORSIZE;
				LOG_NORMAL("<threadId:%lu>\t[%s] minRCS[%d];startIndex[%d];bitLength[%d];tp[%d];pBuf[%p];movLength[0x%x];diskOffset[0x%llx]\n",	\
					pthread_self(),__FUNCTION__,minRCS,startIndex,bitLength,tp,pBuf,movLength,diskOffset);
			}
		} while((tp = bitmap->GetSeriesSetBits(tp,bufEndIndex,startIndex,bitLength)) != -1);

		if(bitLength>=minRCS)	// >minReadCopySize copy (default = 8--->8*512=4k)
		{
			movLength = (startIndex-logBitOffset)*DISKMINSECTORSIZE;
			if(movLength != 0)
			{
				diskOffset = pDiskInfo->ioDiskOffset+(u64)(logBitOffset*DISKMINSECTORSIZE);

				diskReadIoPar.diskId = pDiskInfo->diskId;
				diskReadIoPar.ioBuf = (char *)pBuf;
				diskReadIoPar.diskLength = movLength;
				diskReadIoPar.diskOffset = diskOffset;
				//if(!isInDirtyList)
				//{
				//	driobmp.bitmap = bitmap;
				//	driobmp.startIndex = logBitOffset;
				//	driobmp.bitLength = movLength/DISKMINSECTORSIZE;
				//	driobmp.readUnitBuf = (char *)buf;
				//	driobmp.cu = cu;
				//	diskReadIoPar.bmOp.clear();
				//	diskReadIoPar.bmOp.push_back(driobmp);
				//}
				RELEASE_ASSERT((diskReadIoPar.diskOffset-diskOffsetLog) == (diskReadIoPar.ioBuf - bufLog));
				RecordeReadRealDeviceIO(collectReadDeviceIO,diskReadIoPar);
			}
			pBuf += movLength;
			ret = cu->CUReadBytes(pBuf,bitLength*DISKMINSECTORSIZE,startIndex*DISKMINSECTORSIZE);
			LOG_NORMAL("<threadId:%lu>\t[%s] minRCS[%d];startIndex[%d];bitLength[%d];tp[%d];pBuf[%p];movLength[0x%x];diskOffset[0x%llx]\n",	\
				pthread_self(),__FUNCTION__,minRCS,startIndex,bitLength,tp,pBuf,movLength,diskOffset);
			assert(ret);
		}
		else
		{
			movLength = (bufEndIndex+1-logBitOffset)*DISKMINSECTORSIZE;
			diskOffset = pDiskInfo->ioDiskOffset+(u64)(logBitOffset*DISKMINSECTORSIZE);

			diskReadIoPar.diskId = pDiskInfo->diskId;
			diskReadIoPar.ioBuf = (char *)pBuf;
			diskReadIoPar.diskLength = movLength;
			diskReadIoPar.diskOffset = diskOffset;
			//if(!isInDirtyList)
			//{
			//	driobmp.bitmap = bitmap;
			//	driobmp.startIndex = logBitOffset;
			//	driobmp.bitLength = movLength/DISKMINSECTORSIZE;
			//	driobmp.readUnitBuf = (char *)buf;
			//	driobmp.cu = cu;
			//	diskReadIoPar.bmOp.clear();
			//	diskReadIoPar.bmOp.push_back(driobmp);
			//}
			RELEASE_ASSERT((diskReadIoPar.diskOffset-diskOffsetLog) == (diskReadIoPar.ioBuf - bufLog));
			RecordeReadRealDeviceIO(collectReadDeviceIO,diskReadIoPar);
			LOG_NORMAL("<threadId:%lu>\t[%s] minRCS[%d];startIndex[%d];bitLength[%d];tp[%d];pBuf[%p];movLength[0x%x];diskOffset[0x%llx]\n",	\
				pthread_self(),__FUNCTION__,minRCS,startIndex,bitLength,tp,pBuf,movLength,diskOffset);
		}
	}
	return ret;
}

inline bool DiskCacheNode::IsTimeoutInValidList()
{
	bool ret = true;
	ret = IsTimeout_Sub(markTimeInValidList,timeoutInValidList);
	if(ret)
	{
		//TRACE("[%s]	IsTimeout !!!\n");
	}
	return ret;
}

inline bool DiskCacheNode::IsTimeoutInDirtyList()
{
	bool ret = true;
	ret = IsTimeout_Sub(markTimeInDirtyList,timeoutInDirtyList);
	if(ret)
	{
		//TRACE("[%s]	IsTimeout !!!\n");
	}
	return ret;
}

inline ObjID& DiskCacheNode::GetDiskId()
{
	return pDiskInfo->diskId;
}

inline u64 DiskCacheNode::GetDiskOffset()
{
	return pDiskInfo->ioDiskOffset;
}

inline u32 DiskCacheNode::GetDiskLength()
{
	return pDiskInfo->ioDisklength;
}

inline cacheUnit* DiskCacheNode::GetCacheUnit()
{
	return cu;
}

inline void DiskCacheNode::CleanBitMap()
{
	//LOG_NORMAL("<threadId:%lu>\t[%s] dcn[%p]\n",pthread_self(),__FUNCTION__,this);
	bitmap->ClearAllBits();
}

inline bool DiskCacheNode::IsFull()
{
	return bitmap->IsAllMarkBitSet();
}

inline void DiskCacheNode::SetDiskInfo(ObjID diskId,u64 diskOffset,u32 cacheUnitSize)
{
	pDiskInfo->diskId = diskId;
	pDiskInfo->ioDiskOffset = diskOffset;
	pDiskInfo->ioDisklength = cacheUnitSize;
}

inline void DiskCacheNode::SetMarkTimeInValidList()
{
	markTimeInValidList = GetMicroSecTime();
}

inline void DiskCacheNode::SetMarkTimeInDirtyList()
{
	markTimeInDirtyList = GetMicroSecTime();
}

inline void DiskCacheNode::RecordeReadRealDeviceIO(vector<DiskIoPar> &crdio,DiskIoPar &driop)
{
	//assert(driop.bmOp.size() == 0 || driop.bmOp.size() == 1);
	if(crdio.empty())
	{
		crdio.push_back(driop);
		return;
	}
	DiskIoPar &dip = *(crdio.end()-1);
	if(dip.diskOffset+dip.diskLength == driop.diskOffset)
	{
		dip.diskLength += driop.diskLength;
		//if(!driop.bmOp.empty())
		//{
		//	it->bmOp.push_back(*driop.bmOp.begin());
		//}
	}
	else
	{
		LOG_NORMAL("<threadId:%lu>\t[%s]crdio.push_back---ioBuf[%p],diskLength[0x%x],diskOffset[0x%llx]\n",pthread_self(),__FUNCTION__,driop.ioBuf,driop.diskLength,driop.diskOffset);
		crdio.push_back(driop);
	}
}


DiskCacheCom::DiskCacheCom(ThreadPool *lwtp,u32 memSize,u32 cubs):lazyWriteTP(lwtp),mp(new memPool(memSize,cubs)),lastWriteDiskOffset(0),lastWriteDisklength(0)
{
	assert(mp != NULL);
	assert(lazyWriteTP != NULL);

	dirtList.reserve(mp->GetCacheUnitTotNum());
	dirtListLock = new lockUnix();
	assert(dirtListLock != NULL);

	validList.reserve(mp->GetCacheUnitTotNum());
	validListLock = new lockUnix();
	assert(validListLock != NULL);

	initList.reserve(mp->GetCacheUnitTotNum());
	for_each(mp->GetCacheUnitList().begin(),mp->GetCacheUnitList().end(),InitDiskCacheNode(initList));
	validList.insert(validList.begin(),initList.begin(),initList.end());
}

DiskCacheCom::~DiskCacheCom()
{
	validListLock->GetLock();
	dirtListLock->GetLock();
	for_each(initList.begin(),initList.end(),UnInitDiskCacheNode());
	if(mp != NULL)
	{
		delete mp;
	}
	dirtListLock->ReleseLock();
	validListLock->ReleseLock();
	delete validListLock;
	delete dirtListLock;
	lazyWriteTP = NULL;
}

bool DiskCacheCom::DCCFlushDirtyList(NextLayer *nl,bool isFlushAll)
{
	bool ret = true;
	DiskCacheNode *dcn = NULL;

	dirtListLock->GetLock();
	if(dirtList.empty())
	{
		lazyWriteTP->HaltAllThreads();
		dirtListLock->ReleseLock();
		return false;
	}

//	if(cache->GetIsServer())
//	{
		sort(dirtList.begin(),dirtList.end(),SortDirtList());
		dccDirtyIt = find_if(dirtList.begin(),dirtList.end(),FindSortFlushNode(lastWriteDiskOffset,lastWriteDisklength));
		//VIEWUSEDTIME("Sort diskio dirty list for write use time");
		if(dccDirtyIt == dirtList.end())
		{
			dccDirtyIt = find_if(dirtList.begin(),dirtList.end(),FindFlushNode());
		}
//	}
//	else
//	{
//		dccDirtyIt = find_if(dirtList.begin(),dirtList.end(),FindFlushNode());
//	}

	if(dccDirtyIt == dirtList.end())
	{
		lazyWriteTP->HaltAllThreads();
		dirtListLock->ReleseLock();
		return false;
	}

	dcn = (*dccDirtyIt);
	if(!isFlushAll&&!dcn->IsFull()&&!dcn->IsTimeoutInDirtyList())
	{
		LOG_NORMAL("<threadId:%lu>\t[%s] Not flush dcn[%p] & Sleep\n",pthread_self(),__FUNCTION__,dcn);
		dirtListLock->ReleseLock();
		Sleep(180);
		return false;
	}

	LOG_NORMAL("<threadId:%lu>\t[%s] dcn[%p] is flushing\n",pthread_self(),__FUNCTION__,dcn);
	dcn->GetIsFlushDone().Reset();
	dcn->SetIsFlushing(TRUE);
	lastWriteDiskOffset = dcn->GetDiskOffset();
	lastWriteDisklength = dcn->GetDiskLength();
	dirtListLock->ReleseLock();

	vector<DiskIoPar> collectWriteDeviceIO;
	collectWriteDeviceIO.reserve(13);
	dcn->GetWriteDeviceIOAry(collectWriteDeviceIO);

	LOG_NORMAL("<threadId:%lu>\t[%s] collectWriteDeviceIO.size[%lu]\n",pthread_self(),__FUNCTION__,collectWriteDeviceIO.size());
	assert(collectWriteDeviceIO.size());
	//write real device
	for(vector<DiskIoPar>::iterator dipWriteIt = collectWriteDeviceIO.begin();dipWriteIt!=collectWriteDeviceIO.end();++dipWriteIt)
	{
		nl->WriteDatas(dipWriteIt->diskId,dipWriteIt->ioBuf,dipWriteIt->diskLength,dipWriteIt->diskOffset);
	}
	dcn->GetErrRetAry().clear();
	dcn->GetCacheUnit()->FreeCacheUnitBuf();

	validListLock->GetLock();
	dcn->SetMarkTimeInValidList();

	dirtListLock->GetLock();
	dcn->SetIsFlushing(FALSE);
	dcn->GetIsFlushDone().Set();
	dccDirtyIt = find_if(dirtList.begin(),dirtList.end(),FindFlushDoneNode(dcn));
	RELEASE_ASSERT(dccDirtyIt != dirtList.end());
	dirtList.erase(dccDirtyIt);
	LOG_NORMAL("<threadId:%lu>\t[%s] dirtList.erase dcn[%p]\n",pthread_self(),__FUNCTION__,dcn);
	validList.push_back(dcn);
	dirtListLock->ReleseLock();

	validListNotEmpty.Set();
	validListLock->ReleseLock();
	return ret;
}

bool DiskCacheCom::DCCFlushAllDirtyListNode(NextLayer *nl)	
{
	dirtListLock->GetLock();
			
	if(dirtList.empty())
	{
		dirtListLock->ReleseLock();
		return false;
	}

	dccDirtyIt = find_if(dirtList.begin(),dirtList.end(),FindFlushNode());	
	if(dccDirtyIt == dirtList.end())
	{
		dirtListLock->ReleseLock();
		return false;
	}

	dirtListLock->ReleseLock();
	while(DCCFlushDirtyList(nl,true)){};
	return true;
}

bool DiskCacheCom::DCCWriteBytes(ObjID &diskId,void *writeBuf,u32 dcnSize,u32 dcnOffset,u64 diskOffset,ErrRetNode &ern)
{
	//INITTIME;
	bool ret = false;
	DiskCacheNode *dcn = NULL;
	LOG_NORMAL("<threadId:%lu>\t[%s] In\n",pthread_self(),__FUNCTION__);
redo_DCCWriteBytes:
	dirtListLock->GetLock();
	dccDirtyIt = find_if(dirtList.begin(),dirtList.end(),IsInSameDiskRange(diskId,diskOffset));
	if(dccDirtyIt != dirtList.end())
	{
		if((*dccDirtyIt)->GetIsFlushing())
		{
			FiEvent &isFlushDone = (*dccDirtyIt)->GetIsFlushDone();
			dirtListLock->ReleseLock();
			isFlushDone.Wait();
			goto redo_DCCWriteBytes;
		}
		LOG_NORMAL("<threadId:%lu>\t[%s] dirtList DCNWriteBytes \n",pthread_self(),__FUNCTION__);
		ret = (*dccDirtyIt)->DCNWriteBytes(writeBuf,dcnSize,dcnOffset,ern);
		assert(ret);
		//(*it)->ClearWaitStatus();
		dirtListLock->ReleseLock();
		goto DCCWRITEBYTES_SUB_END;
	}
	dirtListLock->ReleseLock();

	validListLock->GetLock();
	dccValidIt = find_if(validList.begin(),validList.end(),IsInSameDiskRange(diskId,diskOffset));
	if(dccValidIt != validList.end())
	{
		dcn = *dccValidIt;
		dcn->CleanBitMap();
		LOG_NORMAL("<threadId:%lu>\t[%s] validList DCNWriteBytes \n",pthread_self(),__FUNCTION__);
		ret = dcn->DCNWriteBytes(writeBuf,dcnSize,dcnOffset,ern);
		dirtListLock->GetLock();
		dirtList.push_back(dcn);
		LOG_NORMAL("<threadId:%lu>\t[%s] dirtList.push_back dcn[%p]\n",pthread_self(),__FUNCTION__,dcn);
		dcn->SetMarkTimeInDirtyList();
		lazyWriteTP->ActiveThreads(1);
		dirtListLock->ReleseLock();
		validList.erase(dccValidIt);
		validListLock->ReleseLock();
		goto DCCWRITEBYTES_SUB_END;
	}
	dccValidIt = find_if(validList.begin(),validList.end(),IsTimeoutDiskCacheNode());
	if(dccValidIt != validList.end())
	{
		dcn = *dccValidIt;
		dcn->SetDiskInfo(diskId,diskOffset,mp->GetCacheUnitBufSize());
		dcn->CleanBitMap();
		LOG_NORMAL("<threadId:%lu>\t[%s] validList Timeout DCNWriteBytes \n",pthread_self(),__FUNCTION__);
		ret = dcn->DCNWriteBytes(writeBuf,dcnSize,dcnOffset,ern);
		dirtListLock->GetLock();
		dirtList.push_back(dcn);
		LOG_NORMAL("<threadId:%lu>\t[%s] dirtList.push_back dcn[%p]\n",pthread_self(),__FUNCTION__,dcn);
		dcn->SetMarkTimeInDirtyList();
		lazyWriteTP->ActiveThreads(1);
		dirtListLock->ReleseLock();
		validList.erase(dccValidIt);
		validListLock->ReleseLock();
		goto DCCWRITEBYTES_SUB_END;
	}

	while(validList.empty())
	{
		//ResetEvent(validListNotEmpty);
		validListLock->ReleseLock();
		LOG_NORMAL("<threadId:%lu>\t[%s]validList.empty() & waiting\n",pthread_self(),__FUNCTION__);
		//INITTIME;
		validListNotEmpty.Wait();
		//VIEWUSEDTIME("Wait validList not empty");
		//Sleep(500);
		validListLock->GetLock();
	}

	dccValidIt = validList.begin();
	dcn = *dccValidIt;
	dcn->SetDiskInfo(diskId,diskOffset,mp->GetCacheUnitBufSize());
	dcn->CleanBitMap();
	LOG_NORMAL("<threadId:%lu>\t[%s] validList Last DCNWriteBytes \n",pthread_self(),__FUNCTION__);
	ret = dcn->DCNWriteBytes(writeBuf,dcnSize,dcnOffset,ern);
	dirtListLock->GetLock();
	dirtList.push_back(dcn);
	LOG_NORMAL("<threadId:%lu>\t[%s] dirtList.push_back dcn[%p]\n",pthread_self(),__FUNCTION__,dcn);
	dcn->SetMarkTimeInDirtyList();
	lazyWriteTP->ActiveThreads(1);
	dirtListLock->ReleseLock();
	validList.erase(dccValidIt);
	validListLock->ReleseLock();

DCCWRITEBYTES_SUB_END:
	return ret;
}

bool DiskCacheCom::DCCReadBytes(ObjID &diskId,void *readBuf,u32 dcnSize,u32 dcnOffset,u64 diskOffset,vector<DiskIoPar> &collectReadDeviceIO)
{
	bool ret = false;
	DiskCacheNode *dcn = NULL;
	LOG_NORMAL("<threadId:%lu>\t[%s] In\n",pthread_self(),__FUNCTION__);
//redo_DCCReadBytes:
	dirtListLock->GetLock();
	dccDirtyIt = find_if(dirtList.begin(),dirtList.end(),IsInSameDiskRange(diskId,diskOffset));
	if(dccDirtyIt != dirtList.end())
	{
		//if((*dccDirtyIt)->GetIsFlushing())
		//{
		//	HANDLE isFlushDone = (*dccDirtyIt)->GetIsFlushDone();
		//	dirtListLock->ReleseLock();
		//	WaitForSingleObject(isFlushDone,INFINITE);
		//	goto redo_DCCReadBytes;
		//}

		ret = (*dccDirtyIt)->DCNReadBytes(readBuf,dcnSize,dcnOffset,true,false,collectReadDeviceIO);
		dirtListLock->ReleseLock();
		goto DCCREADBYTES_SUB_END;
	}
	dirtListLock->ReleseLock();

	validListLock->GetLock();
	dccValidIt = find_if(validList.begin(),validList.end(),IsInSameDiskRange(diskId,diskOffset));
	if(dccValidIt != validList.end())
	{
		ret = (*dccValidIt)->DCNReadBytes(readBuf,dcnSize,dcnOffset,false,false,collectReadDeviceIO);
		validListLock->ReleseLock();
		goto DCCREADBYTES_SUB_END;
	}
	dccValidIt = find_if(validList.begin(),validList.end(),IsTimeoutDiskCacheNode());
	if(dccValidIt != validList.end())
	{
		dcn = *dccValidIt;
		dcn->SetDiskInfo(diskId,diskOffset,mp->GetCacheUnitBufSize());
		dcn->CleanBitMap();
		//if(cache->GetIsServer())
		//{
		//	dcn->SetMarkTimeInValidList();
		//}
		ret = dcn->DCNReadBytes(readBuf,dcnSize,dcnOffset,false,true,collectReadDeviceIO);
		validListLock->ReleseLock();
		goto DCCREADBYTES_SUB_END;
	}

	while(validList.empty())
	{
		//ResetEvent(validListNotEmpty);
		validListLock->ReleseLock();
		LOG_NORMAL("<threadId:%lu>\t[%s]validList.empty() & waiting\n",pthread_self(),__FUNCTION__);
		//INITTIME;
		validListNotEmpty.Wait();
		//VIEWUSEDTIME("Wait validList not empty");
		//Sleep(500);
		validListLock->GetLock();
	}

	dccValidIt = validList.begin();
	dcn = *dccValidIt;
	dcn->SetDiskInfo(diskId,diskOffset,mp->GetCacheUnitBufSize());
	dcn->CleanBitMap();
	//if(cache->GetIsServer())
	//{
	//	dcn->SetMarkTimeInValidList();
	//}
	ret = dcn->DCNReadBytes(readBuf,dcnSize,dcnOffset,false,true,collectReadDeviceIO);
	validListLock->ReleseLock();

DCCREADBYTES_SUB_END:
	return ret;
}

inline FiErr DiskCacheCom::DCCOperator(IoFileInfoHandle &fih,ObjID &diskId,void *buf,u32 bufSize,u64 DiskOffset,bool isWrite,vector<DiskIoPar> *collectReadDeviceIO)
{
	FiErr ret;
	if(bufSize == 0)
	{
		return ret;
	}

	char *tpBuf = (char *)buf;
	u32 alignSize = mp->GetCacheUnitBufSize();

	if(alignSize == 0)
	{
		return ret;
	}

	u32 dcnOffset = (u32)(DiskOffset%(u64)alignSize);
	u32 dcnSize = ((alignSize - dcnOffset)>bufSize)?bufSize:alignSize - dcnOffset;
	u32 leftBufSize = bufSize - dcnSize;
	u64 tpDiskOffset = DiskOffset - (u64)dcnOffset;

	ErrRetNode ern;
	memset(&ern,0,sizeof(ErrRetNode));
	
	if(isWrite)
	{
		ern.pcId = fih.GetPcId();
		ern.fileId = fih.GetFileId();
		DCCWriteBytes(diskId,tpBuf,dcnSize,dcnOffset,tpDiskOffset,ern);
	}
	else
	{
		collectReadDeviceIO->reserve(13);
		DCCReadBytes(diskId,tpBuf,dcnSize,dcnOffset,tpDiskOffset,*collectReadDeviceIO);
	}
	dcnOffset = 0;

	while(leftBufSize > 0) 
	{
		tpBuf += dcnSize;
		dcnSize = (leftBufSize>alignSize)?alignSize:leftBufSize;
		leftBufSize = leftBufSize - dcnSize;
		tpDiskOffset += alignSize;

		if(isWrite)
		{
			ern.pcId = fih.GetPcId();
			ern.fileId = fih.GetFileId();
			DCCWriteBytes(diskId,tpBuf,dcnSize,dcnOffset,tpDiskOffset,ern);
		}
		else
		{
			DCCReadBytes(diskId,tpBuf,dcnSize,dcnOffset,tpDiskOffset,*collectReadDeviceIO);
		}
	}

	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  FileCacheCom  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void* ReadAheadThread(LPVOID par)
{
	cacheReadWrite *pCache = (cacheReadWrite *)par;
	FileCacheCom *pFileCache = pCache->GetFileCache();
	assert(pFileCache!=NULL);
	pFileCache->FCCReadAhead(pCache->GetNextLayer());
	return NULL;
}

FileCacheNode::FileCacheNode(cacheUnit *cuPar):cu(cuPar),diskInfoNum(1),pfih(NULL),isRHReading(0),CNUsedSize(0)
{
	markTime = -1;	//init timeout
	assert(cu != NULL);
	pDiskInfo = new IoDiskInfo[diskInfoNum];
	assert(pDiskInfo != NULL);
	memset(pDiskInfo,0,sizeof(IoDiskInfo)*diskInfoNum);
	//readAheadLock = new lockUnix();
	//assert(readAheadLock != NULL);
	bitmap = new cacheUnitMarkBits(cu->GetMemPool()->GetCacheUnitBufSize()/DISKMINSECTORSIZE);
	assert(bitmap != NULL);
	bitmap->SetAllBits();
	isRHDone.SetMode(TRUE);
}

FileCacheNode::~FileCacheNode()
{
	//readAheadLock->GetLock();
	diskInfoNum = 0;
	if(pDiskInfo != NULL)
	{
		delete [] pDiskInfo;
	}
	//readAheadLock->ReleseLock();
	//delete readAheadLock;
	cu = NULL;
	if(pfih != NULL)
	{
		delete pfih;
	}
	if(bitmap != NULL)
	{
		delete bitmap;
	}
}

inline bool FileCacheNode::FCNWriteBytes(void *buf,u32 bufSize,u32 FCNOffset)
{
	bool ret = true;
	assert(bufSize+FCNOffset-1<cu->GetMemPool()->GetCacheUnitBufSize());
	ret = cu->CUWriteBytes(buf,bufSize,FCNOffset);
	return ret;
}

inline bool FileCacheNode::FCNReadBytes(void *buf,u32 bufSize,u32 FCNOffset)
{
	bool ret = true;
	assert(bufSize+FCNOffset-1<cu->GetMemPool()->GetCacheUnitBufSize());
	ret = cu->CUReadBytes(buf,bufSize,FCNOffset);
	return ret;
}

//inline void FileCacheNode::SetMarkTime()

inline void FileCacheNode::SetIsRHReading(u32 setVal)
{
	isRHReading = setVal;
}

inline u32 FileCacheNode::GetIsRHReading()
{
	return isRHReading;
}

inline bool FileCacheNode::IsTimeout()
{
	bool ret = true;
	if(markTime<0)
	{
		return ret;
	}
	ret = IsTimeout_Sub(markTime,timeout);
	if(ret)
	{
		//TRACE("[%s]	IsTimeout !!!\n");
	}
	return ret;
}

inline bool FileCacheNode::IsTimeoutFast(u64 &curTime)
{
	bool ret = true;
	if(markTime<0)
	{
		return ret;
	}
	assert(frequency != 0);
	if(!((u64)(curTime - markTime)>((u64)timeout*frequency)))
	{
		ret = false;
	}
	return ret;
}

inline void FileCacheNode::SetRHNeedInfo(IoDiskInfo *pDiskInfoPar,u32 diskInfoNumPar,IoFileInfoHandle &fihtp)
{
	assert(pDiskInfoPar != NULL);
	assert(diskInfoNumPar != 0);
	if(diskInfoNumPar>diskInfoNum)
	{
		diskInfoNum = diskInfoNumPar;
		delete [] pDiskInfo;
		pDiskInfo = new IoDiskInfo[diskInfoNum];
		assert(pDiskInfo != NULL);
		memcpy(pDiskInfo,pDiskInfoPar,sizeof(IoDiskInfo)*diskInfoNum);
	}
	else
	{
		diskInfoNum = diskInfoNumPar;
		memcpy(pDiskInfo,pDiskInfoPar,sizeof(IoDiskInfo)*diskInfoNum);
	}

	CNUsedSize = 0;
	for(u32 i = 0;i<diskInfoNum;++i)
	{
		CNUsedSize += pDiskInfo[i].ioDisklength;		
	}

	if(pfih!=NULL && pfih->GetIoFileInfoBuf()!=fihtp.GetIoFileInfoBuf())
	{
		delete pfih;
		pfih = new IoFileInfoHandle(fihtp);
		assert(pfih != NULL);
	}
	else if(pfih == NULL)
	{
		pfih = new IoFileInfoHandle(fihtp);
		assert(pfih != NULL);
	}
}

FilterDiskIoInValidList::FilterDiskIoInValidList(ObjID fid,FileCacheCom *fcc,vector<IoDiskInfo> &dia,vector<FileIoPar>::iterator &fipIt):fileId(fid),currentfcc(fcc),currentDiskIoAry(dia),mfipIt(fipIt){assert(currentfcc!=NULL);}
inline void FilterDiskIoInValidList::operator()(IoDiskInfo &di)
{
	vector<FileIoPar> collectReadDiskCacheIO;
	IoDiskInfo diTemp;
	diTemp.diskId = di.diskId;
	collectReadDiskCacheIO.reserve(3);
	currentfcc->CheckBufFromValidList(fileId,di.diskId,NULL,di.ioDisklength,di.ioDiskOffset,collectReadDiskCacheIO,NULL);
	for(mfipIt=collectReadDiskCacheIO.begin();mfipIt!=collectReadDiskCacheIO.end();++mfipIt)
	{
		diTemp.ioDiskOffset = mfipIt->diskOffset;
		diTemp.ioDisklength = mfipIt->diskLength;
		LOG_READAHEADRUN("<threadId:%lu>\t[%s]RHFiltedDiskIo.push_back---ioDiskOffset[0x%llx];ioDisklength[0x%x]\n",pthread_self(),__FUNCTION__,diTemp.ioDiskOffset,diTemp.ioDisklength);
		currentDiskIoAry.push_back(diTemp);
	}
}

FilterDiskIoInRHList::FilterDiskIoInRHList(FileCacheCom *fcc,vector<IoDiskInfo> &dia,vector<FileIoPar>::iterator &fipIt):currentfcc(fcc),currentDiskIoAry(dia),mfipIt(fipIt){assert(currentfcc!=NULL);}
inline void FilterDiskIoInRHList::operator()(IoDiskInfo &di)
{
	vector<FileIoPar> collectReadDiskCacheIO;
	IoDiskInfo diTemp;

	LOG_READAHEADRUN("<threadId:%lu>\t[%s] In\n",pthread_self(),__FUNCTION__);
	diTemp.diskId = di.diskId;
	collectReadDiskCacheIO.reserve(3);
	currentfcc->CheckBufFromRHList(di.diskId,NULL,di.ioDisklength,di.ioDiskOffset,collectReadDiskCacheIO,NULL);
	for(mfipIt=collectReadDiskCacheIO.begin();mfipIt!=collectReadDiskCacheIO.end();++mfipIt)
	{
		diTemp.ioDiskOffset = mfipIt->diskOffset;
		diTemp.ioDisklength = mfipIt->diskLength;
		LOG_READAHEADRUN("<threadId:%lu>\t[%s]RHFiltedDiskIo.push_back---ioDiskOffset[0x%llx];ioDisklength[0x%x]\n",pthread_self(),__FUNCTION__,diTemp.ioDiskOffset,diTemp.ioDisklength);
		currentDiskIoAry.push_back(diTemp);
	}
}


FileCacheReadAheadOp::FileCacheReadAheadOp()
{
	fileReadHistoryList.reserve(32);
	fileReadHistoryListLock = new lockUnix();
	assert(fileReadHistoryListLock != NULL);
}

FileCacheReadAheadOp::~FileCacheReadAheadOp()
{
	fileReadHistoryListLock->GetLock();
	for_each(fileReadHistoryList.begin(),fileReadHistoryList.end(),UnInitFileInfoNode());
	fileReadHistoryListLock->ReleseLock();
	delete fileReadHistoryListLock;
}

inline bool FileCacheReadAheadOp::LogUserSpeedTime(IoFileInfoHandle &fih)
{
	fileReadHistoryListLock->GetLock();
	fcrahIt1 = find_if(fileReadHistoryList.begin(),fileReadHistoryList.end(),IsTheSameUser(fih));
	if(fcrahIt1 != fileReadHistoryList.end())
	{
		(*fcrahIt1)->LogUserSpeedTime();
	}
#ifdef _f0xCacheDbg
	else
	{
		LOG_READAHEADRUN("<threadId:%lu>\t[%s]Not Found FileReadHistory\n",pthread_self(),__FUNCTION__);
	}
#endif
	fileReadHistoryListLock->ReleseLock();
	return true;
}

inline bool FileCacheReadAheadOp::AddReadCount(IoFileInfoHandle &fih)
{
	fileReadHistoryListLock->GetLock();
	fih.AddReadCount();
	fileReadHistoryListLock->ReleseLock();
	return true;
}

inline bool FileCacheReadAheadOp::AddHitCount(IoFileInfoHandle &fih)
{
	fileReadHistoryListLock->GetLock();
	fih.AddHitCount();
	fileReadHistoryListLock->ReleseLock();
	return true;
}

inline bool FileCacheReadAheadOp::CleanReadCount(IoFileInfoHandle &fih)
{
	fileReadHistoryListLock->GetLock();
	fih.CleanReadCount();
	fileReadHistoryListLock->ReleseLock();
	return true;
}

inline bool FileCacheReadAheadOp::CleanHitCount(IoFileInfoHandle &fih)
{
	fileReadHistoryListLock->GetLock();
	fih.CleanHitCount();
	fileReadHistoryListLock->ReleseLock();
	return true;
}

void TuneRHFiltedDiskIo(vector<IoDiskInfo> &RHFiltedDiskIoAry,u32 AlignSize,vector<IoDiskInfo>::iterator &idiIt)
{
	//LOG_READAHEADRUN("<threadId:%lu>\t[%s]~((u64)AlignSize-1)[0x%llx];~(AlignSize-1)[0x%x];AlignSize[0x%x]\n",pthread_self(),__FUNCTION__,~((u64)AlignSize-1),~(AlignSize-1),AlignSize);
	if(RHFiltedDiskIoAry.size() == 1)
	{
		RHFiltedDiskIoAry.begin()->ioDiskOffset &= ~((u64)AlignSize-1);
		if(RHFiltedDiskIoAry.begin()->ioDisklength%AlignSize != 0)
		{
			RHFiltedDiskIoAry.begin()->ioDisklength &= ~(AlignSize-1);
			RHFiltedDiskIoAry.begin()->ioDisklength += AlignSize;
		}
	}
	else
	{
		idiIt=RHFiltedDiskIoAry.begin();
		for(;idiIt!=RHFiltedDiskIoAry.end();++idiIt)
		{
			RHFiltedDiskIoAry.begin()->ioDiskOffset &= ~((u64)AlignSize-1);
			if(RHFiltedDiskIoAry.begin()->ioDisklength%AlignSize != 0)
			{
				RHFiltedDiskIoAry.begin()->ioDisklength &= ~(AlignSize-1);
				RHFiltedDiskIoAry.begin()->ioDisklength += AlignSize;
			}
		}

		SpaceNode snA,snB;
		for(idiIt=RHFiltedDiskIoAry.begin();(idiIt+1)!=RHFiltedDiskIoAry.end();)
		{
			if(idiIt->diskId == (idiIt+1)->diskId)
			{
				snA.start = idiIt->ioDiskOffset;
				snA.length = idiIt->ioDisklength;
				snB.start = (idiIt+1)->ioDiskOffset;
				snB.length = (idiIt+1)->ioDisklength;
				SpaceAnalyse sa(snA,snB);
				if(sa.IsHaveSameSpace())
				{
					idiIt->ioDiskOffset = (idiIt->ioDiskOffset<(idiIt+1)->ioDiskOffset)?idiIt->ioDiskOffset:(idiIt+1)->ioDiskOffset;
					idiIt->ioDisklength = idiIt->ioDisklength+(idiIt+1)->ioDisklength-(u32)sa.GetSameSapce()->length;
					RHFiltedDiskIoAry.erase(idiIt+1);
					continue;
				}
			}
			++idiIt;
		}
	}
}

void FileCacheReadAheadOp::MergeAndAlignDiskIoAry(vector<IoDiskInfo> &RHDiskIoAry)
{
	if(RHDiskIoAry.size() == 1)
	{
		RHDiskIoAry.begin()->ioDiskOffset &= 0xfffffffffffff000;
		if(RHDiskIoAry.begin()->ioDisklength%0x1000 != 0)
		{
			RHDiskIoAry.begin()->ioDisklength &= 0xfffff000;
			RHDiskIoAry.begin()->ioDisklength += 0x1000;
		}
	}
	else
	{
		idiIt=RHDiskIoAry.begin();
		for(;idiIt!=RHDiskIoAry.end();++idiIt)
		{
			RHDiskIoAry.begin()->ioDiskOffset &= 0xfffffffffffff000;
			if(RHDiskIoAry.begin()->ioDisklength%0x1000 != 0)
			{
				RHDiskIoAry.begin()->ioDisklength &= 0xfffff000;
				RHDiskIoAry.begin()->ioDisklength += 0x1000;
			}
		}

		SpaceNode snA,snB;
		for(idiIt=RHDiskIoAry.begin();(idiIt+1)!=RHDiskIoAry.end();)
		{
			if(idiIt->diskId == (idiIt+1)->diskId)
			{
				snA.start = idiIt->ioDiskOffset;
				snA.length = idiIt->ioDisklength;
				snB.start = (idiIt+1)->ioDiskOffset;
				snB.length = (idiIt+1)->ioDisklength;
				SpaceAnalyse sa(snA,snB);
				if(sa.IsHaveSameSpace())
				{
					idiIt->ioDiskOffset = (idiIt->ioDiskOffset<(idiIt+1)->ioDiskOffset)?idiIt->ioDiskOffset:(idiIt+1)->ioDiskOffset;
					idiIt->ioDisklength = idiIt->ioDisklength+(idiIt+1)->ioDisklength-(u32)sa.GetSameSapce()->length;
					RHDiskIoAry.erase(idiIt+1);
					continue;
				}
			}
			++idiIt;
		}
	}
}

bool FileCacheReadAheadOp::ReadAheadCheck(IoFileInfoHandle &fih,IoFileInfoHandle **pfihRet,vector<IoDiskInfo> &RHDiskIoAry)
{
	bool ret = false;
	IoFileInfoHandle *pfihtp = NULL;
	*pfihRet = NULL;
	fileReadHistoryListLock->GetLock();
	fcrahIt = find_if(fileReadHistoryList.begin(),fileReadHistoryList.end(),IsTheSameUser(fih));
	if(fcrahIt != fileReadHistoryList.end())
	{
		pfihtp = *fcrahIt;
		FileReadIoLogNode &newNode = fih.GetIoLogList()[0];
		vector<FileReadIoLogNode> &readIoLogList = pfihtp->GetIoLogList();

		pfihtp->SetMarkTime();
		assert(pfihtp->GetMarkUserSpeedTime() != 0);
		u32 userSpeed = pfihtp->CalUserReadSpeed(newNode.oriFilelength);
		if(readIoLogList.size() == MAXREADIOLOG)
		{
			readIoLogList.erase(readIoLogList.begin());
			//vector<FileReadIoLogNode>().swap(readIoLogList);
		}
		readIoLogList.push_back(newNode);
		pfihtp->SetLastFRL(newNode);
		LOG_READAHEADRUN("<threadId:%lu>\t[%s]newNode---fileOffset[0x%llx],fileLength[0x%x],IoLogListSize[0x%lu]\n",pthread_self(),__FUNCTION__,newNode.oriFileOffset,newNode.oriFilelength,readIoLogList.size());

		sort(readIoLogList.begin(),readIoLogList.end(),SortIoLogList());

		LOG_READAHEADRUN("<threadId:%lu>\t[%s]userSpeed[%d]\n",pthread_self(),__FUNCTION__,userSpeed);
		if(userSpeed>pfihtp->GetMaxReadAheadSize())
		{
			userSpeed = pfihtp->GetMaxReadAheadSize();
		}
		else if(userSpeed<pfihtp->GetMinReadAheadSize())
		{
			userSpeed = pfihtp->GetMinReadAheadSize();
		}
		else
		{
			userSpeed = (userSpeed%0xfffff000)?(userSpeed+0xfff)&0xfffff000:userSpeed;
		}
		LOG_READAHEADRUN("<threadId:%lu>\t[%s]RH userSpeed[%d]\n",pthread_self(),__FUNCTION__,userSpeed);
		vector<SpaceNode> RHFileIoAry;
		RHFileIoAry.reserve(3);
		*pfihRet = pfihtp;
		if(pfihtp->GetRHFileIoAry(RHFileIoAry,userSpeed,newNode))
		{
			assert(RHFileIoAry.size() != 0);
			LOG_READAHEADRUN("<threadId:%lu>\t[%s]RHFileIoAry size[%lu]\n",pthread_self(),__FUNCTION__,RHFileIoAry.size());

#ifdef _f0xCacheDbg
			{
				int iDbg=0;
				for(vector<SpaceNode>::iterator itDbg=RHFileIoAry.begin();itDbg!=RHFileIoAry.end();++itDbg,++iDbg)
				{
					LOG_READAHEADRUN("<threadId:%lu>\t[%s]RHFileIoAry%d---start[0x%llx];length[0x%llx]\n",pthread_self(),__FUNCTION__,iDbg,itDbg->start,itDbg->length);
				}
			}
#endif
			for_each(RHFileIoAry.begin(),RHFileIoAry.end(),FileIoAry2DiskIoAry(pfihtp,RHDiskIoAry));
			LOG_READAHEADRUN("<threadId:%lu>\t[%s]GetDiskIoAry size[%lu]\n",pthread_self(),__FUNCTION__,RHDiskIoAry.size());
			if(RHDiskIoAry.empty())
			{
				fileReadHistoryListLock->ReleseLock();
				return ret;
			}
#ifdef _f0xCacheDbg
			{
				int iDbg=0;
				for(vector<IoDiskInfo>::iterator itDbg=RHDiskIoAry.begin();itDbg!=RHDiskIoAry.end();++itDbg,++iDbg)
				{
					LOG_READAHEADRUN("<threadId:%lu>\t[%s]RHDiskIoAryTemp%d---diskId[%s];ioDiskOffset[0x%llx],ioDisklength[0x%x]\n",pthread_self(),__FUNCTION__,iDbg,itDbg->diskId.GetIDStr().c_str(),itDbg->ioDiskOffset,itDbg->ioDisklength);
				}
			}
#endif
			MergeAndAlignDiskIoAry(RHDiskIoAry);
			assert(!RHDiskIoAry.empty());
#ifdef _f0xCacheDbg
			{
				int iDbg=0;
				for(vector<IoDiskInfo>::iterator itDbg=RHDiskIoAry.begin();itDbg!=RHDiskIoAry.end();++itDbg,++iDbg)
				{
					LOG_READAHEADRUN("<threadId:%lu>\t[%s]RHDiskIoAry%d---diskId[%s];ioDiskOffset[0x%llx],ioDisklength[0x%x]\n",pthread_self(),__FUNCTION__,iDbg,itDbg->diskId.GetIDStr().c_str(),itDbg->ioDiskOffset,itDbg->ioDisklength);
				}
			}
#endif
			ret = true;
		}
	}
	else
	{
		fih.SetMarkTime();
		fcrahIt = find_if(fileReadHistoryList.begin(),fileReadHistoryList.end(),FindTimeOutNode());
		if(fcrahIt != fileReadHistoryList.end())
		{
			IoFileInfoHandle *delOne = *fcrahIt;
			*fcrahIt = new IoFileInfoHandle(fih);
			delete delOne;
			LOG_READAHEADRUN("<threadId:%lu>\t[%s]fileReadHistoryList delete\n",pthread_self(),__FUNCTION__);
		}
		else
		{
			fileReadHistoryList.push_back(new IoFileInfoHandle(fih));
			LOG_READAHEADRUN("<threadId:%lu>\t[%s]fileReadHistoryList push_back\n",pthread_self(),__FUNCTION__);
		}
		*pfihRet = &fih;
		IoDiskInfo tp;
		tp.diskId = fih.GetDiskId();
		tp.ioDisklength = fih.GetDiskLength();
		tp.ioDiskOffset = fih.GetDiskOffset();
		u32 AlignSize = 0x20000;	 //align size 128k 
		tp.ioDiskOffset &= ~((u64)AlignSize-1);
		if(tp.ioDisklength%AlignSize != 0)
		{
			tp.ioDisklength &= ~(AlignSize-1);
			tp.ioDisklength += AlignSize;
		}
		RHDiskIoAry.push_back(tp);
		//MergeAndAlignDiskIoAry(RHDiskIoAry);

		ret = true;
	}
	fileReadHistoryListLock->ReleseLock();
	return ret;
}

FileCacheCom::FileCacheCom(DiskCacheCom *dccPar,ThreadPool *rhtp,u32 memSize,u32 cubs):readAheadTP(rhtp),mp(new memPool(memSize,cubs)),lastRHDiskOffset(0),lastRHDisklength(0),isRHReading(false)
{
	assert(mp != NULL);
	assert(readAheadTP != NULL);

	fileCacheValidList.reserve(mp->GetCacheUnitTotNum());
	fileCacheValidListLock = new lockUnix();
	assert(fileCacheValidListLock != NULL);

	fileCacheRHList.reserve(mp->GetCacheUnitTotNum());
	fileCacheRHListLock = new lockUnix();
	assert(fileCacheRHListLock != NULL);

	fileCacheList.reserve(mp->GetCacheUnitTotNum());
	for_each(mp->GetCacheUnitList().begin(),mp->GetCacheUnitList().end(),InitFileCacheNode(fileCacheList));
	fileCacheValidList.insert(fileCacheValidList.begin(),fileCacheList.begin(),fileCacheList.end());

	fcReadAheadOp = new FileCacheReadAheadOp();
	assert(fcReadAheadOp != NULL);

	fileCacheValidHashList.reserve(3000);
	//fileCacheRHHashList.reserve(3000);
}

FileCacheCom::~FileCacheCom()
{
	fileCacheRHListLock->GetLock();
	fileCacheValidListLock->GetLock();
	for_each(fileCacheRHList.begin(),fileCacheRHList.end(),UnLockFileCacheRHNode());
	for_each(fileCacheList.begin(),fileCacheList.end(),UnInitFileCacheNode());
	if(mp != NULL)
	{
		delete mp;
	}
	fileCacheValidListLock->ReleseLock();
	fileCacheRHListLock->ReleseLock();
	delete fileCacheValidListLock;
	delete fileCacheRHListLock;
	readAheadTP = NULL;
	if(fcReadAheadOp != NULL)
	{
		delete fcReadAheadOp;
	}
}

void FileCacheCom::CheckFromFileCacheNode(FileCacheNode *fcn,ObjID &diskId,vector<FileIoPar> &collectReadDiskCacheIO,vector<FCNReadIoPar> *pCollectReadFileCacheIO)
{
	vector<FileIoPar> temp;
	SpaceNode snA,snB;
	SpaceNode *sameSpace = NULL;
	SpaceNode *diffB = NULL;
	IoDiskInfo *pdi = fcn->GetDiskInfo();
	FileIoPar fip = {0};
	FCNReadIoPar fcnrip;
	u32 dcnOffsetSum = 0;
	bool needSwap = false;
	temp.reserve(9);

	LOG_PERFORMACE_DOWN("<threadId:%lu>\t[%s] in\n",pthread_self(),__FUNCTION__);
	if(pdi->ioDisklength == 0)
	{
		LOG_PERFORMACE_DOWN("<threadId:%lu>\t[%s] out ioDisklength[0x%x]\n",pthread_self(),__FUNCTION__,pdi->ioDisklength);
		return;
	}
	for(u32 i = 0;i<fcn->GetDiskInfoNum();++i)
	{
		LOG_PERFORMACE_DOWN("<threadId:%lu>\t[%s] GetDiskInfoNum i[%d]\n",pthread_self(),__FUNCTION__,i);
		if(pdi[i].diskId == diskId)
		{
			snA.start = pdi[i].ioDiskOffset;
			snA.length = (u64)pdi[i].ioDisklength;
			LOG_PERFORMACE_DOWN("<threadId:%lu>\t[%s] pdi---i[%d],start[0x%llx],length[0x%llx]\n",pthread_self(),__FUNCTION__,i,snA.start,snA.length);
			for(fipIt = collectReadDiskCacheIO.begin();fipIt != collectReadDiskCacheIO.end();++fipIt)
			{
				snB.start = fipIt->diskOffset;
				snB.length = (u64)fipIt->diskLength;
				SpaceAnalyse sa(snA,snB);
				if(sa.IsHaveSameSpace())
				{
					needSwap = true;
					sameSpace = sa.GetSameSapce();
					diffB = sa.GetBDiff();
					if(pCollectReadFileCacheIO != NULL)
					{
						fcnrip.fcn = fcn;
						fcnrip.buf = fipIt->ioBuf+(u32)(sameSpace->start-snB.start);
						fcnrip.bufSize = (u32)sameSpace->length;
						fcnrip.FCNOffset = dcnOffsetSum+(u32)(sameSpace->start-snA.start);
						pCollectReadFileCacheIO->push_back(fcnrip);
						LOG_SPEC("<threadId:%lu>\t[%s]CollectReadFileCacheIO push_back fcn[%p];buf[%p];FCNOffset[0x%x];lenth[0x%x]\n",pthread_self(),__FUNCTION__,fcn,fcnrip.buf,fcnrip.FCNOffset,fcnrip.bufSize);
						//fcn->FCNReadBytes(fipit->ioBuf+(u32)(sameSpace->start-snB.start),(u32)sameSpace->length,dcnOffsetSum+(u32)(sameSpace->start-snA.start));
					}
					if(diffB->length != 0)
					{
						fip.diskLength = (u32)diffB->length;
						fip.diskOffset = diffB->start;
						fip.ioBuf = fipIt->ioBuf+(u32)(diffB->start-snB.start);
						temp.push_back(fip);
						LOG_PERFORMACE_DOWN("<threadId:%lu>\t[%s]collectReadDiskCacheIO.push_back buf[%p];diskOffset[0x%llx];diskLength[0x%x]\n",pthread_self(),__FUNCTION__,fip.ioBuf,fip.diskOffset,fip.diskLength);
					}
					if((++diffB)->length != 0)
					{
						fip.diskLength = (u32)diffB->length;
						fip.diskOffset = diffB->start;
						fip.ioBuf = fipIt->ioBuf+(u32)(diffB->start-snB.start);
						temp.push_back(fip);
						LOG_PERFORMACE_DOWN("<threadId:%lu>\t[%s]collectReadDiskCacheIO.push_back buf[%p];diskOffset[0x%llx];diskLength[0x%x]\n",pthread_self(),__FUNCTION__,fip.ioBuf,fip.diskOffset,fip.diskLength);
					}
				}
			}

			if(needSwap)
			{
				collectReadDiskCacheIO.swap(temp);
				LOG_PERFORMACE_DOWN("<threadId:%lu>\t[%s]collectReadDiskCacheIO.swap---i[%d]\n",pthread_self(),__FUNCTION__,i);
			}

#ifdef _f0xCacheDbg
			{
				int iDbg=0;
				for(vector<FileIoPar>::iterator itDbg=collectReadDiskCacheIO.begin();itDbg!=collectReadDiskCacheIO.end();++itDbg,++iDbg)
				{
					LOG_PERFORMACE_DOWN("<threadId:%lu>\t[%s]collectReadDiskCacheIO%d---ioBuf[%p];diskOffset[0x%llx];diskLength[0x%x]\n",pthread_self(),__FUNCTION__,iDbg,itDbg->ioBuf,itDbg->diskOffset,itDbg->diskLength);
				}
			}
#endif
		}
		dcnOffsetSum += pdi[i].ioDisklength;
	}
}

inline void FileCacheCom::CheckBufFromValidList(ObjID fileId,ObjID &diskId,char *readBuf,u32 readBufSize,u64 DiskOffset,vector<FileIoPar> &collectReadDiskCacheIO,vector<FCNReadIoPar> *pCollectReadFileCacheIO)
{
	vector<FileCacheNode *>* ValidListTP = NULL;
	FileIoPar fip;
	fip.ioBuf = readBuf;
	fip.diskOffset = DiskOffset;
	fip.diskLength = readBufSize;
	collectReadDiskCacheIO.push_back(fip);

	u64 curTime = GetMicroSecTime();

	//LOG_NORMAL("<threadId:%lu>\t[%s]collect ReadDiskCacheIO size[%d] before CheckFromFileCacheNode\n",pthread_self(),__FUNCTION__,collectReadDiskCacheIO.size());
	LOG_NORMAL("<threadId:%lu>\t[%s]fileCacheValidList size[%lu]\n",pthread_self(),__FUNCTION__,fileCacheValidList.size());

	validListRange = equal_range(fileCacheValidHashList.begin(),		\
		fileCacheValidHashList.end(),	\
		fileId,	\
		FccHashNodeCompare());
	if (validListRange.first != validListRange.second)
	{
		//fccHashNode fhnFind = *(range.first);
		ValidListTP = validListRange.first->second;
		//LOG_SPEC("<threadId:%lu>\t[%s]fileCacheValidHashList find fileid[%d]\n",pthread_self(),__FUNCTION__,validListRange.first->first);
	}
	else
	{
		//LOG_SPEC("<threadId:%lu>\t[%s]fileCacheValidHashList not find\n",pthread_self(),__FUNCTION__);
		return;
	}
	
	for(fccValidIt = ValidListTP->begin();fccValidIt != ValidListTP->end();++fccValidIt)
	{
		if(collectReadDiskCacheIO.empty())
		{
			return;
		}
		if((*fccValidIt)->IsTimeoutFast(curTime))
		{
//#ifdef _f0xCacheDbg
//			{
//				IoDiskInfo *pdi = (*fccValidIt)->GetDiskInfo();
//				for(u32 iDbg = 0;iDbg<(*fccValidIt)->GetDiskInfoNum();++iDbg)
//				{
//					LOG_PERFORMACE_DOWN("<threadId:%lu>\t[%s]fcnDiskInfo%d---diskOffset[0x%llx];diskLength[0x%x]\n",pthread_self(),__FUNCTION__,iDbg,pdi[iDbg].ioDiskOffset,pdi[iDbg].ioDisklength);
//				}
//			}
//#endif
			continue;
		}
		//LOG_SPEC("<threadId:%lu>\t[%s]CheckFromFileCacheNode get fcn[%p]\n",pthread_self(),__FUNCTION__,*fccValidIt);
		CheckFromFileCacheNode(*fccValidIt,diskId,collectReadDiskCacheIO,pCollectReadFileCacheIO);
	}

#ifdef _f0xCacheDbg
	LOG_PERFORMACE_DOWN("<threadId:%lu>\t[%s]collect ReadDiskCacheIO size[%lu] after CheckFromFileCacheNode\n",pthread_self(),__FUNCTION__,collectReadDiskCacheIO.size());
	if(pCollectReadFileCacheIO != NULL)
	{
		LOG_PERFORMACE_DOWN("<threadId:%lu>\t[%s]Collect ReadFileCacheIO size[%lu] after CheckFromFileCacheNode\n",pthread_self(),__FUNCTION__,pCollectReadFileCacheIO->size());
	}
#endif

	////VIEWUSEDTIME("CheckBufFromValidList");
}

inline void FileCacheCom::CheckBufFromRHList(ObjID &diskId,char *readBuf,u32 readBufSize,u64 DiskOffset,vector<FileIoPar> &collectReadDiskCacheIO,vector<FCNReadIoPar> *pCollectReadFileCacheIO)
{
	FileIoPar fip;
	fip.ioBuf = readBuf;
	fip.diskOffset = DiskOffset;
	fip.diskLength = readBufSize;
	collectReadDiskCacheIO.push_back(fip);

	LOG_NORMAL("<threadId:%lu>\t[%s]collect ReadDiskCacheIO size[%lu] before CheckFromFileCacheNode\n",pthread_self(),__FUNCTION__,collectReadDiskCacheIO.size());
	for(fccRHIt = fileCacheRHList.begin();fccRHIt != fileCacheRHList.end();++fccRHIt)
	{
		if(collectReadDiskCacheIO.empty())
		{
			return;
		}
		CheckFromFileCacheNode(*fccRHIt,diskId,collectReadDiskCacheIO,pCollectReadFileCacheIO);
	}

#ifdef _f0xCacheDbg
	LOG_PERFORMACE_DOWN("<threadId:%lu>\t[%s]collect ReadDiskCacheIO size[%lu] after CheckFromFileCacheNode\n",pthread_self(),__FUNCTION__,collectReadDiskCacheIO.size());
	if(pCollectReadFileCacheIO != NULL)
	{
		LOG_PERFORMACE_DOWN("<threadId:%lu>\t[%s]Collect ReadFileCacheIO size[%lu] after CheckFromFileCacheNode\n",pthread_self(),__FUNCTION__,pCollectReadFileCacheIO->size());
	}
#endif
}

inline bool FileCacheCom::ReadFromDiskCache(IoFileInfoHandle &fih,vector<FileIoPar> &collectReadDiskCacheIO,char *readBuf,u32 readBufSize,u64 DiskOffset)
{
//	sort(collectReadDiskCacheIO.begin(),collectReadDiskCacheIO.end(),SortReadIO());
//	vector<FileIoPar>::iterator	fipItTP = collectReadDiskCacheIO.begin();
//	FileIoPar fip = *fipItTP;
//	for(++fipItTP;fipItTP != collectReadDiskCacheIO.end();++fipItTP)
//	{
//		if(fip.diskOffset+fip.diskLength == fipItTP->diskOffset)	//merge series io
//		{
//			fip.diskLength += fipItTP->diskLength;
//		}
//		else
//		{
//			assert(fip.diskLength<=readBufSize);
//			assert(fip.diskOffset>=DiskOffset && fip.diskOffset<DiskOffset+readBufSize);
//			assert(fip.ioBuf>=readBuf && fip.ioBuf<readBuf+readBufSize);
//			if(!dcc->DCCOperator(fih,fih.GetDiskId(),fip.ioBuf,fip.diskLength,fip.diskOffset,false))
//			{
//				return false;
//			}
//			fip = *fipItTP;
//		}
//	}
//
//	assert(fip.diskLength<=readBufSize);
//	assert(fip.diskOffset>=DiskOffset && fip.diskOffset<DiskOffset+readBufSize);
//	assert(fip.ioBuf>=readBuf && fip.ioBuf<readBuf+readBufSize);
//	if(!dcc->DCCOperator(fih,fih.GetDiskId(),fip.ioBuf,fip.diskLength,fip.diskOffset,false))	//handle last FileIoPar
//	{
//		return false;
//	}
	return true;
}

FiErr FileCacheCom::FCCWriteBytes(IoFileInfoHandle &fih,char *writeBuf,u32 writeBufSize,u64 DiskOffset)
{
	FiErr ret;
    pthread_t thId = pthread_self();
    pthread_attr_t thAttr;
    int policy = 0;
    int max_prio_for_policy = 0;
	pthread_attr_init(&thAttr);
	pthread_attr_getschedpolicy(&thAttr, &policy);
	max_prio_for_policy = sched_get_priority_max(policy);

	if(writeBufSize == 0)
	{
		LOG_NORMAL("<threadId:%lu>\t[%s]writeBufSize == 0\n",pthread_self(),__FUNCTION__);
		return ret;
	}
	assert(writeBuf!=NULL);

	LOG_READAHEADRUN("<threadId:%lu>\t[%s]writeBuf[%p];writeBufSize[0x%x];diskOffset[0x%llx]===FileId[%s]\n",pthread_self(),__FUNCTION__,writeBuf,writeBufSize,DiskOffset,fih.GetFileId().GetIDStr().c_str());
	fileCacheRHListLock->GetLock();
	fileCacheValidListLock->GetLock();

	do 
	{
		fccRHIt = find_if(fileCacheRHList.begin(),fileCacheRHList.end(),IsDiskRangeInRHList(fih,writeBufSize,DiskOffset));
		if(fccRHIt == fileCacheRHList.end())
		{
			break;
		}
		FiEvent &isRHDone = (*fccRHIt)->GetRHEventHandle();
		fileCacheValidListLock->ReleseLock();
		fileCacheRHListLock->ReleseLock();

		isRHDone.Wait();

		pthread_setschedprio(thId, max_prio_for_policy);

		fileCacheRHListLock->GetLock();
		fileCacheValidListLock->GetLock();

		pthread_setschedprio(thId, policy);

	} while (1);

	//VIEWUSEDTIME("Wait readahead back");

	if(!fileCacheValidList.empty()&&!fileCacheValidHashList.empty())
	{
		LOG_NORMAL("<threadId:%lu>\t[%s]SynFileCacheNode\n",pthread_self(),__FUNCTION__);
		validListRange = equal_range(fileCacheValidHashList.begin(),		\
			fileCacheValidHashList.end(),	\
			fih.GetFileId(),	\
			FccHashNodeCompare());
		if (validListRange.first != validListRange.second)
		{
			vector<FileCacheNode *>* fcTp = validListRange.first->second;
			for_each(fcTp->begin(),fcTp->end(),SynFileCacheNode(fih,writeBuf,writeBufSize,DiskOffset));	//Write to FileCache
		}
	}
#ifdef _f0xCacheDbg
	else
	{
		LOG_NORMAL("<threadId:%lu>\t[%s]fileCacheValidList is empty\n",pthread_self(),__FUNCTION__);
	}
#endif
	fileCacheValidListLock->ReleseLock();
	fileCacheRHListLock->ReleseLock();

	return ret;
}

FiErr FileCacheCom::FCCReadBytes(IoFileInfoHandle &fih,char *readBuf,u32 readBufSize,u64 DiskOffset,vector<FileIoPar> &collectReadDiskCacheIO)
{
	FiErr ret;
	bool isHit = false;
	bool isRH = false;
	if(readBufSize == 0)
	{
		LOG_NORMAL("<threadId:%lu>\t[%s]readBufSize == 0\n",pthread_self(),__FUNCTION__);
		return ret;
	}
	RELEASE_ASSERT(readBuf!=NULL);

	LOG_READAHEADRUN("<threadId:%lu>\t===%s===begin!!!\n",pthread_self(),__FUNCTION__);
	LOG_READAHEADRUN("<threadId:%lu>\t[%s]readBuf[%p];readBufSize[0x%x];diskOffset[0x%llx]===FileId[%s]\n",pthread_self(),__FUNCTION__,readBuf,readBufSize,DiskOffset,fih.GetFileId().GetIDStr().c_str());

	IoFileInfoHandle* pfih = NULL;
	vector<IoDiskInfo> RHDiskIoAry;
	fileCacheRHListLock->GetLock();
	fileCacheValidListLock->GetLock();

	LOG_READAHEADRUN("<threadId:%lu>\t===%s===ReadAhead check&start logic---begin!!!\n",pthread_self(),__FUNCTION__);
	if(fcReadAheadOp->ReadAheadCheck(fih,&pfih,RHDiskIoAry))
	{
		//VIEWUSEDTIME("ReadAhead Check");
		//LOGUSEDTIMEARY(ReadAheadCheck);
		assert(pfih!=NULL);
		isRH = true;
		fcReadAheadOp->AddReadCount(*pfih);
		StartReadAhead(RHDiskIoAry,*pfih);
	}
	else if(pfih!=NULL)
	{
		fcReadAheadOp->CleanReadCount(*pfih);
		fcReadAheadOp->CleanHitCount(*pfih);
	}
	LOG_READAHEADRUN("<threadId:%lu>\t===%s===ReadAhead check&start logic---end!!!\n",pthread_self(),__FUNCTION__);

	LOG_READAHEADRUN("<threadId:%lu>\t===%s===Wait readahead back---begin!!!\n",pthread_self(),__FUNCTION__);
	collectReadDiskCacheIO.reserve(3);
	vector<FCNReadIoPar> collectReadFileCacheIO;
	collectReadFileCacheIO.reserve(3);

	pthread_t thId = pthread_self();
    pthread_attr_t thAttr;
    int policy = 0;
    int max_prio_for_policy = 0;
	pthread_attr_init(&thAttr);
	pthread_attr_getschedpolicy(&thAttr, &policy);
	max_prio_for_policy = sched_get_priority_max(policy);

	do 
	{
		fccRHIt = find_if(fileCacheRHList.begin(),fileCacheRHList.end(),IsDiskRangeInRHList(fih,readBufSize,DiskOffset));
		if(fccRHIt == fileCacheRHList.end())
		{
			break;
		}
		FiEvent &isRHDone = (*fccRHIt)->GetRHEventHandle();
		fileCacheValidListLock->ReleseLock();
		fileCacheRHListLock->ReleseLock();

//		LOG_NORMAL("<threadId:%lu>\t[%s]InRHList waiting begin\n",pthread_self(),__FUNCTION__);
		isRHDone.Wait();

		pthread_setschedprio(thId, max_prio_for_policy);

		LOG_NORMAL("<threadId:%lu>\t[%s]InRHList waiting done\n",pthread_self(),__FUNCTION__);

		fileCacheRHListLock->GetLock();
		fileCacheValidListLock->GetLock();

		pthread_setschedprio(thId, policy);

	} while (1);
	LOG_READAHEADRUN("<threadId:%lu>\t===%s===Wait readahead back---end!!!\n",pthread_self(),__FUNCTION__);

	LOG_READAHEADRUN("<threadId:%lu>\t===%s===Read data from file read cache---begin!!!\n",pthread_self(),__FUNCTION__);
	CheckBufFromValidList(fih.GetFileId(),fih.GetDiskId(),readBuf,readBufSize,DiskOffset,collectReadDiskCacheIO,&collectReadFileCacheIO);
	if(!collectReadFileCacheIO.empty())
	{
#ifdef _f0xCacheDbg
		{
			int iDbg=0;
			for(vector<FCNReadIoPar>::iterator itDbg=collectReadFileCacheIO.begin();itDbg!=collectReadFileCacheIO.end();++itDbg,++iDbg)
			{
				LOG_SPEC("<threadId:%lu>\t[%s]collectReadFileCacheIO%d---fcn[%p];buf[%p];bufSize[0x%x];DCNOffset[0x%x]\n",pthread_self(),__FUNCTION__,iDbg,itDbg->fcn,itDbg->buf,itDbg->bufSize,itDbg->FCNOffset);
			}
		}
#endif
		isHit = true;
		for_each(collectReadFileCacheIO.begin(),collectReadFileCacheIO.end(),ReadFromFileCacheNode(fih,fileCacheValidList));	//Read from FileCache
	}
	LOG_READAHEADRUN("<threadId:%lu>\t===%s===Read data from file read cache---end!!!\n",pthread_self(),__FUNCTION__);

	LOG_READAHEADRUN("<threadId:%lu>\t===%s===Read data from write cache or stor---begin!!!\n",pthread_self(),__FUNCTION__);

	if (isHit&&isRH)
	{
		fcReadAheadOp->AddHitCount(*pfih);
	}

	fileCacheValidListLock->ReleseLock();
	fileCacheRHListLock->ReleseLock();

	if(collectReadDiskCacheIO.empty())
	{
		LOG_NORMAL("<threadId:%lu>\t[%s]all read from file cache!\n",pthread_self(),__FUNCTION__);
	}

	fcReadAheadOp->LogUserSpeedTime(fih);
	LOG_READAHEADRUN("<threadId:%lu>\t===%s===Read data from write cache or stor---end!!!\n",pthread_self(),__FUNCTION__);

	LOG_READAHEADRUN("<threadId:%lu>\t===%s===end!!!\n",pthread_self(),__FUNCTION__);
	return ret;
}

void FileCacheCom::AddFileCacheNodeToValidList(FileCacheNode *fcn,bool isAddFront)
{
	if(isAddFront)
	{
		fileCacheValidList.insert(fileCacheValidList.begin(),fcn);
	}
	else
	{
		fileCacheValidList.push_back(fcn);
	}

	if(fileCacheValidHashList.empty())
	{
		vector<FileCacheNode *> *hashNodeValTp = new vector<FileCacheNode *>;
		hashNodeValTp->reserve(18);
		hashNodeValTp->push_back(fcn);
		fileCacheValidHashList.push_back(make_pair(fcn->GetIoFileInfoHandle()->GetFileId(),hashNodeValTp));
		return;
	}

	validListRange = equal_range(fileCacheValidHashList.begin(),		\
						fileCacheValidHashList.end(),	\
						fcn->GetIoFileInfoHandle()->GetFileId(),	\
						FccHashNodeCompare());
	if (validListRange.first != validListRange.second)
	{
		//fccHashNode fhnFind = *(validListRange.first);
		validListRange.first->second->push_back(fcn);
	}
	else
	{
		vector<FileCacheNode *> *hashNodeValTp = new vector<FileCacheNode *>;
		hashNodeValTp->reserve(18);
		hashNodeValTp->push_back(fcn);
		fileCacheValidHashList.push_back(make_pair(fcn->GetIoFileInfoHandle()->GetFileId(),hashNodeValTp));
		sort(fileCacheValidHashList.begin(),fileCacheValidHashList.end(),FccHashNodeCompare());
	}
//#ifdef _f0xCacheDbg
//	{
//		int iDbg=0;
//		for(FCC_HASHNODE_ITR itDbg=fileCacheValidHashList.begin();itDbg!=fileCacheValidHashList.end();++itDbg,++iDbg)
//		{
//			LOG_SPEC("<threadId:%lu>\t[%s] <fileCacheValidHashList%d>---fileid[0x%x]\n",pthread_self(),__FUNCTION__,iDbg,itDbg->first);
//		}
//	}
//#endif
}

void FileCacheCom::DelFileCacheNodeFromValidList(FCC_ITR &fccValidItTp)
{
	if(fileCacheValidHashList.empty() || (*fccValidItTp)->GetIoFileInfoHandle() == NULL)
	{
		fileCacheValidList.erase(fccValidItTp);
		return;
	}

	validListRange = equal_range(fileCacheValidHashList.begin(),		\
						fileCacheValidHashList.end(),	\
						(*fccValidItTp)->GetIoFileInfoHandle()->GetFileId(),	\
						FccHashNodeCompare());
	RELEASE_ASSERT(validListRange.first != validListRange.second);
	if (validListRange.first != validListRange.second)
	{
		fccHashNode fhnFind = *(validListRange.first);
		vector<FileCacheNode *>* fcTp = fhnFind.second;
		fccValidIt = find(fcTp->begin(),fcTp->end(),*fccValidItTp);
		RELEASE_ASSERT(fccValidIt != fcTp->end());
		fcTp->erase(fccValidIt);
		if(fcTp->empty())
		{
			delete fcTp;
			fileCacheValidHashList.erase(validListRange.first);
		}
	}

//#ifdef _f0xCacheDbg
//	{
//		int iDbg=0;
//		for(FCC_HASHNODE_ITR itDbg=fileCacheValidHashList.begin();itDbg!=fileCacheValidHashList.end();++itDbg,++iDbg)
//		{
//			LOG_SPEC("<threadId:%lu>\t[%s] <fileCacheValidHashList%d>---fileid[0x%x]\n",pthread_self(),__FUNCTION__,iDbg,itDbg->first);
//		}
//	}
//#endif

	fileCacheValidList.erase(fccValidItTp);
}

bool FileCacheCom::FCCReadAhead(NextLayer *nl)
{
	bool ret = true;
	FileCacheNode *fcn = NULL;

	//HANDLE curThread = GetCurrentThread();
	//DWORD dwThreadPri = GetThreadPriority(curThread);
	//if(!SetThreadPriority(curThread, THREAD_PRIORITY_TIME_CRITICAL))
	//{
	//	LOG_NORMAL("<threadId:%lu>\t[%s]Failed SetThreadPriority THREAD_PRIORITY_HIGHEST err(%d)\n",pthread_self(),__FUNCTION__,GetLastError());
	//}

	fileCacheRHListLock->GetLock();
	fileCacheValidListLock->GetLock();
	//INITLOGTIME;
	if(fileCacheRHList.empty())
	{
		readAheadTP->HaltThreads(1);
		fileCacheValidListLock->ReleseLock();
		fileCacheRHListLock->ReleseLock();
		return false;
	}

	////INITTIME;
	//if(cache->GetIsServer())
	//{
	//	sort(fileCacheRHList.begin(),fileCacheRHList.end(),SortRHList());
	//	it = find_if(fileCacheRHList.begin(),fileCacheRHList.end(),FindSortedRHNode(lastRHDiskOffset,lastRHDisklength));
	//	//VIEWUSEDTIME("sort&find_if use time");
	//	if(it == fileCacheRHList.end())
	//	{
	//		it = find_if(fileCacheRHList.begin(),fileCacheRHList.end(),FindRHNode());
	//		if(it == fileCacheRHList.end())
	//		{
	//			//readAheadTP->HaltAllThreads();
	//			fileCacheValidListLock->ReleseLock();
	//			fileCacheRHListLock->ReleseLock();
	//			return false;
	//		}
	//		fcn = (*it);
	//	}
	//	else
	//	{
	//		fcn = (*it);
	//	}
	//	lastRHDiskOffset = fcn->GetDiskInfo()->ioDiskOffset;
	//	lastRHDisklength = fcn->GetDiskInfo()->ioDisklength;
	//	fcn->SetIsRHReading(true);
	//}
	//else
	//{
		fccRHIt = find_if(fileCacheRHList.begin(),fileCacheRHList.end(),FindRHNode());
		if(fccRHIt == fileCacheRHList.end())
		{
			readAheadTP->HaltThreads(1);		//no find read ahead node so halt all thread
			fileCacheValidListLock->ReleseLock();
			fileCacheRHListLock->ReleseLock();
			return false;
		}
		fcn = *fccRHIt;
		fcn->SetIsRHReading(TRUE);
	//}
	LOG_READAHEADRUN("<threadId:%lu>\t[%s] get fcn[%p]\n",pthread_self(),__FUNCTION__,fcn);

	fileCacheValidListLock->ReleseLock();
	fileCacheRHListLock->ReleseLock();

	IoDiskInfo *temp = fcn->GetDiskInfo();
	u32 lenSum = 0;
	char *readBuf = fcn->GetCacheUnit()->GetCacheUnitBuf();
	LOG_READAHEADRUN("<threadId:%lu>\t[%s]DiskInfoNum[0x%x]\n",pthread_self(),__FUNCTION__,fcn->GetDiskInfoNum());
	for(u32 i=0;i<fcn->GetDiskInfoNum();++i)
	{
		LOG_READAHEADRUN("<threadId:%lu>\t[%s]DiskInfo%d---IoFileInfoBuf[%p];diskId[%s];readBuf[%p];ioDisklength[0x%x];ioDiskOffset[0x%llx] \n"	\
			,pthread_self(),__FUNCTION__	\
			,i	\
			,fcn->GetIoFileInfoHandle()->GetIoFileInfoBuf()	\
			,temp[i].diskId.GetIDStr().c_str()	\
			,readBuf+lenSum	\
			,temp[i].ioDisklength	\
			,temp[i].ioDiskOffset);

		if(nl->ReadDatas(temp[i].diskId,readBuf+lenSum,temp[i].ioDisklength,temp[i].ioDiskOffset).IsErr())
		{
			ret = false;
			goto FCCREADAHEADEND;
		}
		lenSum += temp[i].ioDisklength;
	}
	//assert(lenSum == mp->GetCacheUnitBufSize());
FCCREADAHEADEND:
	fcn->GetCacheUnit()->FreeCacheUnitBuf();
	fcn->ResetCUBufStatus();
	LOG_SPEC("<threadId:%lu>\t[%s]ResetCUBufStatus---fcn[%p]\n",pthread_self(),__FUNCTION__,fcn);

	fileCacheRHListLock->GetLock();
	fileCacheValidListLock->GetLock();

#ifdef _f0xCacheDbg
	{
		LOG_READAHEADRUN("<threadId:%lu>\t[%s] fileCacheRHList.erase fcn[%p];fileCacheValidList size[%lu];fileCacheRHList size[%lu]\n",pthread_self(),__FUNCTION__,fcn,fileCacheValidList.size(),fileCacheRHList.size());
		int iDbg=0;
		for(vector<FileCacheNode *>::iterator itDbg=fileCacheRHList.begin();itDbg!=fileCacheRHList.end();++itDbg,++iDbg)
		{
			LOG_READAHEADRUN("<threadId:%lu>\t[%s] <fileCacheRHList%d>---fcn[%p]\n",pthread_self(),__FUNCTION__,iDbg,*itDbg);
		}
	}
#endif

	if(fileCacheRHList.size()==1)
	{
		fccRHIt = fileCacheRHList.begin();
		assert(*fccRHIt == fcn);
	}
	else
	{
		fccRHIt = find(fileCacheRHList.begin(),fileCacheRHList.end(),fcn);
		assert(fccRHIt != fileCacheRHList.end());
	}
	fileCacheRHList.erase(fccRHIt);
	fcn->SetIsRHReading(FALSE);
	if (ret)
	{
		fcn->SetMarkTime();
		AddFileCacheNodeToValidList(fcn);
	}
	else
	{
		AddFileCacheNodeToValidList(fcn,true);
	}
	fileCacheValidListLock->ReleseLock();
	fileCacheRHListLock->ReleseLock();

	fcn->GetRHEventHandle().Set();

	return ret;
}

bool FileCacheCom::StartReadAhead(vector<IoDiskInfo> &RHDiskIoAry,IoFileInfoHandle &fihtp)
{
	bool ret = false;
	const u32 cubs = mp->GetCacheUnitBufSize();
	u32 ioLenSum = 0;
	u32 ioLenLeft = 0;
	//int checktime = 1;
	u32 tpBufSize = 0;
	IoDiskInfo di;
	memset(&di,0,sizeof(IoDiskInfo));

	vector<IoDiskInfo>::iterator idiIt;

	vector<IoDiskInfo> tempDIArry;
	tempDIArry.reserve(RHDiskIoAry.size()+2);
	vector<IoDiskInfo> RHFiltedDiskIoFromRHList;
	RHFiltedDiskIoFromRHList.reserve(RHDiskIoAry.size()+2);
	vector<IoDiskInfo> RHFiltedDiskIoFromValidList;
	RHFiltedDiskIoFromValidList.reserve(RHDiskIoAry.size()+2);

	//fileCacheRHListLock->GetLock();
	//fileCacheValidListLock->GetLock();

	if(fileCacheValidList.empty())
	{
		LOG_READAHEADRUN("<threadId:%lu>\t[%s]fileCacheValidList empty\n",pthread_self(),__FUNCTION__);
		goto STARTREADAHEAD_END;
	}

	//do 
	//{
		for_each(RHDiskIoAry.begin(),RHDiskIoAry.end(),FilterDiskIoInRHList(this,RHFiltedDiskIoFromRHList,fipIt));
		if(RHFiltedDiskIoFromRHList.empty())
		{
			LOG_READAHEADRUN("<threadId:%lu>\t[%s]RHFiltedDiskIoFromRHList empty after FilterDiskIoInRHList\n",pthread_self(),__FUNCTION__);
			goto STARTREADAHEAD_END;
		}

#ifdef _f0xCacheDbg
		{
			LOG_PERFORMACE_DOWN("<threadId:%lu>\t[%s]RHDiskIoAry size[%lu];RHFiltedDiskIoFromRHList size[%lu]\n",pthread_self(),__FUNCTION__,RHDiskIoAry.size(),RHFiltedDiskIoFromRHList.size());
			int iDbg=0;
			for(vector<IoDiskInfo>::iterator itDbg=RHDiskIoAry.begin();itDbg!=RHDiskIoAry.end();++itDbg,++iDbg)
			{
				LOG_PERFORMACE_DOWN("<threadId:%lu>\t[%s] <RHDiskIoAry%d>---diskId[%s];ioDiskOffset[0x%llx],ioDisklength[0x%x]\n",pthread_self(),__FUNCTION__,iDbg,itDbg->diskId.GetIDStr().c_str(),itDbg->ioDiskOffset,itDbg->ioDisklength);
			}
			iDbg=0;
			for(vector<IoDiskInfo>::iterator itDbg=RHFiltedDiskIoFromRHList.begin();itDbg!=RHFiltedDiskIoFromRHList.end();++itDbg,++iDbg)
			{
				LOG_PERFORMACE_DOWN("<threadId:%lu>\t[%s] <RHFiltedDiskIoFromRHList%d>---diskId[%s];ioDiskOffset[0x%llx],ioDisklength[0x%x]\n",pthread_self(),__FUNCTION__,iDbg,itDbg->diskId.GetIDStr().c_str(),itDbg->ioDiskOffset,itDbg->ioDisklength);
			}
		}
#endif

		for_each(RHFiltedDiskIoFromRHList.begin(),RHFiltedDiskIoFromRHList.end(),FilterDiskIoInValidList(fihtp.GetFileId(),this,RHFiltedDiskIoFromValidList,fipIt));
		if(RHFiltedDiskIoFromValidList.empty())
		{
			LOG_READAHEADRUN("<threadId:%lu>\t[%s]RHFiltedDiskIoFromValidList empty after FilterDiskIoInValidList\n",pthread_self(),__FUNCTION__);
			goto STARTREADAHEAD_END;
		}
		
		for(vector<IoDiskInfo>::iterator itDbg=RHFiltedDiskIoFromValidList.begin();itDbg!=RHFiltedDiskIoFromValidList.end();++itDbg)
		{
			tpBufSize += itDbg->ioDisklength;
		}
		
#ifdef _f0xCacheDbg
		{
			LOG_PERFORMACE_DOWN("<threadId:%lu>\t[%s]RHFiltedDiskIoFromRHList size[%lu];RHFiltedDiskIoValidList size[%lu]\n",pthread_self(),__FUNCTION__,RHFiltedDiskIoFromRHList.size(),RHFiltedDiskIoFromValidList.size());
			int iDbg=0;
			for(vector<IoDiskInfo>::iterator itDbg=RHFiltedDiskIoFromRHList.begin();itDbg!=RHFiltedDiskIoFromRHList.end();++itDbg,++iDbg)
			{
				LOG_PERFORMACE_DOWN("<threadId:%lu>\t[%s] <RHFiltedDiskIoFromRHList%d>---diskId[%s];ioDiskOffset[0x%llx],ioDisklength[0x%x]\n",pthread_self(),__FUNCTION__,iDbg,itDbg->diskId.GetIDStr().c_str(),itDbg->ioDiskOffset,itDbg->ioDisklength);
			}
			iDbg=0;
			for(vector<IoDiskInfo>::iterator itDbg=RHFiltedDiskIoFromValidList.begin();itDbg!=RHFiltedDiskIoFromValidList.end();++itDbg,++iDbg)
			{
				LOG_PERFORMACE_DOWN("<threadId:%lu>\t[%s] <RHFiltedDiskIoFromValidList%d>---diskId[%s];ioDiskOffset[0x%llx],ioDisklength[0x%x]\n",pthread_self(),__FUNCTION__,iDbg,itDbg->diskId.GetIDStr().c_str(),itDbg->ioDiskOffset,itDbg->ioDisklength);
			}
		}
#endif

		if(tpBufSize < mp->GetCacheUnitBufSize())
		{
			//if(fihtp.GetRHStatus()!=NonReadAhead && fihtp.GetDistance() != 0)
			{
				return ret;
			}
		}

		LOG_PERFORMACE_DOWN("<threadId:%lu>\t[%s]passedBufSize[0x%x]\n",pthread_self(),__FUNCTION__,tpBufSize);
		//if(checktime++)
		//{
		//	break;
		//}
		//TuneRHFiltedDiskIo(RHFiltedDiskIoFromValidList,/*0x200000*/mp->GetCacheUnitBufSize(),idiIt);	//align size 2*1024*1024
		//RHDiskIoAry.swap(RHFiltedDiskIoFromValidList);
		//RHFiltedDiskIoFromValidList.clear();
		//RHFiltedDiskIoFromRHList.clear();
	//} while (0);


	for(idiIt=RHFiltedDiskIoFromValidList.begin();idiIt!=RHFiltedDiskIoFromValidList.end();)	//align size DISKMINSECTORSIZE
	{
		idiIt->ioDisklength += mp->GetCacheUnitBufSize() - idiIt->ioDisklength%mp->GetCacheUnitBufSize();
		if(idiIt->ioDisklength == 0)
		{
			idiIt = RHFiltedDiskIoFromValidList.erase(idiIt);
		}
		else
		{
			idiIt->ioDiskOffset -= idiIt->ioDiskOffset%mp->GetCacheUnitBufSize();
			++idiIt;
		}
	}

	for(idiIt=RHFiltedDiskIoFromValidList.begin();idiIt!=RHFiltedDiskIoFromValidList.end();++idiIt)	//align size DISKMINSECTORSIZE
	{
		tpBufSize += idiIt->ioDisklength;
	}

#ifdef _f0xCacheDbg
	{
		int iDbg=0;
		for(vector<IoDiskInfo>::iterator itDbg=RHFiltedDiskIoFromValidList.begin();itDbg!=RHFiltedDiskIoFromValidList.end();++itDbg,++iDbg)
		{
			LOG_PERFORMACE_DOWN("<threadId:%lu>\t[%s] <SendToRHThreadIo%d>---diskId[%s];ioDiskOffset[0x%llx],ioDisklength[0x%x]\n",pthread_self(),__FUNCTION__,iDbg,itDbg->diskId.GetIDStr().c_str(),itDbg->ioDiskOffset,itDbg->ioDisklength);
			assert(itDbg->ioDisklength % 512 == 0 && itDbg->ioDiskOffset % 512 == 0);
		}
	}
#endif

	for(idiIt=RHFiltedDiskIoFromValidList.begin();idiIt!=RHFiltedDiskIoFromValidList.end();)
	{
		if(fileCacheValidList.empty())
		{
			LOG_READAHEADRUN("<threadId:%lu>\t[%s]fileCacheValidList empty\n",pthread_self(),__FUNCTION__);
			goto STARTREADAHEAD_END;
		}

		ioLenSum += idiIt->ioDisklength;
		if(ioLenSum > cubs)
		{
			ioLenLeft = ioLenSum - cubs;
			di.diskId = idiIt->diskId;
			di.ioDiskOffset = idiIt->ioDiskOffset;
			di.ioDisklength = idiIt->ioDisklength-ioLenLeft;
			tempDIArry.push_back(di);
			SendToRHThread(tempDIArry,fihtp);
			tempDIArry.clear();
			idiIt->ioDiskOffset += di.ioDisklength;
			idiIt->ioDisklength = ioLenLeft;
			ioLenSum = 0;
			ioLenLeft = 0;
		}
		else if(ioLenSum == cubs)
		{
			tempDIArry.push_back(*idiIt);
			SendToRHThread(tempDIArry,fihtp);
			tempDIArry.clear();
			ioLenSum = 0;
			ioLenLeft = 0;
			++idiIt;
		}
		else
		{
			tempDIArry.push_back(*idiIt);
			++idiIt;
		}
	}
	if(!fileCacheValidList.empty()&&!tempDIArry.empty())
	{
		assert(ioLenSum<cubs);
		//(tempDIArry.end()-1)->ioDisklength += cubs-ioLenSum;
		SendToRHThread(tempDIArry,fihtp);
	}
STARTREADAHEAD_END:
	//fileCacheValidListLock->ReleseLock();
	//fileCacheRHListLock->ReleseLock();
	ret = true;
	return ret;
}


inline bool FileCacheCom::SendToRHThread(vector<IoDiskInfo> &tempDIArry,IoFileInfoHandle &fihtp)
{
#ifdef _f0xCacheDbg
	{
		int iDbg=0;
		u32 sum = 0;
		for(vector<IoDiskInfo>::iterator itDbg=tempDIArry.begin();itDbg!=tempDIArry.end();++itDbg,++iDbg)
		{
			sum += itDbg->ioDisklength;
			LOG_PERFORMACE_DOWN("<threadId:%lu>\t[%s]tempDIArry%d---diskId[%s];ioDiskOffset[0x%llx],ioDisklength[0x%x]\n",pthread_self(),__FUNCTION__,iDbg,itDbg->diskId.GetIDStr().c_str(),itDbg->ioDiskOffset,itDbg->ioDisklength);
		}
		//assert(sum==mp->GetCacheUnitBufSize());
	}
#endif

	FileCacheNode *pfcn = GetFileCacheNodeForRH();
	assert(pfcn!=NULL);
	pfcn->SetRHNeedInfo(&tempDIArry[0],tempDIArry.size(),fihtp);
	fileCacheRHList.push_back(pfcn);

	LOG_NORMAL("<threadId:%lu>\t[%s] fileCacheRHList.push_back fcn[%p];fileCacheValidList size[%lu];fileCacheRHList size[%lu]\n",pthread_self(),__FUNCTION__,pfcn,fileCacheValidList.size(),fileCacheRHList.size());
#ifdef _f0xCacheDbg
	{
		int iDbg=0;
		for(vector<FileCacheNode *>::iterator itDbg=fileCacheRHList.begin();itDbg!=fileCacheRHList.end();++itDbg,++iDbg)
		{
			LOG_PERFORMACE_DOWN("<threadId:%lu>\t[%s] <fileCacheRHList%d>---fcn[%p]\n",pthread_self(),__FUNCTION__,iDbg,*itDbg);
		}
	}
#endif

	readAheadTP->ActiveAllThreads();
	return true;
}

inline FileCacheNode* FileCacheCom::GetFileCacheNodeForRH()
{
	FileCacheNode * pfcn = NULL;
	u64 curTime;

	curTime = GetMicroSecTime();
//REGETCURTIME1:
//	if(!QueryPerformanceCounter(&curTime))
//	{
//		LOG_NORMAL("<threadId:%lu>\t\t[%s]	QueryPerformanceCounter failed!\n",pthread_self(),__FUNCTION__);
//		goto REGETCURTIME1;
//	}
	//FCC_ITR fccValidItTp = find_if(fileCacheValidList.begin(),fileCacheValidList.end(),FindTimeOutNode(curTime));
	FCC_ITR fccValidItTp = fileCacheValidList.begin();		//fileCacheValidList.begin() have the max marktime
	//FCC_ITR fccValidItTp1 = fileCacheValidList.begin()+18;
	while(!(*fccValidItTp)->IsTimeoutFast(curTime))
	{
		//fccValidItTp = find_if(fileCacheValidList.begin(),fileCacheValidList.begin()+18,FindBufUsedNode());
		//if(fccValidItTp != fccValidItTp1)
		//{
		//	LOG_READAHEADRUN("<threadId:%lu>\t[%s]FindBufUsedNode OK & break\n",pthread_self(),__FUNCTION__);
		//	break;
		//}
		//LOG_READAHEADRUN("<threadId:%lu>\t[%s]FindTimeOutNode fault & waiting\n",pthread_self(),__FUNCTION__);
		fileCacheValidListLock->ReleseLock();
		fileCacheRHListLock->ReleseLock();
		
		Sleep(180);

		fileCacheRHListLock->GetLock();
		fileCacheValidListLock->GetLock();

		curTime = GetMicroSecTime();
//REGETCURTIME2:
//		if(!QueryPerformanceCounter(&curTime))
//		{
//			LOG_NORMAL("<threadId:%lu>\t\t[%s]	QueryPerformanceCounter failed!\n",pthread_self(),__FUNCTION__);
//			goto REGETCURTIME2;
//		}
		fccValidItTp = fileCacheValidList.begin();
	}
	pfcn = *fccValidItTp;

//	{	//log valid memory not read size
//		int tp = 0;
//		u32 startIndex = 0;
//		u32 bitLength = 0;
//		u32 readSize = 0;
//		cacheUnitMarkBits * bitmap = pfcn->GetBitmap();
//		while((tp = bitmap->GetSeriesSetBits(tp,bitmap->GetMarkBitNum()-1,startIndex,bitLength)) != -1)
//		{
//			readSize += bitLength*DISKMINSECTORSIZE;
//		}
//
//		if(bitLength != 0)
//		{
//			readSize += bitLength*DISKMINSECTORSIZE;
//		}
//		LOG_SPEC("<threadId:%lu>\t[%s]GetSeriesSetBits---fcn[%p];readSize[0x%x]\n",pthread_self(),__FUNCTION__,pfcn,readSize);
//
//		if(ValidCacheNotReadSizeCount>=LOGBUFSIZE)
//		{
//			ValidCacheNotReadSizeCount = 0;
//		}
//
//		ValidCacheNotReadSizeAry[ValidCacheNotReadSizeCount++] = bitmap->GetMarkBitNum()*DISKMINSECTORSIZE - readSize;
//	}

	DelFileCacheNodeFromValidList(fccValidItTp);
	LOG_READAHEADRUN("<threadId:%lu>\t[%s]FindUseLessNode OK\n",pthread_self(),__FUNCTION__);
	//pfcn->GetReadAheadLock()->GetLock();
	pfcn->GetRHEventHandle().Reset();
	return pfcn;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  cache (support file level readahead & disk level lazywrite)  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FiErr cacheReadWrite::WriteDatas(ObjID fileID,char *buf,u32 length,u64 offset,u32 pcID)
{
	FiErr ret;
	if(isUseFileCache && isUseDiskCache)
	{
		IoFileInfoHandle fih(fileID,length,offset,pcID);
		ret = fileCache->FCCWriteBytes(
			fih,	\
			buf,	\
			length,	\
			offset
			);

		ret = diskCache->DCCOperator(fih,fih.GetDiskId(),buf,length,offset,true);
	}
	else if (isUseFileCache)
	{
		IoFileInfoHandle fih(fileID,length,offset,pcID);
		ret = fileCache->FCCWriteBytes(
			fih,	\
			buf,	\
			length,	\
			offset
			);

		ret = nl->WriteDatas(fileID,buf,length,offset);
	}
	else if (isUseDiskCache)
	{
		IoFileInfoHandle fih(fileID,length,offset,pcID);
		ret = diskCache->DCCOperator(fih,fih.GetDiskId(),buf,length,offset,true);
	}
	else 
	{
		ret = nl->WriteDatas(fileID,buf,length,offset);
	}
	return ret;
}

FiErr cacheReadWrite::ReadDatas(ObjID fileID,char *buf,u32 length,u64 offset,u32 pcID)
{
	FiErr ret;
	if(isUseFileCache && isUseDiskCache)
	{
		IoFileInfoHandle fih(fileID,length,offset,pcID);
		vector<FileIoPar> collectReadDiskCacheIO;
		ret = fileCache->FCCReadBytes(
			fih,	\
			buf,	\
			length,	\
			offset,	\
			collectReadDiskCacheIO
			);

		if (!collectReadDiskCacheIO.empty())
		{
			vector<DiskIoPar> collectReadDeviceIO;
			sort(collectReadDiskCacheIO.begin(),collectReadDiskCacheIO.end(),SortReadIO());
			vector<FileIoPar>::iterator	fipItTP = collectReadDiskCacheIO.begin();
			FileIoPar fip = *fipItTP;
			for(++fipItTP;fipItTP != collectReadDiskCacheIO.end();++fipItTP)
			{
				if(fip.diskOffset+fip.diskLength == fipItTP->diskOffset)	//merge series io
				{
					fip.diskLength += fipItTP->diskLength;
				}
				else
				{
					assert(fip.diskLength<=length);
					assert(fip.diskOffset>=offset && fip.diskOffset<offset+length);
					assert(fip.ioBuf>=buf && fip.ioBuf<buf+length);
					ret = diskCache->DCCOperator(fih,fih.GetDiskId(),fip.ioBuf,fip.diskLength,fip.diskOffset,false,&collectReadDeviceIO);
					if(ret.IsErr())
					{
						return ret;
					}
					fip = *fipItTP;
				}
			}

			assert(fip.diskLength<=length);
			assert(fip.diskOffset>=offset && fip.diskOffset<offset+length);
			assert(fip.ioBuf>=buf && fip.ioBuf<buf+length);
			ret = diskCache->DCCOperator(fih,fih.GetDiskId(),fip.ioBuf,fip.diskLength,fip.diskOffset,false,&collectReadDeviceIO);
			if(ret.IsErr())
			{
				return ret;
			}

			if (!collectReadDeviceIO.empty())
			{
				LOG_NORMAL("<threadId:%lu>\t[%s]collectReadDeviceIO.size[%lu]\n",pthread_self(),__FUNCTION__,collectReadDeviceIO.size());
				for(vector<DiskIoPar>::iterator dipReadIt = collectReadDeviceIO.begin();dipReadIt!=collectReadDeviceIO.end();++dipReadIt)
				{
					ret = nl->ReadDatas(fileID,dipReadIt->ioBuf,dipReadIt->diskLength,dipReadIt->diskOffset);
					if(ret.IsErr())
					{
						break;
					}
				}
			}
		}
	}
	else if (isUseFileCache)
	{
		IoFileInfoHandle fih(fileID,length,offset,pcID);
		vector<FileIoPar> collectReadDiskCacheIO;
		ret = fileCache->FCCReadBytes(
			fih,	\
			buf,	\
			length,	\
			offset,	\
			collectReadDiskCacheIO
			);

		if (!collectReadDiskCacheIO.empty())
		{
			sort(collectReadDiskCacheIO.begin(),collectReadDiskCacheIO.end(),SortReadIO());
			vector<FileIoPar>::iterator	fipItTP = collectReadDiskCacheIO.begin();
			FileIoPar fip = *fipItTP;
			for(++fipItTP;fipItTP != collectReadDiskCacheIO.end();++fipItTP)
			{
				if(fip.diskOffset+fip.diskLength == fipItTP->diskOffset)	//merge series io
				{
					fip.diskLength += fipItTP->diskLength;
				}
				else
				{
					assert(fip.diskLength<=length);
					assert(fip.diskOffset>=offset && fip.diskOffset<offset+length);
					assert(fip.ioBuf>=buf && fip.ioBuf<buf+length);
					ret = nl->ReadDatas(fileID,fip.ioBuf,fip.diskLength,fip.diskOffset);
					if(ret.IsErr())
					{
						return ret;
					}
					fip = *fipItTP;
				}
			}

			assert(fip.diskLength<=length);
			assert(fip.diskOffset>=offset && fip.diskOffset<offset+length);
			assert(fip.ioBuf>=buf && fip.ioBuf<buf+length);
			ret = nl->ReadDatas(fileID,fip.ioBuf,fip.diskLength,fip.diskOffset);
		}
	}
	else if (isUseDiskCache)
	{
		IoFileInfoHandle fih(fileID,length,offset,pcID);
		vector<DiskIoPar> collectReadDeviceIO;
		ret = diskCache->DCCOperator(fih,fih.GetDiskId(),buf,length,offset,false,&collectReadDeviceIO);

		for(vector<DiskIoPar>::iterator dipReadIt = collectReadDeviceIO.begin();dipReadIt!=collectReadDeviceIO.end();++dipReadIt)
		{
			ret = nl->ReadDatas(fileID,dipReadIt->ioBuf,dipReadIt->diskLength,dipReadIt->diskOffset);
			if(ret.IsErr())
			{
				break;
			}
		}
	}
	else 
	{
		ret = nl->ReadDatas(fileID,buf,length,offset);
	}
	return ret;
}

FiErr cacheReadWrite::FlushDatas()
{
	FiErr ret;
	if(isUseDiskCache)
	{
		//PDIFSDISKIONODE_APP ddion = (PDIFSDISKIONODE_APP)buf;
		if(!diskCache->DCCFlushAllDirtyListNode(nl))
		{
			LOG_NORMAL("<threadId:%lu>\t[%s] No dirty data!\n",pthread_self(),__FUNCTION__);
			//ddion->difsdiskionode.difsdiskio.IoStatusBlock.Status = 0;
			//ddion->difsdiskionode.difsdiskio.IoStatusBlock.Information = 0;
		}
	}
	return ret;
}

cacheReadWrite::cacheReadWrite(string cfgFile,NextLayer *nlPar):diskCache(NULL),fileCache(NULL),lazyWriteTP(NULL),readAheadTP(NULL),nl(nlPar)
{
	if(!GetCacheConfig(cc,cfgFile))
	{
		//default config
		cc.isUseFileCache = 0;
		cc.isUseDiskCache = 0;

		//cc.lazyWriteThreadNum = 8;
		//cc.readAheadThreadNum = 8;
		//cc.diskCacheSize = 20;
		//cc.diskCacheUnitSize = DEFAULTCACHEUNITSIZE*2;
		//cc.fileCacheSize = 20;
		//cc.fileCacheUnitSize = DEFAULTCACHEUNITSIZE*2;

		//cc.diskCacheNodeTimeOutInValidList = 4;	//seconds
		//cc.diskCacheNodeTimeOutInDirtyList = 2;	//seconds
		//cc.diskCacheNodeMinReadCopySize = 512;	//8*512 = 4k

		//cc.fileCacheNodeTimeOut = 4;	//seconds

		//cc.fileHistoryNodeTimeOut = 30;	//seconds
		//cc.fileRHMaxSize = 16*1024*1024;	
		//cc.fileRHSeriesNum = 3;		//seriesNum = 1 will shutdown read ahead

		//cc.isOpRealDevice = 1;
	}

	isUseFileCache = (cc.isUseFileCache == 0)?false:true;
	isUseDiskCache = (cc.isUseDiskCache == 0)?false:true;
	
	cc.diskCacheUnitSize = (cc.diskCacheUnitSize <= 0x20000)?0x20000:cc.diskCacheUnitSize&0xfffe0000;
	cc.fileCacheUnitSize = (cc.fileCacheUnitSize <= 0x20000)?0x20000:cc.fileCacheUnitSize&0xfffe0000;
	cc.diskCacheNodeMinReadCopySize = (cc.diskCacheNodeMinReadCopySize <= 0x200)?0x200:cc.fileCacheUnitSize&0xfffffe00;
	cc.fileRHMaxSize = (cc.fileRHMaxSize <= cc.fileCacheUnitSize)?cc.fileCacheUnitSize:cc.fileRHMaxSize&0xffe00000;

	isOpRealDevice = (cc.isOpRealDevice == 0)?false:true;
	if(cc.fileRHSeriesNum<2)
	{
		LOG_ERR("<threadId:%lu>\t[%s]fileRHSeriesNum<2---not use ReadAhead!!!\n",pthread_self(),__FUNCTION__);
	}

	DiskCacheNode::SetTimeOutInValidList(cc.diskCacheNodeTimeOutInValidList);	//seconds
	DiskCacheNode::SetTimeOutInDirtyList(cc.diskCacheNodeTimeOutInDirtyList);	//seconds
	DiskCacheNode::SetMinReadCopySize(cc.diskCacheNodeMinReadCopySize/512);	//8*512 = 4k

	FileCacheNode::SetTimeOut(cc.fileCacheNodeTimeOut);	//seconds

	IoFileInfo::timeout = cc.fileHistoryNodeTimeOut;	//seconds
	IoFileInfo::maxReadAheadSize = cc.fileRHMaxSize;	
	IoFileInfo::seriesNum = cc.fileRHSeriesNum;	
	IoFileInfo::minReadAheadSize = cc.fileRHMinSize;	

	if (isUseDiskCache)
	{
		lazyWriteTP = new ThreadPool(cc.lazyWriteThreadNum,LazyWriteThread,this);
		assert(lazyWriteTP != NULL);
		diskCache = new DiskCacheCom(lazyWriteTP,cc.diskCacheSize,cc.diskCacheUnitSize);
		assert(diskCache != NULL);
	}

	if (isUseFileCache)
	{
		readAheadTP = new ThreadPool(cc.readAheadThreadNum,ReadAheadThread,this);
		assert(readAheadTP != NULL);
		fileCache = new FileCacheCom(diskCache,readAheadTP,cc.fileCacheSize,cc.fileCacheUnitSize);
		assert(fileCache != NULL);
	}

	frequency = 1000000;
//	if(!QueryPerformanceFrequency(&frequency))
//	{
//		LOG_NORMAL("<threadId:%lu>\t[%s]	QueryPerformanceFrequency failed!\n",pthread_self(),__FUNCTION__);
//	}
}

cacheReadWrite::~cacheReadWrite()
{
	exit(0);

	if(readAheadTP != NULL)
	{
		delete readAheadTP;
	}
	if(fileCache != NULL)
	{
		delete fileCache;
	}
	if(lazyWriteTP != NULL)
	{
		delete lazyWriteTP;
	}
	if(diskCache != NULL)
	{
		delete diskCache;
	}
}

string trimEnd(string &str)
{
	const string &delim =" \t" ;
	string r=str.erase(str.find_last_not_of(delim)+1);
	return r.erase(0,r.find_first_not_of(delim));
}

bool GetCacheConfigNode(string &content,const char *nodeStrB,const char *nodeStrE,u32 &nodeVal)
{
	string temp;
	int bos = content.find(nodeStrB)+strlen(nodeStrB);
	int eos = content.find(nodeStrE);
	temp = content.substr(bos,eos-bos);
	if(temp.length()==0)
	{
		return false;
	}

	trimEnd(temp);

	nodeVal = atoi(&temp[0]);
	return true;
}

bool cacheReadWrite::GetCacheConfig(CacheConfig &cc,string &cfgFile)
{
	ifstream ifs(cfgFile.c_str());
	string content;
	char ch;
	if (ifs.bad())
	{
		return false;
	}
	while(ifs.get(ch)) 
	{ 
		content+=ch; 
	}
	ifs.close();
	if(content.length()==0)
	{
		return false;
	}
	if(GetCacheConfigNode(content,"<isUseFileCache>","</isUseFileCache>",cc.isUseFileCache)&&	\
		\
		GetCacheConfigNode(content,"<isUseDiskCache>","</isUseDiskCache>",cc.isUseDiskCache)&&	\
		\
		GetCacheConfigNode(content,"<lazyWriteThreadNum>","</lazyWriteThreadNum>",cc.lazyWriteThreadNum)&&	\
		GetCacheConfigNode(content,"<readAheadThreadNum>","</readAheadThreadNum>",cc.readAheadThreadNum)&&	\
		\
		GetCacheConfigNode(content,"<diskCacheSize>","</diskCacheSize>",cc.diskCacheSize)&&	\
		GetCacheConfigNode(content,"<diskCacheUnitSize>","</diskCacheUnitSize>",cc.diskCacheUnitSize)&&	\
		GetCacheConfigNode(content,"<diskCacheNodeTimeOutInValidList>","</diskCacheNodeTimeOutInValidList>",cc.diskCacheNodeTimeOutInValidList)&&	\
		GetCacheConfigNode(content,"<diskCacheNodeTimeOutInDirtyList>","</diskCacheNodeTimeOutInDirtyList>",cc.diskCacheNodeTimeOutInDirtyList)&&	\
		GetCacheConfigNode(content,"<diskCacheNodeMinReadCopySize>","</diskCacheNodeMinReadCopySize>",cc.diskCacheNodeMinReadCopySize)&&	\
		\
		GetCacheConfigNode(content,"<fileCacheSize>","</fileCacheSize>",cc.fileCacheSize)&&	\
		GetCacheConfigNode(content,"<fileCacheUnitSize>","</fileCacheUnitSize>",cc.fileCacheUnitSize)&&	\
		GetCacheConfigNode(content,"<fileCacheNodeTimeOut>","</fileCacheNodeTimeOut>",cc.fileCacheNodeTimeOut)&&	\
		GetCacheConfigNode(content,"<fileHistoryNodeTimeOut>","</fileHistoryNodeTimeOut>",cc.fileHistoryNodeTimeOut)&&	\
		GetCacheConfigNode(content,"<fileRHMaxSize>","</fileRHMaxSize>",cc.fileRHMaxSize)&&	\
		GetCacheConfigNode(content,"<fileRHMinSize>","</fileRHMinSize>",cc.fileRHMinSize)&&	\
		GetCacheConfigNode(content,"<fileRHSeriesNum>","</fileRHSeriesNum>",cc.fileRHSeriesNum)&&	\
		GetCacheConfigNode(content,"<isOpRealDevice>","</isOpRealDevice>",cc.isOpRealDevice))
	{
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  NextLayer  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FiErr NextLayer::WriteDatas(ObjID fileID, char *buf, u32 length, u64 offset)
{
	FiErr ret;
	if(!isOpRealDevice)
	{
		return ret;
	}

	string fpStr("/tmp/");
	fpStr += fileID.GetIDStr();
	ret = ficsLocIF::GetFicsLocIF().GetFileIF(fpStr)->WriteData(buf, length, offset);
	return ret;
}

FiErr NextLayer::ReadDatas(ObjID fileID, char *buf, u32 length, u64 offset)
{
	FiErr ret;
	if(!isOpRealDevice)
	{
		return ret;
	}

	string fpStr("/tmp/");
	fpStr += fileID.GetIDStr();
	ret = ficsLocIF::GetFicsLocIF().GetFileIF(fpStr)->ReadData(buf, length, offset);
	return ret;
}
