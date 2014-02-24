///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  hunter code ([hunter.f0x] remeber my brothers) email:hunterlhy@163.com /////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __FIOSD_H__
#define __FIOSD_H__
#include <vector>
#include <sys/statvfs.h>
#include "objIo.h"
#include "fiOsdIF.h"
#include "ficsLocIF.h"
#include "../network/tcpio.h"
#include "../ficsNetSyn/ficsNetSyn.h"
#include "../fistate/allsubstate.h"
#include "../public/netIpBook.h"
using namespace std;

#define HashPathNum (1000)
string GetStorDirPath(bool isMeta,ObjID &fileID);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define NET_MOD_ID 1118
#define OSD_PORT 31118

#define DELTYPE (0xffffffff)
#define WRITETYPE (0xffffffff)

extern bool gIsUseFiState;

#pragma pack(push,1)
struct IoFileLogElement
{
	ObjID fileID;
	u32 order;
	u32 offset;		//truncateIo(data slice offset) writeIo(always WRITETYPE)
	s32 nodeID;
	s32 mainNode;
	s32 bakNode;
	u32 markNum;
};
#pragma pack(pop)

struct IoFileLog
{
	#define READ_IOFILELOGELEMENT_BUFSIZE (sizeof(IoFileLogElement)*80*1024)

	struct iflecKey
	{
		iflecKey():order(0),nodeID(0),offset(WRITETYPE),mainNode(-1),bakNode(-1) { /*fileID.ClearID();*/ }
		iflecKey(IoFileLogElement *e) : fileID(e->fileID), order(e->order), offset(e->offset), nodeID(e->nodeID), mainNode(e->mainNode), bakNode(e->bakNode) { }
		inline bool operator<(const iflecKey& k) const
		{
			if (fileID == k.fileID && order == k.order)
			{
				return offset < k.offset;		//truncateIo first get
			} 
			else if (fileID == k.fileID)
			{
				return order < k.order;
			}
			else
			{
				return fileID < k.fileID;
			}
		}
		ObjID fileID;
		u8 type;
		u32 order;
		u32 offset;
		s32 nodeID;
		s32 mainNode;
		s32 bakNode;
	};

	struct iflecData
	{
		iflecData() { }
		iflecData(IoFileLogElement *e) : markNum(e->markNum) { }
		u32 markNum;
	};

	typedef multimap<iflecKey, iflecData> ifleMMap;
	typedef pair<iflecKey, iflecData> ifleMMapValue;

	struct FindFileLogElementWithFileIDAndOrder
	{
		inline bool operator()(const ifleMMap::value_type &l,const ifleMMap::value_type &r)
		{
			if (l.first.fileID == r.first.fileID)
			{
				return l.first.order < r.first.order;
			}
			else
			{
				return l.first.fileID < r.first.fileID;
			}
		}
	};

	struct FindFileLogElementWithFileID
	{
		inline bool operator()(const ifleMMap::value_type &l,const ifleMMap::value_type &r)
		{
			return (l.first.fileID < r.first.fileID);
		}
	};

	void PrintElement()
	{
//		lock.Lock();
//		for (it = logMMap.begin(); it != logMMap.end(); ++it)
//		{
//			OSD_LOG_ERR("===IoFileLogPrintElement=== this[%p],fileID[%s],order[%d],offset[%d],nodeID[%d],mainNode[%d],bakNode[%d],markNum[%d]",	\
//						this,it->first.fileID.GetIDStr().c_str(),it->first.order,it->first.offset,it->first.nodeID,it->first.mainNode,it->first.bakNode,it->second.markNum);
//		}
//		lock.Unlock();
	}

	bool IsFileLogNull()
	{
		bool ret = true;
		lock.Lock();
		if (logMMap.empty())
		{
			lock.Unlock();
			return ret;
		}

		it = logMMap.upper_bound(iflecKey());
		ret = (logMMap.end() == it);
		lock.Unlock();
		return ret;
	}

	bool IsFileInIoFileLog(ObjID &fid, u32 order, s32& nodeID)
	{
		bool ret = false;

		lock.Lock();
		if (logMMap.empty())
		{
			lock.Unlock();
			return ret;
		}

		ifleMMapValue e;
		e.first.fileID = fid;
		e.first.order = order;
		rst = equal_range(logMMap.begin(), logMMap.end(), e, FindFileLogElementWithFileIDAndOrder());
		if (rst.first != rst.second)
		{
			nodeID = rst.first->first.nodeID;
			ret = true;
		}
		lock.Unlock();

		return ret;
	}

	void GetFileLogAryWithFileID(vector<IoFileLogElement> &ifleAry, ObjID& fileID, bool isOnlyGetTruncateType = false)
	{
		IoFileLogElement g;

		lock.Lock();
		if (logMMap.empty())
		{
			lock.Unlock();
			return;
		}

		ifleMMapValue e;
		e.first.fileID = fileID;
		rst = equal_range(logMMap.begin(), logMMap.end(), e, FindFileLogElementWithFileID());
		while(rst.first != rst.second)
		{
			if (isOnlyGetTruncateType)
			{
				if (rst.first->first.offset == WRITETYPE || rst.first->first.offset == DELTYPE)
				{
					++rst.first;
					continue;
				}
			}
			g.fileID = rst.first->first.fileID;
			g.order = rst.first->first.order;
			g.offset = rst.first->first.offset;
			g.nodeID = rst.first->first.nodeID;
			g.mainNode = rst.first->first.mainNode;
			g.bakNode = rst.first->first.bakNode;
			g.markNum = rst.first->second.markNum;
			ifleAry.push_back(g);
			OSD_LOG_ERR("===GetFileLogAryWithFileID=== fileID[%s],order[%d],offset[%d],nodeID[%d],markNum[%d]",g.fileID.GetIDStr().c_str() , g.order, g.offset, g.nodeID, g.markNum);
			++rst.first;
		}
		lock.Unlock();
	}

	bool GetFileLogElementWithFileID(IoFileLogElement& g, ObjID& fileID)
	{
		bool ret = false;

		lock.Lock();
		if (logMMap.empty())
		{
			lock.Unlock();
			return ret;
		}

		ifleMMapValue e;
		e.first.fileID = fileID;
		rst = equal_range(logMMap.begin(), logMMap.end(), e, FindFileLogElementWithFileID());
		if (rst.first != rst.second)
		{
			g.fileID = rst.first->first.fileID;
			g.order = rst.first->first.order;
			g.offset = rst.first->first.offset;
			g.nodeID = rst.first->first.nodeID;
			g.mainNode = rst.first->first.mainNode;
			g.bakNode = rst.first->first.bakNode;
			g.markNum = rst.first->second.markNum;
			OSD_LOG_ERR("===GetFileLogElementWithFileID=== fileID[%s],order[%d],offset[%d],nodeID[%d],markNum[%d]",g.fileID.GetIDStr().c_str(), g.order, g.offset, g.nodeID, g.markNum);
			ret = true;
		}
		lock.Unlock();

		return ret;
	}

	FiErr AddFileLogElement(ObjID &fid, u32 order, s32 nodeID, u32 offset, s32 mainNode = -1, s32 bakNode = -1);
	FiErr DelFileLogElement(IoFileLogElement& d);

	bool GetFileLogElement(IoFileLogElement& g)
	{
		bool ret = false;

		lock.Lock();
		if (logMMap.empty())
		{
			lock.Unlock();
			return ret;
		}

		it = logMMap.upper_bound(iflecKey());
		if (logMMap.end() != it)
		{
			g.fileID = it->first.fileID;
			g.order = it->first.order;
			g.offset = it->first.offset;
			g.nodeID = it->first.nodeID;
			g.mainNode = it->first.mainNode;
			g.bakNode = it->first.bakNode;
			g.markNum = it->second.markNum;
			OSD_LOG_ERR("===GetFileLogElement=== fileID[%s],order[%d],offset[%d],nodeID[%d],markNum[%d]",g.fileID.GetIDStr().c_str() , g.order, g.offset, g.nodeID, g.markNum);
			ret = true;
		}
		lock.Unlock();

		return ret;
	}

	inline const string& GetFileLogPath(){return fileLogPath;}
	inline const void ClearFileLogMMap(){logMMap.clear();}
	IoFileLog(string flp) : fileLogPath(flp)
	{
		u32 fileSize = ficsLocIF::GetFicsLocIF().FileSize(fileLogPath);
		if (fileSize > 0)
		{
			IoFileLogElement *e = NULL;
			char *bufTp = new char[READ_IOFILELOGELEMENT_BUFSIZE];
			RELEASE_ASSERT(bufTp != NULL);
			memset(bufTp, 0, READ_IOFILELOGELEMENT_BUFSIZE);
			u32 offset = 0;
			u32 length = (fileSize > READ_IOFILELOGELEMENT_BUFSIZE) ? READ_IOFILELOGELEMENT_BUFSIZE : fileSize;
			u32 lenBak = 0;
			FiErr ret;
			u32 copyNum = length / sizeof(IoFileLogElement);

			do
			{
				lenBak = length;
			IoFileLogRedoReadData:
				ret = ficsLocIF::GetFicsLocIF().GetFileIF(fileLogPath)->ReadData(bufTp, length, offset);
				if (ret.IsErr())
				{
					FiEvent evt;
					OSD_LOG_ERR("===IoFileLog=== ReadData <failed> path[%s],errInfo[%s],2s later redo",fileLogPath.c_str(),ret.StrFiText().c_str());
					evt.Wait(2000);
					goto IoFileLogRedoReadData;
				}
				RELEASE_ASSERT(length == lenBak && length % sizeof(IoFileLogElement) == 0);
				for (u32 i = 0; i < copyNum; ++i)
				{
					e = (IoFileLogElement *)bufTp + i;
					logMMap.insert(ifleMMap::value_type(iflecKey(e),iflecData(e)));
				}
				offset += length;
				fileSize -= length;
				length = (fileSize > READ_IOFILELOGELEMENT_BUFSIZE) ? READ_IOFILELOGELEMENT_BUFSIZE : fileSize;
				copyNum = length / sizeof(IoFileLogElement);
			}
			while (fileSize);
			delete[] bufTp;
			PrintElement();
		}
	}

private:
	ifleMMap logMMap;
	OsdLock lock;

	string fileLogPath;

	ifleMMap::iterator it;
	pair <ifleMMap::iterator,ifleMMap::iterator> rst;
};

struct NodeSpaceCollecter 
{
	static NodeSpaceCollecter& GetNodeSpaceCollecter(){static NodeSpaceCollecter nsc;return nsc;}

	static void InitAllNodeSpace()		//格式化的时候调用
	{
		FiErr ret;
		s32 nodesNumTp = OsdCfg::GetOsdCfg().GetNodeTotal();
		s64 *spaceTp = new s64[nodesNumTp];
		RELEASE_ASSERT(spaceTp != NULL);
		string pathTp;

	InitAllNodeSpaceRedo:
		do
		{
			pathTp = string("/meta/SpaceCollecter");
			memset(spaceTp,0,sizeof(s64)*nodesNumTp);
			if (zkDataBase::GetZkDataBase().NodeExists(pathTp).IsSuccess())
			{
				ret = zkDataBase::GetZkDataBase().DeleteNode(pathTp);
				if(ret.IsErr())
				{
					break;
				}
			}
			ret = zkDataBase::GetZkDataBase().CreateNode(pathTp,(char *)spaceTp,sizeof(s64)*nodesNumTp);
			if(ret.IsErr())
			{
				break;
			}

			pathTp = string("/data/SpaceCollecter");
			if (zkDataBase::GetZkDataBase().NodeExists(pathTp).IsSuccess())
			{
				ret = zkDataBase::GetZkDataBase().DeleteNode(pathTp);
				if(ret.IsErr())
				{
					break;
				}
			}
			ret = zkDataBase::GetZkDataBase().CreateNode(pathTp,(char *)spaceTp,sizeof(s64)*nodesNumTp);
		}
		while (0); 

		if (ret.IsErr())
		{
			FiEvent evt;
			evt.Wait(2000);
			goto InitAllNodeSpaceRedo;
		}
	}

