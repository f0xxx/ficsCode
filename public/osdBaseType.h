///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  hunter code ([hunter.f0x] remeber my brothers) email:hunterlhy@163.com /////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __OSDBASETYPE_H__
#define __OSDBASETYPE_H__

#include "osdTools.h"
#if defined(__linux__) || defined(__linux)
#include <uuid/uuid.h>
#include <string>
using namespace std;

#define RESERVE_BUF_SIZE 100

#define DATA_NODE_MAX_NUM 6		//受元数据的大小限制（osd获得100字节支持）,DATA_NODE_MAX_NUM为6占用24个字节,剩下的空间保留使用。注:此宏必须大于等于2

#pragma pack(push,1)

enum RddcPolicy
{
	FiDefault = 0,
	FiRAID1,
	FiRAID5
};

struct ObjInfoSequence
{
	s32 rev1;
	s32 rev2;
	s32 metaNode[2];			//设计上元数据始终做备份处理
	RddcPolicy DataRP;
	s32 dataNodeNum;
	s32 dataNode[DATA_NODE_MAX_NUM];	//数据上可以支持多种raid
};

struct ObjID
{
	ObjID(){uuid_clear(oname);}
	ObjID(uuid_t &onp){uuid_copy(oname,onp);}
	ObjID(string &onpStr){RELEASE_ASSERT(onpStr.length() == 36);uuid_parse(onpStr.c_str(),oname);}
	ObjID(const ObjID &oid){uuid_copy(oname,oid.oname);}

	inline bool operator < (const ObjID &dst) const{return (uuid_compare(oname, dst.oname) < 0);}
	inline bool operator > (const ObjID &dst) const{return (uuid_compare(oname, dst.oname) > 0);}
	inline bool operator == (const ObjID &dst) const{return (uuid_compare(oname, dst.oname) == 0);}
	inline bool operator != (const ObjID &dst) const{return (uuid_compare(oname, dst.oname) != 0);}
	inline bool IsNull(){return (uuid_is_null(oname) == 1);}

	inline void GenerateID(){uuid_generate(oname);}
	inline void SetID(uuid_t &onp){uuid_copy(oname,onp);}
	inline void GetID(uuid_t &onp){uuid_copy(onp,oname);}
	inline void SetIDStr(string &onpStr){RELEASE_ASSERT(onpStr.length() == 36);uuid_parse(onpStr.c_str(),oname);}
	inline void GetIDStr(string &onpStr){char tp[36] = {0};uuid_unparse_upper(oname,tp);onpStr = tp;}
	inline string GetIDStr() const{string onpStr;char tp[36] = {0};uuid_unparse_upper(oname,tp);onpStr = tp;return onpStr;}
	inline void ClearID(){uuid_clear(oname);}

	inline ObjID& operator = (const ObjID &src) {uuid_copy(oname,src.oname);return *this;}
	inline u32 GetU32() {u32 ret;memcpy(&ret,oname,sizeof(u32));return ret;}

private:
	uuid_t oname;
};

#pragma pack(pop)

struct ObjInfoHandle
{
	struct ObjInfo
	{
		ObjInfo():objInfoBufLen(0){};

		ObjInfo(ObjID &fid,char *oib,u32 oil):fileID(fid),objInfoBufLen(oil)
		{
			memcpy(objInfoBuf,oib,oil);
		};

		ObjID fileID;
		u32 objInfoBufLen;
		char objInfoBuf[RESERVE_BUF_SIZE];
	};

	ObjInfoHandle():oi(NULL),cnt(new s32(1)){oi = new ObjInfo();RELEASE_ASSERT(oi != NULL);}
	ObjInfoHandle(ObjID &fid,s32 nodeID = -1);
	ObjInfoHandle(ObjID &fid,s32 mainNode,s32 bakNode):oi(NULL),cnt(new s32(1))
	{
		RELEASE_ASSERT(!fid.IsNull());

		oi = new ObjInfo();
		RELEASE_ASSERT(oi != NULL);

		oi->fileID = fid;
		ObjInfoSequence *bufTp = (ObjInfoSequence *)oi->objInfoBuf;
		bufTp->rev1 = bufTp->metaNode[0] = mainNode;
		bufTp->rev2 = bufTp->metaNode[1] = bakNode;
	}

	ObjInfoHandle(ObjID &fid,char *oib,u32 oil):oi(NULL),cnt(new s32(1))
	{
		RELEASE_ASSERT(oil <= RESERVE_BUF_SIZE && oib != NULL && !fid.IsNull());

		oi = new ObjInfo(fid,oib,oil);
		RELEASE_ASSERT(oi != NULL);
	}

