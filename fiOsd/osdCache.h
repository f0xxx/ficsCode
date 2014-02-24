///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  hunter code ([hunter.f0x] remeber my brothers) email:hunterlhy@163.com /////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _OSDCACHE_H_
#define _OSDCACHE_H_
#include "../public/osdBaseType.h"
#include <vector>
#include <algorithm>
using namespace std;

#define _f0xCacheDbg

#define WAITTHREADENDTIME (2000)

#define DISKMINSECTORSIZE (512)
#define DEFAULTCACHEUNITSIZE (256*1024)
#define DEFAULTALLOCGRAINSIZE (1*1024*1024)		//default alloc grain size

#define BITALIGN (64)		//bit align
#define MARKCONST64 (0xffffffffffffffff)
	
#define MAXREADIOLOG (12)

typedef void* (*LPTHREAD_START_ROUTINE )(LPVOID lpThreadParameter);

void* LazyWriteThread(LPVOID par);
void* ReadAheadThread(LPVOID par);

bool IsTimeout_Sub(u64 markTime,u32 timeout);

/*for test*/
struct IoFileInfoHandle;
bool ioReadWrite(void *pddionbuf ,char *pBuf,u32 bufLength,u64 diskOffset,bool isWrite);
//extern u32 lastErr;

#define LOG_NORMAL printf
#define LOG_READAHEADRUN printf
#define LOG_SPEC printf
#define LOG_PERFORMACE_DOWN printf
#define LOG_ERR printf

struct NextLayer;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  Io info  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct IoDiskInfo
{
	ObjID diskId;	
	u64 ioDiskOffset;
	u32 ioDisklength;
};

//struct runList
//{
//	u64 length;
//	u64 volOffset;
//	u64 fileOffset;
//};
//
//
//struct DiskInfo
//{
//	ObjID diskId;
//	u64 diskSize;
//};

//struct IoHeadInfo
//{
//	u64 ioOrder;
//	u64 pcId;
//	u32 processId;
////	HANDLE fileHandle;
//	u64 fileId;
//	u64 oriFileOffset;
//	u32 oriFilelength;
//
//	ObjID diskId;
//	u64 ioDiskOffset;
//	u32 ioDisklength;
//	//u64 ioFileOffset;
//	//u32 ioFilelength;
//
//	bool isServer;
//
//	//u32 leftSize;
//	//char leftBuf[1];
//
//	u32 fileBlockNum;
//	runList rlAry[3];
//
//	u32 diskNum;
//	DiskInfo diskAry[3];
//};

struct FileReadIoLogNode
{
	u64 oriFileOffset;
	u32 oriFilelength;
};

enum ReadAheadStatus
{
	NonReadAhead,
	ForwardReadAhead,
	BackReadAhead
};

struct IoFileInfo
{
	u32 pcId;
//	ObjID volGuid;
//	u32 processId;
//	u64 fileHandle;
	ObjID fileId;

	u64 ioDiskOffset;
	u32 ioDisklength;

//	bool isNetIo;

	u64 markTime;
	static u32 timeout;

	u64 markUserSpeedTime;
	static u32 maxReadAheadSize;
	static u32 minReadAheadSize;

	ReadAheadStatus raStatus;
	u64 startReadAheadPos;
	static u32 seriesNum;

	u32 readCount;
	u32 hitCount;
	__int64 distance;
	vector<FileReadIoLogNode> ioLogList;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  lock  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct lockBase
{
	virtual void GetLock() = 0;
	virtual void ReleseLock() = 0;
};

//struct lockWin:public lockBase
//{
//	lockWin()
//	{
//		InitializeCriticalSection(&lock);
//		//LOG_NORMAL("<threadId:%lu>\t[%s] lock[%p]\n",pthread_self(),__FUNCTION__,this);
//	}
//	~lockWin()
//	{
//		DeleteCriticalSection(&lock);
//	}
//	void GetLock()
//	{
//		//INITTIME;
//		EnterCriticalSection(&lock);
//		//LOG_NORMAL("<threadId:%lu>\t[%s] lock[%p]\n",pthread_self(),__FUNCTION__,this);
//		//VIEWUSEDTIME("GetLock");
//	}
//	void ReleseLock()
//	{
//		LeaveCriticalSection(&lock);
//	}
//
//private:
//	CRITICAL_SECTION lock;
//}

struct lockUnix:public lockBase
{
	lockUnix(){}
	~lockUnix(){}
	void GetLock(){lock.Lock();}
	void ReleseLock(){lock.Unlock();}

private:
	OsdLock lock;
};

//////////////////////////////////f/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  IoFileInfoHandle  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//struct SpaceNode
//{
//	u64 start;
//	u64 length;
//};
//
//struct SpaceAnalyse
//{
//	SpaceAnalyse(SpaceNode &APar,SpaceNode &BPar);
//
//	inline bool IsHaveSameSpace(){return isHaveSameSpace;}
//	inline SpaceNode* GetSameSapce(){return &Same;}
//	inline SpaceNode* GetADiff(){return ADiff;}
//	inline SpaceNode* GetBDiff(){return BDiff;}
//private:
//	SpaceNode &A;
//	SpaceNode &B;
//	bool isHaveSameSpace;
//	SpaceNode Same;
//	SpaceNode ADiff[2];
//	SpaceNode BDiff[2];
//};

struct IoFileInfoHandle
{
	typedef vector<FileReadIoLogNode>::iterator IOFIH_ITR;

	IoFileInfoHandle(ObjID fileID,u32 length,u64 offset,u32 pcID = 0);