	struct NodeSpaceMrg
	{
		static void SpaceWatcher(void *par)
		{
			NodeSpaceMrg *nsm = (NodeSpaceMrg *)par;
			nsm->RefreshAllNodeSpace();
		}

		static void LocNodeSpaceLeftTimer(int nTimerId, void* pPara)
		{
			NodeSpaceMrg *nsm = (NodeSpaceMrg *)pPara;
			nsm->RefreshLocNodeSpace();
		}

		inline s64 LocNodeSpaceLeft(){return spaceLeft;}

		inline FiErr RefreshAllNodeSpace()
		{
			FiErr ret;
			if (space == NULL) 
			{
				space = new s64[nodeTotal];
				RELEASE_ASSERT(space != NULL);
				memset(space,0,sizeof(s64)*nodeTotal);
			}

			do
			{
				ret = zkDataBase::GetZkDataBase().GetNodeData((char *)space,sizeof(s64)*nodeTotal,zkDataPath,SpaceWatcher,this);
				if (ret.IsErr())
				{
					FiEvent evt;
					evt.Wait(2000);
					OSD_LOG_ERR("===RefreshAllNodeSpace=== GetNodeData <failed> path[%s],errInfo[%s]",zkDataPath.c_str(),ret.StrFiText().c_str());
				}
				else
				{
					OSD_LOG_ERR("===RefreshAllNodeSpace=== GetNodeData suc path[%s]",zkDataPath.c_str());
				}
			}
			while (ret.IsErr()); 

			return ret;
		}

		inline FiErr RefreshLocNodeSpace()
		{
			FiErr ret;
			struct statvfs info;
			RELEASE_ASSERT(-1 != statvfs(fsMountPath.c_str(),&info));
			spaceLeft = (s64)info.f_bsize*(s64)info.f_bfree;
			if (space[nodeID] - spaceLeft > refreshGrainSize || spaceLeft - space[nodeID] > refreshGrainSize)
			{
				s64 oldValue = space[nodeID];
				space[nodeID] = spaceLeft;
				ret = zkDataBase::GetZkDataBase().SetNodeData(zkDataPath,(char *)space,sizeof(s64)*nodeTotal);
				if (ret.IsErr())
				{
					space[nodeID] = oldValue;
					OSD_LOG_ERR("===RefreshLocNodeSpace=== SetNodeData <failed> path[%s],errInfo[%s]",zkDataPath.c_str(),ret.StrFiText().c_str());
				}
			}
			return ret;
		}

		s32 AllocOneNode(s32 *exceptNodeAry = NULL,u32 nodeNum = 0,vector<s32> *exceptNodes = NULL)
		{
			s32 retNodeID = -1;
			vector<s64> spaceVec(space,space+nodeTotal);
			vector<s64>::iterator it;
			if (exceptNodeAry != NULL && nodeNum != 0) 
			{
				it = spaceVec.begin();
				for (s32 i = 0;i < nodeNum;++i) 
				{
					*(it+exceptNodeAry[i]) = 0;
				}
			}
			if (exceptNodes != NULL)
			{
				it = spaceVec.begin();
				for (vector<s32>::iterator iit = exceptNodes->begin(); iit != exceptNodes->end(); ++iit)
				{
					*(it + *iit) = 0;
				}
			}
			
			it = max_element(spaceVec.begin(),spaceVec.end());
			retNodeID = (it - spaceVec.begin());
			RELEASE_ASSERT(retNodeID >= 0);
			return retNodeID;
		}

		void ExpandUpdate(s32 newNodeTotal)		//???
		{
		}

		NodeSpaceMrg(string &dataPath,string &mountPath,u32 refreshTimePar,u32 refreshGrainSizePar):	\
			zkDataPath(dataPath),	\
			fsMountPath(mountPath),	\
			space(NULL),	\
			nodeTotal(OsdCfg::GetOsdCfg().GetNodeTotal()),	\
			nodeID(OsdCfg::GetOsdCfg().GetLocNodeID()),	\
			refreshTime(refreshTimePar),	\
			refreshGrainSize(refreshGrainSizePar)
		{
			RefreshAllNodeSpace();
			RefreshLocNodeSpace();
			timer.SetTimer(refreshTime,LocNodeSpaceLeftTimer,this);
		}
		~NodeSpaceMrg(){if(space != NULL) delete []space;}

	private:
		s32 nodeID;
		s64 spaceLeft;
		s32 nodeTotal;
		u32 refreshGrainSize;
		u32 refreshTime;
		s64 *space;
		string fsMountPath;
		string zkDataPath;
		FiTimer timer;
	};

	inline s64 RefreshSpace()
	{
		if (metaSpace)
		{
			metaSpace->RefreshAllNodeSpace();
			metaSpace->RefreshLocNodeSpace();
		}
		dataSpace->RefreshAllNodeSpace();
		dataSpace->RefreshLocNodeSpace();
	}

	inline s64 LocNodeSpaceLeft(bool isMeta)
	{
		if (isMeta) 
		{
			return metaSpace->LocNodeSpaceLeft();
		}
		else
		{
			return dataSpace->LocNodeSpaceLeft();
		}
	}

	inline s32 AllocOneNode(bool isMeta,s32 *exceptNodeAry = NULL,u32 nodeNum = 0,vector<s32> *exceptNodes = NULL)
	{
		if (isMeta) 
		{
			return metaSpace->AllocOneNode(exceptNodeAry,nodeNum,exceptNodes);
		}
		else
		{
			return dataSpace->AllocOneNode(exceptNodeAry,nodeNum,exceptNodes);
		}
	}

	void ExpandUpdate(s32 newNodeTotal)
	{
		if (metaSpace)
		{
			metaSpace->ExpandUpdate(newNodeTotal);
		}
		dataSpace->ExpandUpdate(newNodeTotal);
	}

private:
	NodeSpaceCollecter():metaSpace(NULL),dataSpace(NULL)
	{
		string zkPathStr;
		if(OsdCfg::GetOsdCfg().IsInstallMetaSvr())
		{
			zkPathStr = "/meta/SpaceCollecter";
			metaSpace = new NodeSpaceMrg(zkPathStr,OsdCfg::GetOsdCfg().GetMetaRootPath(),OsdCfg::GetOsdCfg().GetSpaceRefreshTime(),OsdCfg::GetOsdCfg().GetSpaceRefreshGrainSize());
		}

		zkPathStr = "/data/SpaceCollecter";
		dataSpace = new NodeSpaceMrg(zkPathStr,OsdCfg::GetOsdCfg().GetDataRootPath(),OsdCfg::GetOsdCfg().GetSpaceRefreshTime(),OsdCfg::GetOsdCfg().GetSpaceRefreshGrainSize());
	}

	NodeSpaceMrg *metaSpace;
	NodeSpaceMrg *dataSpace;
};

struct ConsistencyHandler
{
	static ConsistencyHandler& GetConsistencyHandler(){static ConsistencyHandler ch;return ch;}
	void DoConsistency();
	void DoConsistencySub(ObjDataType dataType,IoFileLog *ioConsistencyLog,char *buf,u32 bufLen);

private:
	ConsistencyHandler():chBuf(NULL),chBufLen(1*1024*1024)
	{
		chBuf = new char[chBufLen];
		RELEASE_ASSERT(chBuf != NULL);
	}

	~ConsistencyHandler()
	{
		if (NULL != chBuf)
		{
			delete []chBuf;
		}
	}

	char *chBuf;
	s32 chBufLen;
};

struct FullRecoverHandler
{

private:
	IoFileLog *ifl;
};

enum OsdInternalStatus
{
	StopDoRecoverType = 0,
	StartDoRecoverType,
	DoFullRecoverType,
	LocNodeOK,
	LocNodeRecover,
	LocNodeErr,
	OsdOK,
	OsdErr
};

struct FaultHandler
{
	struct NotifyOthers 
	{
		NotifyOthers(OsdLayerStatus ols):s(ols){}
		inline void operator()(NotifyStatusCallBack f)
		{
			(*f)(s);
		}
	private:
		OsdLayerStatus s;
	};

	static FaultHandler& GetFaultHandler(){static FaultHandler fh;return fh;}

	static void FaultHandlerFunc(u64 taskId, void* par);
	static void* DoRecoverThread(void* par);
	void DoRecover(ObjDataType dataType,FiEvent *evt,char *buf,u32 bufLen);
	
	inline void PushFaultHandlerTask(OsdInternalStatus ois)
	{
		RELEASE_ASSERT(faultHandlerThread.PushTask(0,(void *)ois,NULL,NULL).IsSuccess());
	}

	inline void StartDoRecover(){evt.Set();}
	inline void StopDoRecover(){evt.Reset();}
	void DoFullRecover();

	inline void SetOsdLayerStatus(OsdLayerStatus ols){for_each(notifyAry.begin(),notifyAry.end(),NotifyOthers(ols));}

	void RegisterFuc(NotifyStatusCallBack f){notifyAry.push_back(f);}

	bool DoRecoverDone();
	void ClearRecoverLog();

	bool RecoverIoFind(ObjID &fileID,u32 length,u64 offset,ObjDataType type,FiEvent *pEvt)
	{
		BaseObjIo ioFake;	//fake io,just for search
		ioFake.fileID = fileID;
		ioFake.length = length;
		ioFake.offset = offset;
		ioFake.dataType = type;

		return oim.IoFind(&ioFake,pEvt);
	}
	OsdLock& GetOimLock(){return oimLock;}
private:
	FaultHandler();
	~FaultHandler(){}
	
	FiThreads faultHandlerThread;
	pthread_t doRecoverThreadId;

	vector<NotifyStatusCallBack> notifyAry;

	FiEvent evt;

	ObjIoMutex oim;
	OsdLock oimLock;
};

enum NodeStatusE
{
	OK = '0',
	ERR,
	RECOVER,
	DORECOVER,
	DOFULLRECOVER
};

struct NodeStatusObserver
{
	#define INIT_WAITTIME (125)
	#define MAX_WAITTIME (10000)	//default:(180000)

	static void StatusWatcher(void *par)
	{
		NodeStatusObserver *nso = (NodeStatusObserver *)par;
		if(nso->RefreshNodeStatus().IsSuccess())
		{
			nso->StatusAnalyze();
		}
	}