	ObjInfoHandle(const ObjInfoHandle& oih)
	{
		l.Lock();
		cnt = oih.cnt;
		oi = oih.oi;
		++*cnt;
		l.Unlock();
	}
	ObjInfoHandle& operator=(const ObjInfoHandle& oih)
	{
		l.Lock();
		++*oih.cnt;
		if (--*cnt==0)
		{
			delete cnt;
			if (oi != NULL)
			{
				delete oi;
			}
		}
		cnt=oih.cnt;
		oi=oih.oi;
		l.Unlock();
		return *this;
	}

	~ObjInfoHandle()
	{
		l.Lock();
		if (--*cnt==0)
		{
			delete cnt;
			if (oi != NULL)
			{
				delete oi;
			}
		}
		l.Unlock();
	}

	inline ObjID& GetFileID(){return oi->fileID;}
	inline u32 GetObjInfoBufLen(){return oi->objInfoBufLen;}
    inline void SetFileID(uuid_t &id){ oi->fileID.SetID(id);}
	inline u32 GetMetaMainNode(){return ((ObjInfoSequence*)oi->objInfoBuf)->metaNode[0];}
	inline u32 GetMetaBakNode(){return ((ObjInfoSequence*)oi->objInfoBuf)->metaNode[1];}
	inline RddcPolicy GetDataRP(){return ((ObjInfoSequence*)oi->objInfoBuf)->DataRP;}
	inline s32 GetDataNodeNum(){return ((ObjInfoSequence*)oi->objInfoBuf)->dataNodeNum;}
	inline s32* GetDataNodeAry(){return ((ObjInfoSequence*)oi->objInfoBuf)->dataNode;}
	inline void SetRev1(s32 par){((ObjInfoSequence*)oi->objInfoBuf)->rev1 = par;}
	inline s32 GetRev1(){return ((ObjInfoSequence*)oi->objInfoBuf)->rev1;}
	inline void SetRev2(s32 par){((ObjInfoSequence*)oi->objInfoBuf)->rev2 = par;}
	inline s32 GetRev2(){return ((ObjInfoSequence*)oi->objInfoBuf)->rev2;}

	//请保证在使用对象a获取此buf后，使用buf的过程中a没有析构
	inline char* GetObjInfoBuf(){return oi->objInfoBuf;}

private:
	s32 *cnt;
	ObjInfo *oi;
	static OsdLock l;
};

#else
#include <Rpc.h>
#define RESERVE_BUF_SIZE 100

#define DATA_NODE_MAX_NUM 6		//受元数据的大限制（osd获得100字节支持）,DATA_NODE_MAX_NUM为6占用24个字节,剩下的空间保留使用。注:此宏必须大于等于2

#pragma pack(push,1)

enum RddcPolicy
{
	FiDefault = 0,
	FiRAID1,
	FiRAID5
};

struct ObjInfoSequence
{
	s32 rev1;
	s32 rev2;
	s32 metaNode[2];			//设计上元数据始终做备份处理
	RddcPolicy DataRP;
	s32 dataNodeNum;
	s32 dataNode[DATA_NODE_MAX_NUM];	//数据上可以支持多种raid
};

struct ObjID
{
	ObjID(){memset(&oname,0,sizeof(GUID));}
	ObjID(GUID &onp){oname=onp;}
// 	ObjID(string &onpStr){RELEASE_ASSERT(onpStr.length() == 36);uuid_parse(onpStr.c_str(),oname);}
// 	ObjID(const ObjID &oid){uuid_copy(oname,oid.oname);}
// 
// 	inline bool operator < (const ObjID &dst) const{return (uuid_compare(oname, dst.oname) < 0);}
// 	inline bool operator > (const ObjID &dst) const{return (uuid_compare(oname, dst.oname) > 0);}
// 	inline bool operator == (const ObjID &dst) const{return (uuid_compare(oname, dst.oname) == 0);}
// 	inline bool operator != (const ObjID &dst) const{return (uuid_compare(oname, dst.oname) != 0);}
 	inline bool IsNull(){GUID temp;memset(&temp,0,sizeof(GUID));return ((bool)(temp==oname));}
// 
// 	inline void GenerateID(){uuid_generate(oname);}
 	inline void SetID(GUID &onp){oname=onp;}
// 	inline void GetID(uuid_t &onp){uuid_copy(onp,oname);}
// 	inline void SetIDStr(string &onpStr){RELEASE_ASSERT(onpStr.length() == 36);uuid_parse(onpStr.c_str(),oname);}
 	inline char* GetIDStr()
	{
		static char tp[36] = {0};
		sprintf(tp,("%x-%x-%x-%x%x-%x%x%x%x%x%x"),	\
			oname.Data1,	\
			oname.Data2,	\
			oname.Data3,	\
			*((unsigned char *)oname.Data4),	\
			*((unsigned char *)oname.Data4+1),	\
			*((unsigned char *)oname.Data4+2),	\
			*((unsigned char *)oname.Data4+3),	\
			*((unsigned char *)oname.Data4+4),	\
			*((unsigned char *)oname.Data4+5),	\
			*((unsigned char *)oname.Data4+6),	\
			*((unsigned char *)oname.Data4+7));
		return tp;
	}
// 	inline string GetIDStr() const{string onpStr;char tp[36] = {0};uuid_unparse_upper(oname,tp);onpStr = tp;return onpStr;}
// 	inline void ClearID(){uuid_clear(oname);}
// 
 	inline ObjID& operator = (const ObjID &src) {memcpy(&oname,&src,sizeof(GUID));return *this;}

private:
	GUID oname;
};