	//IoFileInfoHandle(IoHeadInfo *hi):cnt(new int(1))	//for test
	//{
	//	FileReadIoLogNode frl;
	//	assert(hi != NULL);
	//	pfi = new IoFileInfo;	//can use obj pool for optimization
	//	assert(pfi != NULL);
	//	pfi->ioLogList.reserve(1);
	//	pfi->isServer = hi->isServer;
	//	frl.ioOrder = hi->ioOrder;
	//	frl.oriFilelength =  hi->oriFilelength;
	//	frl.oriFileOffset = hi->oriFileOffset;
	//	pfi->ioLogList.push_back(frl);
	//	pfi->readCount = 0;
	//	pfi->hitCount = 0;
	//	pfi->distance = 0;
	//	//assert(ioDiskOffset<diskSize);
	//	pfi->pcId = hi->pcId;
	//	pfi->processId = hi->processId;
	//	pfi->fileHandle = hi->fileHandle;
	//	pfi->fileId = hi->fileId;
	//	pfi->diskId = hi->diskId;
	//	pfi->ioDiskOffset = hi->ioDiskOffset;
	//	pfi->ioDisklength = hi->ioDisklength;

	//	pfi->fileBlockNum = hi->fileBlockNum;
	//	pfi->fbAry = new runList[3];
	//	memcpy(pfi->fbAry,hi->rlAry,sizeof(runList)*3);
	//	pfi->diskNum = hi->diskNum;
	//	pfi->diskAry = new DiskInfo[3];
	//	memcpy(pfi->diskAry,hi->diskAry,sizeof(DiskInfo)*3);

	//	pfi->markUserSpeedTime.QuadPart = -1;
	//	pfi->markTime.QuadPart = -1;
	//	pfi->raStatus = NonReadAhead;
	//	pfi->startReadAheadPos = -1;
	//}

//	inline void BuildIoNode(void *ddionbuf,ObjID diskId,char *ioBuf,u32 diskLength,u64 diskOffset);

	IoFileInfoHandle(const IoFileInfoHandle& fih)
	{
		l.Lock();
		cnt = fih.cnt;
		pfi = fih.pfi;
		++*cnt;
		l.Unlock();
	}

	IoFileInfoHandle& operator=(const IoFileInfoHandle& fih)
	{
		l.Lock();
		++*fih.cnt;
		if(--*cnt==0)
		{
			delete cnt;
			delete pfi;
		}
		cnt=fih.cnt;
		pfi=fih.pfi;
		l.Unlock();
		return *this;
	}

	~IoFileInfoHandle()
	{
		l.Lock();
		if(--*cnt==0)
		{
			delete cnt;
			delete pfi;
		}
		l.Unlock();
	}

	inline void LogUserSpeedTime()
	{
		pfi->markUserSpeedTime = GetMicroSecTime();
	}

	inline void SetMarkTime()
	{
		pfi->markTime = GetMicroSecTime();
	}

	inline bool IsTimeout()
	{
		bool ret = true;
		ret = IsTimeout_Sub(pfi->markTime,pfi->timeout);
		if(ret)
		{
			//TRACE("[%s]	IsTimeout !!!\n");
		}
		return ret;
	}

	inline u32 CalUserReadSpeed(u32 readLength)
	{
		u64 currentMicro = GetMicroSecTime();
		return (u32)(((u64)readLength*1000000) / (currentMicro - pfi->markUserSpeedTime));
//		u64 tpTime;
//		u64 frequency;
//		if(!QueryPerformanceCounter(&tpTime))
//		{
//			LOG_NORMAL("<threadId:%lu>\t[%s]	QueryPerformanceCounter failed!\n",pthread_self(),__FUNCTION__);
//			return 0;
//		}
//		if(!QueryPerformanceFrequency(&frequency))
//		{
//			LOG_NORMAL("<threadId:%lu>\t[%s]	QueryPerformanceFrequency failed!\n",pthread_self(),__FUNCTION__);
//			return 0;
//		}
//		assert(frequency.QuadPart != 0);
//#ifdef _f0xCacheDbg
//		LOG_READAHEADRUN("<threadId:%lu>\t[%s]readLength[0x%x]\n",pthread_self(),__FUNCTION__,readLength);
//		u64 ioUseTime = (tpTime.QuadPart - pfi->markUserSpeedTime.QuadPart);
//		LOG_READAHEADRUN("<threadId:%lu>\t[%s]ioUseTime[0x%I64x]\n",pthread_self(),__FUNCTION__,ioUseTime);
//		u64 ioLength = (u64)readLength*frequency.QuadPart;
//		LOG_READAHEADRUN("<threadId:%lu>\t[%s]ioLength[0x%I64x]\n",pthread_self(),__FUNCTION__,ioLength);
//		return (u32)(ioLength/ioUseTime);
//#else
//		return (u32)(((u64)readLength*frequency.QuadPart)/(tpTime.QuadPart - pfi->markUserSpeedTime.QuadPart));
//#endif
	}

	inline void AddHitCount(){++pfi->hitCount;}
	inline void AddReadCount(){++pfi->readCount;}
	inline void CleanHitCount(){pfi->hitCount = 0;}
	inline void CleanReadCount(){pfi->readCount = 0;}
	inline u32 GetHitCount(){return pfi->hitCount;}
	inline u32 GetReadCount(){return pfi->readCount;}
	inline __int64 GetDistance(){return pfi->distance;}

	void FileInfo2DiskInfo(SpaceNode &fileInfo,vector<IoDiskInfo> &diskInfoAry);
	bool RHStrategy(vector<SpaceNode> &RHFileIoAry,u32 us);
	bool RandomStrategy(vector<SpaceNode> &RHFileIoAry,FileReadIoLogNode &newNode);
	inline bool GetRHFileIoAry(vector<SpaceNode> &RHFileIoAry,u32 us,FileReadIoLogNode &newNode){return (RHStrategy(RHFileIoAry,us) || RandomStrategy(RHFileIoAry,newNode));}