	static void InitNodesStatus()		//格式化的时候调用
	{
		FiErr ret;
		u32 statusLenTp = OsdCfg::GetOsdCfg().GetNodeTotal()*OsdCfg::GetOsdCfg().GetNodeTotal();
		char *nodesStatusTp = new char[statusLenTp];
		RELEASE_ASSERT(NULL != nodesStatusTp);
		string pathTp;

	InitNodesStatusRedo:
		do
		{
			memset(nodesStatusTp,'0',statusLenTp);
			pathTp = string("/nodesState");
			if (zkDataBase::GetZkDataBase().NodeExists(pathTp).IsSuccess())
			{
				ret = zkDataBase::GetZkDataBase().DeleteNode(pathTp);
				if(ret.IsErr())
				{
					break;
				}
			}
			ret = zkDataBase::GetZkDataBase().CreateNode(pathTp,nodesStatusTp,statusLenTp);
			if(ret.IsErr())
			{
				break;
			}

			pathTp = string("/osdLock");
			if (zkDataBase::GetZkDataBase().NodeExists(pathTp).IsSuccess())
			{
				ret = zkDataBase::GetZkDataBase().DeleteNode(pathTp);
				if(ret.IsErr())
				{
					break;
				}
			}
			ret = zkDataBase::GetZkDataBase().CreateNode(pathTp);
		}
		while (0); 

		if (ret.IsErr())
		{
			FiEvent evt;
			evt.Wait(2000);
			goto InitNodesStatusRedo;
		}
	}

	static NodeStatusObserver& GetNodeStatusObserver(){static NodeStatusObserver nso;return nso;}

	const char* GetNodesStatusFMTStr()
	{
		static string fmtStr;
		char a;
		fmtStr.clear();
		fmtStr.push_back('\n');
		for (u32 i=0;i<nodeTotal;++i)
		{
			fmtStr.append("\t\t| ");
			for (u32 j=0;j<nodeTotal;++j)
			{
				a = nodesStatus[i*nodeTotal+j];
				fmtStr.push_back(a);
				fmtStr.push_back(' ');
			}
			fmtStr.append("|\n");
		}
		return fmtStr.c_str();
	}

	FiErr LocStatusHandle(NodeStatusE status);

	bool RemoteStatusHandle(s32 remoteNodeID,NodeStatusE remoteNodeStatus)
	{
		bool ret = true;
		if (remoteNodeStatus == ERR) 
		{
			u32 offset = remoteNodeID*nodeTotal+nodeID;
			ois = StopDoRecoverType;
			RELEASE_ASSERT(ERR != nodesStatus[offset] || RECOVER != nodesStatus[offset]);
		   if (DORECOVER == nodesStatus[offset]) 
		   {
				OSD_LOG_ERR("<<<<< RemoteNodeErr and StopDoRecover,remoteNodeID[%d] >>>>>",remoteNodeID);
				FaultHandler::GetFaultHandler().PushFaultHandlerTask(StopDoRecoverType);
		   }
		   else if (DOFULLRECOVER == nodesStatus[offset])
		   {
			   OSD_LOG_ERR("<<<<< DoFullRecover,remoteNodeID[%d] >>>>>",remoteNodeID);
			   FaultHandler::GetFaultHandler().PushFaultHandlerTask(DoFullRecoverType);
		   }
		   else if (OK == nodesStatus[offset])
		   {
			   OSD_LOG_ERR("<<<<< OK == nodesStatus[offset],remoteNodeID[%d] >>>>>",remoteNodeID);
			   ret = false;
		   }
		   else
		   {
			   RELEASE_ASSERT_FASLE;
		   }
		}
		else if(remoteNodeStatus == RECOVER)
		{
			u32 offset = remoteNodeID*nodeTotal+nodeID;
			RELEASE_ASSERT(ERR != nodesStatus[offset] || RECOVER != nodesStatus[offset]);
			if (DORECOVER == nodesStatus[offset]) 
			{
				ois = StartDoRecoverType;
				OSD_LOG_ERR("<<<<<  RemoteNodeRecover and StartDoRecover,remoteNodeID[%d] >>>>>",remoteNodeID);
				FaultHandler::GetFaultHandler().PushFaultHandlerTask(StartDoRecoverType);
			}
			else if (OK == nodesStatus[offset])
			{
				OSD_LOG_ERR("<<<<< DoRecoverDone,remoteNodeID[%d] >>>>>",remoteNodeID);
				ret = false;
			}
			else
			{
				RELEASE_ASSERT_FASLE;
			}
		}
		else
		{
			RELEASE_ASSERT_FASLE;
		}
		return ret;
	}

	FiErr StatusHandleWrap(vector<char>& statusVec)
	{
		FiErr ret;
		if (statusVec[nodeID] != OK) 
		{
			return LocStatusHandle((NodeStatusE)statusVec[nodeID]);
		}

		for(u32 i = 0;i<nodeTotal;++i)
		{
			if (statusVec[i] != OK) 
			{
				RELEASE_ASSERT(i != nodeID);
				notOkNodeIDs.push_back(i);
				if(RemoteStatusHandle(i,(NodeStatusE)statusVec[i]))
				{
					recoverRemoteNodeID = i;
				}
			}
		}
		return ret;
	}

	void StatusAnalyze()
	{
		FiErr ret;
		vector<char> statusVec;
		statusVec.reserve(nodeTotal);

		do
		{
			statusVec.clear();
			notOkNodeIDs.clear();
			nodesStatusLock.Lock();
			for (u32 i = 0;i<nodeTotal;++i) 
			{
				statusVec.push_back(nodesStatus[i*nodeTotal+i]);
			}

			u32 nodeOkNum = count(statusVec.begin(),statusVec.end(),OK);

			if (nodeOkNum == nodeTotal) 
			{
				ois = OsdOK;
				recoverRemoteNodeID = -1;
				RELEASE_ASSERT(FaultHandler::GetFaultHandler().DoRecoverDone());
				FaultHandler::GetFaultHandler().ClearRecoverLog();

				OSD_LOG_ERR("<<<<< OsdOK >>>>>");
				FaultHandler::GetFaultHandler().PushFaultHandlerTask(OsdOK);
			}
			else if (nodeOkNum == nodeTotal-1) 
			{
				ret = StatusHandleWrap(statusVec);
			}
			else
			{
				ret = StatusHandleWrap(statusVec);
				ois = OsdErr;
				OSD_LOG_ERR("<<<<< OsdErr >>>>>");
				FaultHandler::GetFaultHandler().PushFaultHandlerTask(OsdErr);
			}

			statusVec.clear();
			for (u32 i = 0;i<nodeTotal;++i) 
			{
				statusVec.push_back(nodesStatus[i*nodeTotal+i]);
			}
			nodeOkNum = count(statusVec.begin(),statusVec.end(),OK);
			if (nodeOkNum == nodeTotal || nodeOkNum == nodeTotal-1) 
			{
				RefreshOsdVersion();
			}

#ifndef OSD_DEBUG_NONET
			if (gIsUseFiState)
			{
				switch (ois)
				{
				case OsdOK:
					OsdState::GetOsdState().SetHealthy(FSucCode);
					break;
				case OsdErr:
					OsdState::GetOsdState().SetCriticalUnHealthy(FErr(MOsd_OsdIsErr));
					break;
				case LocNodeOK:
					if (nodeOkNum == nodeTotal-1) 
					{
						OsdState::GetOsdState().SetHealthy(FSucCode);
					}
					break;
				case LocNodeRecover:
					OsdState::GetOsdState().SetCriticalUnHealthy(FErr(MOsd_LocNodeIsRecover));
					break;
				case LocNodeErr:
					if (lastErr.Is(MOsd_LocNodeIsDiskErr))
					{
						OsdState::GetOsdState().SetCriticalUnHealthy(FErr(MOsd_LocNodeIsDiskErr));
					}
					else
					{
						OsdState::GetOsdState().SetCriticalUnHealthy(FErr(MOsd_LocNodeIsNetErr));
					}
					break;
				case StopDoRecoverType:
					OsdState::GetOsdState().SetHealthy(FErr(MOsd_LocNodeStopDoRecover));
					break;
				case StartDoRecoverType:
					OsdState::GetOsdState().SetHealthy(FErr(MOsd_LocNodeStartDoRecover));
					break;
				case DoFullRecoverType:
					OsdState::GetOsdState().SetHealthy(FErr(MOsd_LocNodeDoFullRecover));
					break;
				default:
					break;
				}
			}
#endif
			nodesStatusLock.Unlock();
			if (ret.IsErr())
			{
				FiEvent evt;
				evt.Wait(500);
			}
		}
		while (ret.IsErr()); 
	}

	FiErr RefreshNodeStatus()
	{
		FiErr ret;
		do
		{
			nodesStatusLock.Lock();
			ret = zkDataBase::GetZkDataBase().GetNodeData(nodesStatus,nodeTotal*nodeTotal,path,StatusWatcher,this);
			nodesStatusLock.Unlock();
			if (ret.IsErr())
			{
				FiEvent evt;
				evt.Wait(2000);
				OSD_LOG_ERR("===RefreshNodeStatus=== GetNodeData failed path[%s],errInfo[%s]",path.c_str(),ret.StrFiText().c_str());
			}
			else
			{
				OSD_LOG_ERR("===RefreshNodeStatus=== GetNodeData suc path[%s] %s",path.c_str(),GetNodesStatusFMTStr());
			}
		}
		while (ret.IsErr());
		return ret;
	}

//	bool IsStatusOK(s32 nodeID)
//	{
//		u32 offset = 0;
//		do
//		{
//			nodesStatusLock.Lock();
//			offset = nodeID*nodeTotal+nodeID;
//			FiErr ret = zkDataBase::GetZkDataBase().GetNodeData(nodesStatus,nodeTotal*nodeTotal,path,StatusWatcher,this);
//			nodesStatusLock.Unlock();
//			if (ret.IsSuccess())
//			{
//				OSD_LOG_ERR("===IsStatusOK=== GetNodeData suc path[%s] %s",path.c_str(),GetNodesStatusFMTStr());
//				break;
//			}
//			else
//			{
//				OSD_LOG_ERR("===IsStatusOK=== GetNodeData failed path[%s],errInfo[%s]",path.c_str(),ret.StrFiText().c_str());
//				FiEvent evt;
//				evt.Wait(3000);
//			}
//
//		}while(1);
//
//		return (nodesStatus[offset] == OK);
//	}

	bool IsOthersModuleOk()
	{
#ifndef OSD_DEBUG_NONET
		if (gIsUseFiState)
		{
			return FStateTool::GetInstance()->IsOtherSubStateHlty(&OsdState::GetOsdState());
		}
		return true;
#else
		return true;
#endif
	}