#pragma pack(pop)

struct ObjInfoHandle
{
	struct ObjInfo
	{
		ObjInfo():objInfoBufLen(0){};

		ObjInfo(ObjID &fid,char *oib,u32 oil):fileID(fid),objInfoBufLen(oil)
		{
			memcpy(objInfoBuf,oib,oil);
		};

		ObjID fileID;
		u32 objInfoBufLen;
		char objInfoBuf[RESERVE_BUF_SIZE];
	};

	ObjInfoHandle():oi(NULL),cnt(new s32(1)){oi = new ObjInfo();RELEASE_ASSERT(oi != NULL);}
	ObjInfoHandle(ObjID &fid,s32 nodeID = -1);
	ObjInfoHandle(ObjID &fid,char *oib,u32 oil):oi(NULL),cnt(new s32(1))
	{
		RELEASE_ASSERT(oil <= RESERVE_BUF_SIZE && oib != NULL && !fid.IsNull());

		oi = new ObjInfo(fid,oib,oil);
		RELEASE_ASSERT(oi != NULL);
	}

	ObjInfoHandle(const ObjInfoHandle& oih)
	{
		l.Lock();
		cnt = oih.cnt;
		oi = oih.oi;
		++*cnt;
		l.Unlock();
	}
	ObjInfoHandle& operator=(const ObjInfoHandle& oih)
	{
		l.Lock();
		++*oih.cnt;
		if (--*cnt==0)
		{
			delete cnt;
			if (oi != NULL)
			{
				delete oi;
			}
		}
		cnt=oih.cnt;
		oi=oih.oi;
		l.Unlock();
		return *this;
	}

	~ObjInfoHandle()
	{
		l.Lock();
		if (--*cnt==0)
		{
			delete cnt;
			if (oi != NULL)
			{
				delete oi;
			}
		}
		l.Unlock();
	}

	inline ObjID& GetFileID(){return oi->fileID;}
	inline u32 GetObjInfoBufLen(){return oi->objInfoBufLen;}
    inline void SetFileID(uuid_t &id){ oi->fileID.SetID(id);}
	inline u32 GetMetaMainNode(){return ((ObjInfoSequence*)oi->objInfoBuf)->metaNode[0];}
	inline u32 GetMetaBakNode(){return ((ObjInfoSequence*)oi->objInfoBuf)->metaNode[1];}
	inline RddcPolicy GetDataRP(){return ((ObjInfoSequence*)oi->objInfoBuf)->DataRP;}
	inline s32 GetDataNodeNum(){return ((ObjInfoSequence*)oi->objInfoBuf)->dataNodeNum;}
	inline s32* GetDataNodeAry(){return ((ObjInfoSequence*)oi->objInfoBuf)->dataNode;}
	inline void SetRev1(s32 par){((ObjInfoSequence*)oi->objInfoBuf)->rev1 = par;}
	inline s32 GetRev1(){return ((ObjInfoSequence*)oi->objInfoBuf)->rev1;}
	inline void SetRev2(s32 par){((ObjInfoSequence*)oi->objInfoBuf)->rev2 = par;}
	inline s32 GetRev2(){return ((ObjInfoSequence*)oi->objInfoBuf)->rev2;}

	//请保证在使用对象a获取此buf后，使用buf的过程中a没有析构
	inline char* GetObjInfoBuf(){return oi->objInfoBuf;}

private:
	s32 *cnt;
	ObjInfo *oi;
	static OsdLock l;
};

#endif

#endif