	inline u32 GetPcId(){return pfi->pcId;}
//	inline u32 GetProcessId(){return pfi->processId;}
//	inline u64 GetFileHandle(){return pfi->fileHandle;}
	inline ObjID GetFileId(){return pfi->fileId;}
//	inline u32 GetFileBlockNum(){return pfi->fileBlockNum;}

	inline u64& GetMarkUserSpeedTime(){return pfi->markUserSpeedTime;}
	inline vector<FileReadIoLogNode>& GetIoLogList(){return pfi->ioLogList;}
	inline u32 GetMaxReadAheadSize(){return pfi->maxReadAheadSize;}
	inline u32 GetMinReadAheadSize(){return pfi->minReadAheadSize;}
	inline ObjID& GetDiskId(){return pfi->fileId;}
	inline u64 GetDiskOffset(){return pfi->ioDiskOffset;}
	inline u32 GetDiskLength(){return pfi->ioDisklength;}
	inline void SetDiskId(ObjID diskId){pfi->fileId = diskId;}
	inline void SetLastFRL(FileReadIoLogNode &frl){lastfrl = frl;}
	inline ReadAheadStatus GetRHStatus(){return pfi->raStatus;}
	//inline void SetReadAheadSize(u32 RHSizeS){pfi->RHSize = RHSizeS;}
	//inline u32 GetReadAheadSize(){return pfi->RHSize;}
	
	inline IoFileInfo* GetIoFileInfoBuf(){return pfi;}

//	inline bool IsNetIo(){return pfi->isNetIo;}
	
private:
	IoFileInfo *pfi;
	FileReadIoLogNode lastfrl;
	int *cnt;

	IOFIH_ITR iofihIt;
	static OsdLock l;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  memory  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct memPool;

struct cacheUnitMarkBits
{
	cacheUnitMarkBits(u32 bitNum);
	~cacheUnitMarkBits();

	void ClearAllBits();
	inline void SetAllBits();

	inline void ClearBitRange(u32 startIndex,u32 bitLength);
	void SetBitRange(u32 startIndex,u32 bitLength);

	inline void ClearBit(u32 Index);
	inline void SetBit(u32 Index);

	inline int GetSeriesSetBits(u32 startGetIndex,u32 endGetIndex,u32 &startIndex,u32 &bitLength);
	inline int GetSeriesSetBitsNum();
	inline bool FindSeriesSetBits(u32 startFindIndex,u32 endFindIndex,u32 &SeriesBitsNum,u32 &startIndex,u32 &bitLength);

	inline u32 GetMarkBitNum(){return markBitNum;}
	bool IsAllMarkBitSet();
	//inline bool IsLastBitSet();

private:
	u64 *bs;
	u32 markBitNum;
	u32 mark64Num;
};

struct cacheUnit
{
	cacheUnit(lockBase *lkPar,memPool *mp,u32 cuid):lk(lkPar),mop(mp),cacheUnitID(cuid){}
	~cacheUnit(){}

	bool CUWriteBytes(void *buf,u32 bufSize,u32 CUOffset);
	bool CUReadBytes(void *buf,u32 bufSize,u32 CUOffset);

	inline memPool* GetMemPool(){return mop;}

	inline char* GetCacheUnitBuf();	
	inline void FreeCacheUnitBuf();	

private:
	lockBase *lk;
	u32 cacheUnitID;
	memPool *mop;
};

struct memPool
{
	memPool(u32 memSize,u32 cubs = DEFAULTCACHEUNITSIZE)	//min Alloc 1M
	{
		if(memSize == 0)
		{
			memSize = 1;
		}

		if(cubs == 0 || (DEFAULTALLOCGRAINSIZE*8)%cubs != 0 || cubs>memSize*DEFAULTALLOCGRAINSIZE)
		{
			LOG_NORMAL("<threadId:%lu>\t[%s]Set CacheUnitBufSize[0x%x] to default\n",pthread_self(),__FUNCTION__,cubs);
			assert(false);
			cubs = DEFAULTCACHEUNITSIZE;
		}

		if(!memAlloc(memSize))
		{
			RELEASE_ASSERT(FALSE);
			LOG_NORMAL("<threadId:%lu>\t[%s]memAlloc Failed\n",pthread_self(),__FUNCTION__);
			return;
		}

		cacheSize = memSize*DEFAULTALLOCGRAINSIZE;
		cacheUnitBufSize = cubs;
		if(cacheSize/(u64)cacheUnitBufSize > (u64)0xffffffff)
		{
			LOG_NORMAL("<threadId:%lu>\t[%s]cacheSize/cacheUnitBufSize > 0xffffffff overflow!!!\n",pthread_self(),__FUNCTION__);
		}
		cacheUnitTotNum = (u32)(cacheSize/(u64)cacheUnitBufSize);
		cacheUnitList.reserve(cacheUnitTotNum);
		for(u32 i = 0;i<cacheUnitTotNum;++i)
		{
			cacheUnitList.push_back(new cacheUnit(new lockUnix(),this,i));
		}
	}
	~memPool()
	{
		memFree();
		for(u32 i = 0;i<cacheUnitTotNum;++i)
		{
			delete cacheUnitList[i];
		}
	}

	inline bool MPWriteBytes(void *buf,u32 bufSize,u32 index,u32 CUOffset)
	{
		assert(buf != NULL && bufSize > 0);
		LOG_NORMAL("<threadId:%lu>\t[%s] bufD[%p],bufS[%p],index[%d],bufSize[0x%x]\n",pthread_self(),__FUNCTION__,cacheBuf+index*cacheUnitBufSize+CUOffset,buf,index,bufSize);
		memcpy(cacheBuf+index*cacheUnitBufSize+CUOffset,buf,bufSize);
		return true;
	}