	FiErr LocStatusSetOK()
	{
		FiErr ret;
		u32 locOffset = nodeID*nodeTotal+nodeID;

		nodesStatusLock.Lock();
		if (OK == nodesStatus[locOffset])
		{
			nodesStatusLock.Unlock();
			OSD_LOG_ERR("===LocStatusSetRecover=== (OK == nodesStatus[locOffset]) ret");
			return ret;
		}
		OSD_LOG_ERR("===LocStatusSetOK=== GetZKLock begin");
		zkl->GetZKLock();
		ret = zkDataBase::GetZkDataBase().GetNodeData(nodesStatus,nodeTotal*nodeTotal,path,StatusWatcher,this);
		if (ret.IsSuccess())
		{
			OSD_LOG_ERR("===LocStatusSetOK=== GetNodeData %s",GetNodesStatusFMTStr());

			if (IsOthersModuleOk())
			{
				vector<char> statusVec;
				statusVec.reserve(nodeTotal);

				for (u32 i = 0; i < nodeTotal; ++i)
				{
					statusVec.push_back(nodesStatus[nodeID * nodeTotal + i]);
				}
				u32 nodeOkNum = count(statusVec.begin(), statusVec.end(), OK);

				if (nodeOkNum == nodeTotal - 1)
				{
					char statusBak = nodesStatus[locOffset];
					nodesStatus[locOffset] = OK; 
					ret = zkDataBase::GetZkDataBase().SetNodeData(path,nodesStatus,nodeTotal*nodeTotal);
					if (ret.IsErr())
					{
						OSD_LOG_ERR("===LocStatusSetOK=== SetNodeData locNode[%d-OK] <failed> path[%s],errInfo[%s] %s",nodeID,path.c_str(),ret.StrFiText().c_str(),GetNodesStatusFMTStr());
						nodesStatus[locOffset] = statusBak;
					}
					else
					{
						OSD_LOG_ERR("===LocStatusSetOK=== SetNodeData locNode[%d-OK] suc %s",nodeID,GetNodesStatusFMTStr());
						ois = LocNodeOK;
						lastErr = FSucCode;
						OSD_LOG_ERR("<<<<< RecoverDone and LocNodeOK >>>>>");
					}
					waitTime = INIT_WAITTIME;
				}
			}
			else
			{
				ret = FErr(MOsd_LocNodeIsNotOK);
			}
		}
		else
		{
			OSD_LOG_ERR("===LocStatusSetOK=== GetNodeData <failed> path[%s],errInfo[%s]",path.c_str(),ret.StrFiText().c_str());
		}
		zkl->ReleaseZKLock();
		OSD_LOG_ERR("===LocStatusSetOK=== GetZKLock end");
		nodesStatusLock.Unlock();

		if (ret.IsErr())
		{
			OSD_LOG_ERR("===LocStatusSetOK=== <failed> path[%s],errInfo[%s]",path.c_str(),ret.StrFiText().c_str());
		}
		return ret;
	}

	bool FindDoFullRecoverStatus()
	{
		vector<char> statusVec;
		statusVec.reserve(nodeTotal);

		for (u32 i = 0;i<nodeTotal;++i) 
		{
			statusVec.push_back(nodesStatus[nodeID*nodeTotal+i]);
		}
		return (count(statusVec.begin(),statusVec.end(),DOFULLRECOVER) != 0);
	}

	FiErr LocStatusSetRecover()
	{
		FiErr ret;
		u32 locOffset = nodeID*nodeTotal+nodeID;

		nodesStatusLock.Lock();
		if (ERR != nodesStatus[locOffset])
		{
			nodesStatusLock.Unlock();
			OSD_LOG_ERR("===LocStatusSetRecover=== (ERR != nodesStatus[locOffset]) ret");
			return ret;
		}

		if (FindDoFullRecoverStatus())
		{
			nodesStatusLock.Unlock();
			OSD_LOG_ERR("===LocStatusSetRecover=== FindDoFullRecoverStatus[true] ret");
			return ret;
		}

		OSD_LOG_ERR("===LocStatusSetRecover=== GetZKLock begin");
		zkl->GetZKLock();
		ret = zkDataBase::GetZkDataBase().GetNodeData(nodesStatus,nodeTotal*nodeTotal,path,StatusWatcher,this);
		if (ret.IsSuccess())
		{
			OSD_LOG_ERR("===LocStatusSetRecover=== GetNodeData %s",GetNodesStatusFMTStr());
			char statusBak = nodesStatus[locOffset];
			nodesStatus[locOffset] = RECOVER;
			ret = zkDataBase::GetZkDataBase().SetNodeData(path, nodesStatus, nodeTotal * nodeTotal);
			if (ret.IsErr())
			{
				OSD_LOG_ERR("===LocStatusSetRecover=== SetNodeData locNode[%d-RECOVER] <failed> path[%s],errInfo[%s] %s",nodeID,path.c_str(),ret.StrFiText().c_str(),GetNodesStatusFMTStr());
				nodesStatus[locOffset] = statusBak;
			}
			else
			{
				OSD_LOG_ERR("===LocStatusSetRecover=== SetNodeData locNode[%d-RECOVER] suc %s",nodeID,GetNodesStatusFMTStr());
			}
		}
		else
		{
			OSD_LOG_ERR("===LocStatusSetRecover=== GetNodeData <failed> path[%s],errInfo[%s]",path.c_str(),ret.StrFiText().c_str());
		}
		zkl->ReleaseZKLock();
		OSD_LOG_ERR("===LocStatusSetRecover=== GetZKLock end");
		nodesStatusLock.Unlock();

		if (ret.IsErr())
		{
			OSD_LOG_ERR("===LocStatusSetRecover=== <failed> path[%s],errInfo[%s]",path.c_str(),ret.StrFiText().c_str());
		}
		return ret;
	}

	bool NodeDoRecoverReset(s32 remoteNodeID)
	{
		FiErr ret;
		bool isSet = false;
		RELEASE_ASSERT(remoteNodeID >= 0 && remoteNodeID != nodeID);

		nodesStatusLock.Lock();

		u32 locOffset = nodeID*nodeTotal+nodeID;
		u32 remoteOffset = remoteNodeID*nodeTotal+remoteNodeID;
		u32 setOffset = remoteNodeID*nodeTotal+nodeID;

		if (OK != nodesStatus[locOffset]) 
		{
			nodesStatusLock.Unlock();
			OSD_LOG_ERR("===NodeDoRecoverReset=== localNode[%d] is not OK",nodeID);
			return false;
		}

		if (OK == nodesStatus[setOffset])
		{
			nodesStatusLock.Unlock();
			OSD_LOG_ERR("===NodeDoRecoverReset=== remoteNode[%d] is OK",remoteNodeID);
			return true;
		}

		if (!FaultHandler::GetFaultHandler().DoRecoverDone())
		{
			nodesStatusLock.Unlock();
			OSD_LOG_ERR("===NodeDoRecoverReset=== recover is not done");
			return false;
		}

		OSD_LOG_ERR("===NodeDoRecoverReset=== GetZKLock begin");
		zkl->GetZKLock();
		ret = zkDataBase::GetZkDataBase().GetNodeData(nodesStatus,nodeTotal*nodeTotal,path,StatusWatcher,this);
		if(ret.IsSuccess())
		{
			OSD_LOG_ERR("===NodeDoRecoverReset=== GetNodeData %s",GetNodesStatusFMTStr());
			if (OK != nodesStatus[locOffset]) 
			{
				goto NodeDoRecoverResetEnd;
			}
			RELEASE_ASSERT(OK != nodesStatus[setOffset] && OK != nodesStatus[remoteOffset]);

			char statusBak = nodesStatus[setOffset];
			nodesStatus[setOffset] = OK;
			isSet = true;
			ret = zkDataBase::GetZkDataBase().SetNodeData(path,nodesStatus,nodeTotal*nodeTotal);
			if (ret.IsErr())
			{
				nodesStatus[setOffset] = statusBak;
				OSD_LOG_ERR("===NodeDoRecoverReset=== SetNodeData <failed> remoteNodeID[%d-DORECOVER-OK] %s",remoteNodeID,GetNodesStatusFMTStr());
			}
			else
			{
				OSD_LOG_ERR("===NodeDoRecoverReset=== SetNodeData suc remoteNodeID[%d-DORECOVER-OK] %s",remoteNodeID,GetNodesStatusFMTStr());
			}
		}

NodeDoRecoverResetEnd:
		zkl->ReleaseZKLock();
		OSD_LOG_ERR("===NodeDoRecoverReset=== GetZKLock end");
		nodesStatusLock.Unlock();

		if (ret.IsErr())
		{
			OSD_LOG_ERR("===NodeDoRecoverReset=== %s <failed> path[%s],errInfo[%s]",isSet?"SetNodeData":"GetNodeData",path.c_str(),ret.StrFiText().c_str());
			return false;
		}
		return true;
	}

	FiErr NodeDoRecoverSet(s32 remoteNodeID,BaseObjIo *io);
	FiErr NodeDoRecoverSetEx(s32 remoteNodeID,IoFileLog *ioRecoverLog,ObjID &fileID,vector<NodeAndTruncateNo> &ntAry);

	bool RefreshNodeStatusAndSet()
	{
		FiErr ret;
		bool isSet = false;
		bool isSetData = false;
		u32 index = -1;

		static vector<string> &liveNodeAry = zkDataBase::GetZkDataBase().GetLiveNodeAry();
		static OsdLock &liveNodeAryLock = zkDataBase::GetZkDataBase().GetLiveNodeAryLock();
		static u32 preStrLen = zkDataBase::GetZkDataBase().GetLiveNodePathPreLen();

		nodesStatusLock.Lock();

		memset(liveNodes,0,nodeTotal);
		liveNodeAryLock.Lock();
		for (vector<string>::iterator it = liveNodeAry.begin();it != liveNodeAry.end();++it)
		{
			index = str2u32(it->c_str()+preStrLen);
			RELEASE_ASSERT(index < nodeTotal);
			liveNodes[index] = 1;
		}
		liveNodeAryLock.Unlock();

		OSD_LOG_ERR("===RefreshNodeStatusAndSet=== GetZKLock begin");
		zkl->GetZKLock();
		ret = zkDataBase::GetZkDataBase().GetNodeData(nodesStatus,nodeTotal*nodeTotal,path,StatusWatcher,this);
		if (ret.IsSuccess())
		{
			OSD_LOG_ERR("===RefreshNodeStatusAndSet=== GetNodeData %s",GetNodesStatusFMTStr());
			memcpy(nodesStatusBak,nodesStatus,nodeTotal*nodeTotal);
			for (s32 i = 0;i<nodeTotal;++i)
			{
				if (liveNodes[i] == 0 && nodesStatus[i*nodeTotal+i] != ERR)
				{
					OSD_LOG_ERR("===RefreshNodeStatusAndSet=== SetNodeData nodeID[%d-ERR]",i);
					nodesStatus[i*nodeTotal+i] = ERR;
					isSetData = true;
				}
			}

			if (isSetData)
			{
				isSet = true;
				ret = zkDataBase::GetZkDataBase().SetNodeData(path,nodesStatus,nodeTotal*nodeTotal);
				if (ret.IsErr())
				{
					memcpy(nodesStatus,nodesStatusBak,nodeTotal*nodeTotal);
					OSD_LOG_ERR("===RefreshNodeStatusAndSet=== SetNodeData <failed> %s",GetNodesStatusFMTStr());
				}
				else
				{
					OSD_LOG_ERR("===RefreshNodeStatusAndSet=== SetNodeData suc %s",GetNodesStatusFMTStr());
				}
			}
		}

		zkl->ReleaseZKLock();
		OSD_LOG_ERR("===RefreshNodeStatusAndSet=== GetZKLock end");
		nodesStatusLock.Unlock();

		if (ret.IsErr())
		{
			OSD_LOG_ERR("===RefreshNodeStatusAndSet=== %s <failed> path[%s],errInfo[%s]",isSet?"SetNodeData":"GetNodeData",path.c_str(),ret.StrFiText().c_str());
			return false;
		}
		return true;
	}

