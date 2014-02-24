///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  hunter code ([hunter.f0x] remeber my brothers) email:hunterlhy@yahoo.cn /////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __OBJIO_H__
#define __OBJIO_H__
#include <map>
#include "osdCfg.h"
#include "../public/osdBaseType.h"
using namespace std;

enum ObjDataType
{
	Meta = 0,
	Data
//  MetaExLink
};

enum ObjIoType
{
	MetaRead = 0,
	MetaWrite,
	MetaDel,
	MetaTruncate,

//	JournalRead,
	JournalWrite,
//	JournalDel,
//	JournalTruncate,

	DataRead,
	DataWrite,
//	DataDel,
	DataTruncate,
	DataDistributeTruncate,

	NetRead,
	NetWrite,
	NetDel,
	NetTruncate,

	RedirectRead,
	RedirectWrite,
	RedirectDel,
	RedirectTruncate,

	RecoverWrite,
	RecoverTruncate,

	ConsistencyRead,
	NetConsistencyRead,
	ConsistencyTruncate
};
const char* GetObjIoTypeStr(u32 num);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  BaseObjIo  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct BaseObjIo
{
//  void InitDataCmdObjIo(char *objInfoBuf)
//  {
//  	ObjInfoSequence *bis = (ObjInfoSequence *)objInfoBuf;
//  	RELEASE_ASSERT(bis->dataNodeNum <= DATA_NODE_MAX_NUM && length <= OsdCfg::GetOsdCfg()->GetDataSliceSize());
//
//  	rp = bis->DataRP;
//  	RELEASE_ASSERT(rp == FiRAID1);
//  	nodeNum = bis->dataNodeNum;
//  	memcpy(nodeInfo,bis->dataNode,nodeNum*sizeof(u32));
//  	for (u32 i=0;i<nodeNum;++i)
//  	{
//  		if (nodeInfo[i] == srcNode)
//  		{
//  			if (i+1<nodeNum)
//  			{
//  				dstNode = nodeInfo[i+1];
//  			}
//  			else
//  			{
//  				dstNode = nodeInfo[0];
//  			}
//  			break;
//  		}
//  	}
//
//  	objIoNum = 1;
//  	objIoAry[0].buf = buf;
//  	objIoAry[0].bufLen = length;
//  	objIoAry[0].offset = offset%OsdCfg::GetOsdCfg()->GetDataSliceSize();
//  	objIoAry[0].fileNo = offset/OsdCfg::GetOsdCfg()->GetDataSliceSize();
//  }
	inline bool IsMetaType(){return !(Data == dataType);}
	inline void SetDstNode(s32 nodeID){dstNode = nodeID;}

	inline void SetRetStatus(FiErr par){ret = par;}
	inline FiErr GetRetStatus(){return ret;}

	inline char* GetBuf(){return buf;}
	inline u32 GetLength(){return length;}
	inline u64 GetOffset(){return offset;}
	inline u32 GetRetLength(){return retLength;}
	inline s32 GetNodeNum(){return nodeNum;}
	inline s32* GetNodeInfo(){return nodeInfo;}
	inline ObjDataType GetDataType(){return dataType;}

	inline ObjID& GetFileID(){return fileID;}
//  inline ObjIoType GetObjDataType(){return dataType;}
	inline ObjIoType GetObjIoType(){return ot;}
	inline ObjIoType GetSrcObjIoType(){return srcOt;}
	inline u32 GetVersion(){return version;}
	inline u32 GetObjIoNum(){return objIoNum;}
	inline IoPiece* GetObjIoAry(){return objIoAry;}
	inline s32 GetSrcNode(){return srcNode;}
	inline u32 GetSrcNodeIP(){return OsdCfg::GetOsdCfg().GetIP(srcNode);}
	inline u32 GetDstNodeIP(){return OsdCfg::GetOsdCfg().GetIP(dstNode);}

	inline void AddRetLength(u32 addLength){retLength += addLength;}

//  virtual BaseObjIo* CreateObjIo(){RELEASE_ASSERT_FASLE;}
	virtual void DelObjIo(BaseObjIo* objIo){RELEASE_ASSERT_FASLE;}
	virtual void InitObjIo(char *headBuf, char *bufPar = NULL, u64 lenPar = 0, u64 offsetPar = 0){RELEASE_ASSERT_FASLE;}
	virtual void ObjStreamProcessObjIo(){RELEASE_ASSERT_FASLE;}
	virtual void Change2NetObjIo(){RELEASE_ASSERT_FASLE;}
	virtual s32 GetDstNode(){RELEASE_ASSERT_FASLE;}
	virtual	bool IsHaveSendBuf(){return false;}
	virtual	bool IsHaveReplyBuf(){return false;}
	virtual string GetStorPath(u32 no){RELEASE_ASSERT_FASLE;}
	virtual s32 GetMainNode(){RELEASE_ASSERT_FASLE;}
	virtual s32 GetBakNode(){RELEASE_ASSERT_FASLE;}
//	virtual string GetPointerStorPath(u32 no)
//	{
//		char noStr[25] = {0};
//		sprintf(noStr,"/data%d",no);
//		if (dataType == Meta)
//		{
//			RELEASE_ASSERT(no < OsdCfg::GetOsdCfg().GetMetaMaxFileNo());
//			return OsdCfg::GetOsdCfg().GetMetaRootPath()+fileID.GetIDStr()+string(noStr)+string("_pointer");
//		}
//		else
//		{
//			RELEASE_ASSERT(no < OsdCfg::GetOsdCfg().GetDataMaxFileNo());
//			return OsdCfg::GetOsdCfg().GetDataRootPath()+fileID.GetIDStr()+string(noStr)+string("_pointer");
//		}
//	}
	virtual void ProcessNetReplyBuf(char* bufPar){RELEASE_ASSERT_FASLE;}
	virtual FiErr LogForRecover(){FiErr ret;return ret;}
	virtual bool IsSetDoRecover(){return false;}
	virtual bool IsCheckNodeStatus(){return true;}

//private:
	u32 version;

	ObjID fileID;

	ObjDataType dataType;
	ObjIoType srcOt;
	ObjIoType ot;

	FiErr ret;

	s32 nodeNum;
	s32 nodeInfo[DATA_NODE_MAX_NUM];	//raid1:nodeInfo[0] == mainNode,nodeInfo[1] == bakNode;raid5:nodeInfo[DATA_NODE_MAX_NUM]
	RddcPolicy rp;

	s32 srcNode;
	s32 dstNode;

	char *buf;
	u64 length;
	u64 offset;

	u32 retLength;		//read ret length

	u32 objIoNum;		//<=2
	IoPiece objIoAry[2];
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  ObjIoMutex  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ObjIoMutex
{
	struct IoCompare
	{
		inline bool operator()(BaseObjIo *l,BaseObjIo *r)
		{
			return l->fileID < r->fileID;
		}
	};

	struct SetEachEvt 
	{
		SetEachEvt(BaseObjIo *ioPar):io(ioPar){}
		inline void operator()(FiEvent *e)
		{
			OSD_LOG_ERR("===SetEachEvt=== io[%p],e[%p] set evt & erase",io,e);
			e->Set();
		}
	private:
		BaseObjIo *io;
	};
	
	void IoAdd(BaseObjIo *io)
	{
		ioAryLock.Lock();
		OSD_LOG_ERR("===IoAdd=== io[%p]",io);
		ioAry.push_back(io);
		sort(ioAry.begin(), ioAry.end(), IoCompare());
		emptyEvt.Reset();
		ioAryLock.Unlock();
	}

	void IoDel(BaseObjIo *io)
	{
		ioAryLock.Lock();
		ioAry.erase(find(ioAry.begin(),ioAry.end(),io));
		OSD_LOG_ERR("===IoDel=== io[%p]",io);
		itm = waitMap.find(io);
		if(waitMap.end() != itm)
		{
			for_each(itm->second.begin(),itm->second.end(),SetEachEvt(io));
			waitMap.erase(itm);
		}
		else
		{
			OSD_LOG_ERR("===IoDel=== io[%p] waitMap not find",io);
		}
		if(ioAry.empty())
		{
			emptyEvt.Set();
		}
		ioAryLock.Unlock();
	}

	bool IoFind(BaseObjIo *io,FiEvent *pEvt)
	{
		bool ret = false;
		SpaceNode snA, snB;
		snB.start = io->GetOffset();
		snB.length = io->GetLength();

		ioAryLock.Lock();
		rst = equal_range(ioAry.begin(),ioAry.end(),io,IoCompare());
		if(rst.first != rst.second)
		{
			for (itv = rst.first; itv != rst.second; ++itv)
			{
				BaseObjIo *ioTmp = (BaseObjIo *)(*itv);
				if (ioTmp->GetDataType() != io->GetDataType())
				{
					continue;
				}
				snA.start = ioTmp->GetOffset();
				snA.length = ioTmp->GetLength();
				SpaceAnalyse sa(snA, snB);
				if(sa.IsHaveSameSpace())
				{
					itm = waitMap.find(ioTmp);
					if(waitMap.end() != itm)
					{
						if(itm->second.end() == find(itm->second.begin(),itm->second.end(),pEvt))
						{
							itm->second.push_back(pEvt);
						}
					}
					else
					{
						vector<FiEvent*> pEvtAry;
						pEvtAry.push_back(pEvt);
						waitMap.insert(value_type(ioTmp, pEvtAry));
					}
					OSD_LOG_ERR("===IoFind=== insert io[%p],e[%p]",ioTmp,pEvt);
					ret = true;
					break;
				}			
			}
		}
		ioAryLock.Unlock();
		return ret;
	}

	inline void WaitEmpty(){emptyEvt.Wait();}
	inline void AssertEmpty(){RELEASE_ASSERT(ioAry.empty());}

	ObjIoMutex()
	{
		ioAry.reserve(64);
		emptyEvt.SetMode(TRUE);
		emptyEvt.Set();
	}

private:
	vector<BaseObjIo*> ioAry;
	OsdLock ioAryLock;
	FiEvent emptyEvt;
	vector<BaseObjIo*>::iterator itv;

	map<BaseObjIo*,vector<FiEvent*> > waitMap;
	map<BaseObjIo*,vector<FiEvent*> >::iterator itm;
	typedef map<BaseObjIo*,vector<FiEvent*> >::value_type value_type;

	pair <vector<BaseObjIo*>::iterator,vector<BaseObjIo*>::iterator> rst;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  ObjIoPool  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ObjIoPool
{
	static ObjIoPool& GetObjIoPool(){static ObjIoPool ioPool;return ioPool;}

	char* AllocIo()
	{
		char *ret = NULL;

ALLOCIOBEGIN:
		l.Lock();
		if (ioPool.empty())
		{
			l.Unlock();
			e.Wait();
			goto ALLOCIOBEGIN;
		}

		ret = *(ioPool.end()-1);
		ioPool.erase((ioPool.end()-1));
		if (ioPool.empty()) 
		{
			e.Reset();
		}
		l.Unlock();

		return ret;
	}

	void ReleaseIo(char* objIo)
	{
		bool isSetEvt = false;
		l.Lock();
		if (ioPool.empty()) 
		{
			isSetEvt = true;
		}
		ioPool.push_back(objIo);
		l.Unlock();

		if (isSetEvt) 
		{
			e.Set();
		}
	}

private:
	ObjIoPool()
	{
		char *io = NULL;
		ioPool.reserve(OsdCfg::GetOsdCfg().GetObjStreamThreadNum()*2);
		for (u32 i = 0;i < OsdCfg::GetOsdCfg().GetObjStreamThreadNum()*2;++i) 
		{
			io = new char[sizeof(BaseObjIo)+sizeof(int)];
			RELEASE_ASSERT(io != NULL);
			ioPool.push_back(io);
			io = NULL;
		}
		e.SetMode(TRUE);
	}

	struct DelIo
	{
		inline void operator()(char *io){delete []io;}
	};

	~ObjIoPool()
	{
		for_each(ioPool.begin(),ioPool.end(),DelIo());
	}

	vector<char *> ioPool;
	OsdLock l;
	FiEvent e;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  JournalObjIo  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct JournalWriteObjIo:public BaseObjIo
{
	static BaseObjIo* CreateObjIo(){return new(ObjIoPool::GetObjIoPool().AllocIo())JournalWriteObjIo;}
	void DelObjIo(BaseObjIo* objIo){((JournalWriteObjIo *)objIo)->~JournalWriteObjIo();ObjIoPool::GetObjIoPool().ReleaseIo((char *)objIo);}	

	void InitObjIo(char *headBuf, char *bufPar = NULL, u64 lenPar = 0, u64 offsetPar = 0);
	void ObjStreamProcessObjIo();
	void ProcessNetReplyBuf(char* bufPar);
	s32 GetMainNode(){return nodeInfo[0];}
	s32 GetBakNode(){return nodeInfo[1];}

	void Change2NetObjIo(){ot = NetWrite;}
	s32 GetDstNode(){return dstNode;}
	bool IsHaveSendBuf(){return true;}

	bool IsSetDoRecover(){return true;}
	FiErr LogForRecover();
	string GetStorPath(u32 no);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  MetaObjIo  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct MetaReadObjIo:public BaseObjIo
{
	static BaseObjIo* CreateObjIo(){return new(ObjIoPool::GetObjIoPool().AllocIo())MetaReadObjIo;}
	void DelObjIo(BaseObjIo* objIo){((MetaReadObjIo *)objIo)->~MetaReadObjIo();ObjIoPool::GetObjIoPool().ReleaseIo((char *)objIo);}	

	void InitObjIo(char *headBuf, char *bufPar = NULL, u64 lenPar = 0, u64 offsetPar = 0);
	void ObjStreamProcessObjIo();
	void ProcessNetReplyBuf(char* bufPar);
	s32 GetMainNode(){return nodeInfo[0];}
	s32 GetBakNode(){return nodeInfo[1];}

	void Change2NetObjIo(){ot = NetRead;}
	s32 GetDstNode(){return dstNode;}
	bool IsHaveReplyBuf(){return true;}
	string GetStorPath(u32 no);
};

struct MetaWriteObjIo:public BaseObjIo
{
	static BaseObjIo* CreateObjIo(){return new(ObjIoPool::GetObjIoPool().AllocIo())MetaWriteObjIo;}
	void DelObjIo(BaseObjIo* objIo){((MetaWriteObjIo *)objIo)->~MetaWriteObjIo();ObjIoPool::GetObjIoPool().ReleaseIo((char *)objIo);}	

	void InitObjIo(char *headBuf, char *bufPar = NULL, u64 lenPar = 0, u64 offsetPar = 0);
	void ObjStreamProcessObjIo();
	void ProcessNetReplyBuf(char* bufPar);
	s32 GetMainNode(){return nodeInfo[0];}
	s32 GetBakNode(){return nodeInfo[1];}

	void Change2NetObjIo(){ot = NetWrite;}
	s32 GetDstNode(){return dstNode;}
	bool IsHaveSendBuf(){return true;}

	bool IsSetDoRecover(){return true;}
	FiErr LogForRecover();
	string GetStorPath(u32 no);
};

struct MetaTruncateObjIo:public BaseObjIo
{
	static BaseObjIo* CreateObjIo(){return new(ObjIoPool::GetObjIoPool().AllocIo())MetaTruncateObjIo;}
	void DelObjIo(BaseObjIo* objIo){((MetaTruncateObjIo *)objIo)->~MetaTruncateObjIo();ObjIoPool::GetObjIoPool().ReleaseIo((char *)objIo);}

	void InitObjIo(char *headBuf, char *bufPar = NULL, u64 lenPar = 0, u64 offsetPar = 0);
	void ObjStreamProcessObjIo();
	void ProcessNetReplyBuf(char* bufPar);

	void Change2NetObjIo(){ot = NetTruncate;}
	s32 GetDstNode(){return dstNode;}

	bool IsSetDoRecover(){return true;}
	FiErr LogForRecover();
};

struct MetaDelObjIo:public BaseObjIo
{
	static BaseObjIo* CreateObjIo(){return new(ObjIoPool::GetObjIoPool().AllocIo())MetaDelObjIo;}
	void DelObjIo(BaseObjIo* objIo){((MetaDelObjIo *)objIo)->~MetaDelObjIo();ObjIoPool::GetObjIoPool().ReleaseIo((char *)objIo);}

	void InitObjIo(char *headBuf, char *bufPar = NULL, u64 lenPar = 0, u64 offsetPar = 0);
	void ObjStreamProcessObjIo();
//	void ProcessNetReplyBuf(char* bufPar);
	s32 GetMainNode(){return nodeInfo[0];}
	s32 GetBakNode(){return nodeInfo[1];}

	void Change2NetObjIo(){ot = NetDel;}
	s32 GetDstNode(){return dstNode;}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  DataObjIo  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct DataReadObjIo:public BaseObjIo
{
	static BaseObjIo* CreateObjIo(){return new(ObjIoPool::GetObjIoPool().AllocIo())DataReadObjIo;}
	void DelObjIo(BaseObjIo* objIo){((DataReadObjIo *)objIo)->~DataReadObjIo();ObjIoPool::GetObjIoPool().ReleaseIo((char *)objIo);}	

	void InitObjIo(char *headBuf, char *bufPar = NULL, u64 lenPar = 0, u64 offsetPar = 0);
	void ObjStreamProcessObjIo();
	void ProcessNetReplyBuf(char* bufPar);
	s32 GetMainNode(){return nodeInfo[0];}
	s32 GetBakNode(){return nodeInfo[1];}

	void Change2NetObjIo(){ot = NetRead;}
	s32 GetDstNode(){return dstNode;}
	bool IsHaveReplyBuf(){return true;}
	string GetStorPath(u32 no);
};

struct DataWriteObjIo:public BaseObjIo
{
	static BaseObjIo* CreateObjIo(){return new(ObjIoPool::GetObjIoPool().AllocIo())DataWriteObjIo;}
	void DelObjIo(BaseObjIo* objIo){((DataWriteObjIo *)objIo)->~DataWriteObjIo();ObjIoPool::GetObjIoPool().ReleaseIo((char *)objIo);}

	void InitObjIo(char *headBuf, char *bufPar = NULL, u64 lenPar = 0, u64 offsetPar = 0);
	void ObjStreamProcessObjIo();
	void ProcessNetReplyBuf(char* bufPar);
	s32 GetMainNode(){return nodeInfo[0];}
	s32 GetBakNode(){return nodeInfo[1];}

	void Change2NetObjIo(){ot = NetWrite;}
	s32 GetDstNode(){return dstNode;}
	bool IsHaveSendBuf(){return true;}

	bool IsSetDoRecover(){return true;}
	FiErr LogForRecover();
	string GetStorPath(u32 no);
};

struct DataTruncateObjIo:public BaseObjIo
{
	static BaseObjIo* CreateObjIo(){return new(ObjIoPool::GetObjIoPool().AllocIo())DataTruncateObjIo;}
	void DelObjIo(BaseObjIo* objIo){((DataTruncateObjIo *)objIo)->~DataTruncateObjIo();ObjIoPool::GetObjIoPool().ReleaseIo((char *)objIo);}

	void InitObjIo(char *headBuf, char *bufPar = NULL, u64 lenPar = 0, u64 offsetPar = 0);
	void ObjStreamProcessObjIo();
	void ProcessNetReplyBuf(char* bufPar){};

	void Change2NetObjIo(){ot = DataDistributeTruncate;}
	s32 GetDstNode(){return dstNode;}

//	bool IsSetDoRecover(){return true;}
//	FiErr LogForRecover();
};

struct DataDistributeTruncateObjIo:public BaseObjIo
{
	static BaseObjIo* CreateObjIo(){return new(ObjIoPool::GetObjIoPool().AllocIo())DataDistributeTruncateObjIo;}
	void DelObjIo(BaseObjIo* objIo){((DataDistributeTruncateObjIo *)objIo)->~DataDistributeTruncateObjIo();ObjIoPool::GetObjIoPool().ReleaseIo((char *)objIo);}

	void InitObjIo(char *headBuf, char *bufPar = NULL, u64 lenPar = 0, u64 offsetPar = 0);
	void ObjStreamProcessObjIo();
	void ProcessNetReplyBuf(char* bufPar);
	s32 GetMainNode(){return nodeInfo[0];}
	s32 GetBakNode(){return nodeInfo[1];}
	
	void Change2NetObjIo(){ot = NetTruncate;}
	s32 GetDstNode(){return dstNode;}

	bool IsSetDoRecover(){return true;}
	FiErr LogForRecover();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  DataRaid5ObjIo  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  NetObjIo  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct NetReadObjIo:public BaseObjIo
{
	static BaseObjIo* CreateObjIo(){return new(ObjIoPool::GetObjIoPool().AllocIo())NetReadObjIo;}
	void DelObjIo(BaseObjIo* objIo){((NetReadObjIo *)objIo)->~NetReadObjIo();ObjIoPool::GetObjIoPool().ReleaseIo((char *)objIo);delete []buf;}

	void InitObjIo(char *headBuf, char *bufPar = NULL, u64 lenPar = 0, u64 offsetPar = 0);
	void ObjStreamProcessObjIo();
	s32 GetMainNode(){return nodeInfo[0];}
	s32 GetBakNode(){return nodeInfo[1];}

	s32 GetDstNode(){return -1;}
	bool IsHaveReplyBuf(){return true;}
	string GetStorPath(u32 no);
};

struct NetWriteObjIo:public BaseObjIo
{
	static BaseObjIo* CreateObjIo(){return new(ObjIoPool::GetObjIoPool().AllocIo())NetWriteObjIo;}
	void DelObjIo(BaseObjIo* objIo){((NetWriteObjIo *)objIo)->~NetWriteObjIo();ObjIoPool::GetObjIoPool().ReleaseIo((char *)objIo);}
	
	void InitObjIo(char *headBuf, char *bufPar = NULL, u64 lenPar = 0, u64 offsetPar = 0);
	void ObjStreamProcessObjIo();
	s32 GetMainNode(){return nodeInfo[0];}
	s32 GetBakNode(){return nodeInfo[1];}

	s32 GetDstNode(){return -1;}
	string GetStorPath(u32 no);
};

struct NetTruncateObjIo:public BaseObjIo
{
	static BaseObjIo* CreateObjIo(){return new(ObjIoPool::GetObjIoPool().AllocIo())NetTruncateObjIo;}
	void DelObjIo(BaseObjIo* objIo){((NetTruncateObjIo *)objIo)->~NetTruncateObjIo();ObjIoPool::GetObjIoPool().ReleaseIo((char *)objIo);}

	void InitObjIo(char *headBuf, char *bufPar = NULL, u64 lenPar = 0, u64 offsetPar = 0);
	void ObjStreamProcessObjIo();
	s32 GetMainNode(){return nodeInfo[0];}
	s32 GetBakNode(){return nodeInfo[1];}

	s32 GetDstNode(){return -1;}
};

struct NetDelObjIo:public BaseObjIo
{
	static BaseObjIo* CreateObjIo(){return new(ObjIoPool::GetObjIoPool().AllocIo())NetDelObjIo;}
	void DelObjIo(BaseObjIo* objIo){((NetDelObjIo *)objIo)->~NetDelObjIo();ObjIoPool::GetObjIoPool().ReleaseIo((char *)objIo);}
	
	void InitObjIo(char *headBuf, char *bufPar = NULL, u64 lenPar = 0, u64 offsetPar = 0);
	void ObjStreamProcessObjIo();

	s32 GetDstNode(){return -1;}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  RedirectObjIo  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct RedirectReadObjIo:public BaseObjIo
{
	static BaseObjIo* CreateObjIo(){return new(ObjIoPool::GetObjIoPool().AllocIo())RedirectReadObjIo;}
	void DelObjIo(BaseObjIo* objIo){((RedirectReadObjIo *)objIo)->~RedirectReadObjIo();ObjIoPool::GetObjIoPool().ReleaseIo((char *)objIo);}
	
	void InitObjIo(char *headBuf, char *bufPar = NULL, u64 lenPar = 0, u64 offsetPar = 0);
	void ProcessNetReplyBuf(char* bufPar);
	s32 GetMainNode(){return nodeInfo[0];}
	s32 GetBakNode(){return nodeInfo[1];}

//	void Change2NetObjIo(){ot = NetRead;nodeInfo[0] = -1;}
	void Change2NetObjIo(){ot = NetRead;}
	s32 GetDstNode(){return dstNode;}
};

struct RedirectWriteObjIo:public BaseObjIo
{
	static BaseObjIo* CreateObjIo(){return new(ObjIoPool::GetObjIoPool().AllocIo())RedirectWriteObjIo;}
	void DelObjIo(BaseObjIo* objIo){((RedirectWriteObjIo *)objIo)->~RedirectWriteObjIo();ObjIoPool::GetObjIoPool().ReleaseIo((char *)objIo);}
	
	void InitObjIo(char *headBuf, char *bufPar = NULL, u64 lenPar = 0, u64 offsetPar = 0);
	void ProcessNetReplyBuf(char* bufPar);
	s32 GetMainNode(){return nodeInfo[0];}
	s32 GetBakNode(){return nodeInfo[1];}

//	void Change2NetObjIo(){ot = NetWrite;nodeInfo[0] = -1;}
	void Change2NetObjIo(){ot = NetWrite;}
	s32 GetDstNode(){return dstNode;}
	bool IsHaveSendBuf(){return true;}
};

struct RedirectTruncateObjIo:public BaseObjIo
{
	static BaseObjIo* CreateObjIo(){return new(ObjIoPool::GetObjIoPool().AllocIo())RedirectTruncateObjIo;}
	void DelObjIo(BaseObjIo* objIo){((RedirectTruncateObjIo *)objIo)->~RedirectTruncateObjIo();ObjIoPool::GetObjIoPool().ReleaseIo((char *)objIo);}

	void InitObjIo(char *headBuf, char *bufPar = NULL, u64 lenPar = 0, u64 offsetPar = 0);
	void ProcessNetReplyBuf(char* bufPar);
	s32 GetMainNode(){return nodeInfo[0];}
	s32 GetBakNode(){return nodeInfo[1];}

//	void Change2NetObjIo(){ot = NetTruncate;nodeInfo[0] = -1;}
	void Change2NetObjIo(){ot = NetTruncate;}
	s32 GetDstNode(){return dstNode;}

	bool IsSetDoRecover(){return true;}
	FiErr LogForRecover();
};

struct RedirectDelObjIo:public BaseObjIo
{
	static BaseObjIo* CreateObjIo(){return new(ObjIoPool::GetObjIoPool().AllocIo())RedirectDelObjIo;}
	void DelObjIo(BaseObjIo* objIo){((RedirectDelObjIo *)objIo)->~RedirectDelObjIo();ObjIoPool::GetObjIoPool().ReleaseIo((char *)objIo);}
	
	void InitObjIo(char *headBuf, char *bufPar = NULL, u64 lenPar = 0, u64 offsetPar = 0);
	void ProcessNetReplyBuf(char* bufPar);

//	void Change2NetObjIo(){ot = NetDel;nodeInfo[0] = -1;}
	void Change2NetObjIo(){ot = NetDel;}
	s32 GetDstNode(){return dstNode;}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  RecoverObjIo  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct RecoverWriteObjIo:public BaseObjIo
{
	static BaseObjIo* CreateObjIo(){return new(ObjIoPool::GetObjIoPool().AllocIo())RecoverWriteObjIo;}
	void DelObjIo(BaseObjIo* objIo){((RecoverWriteObjIo *)objIo)->~RecoverWriteObjIo();ObjIoPool::GetObjIoPool().ReleaseIo((char *)objIo);}
	
	void InitObjIo(char *headBuf, char *bufPar = NULL, u64 lenPar = 0, u64 offsetPar = 0);
	void ProcessNetReplyBuf(char* bufPar);
	s32 GetMainNode(){return nodeInfo[0];}
	s32 GetBakNode(){return nodeInfo[1];}

	void Change2NetObjIo(){ot = NetWrite;}
	s32 GetDstNode(){return dstNode;}
	bool IsHaveSendBuf(){return true;}

	bool IsSetDoRecover(){return true;}
	FiErr LogForRecover(){return FErr(MNet_SendFailed);}
};

struct RecoverTruncateObjIo:public BaseObjIo
{
	static BaseObjIo* CreateObjIo(){return new(ObjIoPool::GetObjIoPool().AllocIo())RecoverTruncateObjIo;}
	void DelObjIo(BaseObjIo* objIo){((RecoverTruncateObjIo *)objIo)->~RecoverTruncateObjIo();ObjIoPool::GetObjIoPool().ReleaseIo((char *)objIo);}

	void InitObjIo(char *headBuf, char *bufPar = NULL, u64 lenPar = 0, u64 offsetPar = 0);
	void ProcessNetReplyBuf(char* bufPar);
	s32 GetMainNode(){return nodeInfo[0];}
	s32 GetBakNode(){return nodeInfo[1];}

//	void Change2NetObjIo(){ot = NetTruncate;nodeInfo[0] = -1;}
	void Change2NetObjIo(){ot = NetTruncate;}
	s32 GetDstNode(){return dstNode;}

	bool IsSetDoRecover(){return true;}
	FiErr LogForRecover(){return FErr(MNet_SendFailed);}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  ConsistencyObjIo  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ConsistencyReadObjIo:public BaseObjIo
{
	static BaseObjIo* CreateObjIo(){return new(ObjIoPool::GetObjIoPool().AllocIo())ConsistencyReadObjIo;}
	void DelObjIo(BaseObjIo* objIo){((ConsistencyReadObjIo *)objIo)->~ConsistencyReadObjIo();ObjIoPool::GetObjIoPool().ReleaseIo((char *)objIo);}
	
	void InitObjIo(char *headBuf, char *bufPar = NULL, u64 lenPar = 0, u64 offsetPar = 0);
//	void ObjStreamProcessObjIo();
	void ProcessNetReplyBuf(char* bufPar);
	s32 GetMainNode(){return nodeInfo[0];}
	s32 GetBakNode(){return nodeInfo[1];}

	void Change2NetObjIo(){ot = NetConsistencyRead;}
	s32 GetDstNode(){return dstNode;}
	bool IsHaveReplyBuf(){return true;}

	bool IsSetDoRecover();
	FiErr LogForRecover();

	bool IsCheckNodeStatus(){return false;}
	string GetStorPath(u32 no);
};

struct NetConsistencyReadObjIo:public BaseObjIo
{
	static BaseObjIo* CreateObjIo(){return new(ObjIoPool::GetObjIoPool().AllocIo())NetConsistencyReadObjIo;}
	void DelObjIo(BaseObjIo* objIo){((NetConsistencyReadObjIo *)objIo)->~NetConsistencyReadObjIo();ObjIoPool::GetObjIoPool().ReleaseIo((char *)objIo);delete []buf;}

	void InitObjIo(char *headBuf, char *bufPar = NULL, u64 lenPar = 0, u64 offsetPar = 0);
	void ObjStreamProcessObjIo();
	s32 GetMainNode(){return nodeInfo[0];}
	s32 GetBakNode(){return nodeInfo[1];}

	s32 GetDstNode(){return -1;}
	bool IsHaveReplyBuf(){return true;}
	string GetStorPath(u32 no);
};

struct ConsistencyTruncateObjIo:public BaseObjIo
{
	static BaseObjIo* CreateObjIo(){return new(ObjIoPool::GetObjIoPool().AllocIo())ConsistencyTruncateObjIo;}
	void DelObjIo(BaseObjIo* objIo){((ConsistencyTruncateObjIo *)objIo)->~ConsistencyTruncateObjIo();ObjIoPool::GetObjIoPool().ReleaseIo((char *)objIo);}

	void InitObjIo(char *headBuf, char *bufPar = NULL, u64 lenPar = 0, u64 offsetPar = 0);
	void ObjStreamProcessObjIo();
	void ProcessNetReplyBuf(char* bufPar);

	void Change2NetObjIo(){}
	s32 GetDstNode(){return dstNode;}

	bool IsSetDoRecover();
	FiErr LogForRecover();

	bool IsCheckNodeStatus(){return false;}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  ObjIoFactory  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef BaseObjIo* (*ObjIoCreateFuc)();
typedef map<ObjIoType,ObjIoCreateFuc> CreateMap;

struct ObjIoFactory
{
	static ObjIoFactory& GetObjIoFactory(){static ObjIoFactory oif;return oif;}

	bool Register(const ObjIoType &type,ObjIoCreateFuc creator)
	{
		return cm.insert(CreateMap::value_type(type,creator)).second;
	}

//  bool Unregister(const ObjIoType &type)
//  {
//  	return (cm.earse(type) == 1);
//  }

	BaseObjIo* CreateObj(const ObjIoType &type)
	{
		CreateMap::iterator i = cm.find(type);
		return (*(i->second))();
	}

private:
	ObjIoFactory(){}
	~ObjIoFactory(){}
	CreateMap cm;
};

void InitObjIoFactory();
BaseObjIo* GetObjIoFromBuf(u8 *buf);
void objmemcpy(void *decPar,void *srcPar,s32 bufLen);

#endif