	inline bool MPReadBytes(void *buf,u32 bufSize,u32 index,u32 CUOffset)
	{
		assert(buf != NULL && bufSize > 0);
		memcpy(buf,cacheBuf+index*cacheUnitBufSize+CUOffset,bufSize);
		return true;
	}

	inline const u32 GetCacheUnitBufSize(){return cacheUnitBufSize;}
	inline const u32 GetCacheUnitTotNum(){return cacheUnitTotNum;}
	inline const u64 GetCacheSize(){return cacheSize;}
	inline const vector<cacheUnit *>& GetCacheUnitList(){return cacheUnitList;}
	inline char* GetCacheUnitBuf(u32 CUID){assert(cacheBuf != NULL);return cacheBuf + (CUID * cacheUnitBufSize);}

private:
	bool memAlloc(u32 memSize)
	{
		assert(memSize>0);
		cacheBuf = new char[memSize*DEFAULTALLOCGRAINSIZE];
		if(cacheBuf == NULL)
		{
			return false;
		}
		memset(cacheBuf,0,memSize*DEFAULTALLOCGRAINSIZE);
		return true;
	}
	void memFree()
	{
		if(cacheBuf != NULL)
		{
			delete [] cacheBuf;
		}
	}
	vector<cacheUnit *> cacheUnitList;
	u32 cacheUnitTotNum;
	u32 cacheUnitBufSize;
	u64 cacheSize;
	char *cacheBuf;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  thread  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct threadUnit
{
	threadUnit(LPTHREAD_START_ROUTINE pwtPar,void *threadPar);
	~threadUnit();

	static void* TemplateThread(LPVOID par)
	{
		threadUnit *tu = (threadUnit *)par;

		//if(!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST))
		//{
		//	LOG_NORMAL("<threadId:%lu>\t[%s]	SetThreadPriority failed errcode[%d]!\n",pthread_self(),__FUNCTION__,GetLastError());
		//} 

		tu->WorkThreadWrapper();
		return 0;
	}

	void WorkThreadWrapper();	

	void ActiveThread();
	void HaltThread();

private:
	LPTHREAD_START_ROUTINE pwt;			//workerThread
	void *par;
	pthread_t hThread;
	FiEvent isEnd;
	FiEvent isActive;
	bool islive;
	bool endThread;
};

//struct threadUnit_WinNT:public threadUnit
//{
//
//};

struct ThreadPool	
{
	ThreadPool(int threadNum,LPTHREAD_START_ROUTINE pwtPar,void *par = NULL);
	~ThreadPool();

	struct ActiveThreadsOp 
	{
		inline void operator()(threadUnit *TU)
		{
			TU->ActiveThread();
		}
	};

	struct HaltThreadsOp
	{
		inline void operator()(threadUnit *TU)
		{
			TU->HaltThread();
		}
	};

	void ActiveThreads(u32 threadNum);
	void HaltThreads(u32 threadNum);

	inline void ActiveAllThreads()
	{
		ActiveThreads(-1);
	}
	inline void HaltAllThreads()
	{
		HaltThreads(-1);
	}

	inline const u32 GetThreadNum(){return thrdNum;}

private:
	vector<threadUnit *> thrdPool;
	lockBase *thrdPoolLock;
	u32 thrdNum;
	u32 activateThrdNum;
	LPTHREAD_START_ROUTINE pwt;
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  DiskCacheCom  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//struct DiskReadIoBitMapPar
//{
//	char *readUnitBuf;
//	cacheUnit *cu;
//	cacheUnitMarkBits *bitmap;
//	u32 startIndex;
//	u32 bitLength;
//};

struct DiskIoPar
{
	ObjID diskId;
	char *ioBuf;
	u32 diskLength;
	u64 diskOffset;
	//vector<DiskReadIoBitMapPar> bmOp;
};

struct ErrRetNode
{
	ObjID fileId;
	u32 pcId;
};

struct DiskCacheNode
{
	DiskCacheNode(cacheUnit *cuPar);
	~DiskCacheNode();

	inline bool DCNWriteBytes(void *buf,u32 bufSize,u32 DCNOffset,ErrRetNode &ern);
	inline bool DCNReadBytes(void *buf,u32 bufSize,u32 DCNOffset,bool isInDirtyList,bool readRealDevice,vector<DiskIoPar> &collectReadDeviceIO);

	bool IsTimeoutInValidList();
	inline bool IsTimeoutInDirtyList();
	inline void CleanBitMap();
	inline bool IsFull();

	inline vector<ErrRetNode>::iterator& GetERAIt(){return eraIt;};
	inline vector<ErrRetNode>& GetErrRetAry(){return ErrRetAry;};
	ObjID& GetDiskId();
	u64 GetDiskOffset();
	inline u32 GetDiskLength();
	inline cacheUnit* GetCacheUnit();
	inline u32 GetIsFlushing(){return isFlushing;};

	inline void SetDiskInfo(ObjID diskId,u64 diskOffset,u32 cacheUnitSize);
	inline void SetMarkTimeInValidList();
	inline void SetMarkTimeInDirtyList();

	inline void GetWriteDeviceIOAry(vector<DiskIoPar> &collectWriteDeviceIO);
	inline void RecordeReadRealDeviceIO(vector<DiskIoPar> &crdio,DiskIoPar &driop);

	static void SetTimeOutInValidList(u32 Par){timeoutInValidList = Par;}
	static void SetTimeOutInDirtyList(u32 Par){timeoutInDirtyList = Par;}
	static void SetMinReadCopySize(u32 Par){minReadCopySize = Par;}

	inline void SetIsFlushing(u32 Par){isFlushing = Par;};
	inline FiEvent& GetIsFlushDone(){return isFlushDone;}

private:
	IoDiskInfo *pDiskInfo;
	cacheUnitMarkBits *bitmap;
	cacheUnit *cu;
	u64 markTimeInValidList;
	u64 markTimeInDirtyList;
	static u32 timeoutInValidList;
	static u32 timeoutInDirtyList;
	static u32 minReadCopySize;
	BOOL isFlushing;
	FiEvent isFlushDone;