	bool NodeErrSet(s32 setNodeID)
	{
		FiErr ret;
		bool isSet = false;
		u32 locOffset = nodeID*nodeTotal+nodeID;
		u32 setOffset = setNodeID*nodeTotal+setNodeID;
		RELEASE_ASSERT(setNodeID >= 0);

NodeErrSetReDo:
		nodesStatusLock.Lock();
//		if (OK != nodesStatus[locOffset])
//		{
//			nodesStatusLock.Unlock();
//			OSD_LOG_ERR("===NodeErrSet=== locNode[%d] is not OK",nodeID);
//			return false;
//		}

		OSD_LOG_ERR("===NodeErrSet=== GetZKLock begin");
		zkl->GetZKLock();
		ret = zkDataBase::GetZkDataBase().GetNodeData(nodesStatus,nodeTotal*nodeTotal,path,StatusWatcher,this);
		if (ret.IsSuccess())
		{
			OSD_LOG_ERR("===NodeErrSet=== GetNodeData %s",GetNodesStatusFMTStr());
			if (ERR == nodesStatus[setOffset]) 
			{
				goto SetNodeErrEnd;
			}
			char statusBak = nodesStatus[setOffset];
			nodesStatus[setOffset] = ERR;
			isSet = true;
			ret = zkDataBase::GetZkDataBase().SetNodeData(path,nodesStatus,nodeTotal*nodeTotal);
			if (ret.IsErr())
			{
				nodesStatus[setOffset] = statusBak;
				OSD_LOG_ERR("===NodeErrSet=== SetNodeData <failed> nodeID[%d-ERR] %s",setNodeID,GetNodesStatusFMTStr());
			}
			else
			{
				if (OsdCfg::GetOsdCfg().GetLocNodeID() == setNodeID)
				{
					lastErr = FErr(MOsd_LocNodeIsDiskErr);
				}
				OSD_LOG_ERR("===NodeErrSet=== SetNodeData suc nodeID[%d-ERR] %s",setNodeID,GetNodesStatusFMTStr());
			}
		}

SetNodeErrEnd:
		zkl->ReleaseZKLock();
		OSD_LOG_ERR("===NodeErrSet=== GetZKLock end");
		nodesStatusLock.Unlock();

		if (ret.IsErr())
		{
			OSD_LOG_ERR("===NodeErrSet=== %s failed path[%s],errInfo[%s]",isSet?"SetNodeData":"GetNodeData",path.c_str(),ret.StrFiText().c_str());
			FiEvent evt;
			evt.Wait(1000);
			goto NodeErrSetReDo;
//			return false;
		}
		return true;
	}

	bool CheckOsdVersion()
	{
		FiErr ret;
		ObjID oid;

		ret = zkDataBase::GetZkDataBase().GetNodeData((char *)&oid,sizeof(ObjID),osdVersionZKPath,NULL,this);
		if (ret.IsSuccess())
		{
			if (oid == osdVersion)
			{
				return true;
			}
		}
		return false;
	}

	bool RefreshOsdVersion()
	{
		FiErr ret;
		ObjID oid;

RefreshOsdVersionReDo:
		nodesStatusLock.Lock();
		OSD_LOG_ERR("===RefreshOsdVersion=== GetZKLock begin");
		zkl->GetZKLock();
		ret = zkDataBase::GetZkDataBase().GetNodeData((char *)&oid,sizeof(ObjID),osdVersionZKPath,NULL,this);
		if (ret.IsSuccess())
		{
			OSD_LOG_ERR("===RefreshOsdVersion=== GetNodeData zkOsdVersion[%s]",oid.GetIDStr().c_str());
			if (oid == osdVersion)
			{
				oid.GenerateID();
				ret = zkDataBase::GetZkDataBase().SetNodeData(osdVersionZKPath,(char *)&oid,sizeof(ObjID));
				if (ret.IsErr())
				{
					OSD_LOG_ERR("===RefreshOsdVersion=== SetNodeData <failed> zkOsdVersion[%s]",oid.GetIDStr().c_str());
					goto RefreshOsdVersionEnd;
				}
			}
			osdVersion = oid;
			ret = ficsLocIF::GetFicsLocIF().GetFileIF(osdVersionLocPath)->WriteData((char *)&osdVersion, sizeof(ObjID), 0);
		}

RefreshOsdVersionEnd:
		zkl->ReleaseZKLock();
		OSD_LOG_ERR("===RefreshOsdVersion=== GetZKLock end");
		nodesStatusLock.Unlock();

		if (ret.IsErr())
		{
			OSD_LOG_ERR("===RefreshOsdVersion=== failed osdVersionZKPath[%s],errInfo[%s]",osdVersionZKPath.c_str(),ret.StrFiText().c_str());
			FiEvent evt;
			evt.Wait(1000);
			goto RefreshOsdVersionReDo;
		}
		return true;
	}

	inline bool FindNotOkNodeID(s32 nodeID){AutoLockHelper l(&nodesStatusLock);return (find(notOkNodeIDs.begin(),notOkNodeIDs.end(),nodeID) != notOkNodeIDs.end());}
	inline s32 GetRecoverRemoteNodeID(){AutoLockHelper l(&nodesStatusLock);return recoverRemoteNodeID;}
	inline bool IsRemoteNodeErr(){AutoLockHelper l(&nodesStatusLock);return (ois == OsdErr || ois == StopDoRecoverType || ois == StartDoRecoverType || ois == DoFullRecoverType);}
	inline bool IsLocNodeOK(){AutoLockHelper l(&nodesStatusLock);return !(ois == OsdErr || ois == LocNodeRecover || ois == LocNodeErr);}
	inline bool IsLocNodeErr(){AutoLockHelper l(&nodesStatusLock);return (ois == OsdErr);}
//	inline FiErr& GetLastErr(){return lastErr;}
		
	inline OsdLock& GetNodesStatusLock(){return nodesStatusLock;}
//  inline zkLock* GetZKLock(){return zkl;}

	void ExpandUpdate(s32 newNodeTotal)		//???
	{
		nodesStatusLock.Lock();

		nodeTotal = newNodeTotal;
		nodesStatus = new char[nodeTotal*nodeTotal];
		RELEASE_ASSERT(NULL != nodesStatus);
		nodesStatusBak = new char[nodeTotal*nodeTotal];
		RELEASE_ASSERT(NULL != nodesStatusBak);
		liveNodes = new char[nodeTotal];
		RELEASE_ASSERT(NULL != liveNodes);
		path = string("/nodesState1");

		nodesStatusLock.Unlock();
	}

private:
	NodeStatusObserver():nodeTotal(OsdCfg::GetOsdCfg().GetNodeTotal()),nodeID(OsdCfg::GetOsdCfg().GetLocNodeID()),path("/nodesState"),osdVersionZKPath("/osdVersion"),waitTime(INIT_WAITTIME),recoverRemoteNodeID(-1)
	{
		nodesStatus = new char[nodeTotal*nodeTotal];
		RELEASE_ASSERT(NULL != nodesStatus);
		nodesStatusBak = new char[nodeTotal*nodeTotal];
		RELEASE_ASSERT(NULL != nodesStatusBak);
		liveNodes = new char[nodeTotal];
		RELEASE_ASSERT(NULL != liveNodes);
		char lockPath[] = "/osdLock";
		zkl = new zkLock(lockPath);
		RELEASE_ASSERT(NULL != zkl);

		if (zkDataBase::GetZkDataBase().NodeExists(osdVersionZKPath).IsErr())
		{
			FiErr ret = zkDataBase::GetZkDataBase().CreateNode(osdVersionZKPath);
			RELEASE_ASSERT(ret.IsSuccess());
		}
		if (OsdCfg::GetOsdCfg().IsInstallMetaSvr())
		{
			osdVersionLocPath = OsdCfg::GetOsdCfg().GetMetaRootPath()+string("OsdVersion");
		}
		else
		{
			osdVersionLocPath = OsdCfg::GetOsdCfg().GetDataRootPath()+string("OsdVersion");
		}
		u32 len = sizeof(ObjID);
		FiErr ret = ficsLocIF::GetFicsLocIF().GetFileIF(osdVersionLocPath)->ReadData((char *)&osdVersion, len, 0);
//		RELEASE_ASSERT(ret.IsSuccess() && sizeof(ObjID) == len);

//		if(RefreshNodeStatus().IsSuccess())
//		{
//			StatusAnalyze();
//		}
	}

	~NodeStatusObserver()
	{
		if (NULL != nodesStatus)
		{
			delete []nodesStatus;
		}

		if (NULL != nodesStatusBak)
		{
			delete []nodesStatusBak;
		}

		if (NULL != liveNodes)
		{
			delete []liveNodes;
		}

		if (NULL != zkl)
		{
			delete zkl;
		}
	}

	char* nodesStatus;
	char* nodesStatusBak;
	char* liveNodes;
	OsdLock nodesStatusLock;
	s32 nodeTotal;
	s32 nodeID;
	string path;
	zkLock *zkl;
	u32 waitTime;
	FiEvent evt;
	OsdInternalStatus ois;
	vector<s32> notOkNodeIDs;
	s32 recoverRemoteNodeID;
	FiErr lastErr;
	string osdVersionZKPath;
	string osdVersionLocPath;
	ObjID osdVersion;
};

struct ObjStream
{
	static ObjStream& GetObjStream(){static ObjStream ostm;return ostm;}

	static void IoHandleFunc(u64 taskId, void* par)
	{
		BaseObjIo *io = (BaseObjIo *)par;	
		io->ObjStreamProcessObjIo();
	}

	void PushIo(BaseObjIo *io)
	{
		FiEvent evt;
		RELEASE_ASSERT(ioHandleThreadPool.PushTask(io->GetDstNode(),io,NULL,&evt).IsSuccess());
		evt.Wait();
	}

	bool WriteIoFind(ObjID &fileID,u32 length,u64 offset,ObjDataType type,FiEvent *evt)
	{
		BaseObjIo ioFake;	//fake io,just for search
		ioFake.fileID = fileID;
		ioFake.length = length;
		ioFake.offset = offset;
		ioFake.dataType = type;

		return oim.IoFind(&ioFake,evt);
	}

	void WriteIoDel(BaseObjIo *io)
	{
		oim.IoDel(io);
	}

	void WriteIoAdd(BaseObjIo *io)
	{
		oim.IoAdd(io);
	}

	inline void WaitIoAryEmpty(){oim.WaitEmpty();}
	inline void AssertIoAryEmpty(){oim.AssertEmpty();}

private:
	ObjStream()
	{
		ioHandleThreadPool.SetDefaultCallBack(IoHandleFunc);
		ioHandleThreadPool.SetMaxThreadNum(OsdCfg::GetOsdCfg().GetObjStreamThreadNum());
		ioHandleThreadPool.SetMaxThreadNumPerTask(2);
		ioHandleThreadPool.Run();
	}
	~ObjStream(){}

	FiThreads ioHandleThreadPool;

	ObjIoMutex oim;
};

struct NetWapper
{
	struct RecvIoHandlePar
	{
		MemItemPtr p1;
		MemItemPtr p2;
	};

	static NetWapper& GetNetWapper(){static NetWapper nw;return nw;}

	static void NetIoHandleFunc(u64 taskId, void * par)
	{
		BaseObjIo *io = (BaseObjIo *)par;
		NetWapper::GetNetWapper().SendNetIo(io);
	}

	static void RecvIoHandleFunc(u64 taskId, void * par)
	{
		RecvIoHandlePar *rihpar = (RecvIoHandlePar *)par;
		Tsheet *sh = (Tsheet *)rihpar->p1->GetCurDataPtr();
		OSD_LOG_ERR("===RecvIoHandleFunc===...pktid[%lld],mod[%d]",sh->sheet_num,sh->reg_module_type);
		NetWapper::GetNetWapper().ReceiveNetIoHandle(rihpar->p1,rihpar->p2);
	}

