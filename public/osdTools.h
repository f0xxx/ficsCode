///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  hunter code ([hunter.f0x] remeber my brothers) email:hunterlhy@163.com /////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __OSDTOOLS_H__
#define __OSDTOOLS_H__

#define __linux__
#if defined(__linux__) || defined(__linux) //linux only

#include "../include_com/os.h"

//#define OSD_DEBUG
#define OSD_DEBUG_NONET

#define RELEASE_ASSERT_FASLE (*((char*)0) = 18)
#define RELEASE_ASSERT(exp) \
	{   \
		if (!(exp)) \
		{   \
			RELEASE_ASSERT_FASLE;\
		}   \
	}

#ifdef OSD_DEBUG
#define OSD_LOG_ERR STRACE
#define OSD_LOG_DBG STRACE
#else
//#define OSD_LOG_ERR VTRACE
#define OSD_LOG_ERR STRACE
#define OSD_LOG_DBG STRACE
//#define OSD_LOG_ERR
//#define OSD_LOG_DBG
#endif

//轻量级锁
struct OsdLock
{
    OsdLock(){pthread_mutexattr_init( &m_mutexAttr );pthread_mutexattr_settype( &m_mutexAttr, PTHREAD_MUTEX_RECURSIVE_NP );pthread_mutex_init( &mutex, &m_mutexAttr );}
    ~OsdLock(){pthread_mutex_destroy(&mutex);pthread_mutexattr_destroy( &m_mutexAttr );}

	void Lock(){pthread_mutex_lock(&mutex);}
    int TryLock(){return pthread_mutex_trylock(&mutex);}
    void Unlock(){pthread_mutex_unlock(&mutex);}
  private:
    pthread_mutex_t mutex;
    pthread_mutexattr_t m_mutexAttr;
};

struct AutoLockHelper
{
	AutoLockHelper(OsdLock *lockPar):lock(lockPar){RELEASE_ASSERT(lock != NULL);lock->Lock();}
	~AutoLockHelper(){lock->Unlock();}
private:
	OsdLock *lock;
};

struct SpaceNode
{
	u64 start;
	u64 length;
};

struct SpaceAnalyse
{
	SpaceAnalyse(SpaceNode &APar,SpaceNode &BPar);
	
	inline bool IsHaveSameSpace(){return isHaveSameSpace;}
	inline SpaceNode* GetSameSapce(){return &Same;}
	inline SpaceNode* GetADiff(){return ADiff;}
	inline SpaceNode* GetBDiff(){return BDiff;}
private:
	SpaceNode &A;
	SpaceNode &B;
	bool isHaveSameSpace;
	SpaceNode Same;
	SpaceNode ADiff[2];
	SpaceNode BDiff[2];
};
	
struct IoPiece
{
	char *buf;
	u32 order;
	u32 length;
	u64 offset;
}; 
	
struct IoDivide
{
	IoDivide(u32 grainSizePar,char *bufPar,u32 lengthPar,u64 offsetPar):	\
		grainSize(grainSizePar),	\
		buf(bufPar),	\
		length(lengthPar),	\
		offset(offsetPar)

	{
		RELEASE_ASSERT(grainSize != 0 && buf != NULL && length != 0);
		ioPieces.reserve(3);

		SpaceNode snA, snB;
		snB.start = 0;
		snB.length = grainSize;

		char *bufTp = buf;
		u64 offsetTp = offset;
		u32 lengthTp = length;

		do
		{
			snA.start = offsetTp % grainSize;
			snA.length = lengthTp;
			SpaceAnalyse sa(snA, snB);

			IoPiece iop;
			iop.buf = bufTp;
			iop.order = offsetTp / grainSize;
			iop.length = sa.GetSameSapce()->length;
			iop.offset = sa.GetSameSapce()->start;
			ioPieces.push_back(iop);

			bufTp += sa.GetSameSapce()->length;
			offsetTp += sa.GetSameSapce()->length;
			lengthTp -= sa.GetSameSapce()->length;
		}
		while (lengthTp);
	}

	inline vector<IoPiece>& GetIoPieces(){return ioPieces;}

private:
	u32 grainSize;
	char *buf;
	u32 length;
	u64 offset;
	vector<IoPiece> ioPieces;
};

unsigned int str2u32(const char *str);

#else

#include "../include_com/BaseType.h"
#include "../include_com/OSMutex.h"
#include <vector>
using namespace std;
#define RELEASE_ASSERT_FASLE (*((char*)0) = 18)
#define RELEASE_ASSERT(exp) \
	{   \
		if (!(exp)) \
		{   \
			RELEASE_ASSERT_FASLE;\
		}   \
	}

#ifdef OSD_DEBUG
#define OSD_LOG_ERR STRACE
#define OSD_LOG_DBG STRACE
#else
#define OSD_LOG_ERR STRACE
#define OSD_LOG_DBG STRACE
#endif

//轻量级锁
struct OsdLock
{
    OsdLock(){}
    ~OsdLock(){}

	void Lock(){ m_lock.Lock();}
    int TryLock(){Lock();}
    void Unlock(){m_lock.Unlock();}
  private:
	  CCritSec m_lock;
};

struct SpaceNode
{
	u64 start;
	u64 length;
};

struct SpaceAnalyse
{
	SpaceAnalyse(SpaceNode &APar,SpaceNode &BPar);
	
	inline bool IsHaveSameSpace(){return isHaveSameSpace;}
	inline SpaceNode* GetSameSapce(){return &Same;}
	inline SpaceNode* GetADiff(){return ADiff;}
	inline SpaceNode* GetBDiff(){return BDiff;}
private:
	SpaceNode &A;
	SpaceNode &B;
	bool isHaveSameSpace;
	SpaceNode Same;
	SpaceNode ADiff[2];
	SpaceNode BDiff[2];
};
	
struct IoPiece
{
	char *buf;
	u32 order;
	u32 length;
	u64 offset;
}; 
	
struct IoDivide
{
	IoDivide(u32 grainSizePar,char *bufPar,u32 lengthPar,u64 offsetPar):	\
		grainSize(grainSizePar),	\
		buf(bufPar),	\
		length(lengthPar),	\
		offset(offsetPar)

	{
		RELEASE_ASSERT(grainSize != 0 && buf != NULL && length != 0);
		ioPieces.reserve(3);

		SpaceNode snA, snB;
		snB.start = 0;
		snB.length = grainSize;

		char *bufTp = buf;
		u64 offsetTp = offset;
		u32 lengthTp = length;

		do
		{
			snA.start = offsetTp % grainSize;
			snA.length = lengthTp;
			SpaceAnalyse sa(snA, snB);

			IoPiece iop;
			iop.buf = bufTp;
			iop.order = (u32)(offsetTp / grainSize);
			iop.length = (u32)(sa.GetSameSapce()->length);
			iop.offset = sa.GetSameSapce()->start;
			ioPieces.push_back(iop);

			bufTp += sa.GetSameSapce()->length;
			offsetTp += sa.GetSameSapce()->length;
			lengthTp -= (u32)(sa.GetSameSapce()->length);
		}
		while (lengthTp);
	}

	inline std::vector<IoPiece>& GetIoPieces(){return ioPieces;}

private:
	u32 grainSize;
	char *buf;
	u32 length;
	u64 offset;
	vector<IoPiece> ioPieces;
};

unsigned int str2u32(const char *str);

#endif

#endif