	vector<ErrRetNode> ErrRetAry;
	vector<ErrRetNode>::iterator eraIt;
};

struct DiskCacheCom
{
	typedef vector<DiskCacheNode *>::iterator DCC_ITR;

	DiskCacheCom(ThreadPool *lwtp,u32 memSize,u32 cubs = DEFAULTCACHEUNITSIZE);
	~DiskCacheCom();

	struct InitDiskCacheNode 
	{
		InitDiskCacheNode(vector<DiskCacheNode *> &listPar):list(listPar){};
		inline void operator()(cacheUnit *CU)
		{
			list.push_back(new DiskCacheNode(CU));
		}
	private:
		vector<DiskCacheNode *> &list;
	};

	struct UnInitDiskCacheNode 
	{
		inline void operator()(DiskCacheNode *dcn)
		{
			delete dcn;
		}
	};

	struct IsInSameDiskRange 
	{
		IsInSameDiskRange(ObjID &guid,u64 doPar):diskId(guid),diskOffset(doPar){};
		inline bool operator()(DiskCacheNode *dcn)
		{
			return (dcn->GetDiskId() == diskId && dcn->GetDiskOffset() == diskOffset);
		}
	private:
		u64 diskOffset;
		ObjID &diskId;
	};

	struct IsTimeoutDiskCacheNode 
	{
		inline bool operator()(DiskCacheNode *dcn)
		{
			return dcn->IsTimeoutInValidList();
		}
	};

	struct SortDirtList
	{
		inline bool operator()(DiskCacheNode *dcnl,DiskCacheNode *dcnr)
		{
			return dcnl->GetDiskOffset() < dcnr->GetDiskOffset();
		}
	};

	struct FindSortFlushNode
	{
		FindSortFlushNode(u64 diskOffsetTP,u32 diskLengthTP):diskOffset(diskOffsetTP),diskLength(diskLengthTP){};
		inline bool operator()(DiskCacheNode *dcn)
		{
			return (dcn->GetDiskOffset() >= diskOffset + (u64)diskLength && !dcn->GetIsFlushing());
		}
	private:
		u64 diskOffset;
		u32 diskLength;
	};

	struct FindFlushNode
	{
		inline bool operator()(DiskCacheNode *dcn)
		{
			return (!dcn->GetIsFlushing());
		}
	};

	struct FindFlushDoneNode
	{
		FindFlushDoneNode(DiskCacheNode *dcnTP):dcnFind(dcnTP){};
		inline bool operator()(DiskCacheNode *dcn)
		{
			return (dcn == dcnFind);
		}
	private:
		DiskCacheNode *dcnFind;
	};

	bool DCCFlushDirtyList(NextLayer *nl,bool isFlushAll);
	bool DCCFlushAllDirtyListNode(NextLayer *nl);

	bool DCCWriteBytes(ObjID &diskId,void *writeBuf,u32 dcnSize,u32 dcnOffset,u64 diskOffset,ErrRetNode &ern);
	bool DCCReadBytes(ObjID &diskId,void *readBuf,u32 dcnSize,u32 dcnOffset,u64 diskOffset,vector<DiskIoPar> &collectReadDeviceIO);

	inline FiErr DCCOperator(IoFileInfoHandle &fih,ObjID &diskId,void *buf,u32 bufSize,u64 DiskOffset,bool isWrite,vector<DiskIoPar> *collectReadDeviceIO = NULL);

private:
	vector<DiskCacheNode *> dirtList;
	lockBase *dirtListLock;
	u64 lastWriteDiskOffset;
	u32 lastWriteDisklength;
	vector<DiskCacheNode *> validList;
	lockBase *validListLock;
	vector<DiskCacheNode *> initList;
	memPool *mp;
	ThreadPool *lazyWriteTP;
	FiEvent validListNotEmpty;

	DCC_ITR dccDirtyIt;
	DCC_ITR dccValidIt;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  FileCacheCom  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct FileCacheCom;
struct FileCacheNode;

struct FileIoPar
{
	char *ioBuf;
	u32 diskLength;
	u64 diskOffset;
};

struct FCNReadIoPar
{
	FileCacheNode *fcn;
	void *buf;
	u32 bufSize;
	u32 FCNOffset;
};

struct FileCacheNode
{
	FileCacheNode(cacheUnit *cuPar);
	~FileCacheNode();

	bool FCNWriteBytes(void *buf,u32 bufSize,u32 FCNOffset);
	bool FCNReadBytes(void *buf,u32 bufSize,u32 FCNOffset);

	inline void SetIsRHReading(u32 setVal);
	u32 GetIsRHReading();

	inline void SetMarkTime(){markTime = GetMicroSecTime();}
	inline void ReSetMarkTime(){markTime = -1;};
	bool IsTimeout();
	bool IsTimeoutFast(u64 &curTime);
	inline void SetRHNeedInfo(IoDiskInfo *pDiskInfoPar,u32 diskInfoNumPar,IoFileInfoHandle &fihtp);

	inline IoDiskInfo* GetDiskInfo(){return pDiskInfo;}
	inline u32 GetDiskInfoNum(){return diskInfoNum;}
	inline cacheUnit* GetCacheUnit(){return cu;}
	inline u32 GetCacheUnitBufSize(){return cu->GetMemPool()->GetCacheUnitBufSize();}
	inline IoFileInfoHandle* GetIoFileInfoHandle(){return pfih;}
	//inline lockBase* GetReadAheadLock(){return readAheadLock;}
	inline FiEvent& GetRHEventHandle(){return isRHDone;}