	FiThreads& GetRecvIoHandleThreadPool(){return recvIoHandleThreadPool;}
	static bool ReceiveNetIo(MemItemPtr sheetData,MemItemPtr soktData, CTCPSocket *psokt, void *userData);
	void ReceiveNetIoHandle(MemItemPtr sheetData,MemItemPtr soktData);

	void PushNetIo(BaseObjIo *io,FiEvent &evt)
	{
		RELEASE_ASSERT(sendIoHandleThreadPool.PushTask(io->GetDstNode(),io,NULL,&evt).IsSuccess());
	}

	bool NetSendTest(BaseObjIo *ioSrc)
	{
		if (NodeStatusObserver::GetNodeStatusObserver().IsRemoteNodeErr())
		{
			return true;
		}

//		if(rand() % 88 == 0)	//close net rand test
//		{
//			return false;
//		}

		return true;

		//single node net test code
//  	BaseObjIo *io = (BaseObjIo *)soktData->GetBufferPtr();
//  	RELEASE_ASSERT(io->GetVersion() == OsdCfg::GetOsdCfg().GetVersion());
//
//  	if (NodeStatusObserver::GetNodeStatusObserver().IsRemoteNodeErr()	&&
//  		io->GetLocNode() == NodeStatusObserver::GetNodeStatusObserver().GetErrRemoteNodeID())
//  	{
//  		io->SetRetStatus(FErr(MOsd_RemoteNodeIsNotOK));
//  		ioSrc->SetRetStatus(io->GetRetStatus());
//  		return true;
//  	}
//
//  	if (io->GetMainNode() == OsdCfg::GetOsdCfg().GetLocNodeID()		&&
//  		NodeStatusObserver::GetNodeStatusObserver().IsLocNodeOK())
//  	{
//  		io->SetRetStatus(FErr(MOsd_MainNodeIsOK));
//  		ioSrc->SetRetStatus(io->GetRetStatus());
//  		return true;
//  	}
//
//  	BaseObjIo *netIo = ObjIoFactory::GetObjIoFactory().CreateObj(io->GetObjIoType());
//  	netIo->InitObjIo((char *)soktData->GetBufferPtr());
//
//  	NetWapper::GetNetWapper().ProcessingNetIoAryAdd(netIo);
//  	ObjStream::GetObjStream().PushIo(netIo);
//  	NetWapper::GetNetWapper().ProcessingNetIoAryDel(netIo);
//
//  	if (netIo->IsHaveReplyBuf())
//  	{
//  		memcpy(ioSrc->buf,netIo->buf,netIo->GetRetLength());
//  		ioSrc->retLength = netIo->GetRetLength();
//  	}
//  	else
//  	{
//  		ioSrc->SetRetStatus(io->GetRetStatus());
//  	}
//  	netIo->DelObjIo(netIo);
//  	return true;
	}

	void SendNetIo(BaseObjIo *io);

	struct ProcessingNetIoCompare
	{
		inline bool operator()(BaseObjIo *l,BaseObjIo *r)
		{
			if (l->fileID == r->fileID)
			{
				return l->srcNode < r->srcNode;
			}
			else
			{
				return l->fileID < r->fileID;
			}
		}
	};

	void ProcessingNetIoAryAdd(BaseObjIo *netIo)
	{
		oim.IoAdd(netIo);
	}

	void ProcessingNetIoAryDel(BaseObjIo *netIo)
	{
		oim.IoDel(netIo);
	}

	bool ProcessingNetIoAryFind(ObjID &fileID,u32 length,u64 offset,ObjDataType type,FiEvent *pEvt)
	{
		BaseObjIo ioFake;	//fake io,just for search
		ioFake.fileID = fileID;
		ioFake.length = length;
		ioFake.offset = offset;
		ioFake.dataType = type;

		return oim.IoFind(&ioFake,pEvt);
	}

	bool NWSocketExit(CTCPSocket *sokt, void *userData)		//???流窜包
	{
//		FiErr ret;
//		string sessionNoFile("SessionNo");
//		string sessionNoFilePath;
//		if (OsdCfg::GetOsdCfg().IsInstallMetaSvr())
//		{
//			sessionNoFilePath = OsdCfg::GetOsdCfg().GetMetaRootPath()+sessionNoFile;
//		}
//		else
//		{
//			sessionNoFilePath = OsdCfg::GetOsdCfg().GetDataRootPath()+sessionNoFile;
//		}
//
//		sessionNoLock.Lock();
//		++sessionNo;
//
//		ret = ficsLocIF::GetFicsLocIF().GetFileIF(sessionNoFilePath)->WriteData((char *)&sessionNo, sizeof(u64), 0);
//		if (ret.IsErr())
//		{
//			NodeStatusObserver::GetNodeStatusObserver().NodeErrSet(OsdCfg::GetOsdCfg().GetLocNodeID());
//			FiEvent evtTp;
//			evtTp.Wait(2000);
////			goto REDO_WRITESESSIONNO;
//		}
//		sessionNoLock.Unlock();

		return true;
	}

	u64 GetSessionNo()
	{
		u64 ret = 0;
		sessionNoLock.Lock();
		ret = sessionNo;
		sessionNoLock.Unlock();
		return ret;
	}

private:
	NetWapper()
	{
#ifndef OSD_DEBUG_NONET
		net.Start(OSD_PORT);
//		FiNetTool::SyncStartFiSvrNet(net,OSD_PORT);
		CModuleInfo *mi = new CModuleInfo(NET_MOD_ID, (SOCKETCREATE_FUNC)NULL, ReceiveNetIo, (SOCKETEXIT_FUNC)NULL);
		net.RegisterModule(mi);
#endif

		sendIoHandleThreadPool.SetDefaultCallBack(NetIoHandleFunc);
		sendIoHandleThreadPool.SetMaxThreadNum(OsdCfg::GetOsdCfg().GetNetWapperThreadNum());
		sendIoHandleThreadPool.SetMaxThreadNumPerTask(2);
		sendIoHandleThreadPool.Run();

		recvIoHandleThreadPool.SetDefaultCallBack(RecvIoHandleFunc);
		recvIoHandleThreadPool.SetMaxThreadNum(OsdCfg::GetOsdCfg().GetNetWapperThreadNum());
		recvIoHandleThreadPool.SetMaxThreadNumPerTask(2);
		recvIoHandleThreadPool.Run();
	}
	~NetWapper(){}

	FiThreads sendIoHandleThreadPool;
	FiThreads recvIoHandleThreadPool;
	CTcpIo net;

	ObjIoMutex oim;

	u64 sessionNo;
	OsdLock sessionNoLock;
};

struct LocWapper
{

#pragma pack(push,1)
	struct IoCmdLogElement
	{
		IoCmdLogElement():no(0),markNum(0){memset(nodeID,-1,sizeof(s32)*DATA_NODE_MAX_NUM);}
		IoCmdLogElement(ObjID &fileIDPar,s32 *nodeIDPar,s32 nodeIDNum):fileID(fileIDPar){RELEASE_ASSERT(nodeIDNum <= DATA_NODE_MAX_NUM);memcpy(nodeID,nodeIDPar,sizeof(s32)*nodeIDNum);}
		u64 no;
		ObjID fileID;
		s32 nodeID[DATA_NODE_MAX_NUM];
		u32 markNum;
	};
#pragma pack(pop)

	struct IoCmdLog
	{
		#define READ_IOCMDLOGELEMENT_BUFSIZE (sizeof(IoCmdLogElement)*80*1024)

		struct icleData
		{
			icleData() { }
			icleData(IoCmdLogElement *e) : fileID(e->fileID), markNum(e->markNum) {memcpy(nodeID,e->nodeID,sizeof(s32)*DATA_NODE_MAX_NUM);}
			ObjID fileID;
			s32 nodeID[DATA_NODE_MAX_NUM];
			u32 markNum;
		}; 
		typedef multimap<u64, icleData> icleMMap;

		void PrintElement()
		{
//			lock.Lock();
//			for (it = logMMap.begin(); it != logMMap.end(); ++it)
//			{
//				OSD_LOG_ERR("===IoCmdLogPrintElement=== no[%lld],fileID[%s],nodeID[%p],markNum[%d]",it->first,it->second.fileID.GetIDStr().c_str(),it->second.nodeID,it->second.markNum);
//			}
//			lock.Unlock();
		}
		
		bool IsCmdLogNull()
		{
			bool ret = true;
			lock.Lock();
			if (logMMap.empty())
			{
				lock.Unlock();
				return ret;
			}
			it = logMMap.upper_bound(0);
			ret = (logMMap.end() == it);
			lock.Unlock();
			return ret;
		}

		bool GetCmdLogElement(IoCmdLogElement& g)
		{
			bool ret = false;

			lock.Lock();
			if (logMMap.empty())
			{
				noCount = 0;
				evt.Reset();
				lock.Unlock();
				return ret;
			}
			it = logMMap.upper_bound(0);
			if (logMMap.end() != it) 
			{
				g.no = it->first;
				g.fileID = it->second.fileID;
				memcpy(g.nodeID,it->second.nodeID,sizeof(s32)*DATA_NODE_MAX_NUM);
				g.markNum = it->second.markNum;
				OSD_LOG_ERR("===GetCmdLogElement=== no[%lld],fileID[%s],nodeID[%p],markNum[%d]",it->first,it->second.fileID.GetIDStr().c_str(),it->second.nodeID,it->second.markNum);
				ret = true;
			}
			else
			{
				OSD_LOG_ERR("===GetCmdLogElement=== noCount[0]");
				noCount = 0;
				evt.Reset();
			}
			lock.Unlock();

			return ret;
		}

		bool DelCmdLogElement(IoCmdLogElement& d)
		{
			FiErr ret;

REDO_DELCMDLOGELEMENT:
			lock.Lock();
			if (logMMap.empty())
			{
				evt.Reset();
				lock.Unlock();
				return false;
			}
			it = logMMap.find(d.no);
			RELEASE_ASSERT(it != logMMap.end());

			IoCmdLogElement e;
			e.fileID.ClearID();
			e.no = 0;
			memset(e.nodeID,-1,sizeof(s32)*DATA_NODE_MAX_NUM);
			e.markNum = it->second.markNum;
			ret = ficsLocIF::GetFicsLocIF().GetFileIF(cmdLogPath)->WriteData((char *)&e, sizeof(IoCmdLogElement), sizeof(IoCmdLogElement) * e.markNum);
			if (ret.IsSuccess())
			{
				OSD_LOG_ERR("===DelCmdLogElement=== no[%lld],fileID[%s],nodeID[%p],markNum[%d]",it->first,it->second.fileID.GetIDStr().c_str(),it->second.nodeID,it->second.markNum);
				logMMap.erase(it);
				logMMap.insert(icleMMap::value_type(e.no,icleData(&e)));
			}
			PrintElement();
			lock.Unlock();

			if (ret.IsErr())
			{
				NodeStatusObserver::GetNodeStatusObserver().NodeErrSet(OsdCfg::GetOsdCfg().GetLocNodeID());
				FiEvent evtTp;
				evtTp.Wait(2000);
				goto REDO_DELCMDLOGELEMENT;
			}

			return true;
		}