	inline bool IsAllCUBufUsed(){return bitmap->IsAllMarkBitSet();}
	inline void SetCUBufUsedRange(u32 bufSize,u32 FCNOffset){bitmap->SetBitRange(FCNOffset/DISKMINSECTORSIZE,bufSize/DISKMINSECTORSIZE);}
	inline void ResetCUBufStatus(){bitmap->ClearAllBits();}

	inline cacheUnitMarkBits * GetBitmap(){return bitmap;}
	static void SetTimeOut(u32 par){timeout = par;}

private:
	//u32 completeNum;
	//lockBase *completeNumLock;
	IoFileInfoHandle *pfih;
	u32 CNUsedSize;
	u32 diskInfoNum;
	IoDiskInfo *pDiskInfo;
	cacheUnit *cu;
	lockBase *readAheadLock;
	u64 markTime;
	static u32 timeout;
	cacheUnitMarkBits *bitmap;
	FiEvent isRHDone;
	BOOL isRHReading;
};

struct FilterDiskIoInValidList
{
	FilterDiskIoInValidList(ObjID fid,FileCacheCom *fcc,vector<IoDiskInfo> &dia,vector<FileIoPar>::iterator &fipIt);
	inline void operator()(IoDiskInfo &di);
private:
	ObjID fileId;
	FileCacheCom * currentfcc;
	vector<IoDiskInfo> &currentDiskIoAry;
	vector<FileIoPar>::iterator &mfipIt;
};

struct FilterDiskIoInRHList
{
	FilterDiskIoInRHList(FileCacheCom *fcc,vector<IoDiskInfo> &dia,vector<FileIoPar>::iterator &fipIt);
	inline void operator()(IoDiskInfo &di);
private:
	FileCacheCom * currentfcc;
	vector<IoDiskInfo> &currentDiskIoAry;
	vector<FileIoPar>::iterator &mfipIt;
};

struct FileCacheReadAheadOp
{
	typedef vector<IoFileInfoHandle *>::iterator FCRAH_ITR;
	FileCacheReadAheadOp();
	~FileCacheReadAheadOp();

	struct UnInitFileInfoNode 
	{
		inline void operator()(IoFileInfoHandle *pfih)
		{
			delete pfih;
		}
	};

	struct FindTimeOutNode 
	{
		inline bool operator()(IoFileInfoHandle *pfih)
		{
			return pfih->IsTimeout();
		}
	};

	struct SortIoLogList 
	{
		inline bool operator()(const FileReadIoLogNode &frioln1,const FileReadIoLogNode &frioln2)
		{
			return (frioln1.oriFileOffset<frioln2.oriFileOffset);
		}
	};

	struct IsTheSameUser
	{
		IsTheSameUser(IoFileInfoHandle &fih):currentfih(fih){}
		inline bool operator()(IoFileInfoHandle *pfih)
		{
			return (pfih->GetFileId() == currentfih.GetFileId());
		}
	private:
		IoFileInfoHandle &currentfih;
	};

	struct FileIoAry2DiskIoAry
	{
		FileIoAry2DiskIoAry(IoFileInfoHandle *fih,vector<IoDiskInfo> &dia):currentfih(fih),currentDiskIoAry(dia){assert(currentfih!=NULL);}
		inline void operator()(SpaceNode &sn)
		{
			currentfih->FileInfo2DiskInfo(sn,currentDiskIoAry);
		}
	private:
		IoFileInfoHandle *currentfih;
		vector<IoDiskInfo> &currentDiskIoAry;
	};

	inline bool LogUserSpeedTime(IoFileInfoHandle &fih);

	inline bool AddReadCount(IoFileInfoHandle &fih);
	inline bool AddHitCount(IoFileInfoHandle &fih);
	inline bool CleanReadCount(IoFileInfoHandle &fih);
	inline bool CleanHitCount(IoFileInfoHandle &fih);

	inline void MergeAndAlignDiskIoAry(vector<IoDiskInfo> &RHDiskIoAry);

	bool ReadAheadCheck(IoFileInfoHandle &fih,IoFileInfoHandle **pfihRet,vector<IoDiskInfo> &RHDiskIoAry);

private:
	vector<IoFileInfoHandle *> fileReadHistoryList;
	lockBase *fileReadHistoryListLock;

	FCRAH_ITR fcrahIt;
	vector<IoDiskInfo>::iterator idiIt;

	FCRAH_ITR fcrahIt1;

	//DiskCacheCom *dcc;
	//FileCacheCom *fcc;
};

struct SortReadIO 
{
	inline bool operator()(const FileIoPar &fip1,const FileIoPar &fip2)
	{
		return (fip1.diskOffset<fip2.diskOffset);
	}
};

struct FileCacheCom
{
	typedef vector<FileCacheNode *>::iterator FCC_ITR;

	FileCacheCom(DiskCacheCom *dccPar,ThreadPool *rhtp,u32 memSize,u32 cubs = DEFAULTCACHEUNITSIZE);
	~FileCacheCom();

	struct InitFileCacheNode 
	{
		InitFileCacheNode(vector<FileCacheNode *> &listPar):list(listPar){};
		inline void operator()(cacheUnit *CU)
		{
			list.push_back(new FileCacheNode(CU));
		}
	private:
		vector<FileCacheNode *> &list;
	};

	struct UnInitFileCacheNode 
	{
		inline void operator()(FileCacheNode *fcn)
		{
			delete fcn;
		}
	};

	struct UnLockFileCacheRHNode 
	{
		inline void operator()(FileCacheNode *fcn)
		{
			//fcn->GetReadAheadLock()->ReleseLock();
			fcn->GetRHEventHandle().Set();
		}
	};

	struct ReadFromFileCacheNode 
	{
		ReadFromFileCacheNode(IoFileInfoHandle &fihPar,vector<FileCacheNode *> &fcvl):fih(fihPar),fileCacheValidList(fcvl){}
		inline void operator()(FCNReadIoPar &fcnrip)
		{
			fcnrip.fcn->SetCUBufUsedRange(fcnrip.bufSize,fcnrip.FCNOffset);
			LOG_SPEC("<threadId:%lu>\t[%s]SetCUBufUsedRange---fcn[%p];bufSize[0x%x];FCNOffset[0x%x]\n",pthread_self(),__FUNCTION__,fcnrip.fcn,fcnrip.bufSize,fcnrip.FCNOffset);
			fcnrip.fcn->FCNReadBytes(fcnrip.buf,fcnrip.bufSize,fcnrip.FCNOffset);
			fcnrip.fcn->SetMarkTime();

//			if(fcnrip.fcn->IsAllCUBufUsed())	//cache node is all used then make the node timeout
//			{
//				fcnrip.fcn->ReSetMarkTime();
//				//fileCacheValidList.erase(fcnrip.fccIt);
//				fileCacheValidList.erase(find(fileCacheValidList.begin(),fileCacheValidList.end(),fcnrip.fcn));
//				fileCacheValidList.insert(fileCacheValidList.begin(),fcnrip.fcn);
//			}
		}
	private:
		IoFileInfoHandle &fih;
		vector<FileCacheNode *> &fileCacheValidList;
	};

	struct IsDiskRangeInRHList
	{
		IsDiskRangeInRHList(IoFileInfoHandle &fihPar,u32 bs,u64 dos):fih(fihPar),bufSize(bs),DiskOffset(dos){}
		inline bool operator()(FileCacheNode *fcn)
		{
			bool ret = false;
			SpaceNode snA,snB;
			IoDiskInfo *pdi = fcn->GetDiskInfo();
			for(u32 i = 0;i<fcn->GetDiskInfoNum();++i)
			{
				if(pdi[i].diskId == fih.GetDiskId())
				{
					snA.start = pdi[i].ioDiskOffset;
					snA.length = (u64)pdi[i].ioDisklength;
					snB.start = DiskOffset;
					snB.length = (u64)bufSize;
					SpaceAnalyse sa(snA,snB);
					if(sa.IsHaveSameSpace())
					{
						ret = true;
						break;
					}
				}
			}
			return ret;
		}
	private:
		u32 bufSize;
		u64 DiskOffset;
		IoFileInfoHandle &fih;
	};

	struct SynFileCacheNode 
	{
		SynFileCacheNode(IoFileInfoHandle &fihPar,char *wb,u32 wbs,u64 dos):fih(fihPar),writeBuf(wb),writeBufSize(wbs),DiskOffset(dos){}
		inline void operator()(FileCacheNode *fcn)
		{
			SpaceNode snA,snB;
			SpaceNode *sameSpace = NULL;
			IoDiskInfo *pdi = fcn->GetDiskInfo();
			if(pdi->ioDisklength == 0 || fcn->IsTimeout())
			{
				return;
			}
			u32 sum = 0;
			for(u32 i = 0;i<fcn->GetDiskInfoNum();++i)
			{
				if(pdi[i].diskId == fih.GetDiskId())
				{
					snA.start = pdi[i].ioDiskOffset;
					snA.length = (u64)pdi[i].ioDisklength;
					snB.start = DiskOffset;
					snB.length = (u64)writeBufSize;
					SpaceAnalyse sa(snA,snB);
					if(sa.IsHaveSameSpace())
					{
						sameSpace = sa.GetSameSapce();
						fcn->FCNWriteBytes(writeBuf+(u32)(sameSpace->start-DiskOffset),(u32)sameSpace->length,sum+(u32)(sameSpace->start-pdi[i].ioDiskOffset));
					}
				}
				sum += pdi[i].ioDisklength;
			}
		}
	private:
		char *writeBuf;
		u32 writeBufSize;
		u64 DiskOffset;
		IoFileInfoHandle &fih;
	};

	struct GetDiskIoSum 
	{
		GetDiskIoSum(u64 &sum):currentsum(sum){}
		inline void operator()(IoDiskInfo &di)
		{
			currentsum += di.ioDisklength;
		}
	private:
		u64 &currentsum;
	};

	struct FindTimeOutNode 
	{
		FindTimeOutNode(u64 &ct):curTime(ct){}
		inline bool operator()(FileCacheNode *fcn)
		{
			return (fcn->IsTimeoutFast(curTime));
		}
	private:
		u64 curTime;
	};

	struct FindBufUsedNode 
	{
		inline bool operator()(FileCacheNode *fcn)
		{
			return (fcn->IsAllCUBufUsed());
		}
	};

	struct SortRHList
	{
		inline bool operator()(FileCacheNode *fcnl,FileCacheNode *fcnr)
		{
			return fcnl->GetDiskInfo()->ioDiskOffset < fcnr->GetDiskInfo()->ioDiskOffset;
		}
	};

	struct FindSortedRHNode
	{
		FindSortedRHNode(u64 diskOffsetTP,u32 diskLengthTP):diskOffset(diskOffsetTP),diskLength(diskLengthTP){};
		inline bool operator()(FileCacheNode *fcn)
		{
			return ((!fcn->GetIsRHReading()) && (fcn->GetDiskInfo()->ioDiskOffset >= diskOffset + (u64)diskLength));
		}
	private:
		u64 diskOffset;
		u32 diskLength;
	};

	struct FindRHNode
	{
		FindRHNode(){};
		inline bool operator()(FileCacheNode *fcn)
		{
			return (!fcn->GetIsRHReading());
		}
	};