		FiErr AddCmdLogElement(IoCmdLogElement& a)
		{
			FiErr ret;
			lock.Lock();
			if (logMMap.empty())
			{
				a.no = ++noCount;
				a.markNum = 0;
				ret = ficsLocIF::GetFicsLocIF().GetFileIF(cmdLogPath)->WriteData((char *)&a, sizeof(IoCmdLogElement), sizeof(IoCmdLogElement)*a.markNum);
				if (ret.IsSuccess())
				{
					OSD_LOG_ERR("===AddCmdLogElement=== no[%lld],fileID[%s],nodeID[%p],markNum[%d]",a.no,a.fileID.GetIDStr().c_str(),a.nodeID,a.markNum);
					logMMap.insert(icleMMap::value_type(a.no,icleData(&a)));
				}
			}
			else
			{
				it = logMMap.find(0);
				if (it != logMMap.end())
				{
					a.no = ++noCount;
					a.markNum = it->second.markNum;
					ret = ficsLocIF::GetFicsLocIF().GetFileIF(cmdLogPath)->WriteData((char *)&a, sizeof(IoCmdLogElement), sizeof(IoCmdLogElement)*a.markNum);
					if (ret.IsSuccess())
					{
						logMMap.erase(it);
						OSD_LOG_ERR("===AddCmdLogElement=== no[%lld],fileID[%s],nodeID[%p],markNum[%d]",a.no,a.fileID.GetIDStr().c_str(),a.nodeID,a.markNum);
						logMMap.insert(icleMMap::value_type(a.no,icleData(&a)));
					}
				}
				else
				{
					a.no = ++noCount;
					a.markNum = logMMap.size();
					ret = ficsLocIF::GetFicsLocIF().GetFileIF(cmdLogPath)->WriteData((char *)&a, sizeof(IoCmdLogElement), sizeof(IoCmdLogElement)*a.markNum);
					if (ret.IsSuccess())
					{
						OSD_LOG_ERR("===AddCmdLogElement=== no[%lld],fileID[%s],nodeID[%p],markNum[%d]",a.no,a.fileID.GetIDStr().c_str(),a.nodeID,a.markNum);
						logMMap.insert(icleMMap::value_type(a.no,icleData(&a)));
					}
				}
			}
			evt.Set();
			PrintElement();
			lock.Unlock();

			if (ret.IsErr())
			{
				NodeStatusObserver::GetNodeStatusObserver().NodeErrSet(OsdCfg::GetOsdCfg().GetLocNodeID());
			}

			return ret;
		}

		inline FiEvent& GetEvt(){return evt;};
		static void* DoCmdThread(void* par)
		{
			IoCmdLog *cmdLog = (IoCmdLog*)par;
			FiEvent &evt = cmdLog->GetEvt();
			LocWapper &lw = LocWapper::GetLocWapper();
			IoCmdLogElement e,bak;
			bool ret = false;

			while (1) 
			{
				evt.Wait();
				if(cmdLog->GetCmdLogElement(e))
				{
					ret = lw.DoDelete(e) && lw.DoDelete(e,false);
					if (!ret)
					{
						bak = e;
						ret = cmdLog->AddCmdLogElement(bak).IsSuccess();
					}
					if (ret)
					{
						cmdLog->DelCmdLogElement(e);
					}
				}
			}
		}

		IoCmdLog(string clp):noCount(0),cmdLogPath(clp)
		{
			u32 fileSize = ficsLocIF::GetFicsLocIF().FileSize(cmdLogPath);
			if (fileSize > 0)
			{
				char *bufTp = new char[READ_IOCMDLOGELEMENT_BUFSIZE];
				RELEASE_ASSERT(bufTp != NULL);
				memset(bufTp,0,READ_IOCMDLOGELEMENT_BUFSIZE);
				u32 offset = 0;
				u32 length = (fileSize>READ_IOCMDLOGELEMENT_BUFSIZE)?READ_IOCMDLOGELEMENT_BUFSIZE:fileSize;
				u32 lenBak = 0;
				FiErr ret;
				u32 copyNum = length/sizeof(IoCmdLogElement);
				IoCmdLogElement *e = NULL;

				do
				{
					lenBak = length;
				IoCmdLogRedoReadData:
					ret = ficsLocIF::GetFicsLocIF().GetFileIF(cmdLogPath)->ReadData(bufTp,length,offset);
					if (ret.IsErr())
				    {
				      FiEvent evt;
				      OSD_LOG_ERR("===IoCmdLog=== ReadData <failed> path[%s],errInfo[%s],2s later redo",cmdLogPath.c_str(),ret.StrFiText().c_str());
				      evt.Wait(2000);
				      goto IoCmdLogRedoReadData;
				    }
					RELEASE_ASSERT(length == lenBak && length%sizeof(IoCmdLogElement) == 0);
					for (u32 i = 0; i<copyNum; ++i)
					{
						e = (IoCmdLogElement *)bufTp+i;
						noCount = (noCount > e->no) ? noCount : e->no;
						logMMap.insert(icleMMap::value_type(e->no,icleData(e)));
					}
					offset += length;
					fileSize -= length;
					length = (fileSize>READ_IOCMDLOGELEMENT_BUFSIZE)?READ_IOCMDLOGELEMENT_BUFSIZE:fileSize;
					copyNum = length/sizeof(IoCmdLogElement);
				}
				while (fileSize); 
				delete []bufTp;
				PrintElement();
			}

			evt.SetMode(TRUE);
			if(pthread_create(&doCmdThreadId, NULL, DoCmdThread, this) != 0)
			{
				RELEASE_ASSERT_FASLE;
			}
		}

	private:
		icleMMap logMMap;
		OsdLock lock;

		string cmdLogPath;

		u64 noCount;

		pthread_t doCmdThreadId;
		FiEvent evt;
		
		icleMMap::iterator it;
	};

	static LocWapper& GetLocWapper(){static LocWapper lw;return lw;}

	FiErr SendIo(BaseObjIo *io,char *buf,u32 length,u64 offset,ObjIoType ioType)
	{
		FiErr ret;
		FiEvent evt;

		if (0 == length)
		{
			return ret;
		}

		BaseObjIo *redirectIo = ObjIoFactory::GetObjIoFactory().CreateObj(ioType);
		redirectIo->InitObjIo((char *)io,buf,length,offset);
		NetWapper::GetNetWapper().PushNetIo(redirectIo,evt);
		evt.Wait();
		ret = redirectIo->GetRetStatus();
		io->AddRetLength(redirectIo->GetRetLength());
		redirectIo->DelObjIo(redirectIo);
		return ret;
	}

	FiErr InitRedirectWriteFile(BaseObjIo *io)
	{
		FiErr ret;
		char tp = 0;
		s32 redirectNode = -1;
		vector<s32> exceptNodes;
		do
		{
			redirectNode = NodeSpaceCollecter::GetNodeSpaceCollecter().AllocOneNode(io->IsMetaType(),io->GetNodeInfo(),io->GetNodeNum(),&exceptNodes);
			io->SetDstNode(redirectNode);
			//make sure no redirect on alloc node,if read suc realloc node id
			ret = SendIo(io,&tp,1,io->IsMetaType()?OsdCfg::GetOsdCfg().GetMetaSliceSize()-1:OsdCfg::GetOsdCfg().GetDataSliceSize()-1,RedirectRead);
			exceptNodes.push_back(redirectNode);
			if (exceptNodes.size()+io->GetNodeNum() == OsdCfg::GetOsdCfg().GetNodeTotal())
			{
				ret = FErr(MDisk_fwriteFailed);
				OSD_LOG_ERR("===InitRedirectWriteFile=== failed,no space alloc");
				return ret;
			}
		}
		while (ret.IsSuccess() && io->GetRetLength() == 1);

		//make sure one file piece must redirect once
		ret = SendIo(io,&tp,1,io->IsMetaType()?OsdCfg::GetOsdCfg().GetMetaSliceSize()-1:OsdCfg::GetOsdCfg().GetDataSliceSize()-1,RedirectWrite);
		if (ret.IsErr())
		{
			OSD_LOG_ERR("===InitRedirectWriteFile=== SendRedirectIo write failed errInfo[%s]",ret.StrFiText().c_str());
			return ret;
		}
	}

	FiErr RedirectReadWrite(BaseObjIo *io,string &path,char *buf,u32 length,u64 offset,u32 fileSize,bool isWrite)
	{
		FiErr ret;

		ObjIoType ioType = isWrite?RedirectWrite:RedirectRead;

		SpaceNode snA, snB;
		snA.start = offset;
		snA.length = length;
		snB.start = 0;
		snB.length = fileSize;
		SpaceAnalyse sa(snA, snB);

		if (sa.IsHaveSameSpace())
		{
			if (isWrite) 
			{
				ret = ficsLocIF::GetFicsLocIF().GetFileIF(path)->WriteData(buf,sa.GetSameSapce()->length,sa.GetSameSapce()->start);
			}
			else
			{
				u32 length = sa.GetSameSapce()->length;
				ret = ficsLocIF::GetFicsLocIF().GetFileIF(path)->ReadData(buf,length,sa.GetSameSapce()->start);
				io->AddRetLength(length);
			}
			if (ret.IsErr()) 
			{
				NodeStatusObserver::GetNodeStatusObserver().NodeErrSet(OsdCfg::GetOsdCfg().GetLocNodeID());
				return ret;
			}

			if (sa.GetADiff()[0].length != 0) 
			{
				ret = SendIo(io,buf+sa.GetSameSapce()->length,sa.GetADiff()[0].length,0,ioType);
			}
		}
		else
		{
			ret = SendIo(io,buf,length,offset-fileSize,ioType);
		}
		return ret;
	}

	FiErr Write(BaseObjIo *io)
	{
		FiErr ret;
		char *buf = NULL;
		u32 length = 0;
		u64 offset = 0;
		s32 remoteNodeID = -1;

		if(io->IsMetaType())
		{
			RELEASE_ASSERT(OsdCfg::GetOsdCfg().IsInstallMetaSvr());
		}

		IoFileLog *irc = io->IsMetaType()?metaIoRedirectLog:dataIoRedirectLog;

		for (u32 i = 0;i<io->GetObjIoNum();++i)
		{
			buf = io->GetObjIoAry()[i].buf;
			length = io->GetObjIoAry()[i].length;
			offset = io->GetObjIoAry()[i].offset;

			if (irc->IsFileInIoFileLog(io->GetFileID(),io->GetObjIoAry()[i].order,remoteNodeID))
			{
				RELEASE_ASSERT(io->GetObjIoType() != NetWrite);
				string fpStr = io->GetStorPath(io->GetObjIoAry()[i].order);
				u32 fileSize = ficsLocIF::GetFicsLocIF().FileSize(fpStr);

				io->SetDstNode(remoteNodeID);
				ret = RedirectReadWrite(io,fpStr,buf,length,offset,fileSize,true);

			} 
			else
			{
				string fpStr = io->GetStorPath(io->GetObjIoAry()[i].order);
				if (length > NodeSpaceCollecter::GetNodeSpaceCollecter().LocNodeSpaceLeft(io->IsMetaType()) && 	\
					offset + length > ficsLocIF::GetFicsLocIF().FileSize(fpStr))
				{
					ret = FErr(MDisk_fwriteFailed);	//??? close redirect io
					return ret;
					RELEASE_ASSERT(io->GetObjIoType() != NetWrite);
					u32 fileSize = ficsLocIF::GetFicsLocIF().FileSize(fpStr);
					ret = InitRedirectWriteFile(io);
					if (ret.IsErr())
					{
						break;
					}

					ret = irc->AddFileLogElement(io->GetFileID(),io->GetObjIoAry()[i].order,io->GetDstNode(),WRITETYPE,io->GetMainNode(),io->GetBakNode());
					if (ret.IsSuccess())
					{
						ret = RedirectReadWrite(io,fpStr,buf,length,offset,fileSize,true);
						if (ret.IsSuccess())
						{
//  						ficsLocIF::GetFicsLocIF().ChangeToPoint(fpStr);
							continue;
						}
						else
						{
							IoFileLogElement d;
							d.fileID = io->GetFileID();
							d.order = io->GetObjIoAry()[i].order;
							d.offset = WRITETYPE;
							d.nodeID = io->GetDstNode();
							irc->DelFileLogElement(d);
							break;
						}
					}
					else
					{
						break;
					}
				}

				ret = ficsLocIF::GetFicsLocIF().GetFileIF(fpStr)->WriteData(buf,length,offset);
				if (ret.IsErr())
				{
					NodeStatusObserver::GetNodeStatusObserver().NodeErrSet(OsdCfg::GetOsdCfg().GetLocNodeID());
				}
			}

			if (ret.IsErr())
			{
				break;
			}
		}
		return ret;
	}
					