	void CheckFromFileCacheNode(FileCacheNode *fcn,ObjID &diskId,vector<FileIoPar> &collectReadDiskCacheIO,vector<FCNReadIoPar> *pCollectReadFileCacheIO);
	inline void CheckBufFromValidList(ObjID fileId,ObjID &diskId,char *readBuf,u32 readBufSize,u64 DiskOffset,vector<FileIoPar> &collectReadDiskCacheIO,vector<FCNReadIoPar> *pCollectReadFileCacheIO);
	inline void CheckBufFromRHList(ObjID &diskId,char *readBuf,u32 readBufSize,u64 DiskOffset,vector<FileIoPar> &collectReadDiskCacheIO,vector<FCNReadIoPar> *pCollectReadFileCacheIO);
	inline bool ReadFromDiskCache(IoFileInfoHandle &fih,vector<FileIoPar> &collectReadDiskCacheIO,char *readBuf,u32 readBufSize,u64 DiskOffset);

	FiErr FCCWriteBytes(IoFileInfoHandle &fih,char *writeBuf,u32 writeBufSize,u64 DiskOffset);
	FiErr FCCReadBytes(IoFileInfoHandle &fih,char *readBuf,u32 readBufSize,u64 DiskOffset,vector<FileIoPar> &collectReadDiskCacheIO);
	bool FCCReadAhead(NextLayer *nl);

	//inline vector<FileCacheNode *>& GetFileCacheRHList(){return fileCacheRHList;}
	inline vector<FileCacheNode *>& GetFileCacheValidList(){return fileCacheValidList;}

private:
	bool StartReadAhead(vector<IoDiskInfo> &RHDiskIoAry,IoFileInfoHandle &fihtp);
	inline bool SendToRHThread(vector<IoDiskInfo> &tempDIArry,IoFileInfoHandle &fihtp);
	inline FileCacheNode* GetFileCacheNodeForRH();
	inline void AddFileCacheNodeToValidList(FileCacheNode *fcn,bool isAddFront = false);
	inline void DelFileCacheNodeFromValidList(FCC_ITR &fccValidItTp);

	typedef pair<ObjID,vector<FileCacheNode *>*> fccHashNode;
	typedef vector<fccHashNode>::iterator FCC_HASHNODE_ITR;

	struct FccHashNodeCompare 
	{
		bool operator()(const fccHashNode& lhs,const fccHashNode& rhs) const
		{
			return keyLess(lhs.first, rhs.first);
		} 

		bool operator()(const fccHashNode& lhs,const fccHashNode::first_type& k) const
		{
			return keyLess(lhs.first, k);
		} 

		bool operator()(const fccHashNode::first_type& k,const fccHashNode& rhs) const
		{
			return keyLess(k, rhs.first);
		}

	private:
		bool keyLess(const fccHashNode::first_type& k1,const fccHashNode::first_type& k2) const
		{
			return k1 < k2;
		}
	};

	vector<FileCacheNode *> fileCacheRHList;
	//vector<fccHashNode> fileCacheRHHashList;		//Future Optimization
	lockBase *fileCacheRHListLock;
	vector<FileCacheNode *> fileCacheValidList;
	vector<fccHashNode> fileCacheValidHashList;
	lockBase *fileCacheValidListLock;
	vector<FileCacheNode *> fileCacheList;

	FileCacheReadAheadOp *fcReadAheadOp;
	memPool *mp;
	ThreadPool *readAheadTP;

	u64 lastRHDiskOffset;
	u32 lastRHDisklength;

	bool isRHReading;

	FCC_ITR fccRHIt;
	FCC_ITR fccValidIt;
	//FCC_ITR fccValidIt1;

	vector<FileIoPar>::iterator fipIt;
	//vector<IoDiskInfo>::iterator idiIt;

	pair<FCC_HASHNODE_ITR,FCC_HASHNODE_ITR> validListRange;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  cache (support file level readahead & disk level lazywrite)  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct CacheConfig
{
	u32 isUseFileCache;
	u32 isUseDiskCache;

	u32 lazyWriteThreadNum;
	u32 readAheadThreadNum;
	
	u32 diskCacheSize;
	u32 diskCacheUnitSize;
	u32 diskCacheNodeTimeOutInValidList;
	u32 diskCacheNodeTimeOutInDirtyList;
	u32 diskCacheNodeMinReadCopySize;

	u32 fileCacheSize;
	u32 fileCacheUnitSize;
	u32 fileCacheNodeTimeOut;
	u32 fileHistoryNodeTimeOut;
	u32 fileRHMaxSize;
	u32 fileRHMinSize;
	u32 fileRHSeriesNum;

	u32 isOpRealDevice;
};

struct NextLayer
{
	virtual FiErr WriteDatas(ObjID fileID,char *buf,u32 length,u64 offset);
	virtual FiErr ReadDatas(ObjID fileID,char *buf,u32 length,u64 offset);
};

struct cacheReadWrite
{
	cacheReadWrite(string cfgFile,NextLayer *nlPar);
	~cacheReadWrite();

	inline DiskCacheCom* GetDiskCache(){return diskCache;}
	inline FileCacheCom* GetFileCache(){return fileCache;}
	inline ThreadPool* GetLazyWriteTP(){return lazyWriteTP;}
	inline ThreadPool* GetReadAheadTP(){return readAheadTP;}
	inline NextLayer* GetNextLayer(){return nl;}

	FiErr WriteDatas(ObjID fileID,char *buf,u32 length,u64 offset,u32 pcID = 0);
	FiErr ReadDatas(ObjID fileID,char *buf,u32 length,u64 offset,u32 pcID = 0);
	FiErr FlushDatas();
private:
	bool GetCacheConfig(CacheConfig &cc,string &cfgFile);

	DiskCacheCom *diskCache;
	FileCacheCom *fileCache;
	ThreadPool *lazyWriteTP;
	ThreadPool *readAheadTP;
	bool isUseFileCache;
	bool isUseDiskCache;
	NextLayer *nl;
};

#endif