	FiErr Read(BaseObjIo *io)
	{
		FiErr ret;
		char *buf = NULL;
		u32 length = 0;
		u64 offset = 0;
		s32 remoteNodeID = -1;

		if(io->IsMetaType())
		{
			RELEASE_ASSERT(OsdCfg::GetOsdCfg().IsInstallMetaSvr());
		}

		IoFileLog *irc = io->IsMetaType()?metaIoRedirectLog:dataIoRedirectLog;

		for (u32 i = 0;i<io->GetObjIoNum();++i)
		{
			buf = io->GetObjIoAry()[i].buf;
			length = io->GetObjIoAry()[i].length;
			offset = io->GetObjIoAry()[i].offset;

			if (irc->IsFileInIoFileLog(io->GetFileID(),io->GetObjIoAry()[i].order,remoteNodeID))
			{
				RELEASE_ASSERT(io->GetObjIoType() != NetRead);
				string fpStr = io->GetStorPath(io->GetObjIoAry()[i].order);
//				u32 fileSize = ficsLocIF::GetFicsLocIF().IsExist(fpStr)?ficsLocIF::GetFicsLocIF().FileSize(fpStr):0;
				u32 fileSize = ficsLocIF::GetFicsLocIF().FileSize(fpStr);

				io->SetDstNode(remoteNodeID);
				ret = RedirectReadWrite(io,fpStr,buf,length,offset,fileSize,false);

			} else
			{
				string fpStr = io->GetStorPath(io->GetObjIoAry()[i].order);
				ret = ficsLocIF::GetFicsLocIF().GetFileIF(fpStr)->ReadData(buf,length,offset);
				if (ret.IsErr())
				{
					NodeStatusObserver::GetNodeStatusObserver().NodeErrSet(OsdCfg::GetOsdCfg().GetLocNodeID());
				}
				io->AddRetLength(length);
			}

			if (ret.IsErr())
			{
				break;
			}
		}
		return ret;
	}

	FiErr Delete(BaseObjIo *io,s32* nodeID,s32 nodeNum)
	{
		IoCmdLogElement cmd(io->GetFileID(),nodeID,nodeNum);
		return cmdLog->AddCmdLogElement(cmd);
	}

	bool DoDelete(IoCmdLogElement& d,bool isMeta = true)
	{
		IoFileLog *redirectLog = (isMeta)?metaIoRedirectLog:dataIoRedirectLog;
		string pathStr = GetStorDirPath(isMeta,d.fileID);
		BaseObjIo *redirectIo = NULL;
		IoFileLogElement e;
		FiEvent evt;
		FiErr ret;

		if (!ficsLocIF::GetFicsLocIF().RemoveDir(pathStr))
		{
			return false;
		}

		if (-1 != d.remoteNode)
		{
			//del bak node file
			e.fileID = d.fileID;
			e.nodeID = d.remoteNode;
			redirectIo = ObjIoFactory::GetObjIoFactory().CreateObj(RedirectDel);
			redirectIo->InitObjIo((char *)&e,NULL,d.cmd);
			NetWapper::GetNetWapper().PushNetIo(redirectIo,evt);
			evt.Wait();
			ret = redirectIo->GetRetStatus();
			redirectIo->DelObjIo(redirectIo);
			if(ret.IsErr())
			{
				return false;
			}
			if(redirectLog->DelFileLogElement(e).IsErr())
			{
				return false;
			}
		}

		//del redirect node file
		while(redirectLog->GetFileLogElementWithFileID(e,d.fileID))
		{
			redirectIo = ObjIoFactory::GetObjIoFactory().CreateObj(RedirectDel);
			redirectIo->InitObjIo((char *)&e,NULL,d.cmd);
			NetWapper::GetNetWapper().PushNetIo(redirectIo,evt);
			evt.Wait();
			ret = redirectIo->GetRetStatus();
			redirectIo->DelObjIo(redirectIo);
			if(ret.IsErr())
			{
				return false;
			}
			if(redirectLog->DelFileLogElement(e).IsErr())
			{
				return false;
			}
		}

		return true;
	}

	struct RedirectTruncateOp
	{
		RedirectTruncateOp(BaseObjIo *ioPar):io(ioPar){}
		inline void operator()(s32 remoteNodeID)
		{
			io->SetDstNode(remoteNodeID);
			LocWapper::GetLocWapper().SendIo(io,NULL,0,io->GetOffset(),RedirectTruncate);
		}

	private:
		BaseObjIo *io;
	};

	FiErr Truncate(BaseObjIo *io)
	{
//		if(io->IsMetaType())
//		{
//			RELEASE_ASSERT(OsdCfg::GetOsdCfg().IsInstallMetaSvr());
//		}
//		IoCmdLogElement cmd(io->GetFileID(),io->GetRemoteNode(),io->IsMetaType()?MetaTruncate:DataTruncate,io->GetOffset());
//		return cmdLog->AddCmdLogElement(cmd);

		FiErr ret;

		if(io->IsMetaType())
		{
			RELEASE_ASSERT(OsdCfg::GetOsdCfg().IsInstallMetaSvr());
		}

		u32 fileSliceSize = (io->IsMetaType())?OsdCfg::GetOsdCfg().GetMetaSliceSize():OsdCfg::GetOsdCfg().GetDataSliceSize();
		u32 truncateNo = (u32)(io->GetOffset()/(u64)fileSliceSize);
		u32 truncateSize = (u32)(io->GetOffset()%(u64)fileSliceSize);

		IoFileLog *irc = io->IsMetaType()?metaIoRedirectLog:dataIoRedirectLog;
		vector<IoFileLogElement> ifleAry;
		irc->GetFileLogAryWithFileID(ifleAry,io->GetFileID());

		if (io->GetObjIoType() == NetTruncate)
		{
			RELEASE_ASSERT(ifleAry.empty());
		}

		if (!ifleAry.empty())
		{
			vector<u32> nodeIDAry;
			for (vector<IoFileLogElement>::iterator itTp = ifleAry.begin();itTp != ifleAry.end();++itTp)
			{
				if (itTp->order > truncateNo)
				{
					nodeIDAry.push_back(itTp->nodeID);
				}
				else if(itTp->order == truncateNo && 0 != truncateSize)
				{
					nodeIDAry.push_back(itTp->nodeID);
				}
			}
			sort(nodeIDAry.begin(),nodeIDAry.end());
			nodeIDAry.erase(unique(nodeIDAry.begin(),nodeIDAry.end()));
			for_each(nodeIDAry.begin(),nodeIDAry.end(),RedirectTruncateOp(io));
		}
		
		string pathStr = GetStorDirPath(io->IsMetaType(),io->GetFileID());
		ret = ficsLocIF::GetFicsLocIF().TruncateDir(pathStr,truncateNo,truncateSize);
		if (ret.IsErr())
		{
			NodeStatusObserver::GetNodeStatusObserver().NodeErrSet(OsdCfg::GetOsdCfg().GetLocNodeID());
		}
		return ret;
	}

//	bool DoTruncate(IoCmdLogElement& e)
//	{
//
//	}

	inline IoFileLog* GetMetaIoRecoverLog(){return metaIoRecoverLog;}
	inline IoFileLog* GetDataIoRecoverLog(){return dataIoRecoverLog;}

	inline IoFileLog* GetJournalIoConsistencyLog(){return journalIoConsistencyLog;}
	inline IoFileLog* GetDataIoConsistencyLog(){return dataIoConsistencyLog;}

private:
	LocWapper()
	{
		if (OsdCfg::GetOsdCfg().IsInstallMetaSvr())
		{
			metaIoRedirectLog = new IoFileLog(OsdCfg::GetOsdCfg().GetMetaRootPath()+string("MetaIoRedirectLog"));
			metaIoRecoverLog = new IoFileLog(OsdCfg::GetOsdCfg().GetMetaRootPath()+string("MetaIoRecoverLog"));

			dataIoRedirectLog = new IoFileLog(OsdCfg::GetOsdCfg().GetMetaRootPath()+string("DataIoRedirectLog"));
			dataIoRecoverLog = new IoFileLog(OsdCfg::GetOsdCfg().GetMetaRootPath()+string("DataIoRecoverLog"));

			journalIoConsistencyLog = new IoFileLog(OsdCfg::GetOsdCfg().GetMetaRootPath()+string("JournalIoConsistencyLog"));
			dataIoConsistencyLog = new IoFileLog(OsdCfg::GetOsdCfg().GetMetaRootPath()+string("DataIoConsistencyLog"));

			cmdLog = new IoCmdLog(OsdCfg::GetOsdCfg().GetMetaRootPath()+string("CmdLog"));
		}
		else
		{

			dataIoRedirectLog = new IoFileLog(OsdCfg::GetOsdCfg().GetDataRootPath()+string("DataIoRedirectLog"));
			dataIoRecoverLog = new IoFileLog(OsdCfg::GetOsdCfg().GetDataRootPath()+string("DataIoRecoverLog"));

			dataIoConsistencyLog = new IoFileLog(OsdCfg::GetOsdCfg().GetDataRootPath()+string("DataIoConsistencyLog"));

			cmdLog = new IoCmdLog(OsdCfg::GetOsdCfg().GetDataRootPath()+string("CmdLog"));
		}
	}
	~LocWapper(){}

//	bool IsPointer(BaseObjIo *io,u32 fileNo)
//	{
//		string fpStr = io->GetPointerStorPath(fileNo);
//		return ficsLocIF::GetFicsLocIF().IsExist(fpStr);
//	}

	IoFileLog *metaIoRedirectLog;
	IoFileLog *dataIoRedirectLog;

	IoFileLog *metaIoRecoverLog;
	IoFileLog *dataIoRecoverLog;

	IoFileLog *journalIoConsistencyLog;
	IoFileLog *dataIoConsistencyLog;

	IoCmdLog *cmdLog;
};


void SessionWatcher(int state);
void NodeChangeWatcher(void *par);

//void OsdNotifyStatusCallBack(OsdLayerStatus s);

//void InitObjStream();
//void InitNetWapper();
//void InitLocWapper();
void InitNodeStatus();

#endif

