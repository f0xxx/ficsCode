///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  hunter code ([hunter.f0x] remeber my brothers) email:hunterlhy@163.com /////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __OSDCFG_H__
#define __OSDCFG_H__
#if defined(__linux__) || defined(__linux)
#include "../public/osdBaseType.h"
#include "../public/netIpBook.h"
#include "../FicsFormatterEx/Formmater/FicsFmtNSInfo.h"
#else
#include <string>
#include "OSUtility.h"
#include "../FicsFormatterEx/Formmater/FicsFmtNSInfo.h"
using namespace std;
#endif
struct OsdCfg
{
	static OsdCfg& GetOsdCfg(){static OsdCfg oc;return oc;}
	inline u32 GetVersion(){return version;}
	inline u32 GetDataSliceSize(){return dataSliceSize;}
	inline u32 GetNodeStripPar(){return nodeStripPar;}
	inline u32 GetDataNodeHashNum(){return dataNodeHashNum;}
	inline u32 GetNetTimeOut(){return netTimeOut;}
	inline u32 GetMetaMaxFileNo(){return metaMaxFileNo;}
	inline u32 GetDataMaxFileNo(){return dataMaxFileNo;}
	inline u32 GetMetaSliceSize(){return metaSliceSize;}
	inline u32 GetObjStreamThreadNum(){return objStreamThreadNum;}
	inline u32 GetNetWapperThreadNum(){return netWapperThreadNum;}
	inline s32 GetLocNodeID(){return locNodeID;}
	inline string GetLocNodeIPStr(){return locIP;}
#ifdef OSD_DEBUG
	inline s32 GetLocNodeIDEx(){return locNodeID;}
#else
	inline s32 GetLocNodeIDEx()
	{
		CNodeServersInfo *pNsInfo = CNodeServersInfo::GetInstance();
//		pNsInfo->UpdateInformation();

		return pNsInfo->GetLocalNodeSeq();
	}
#endif
	inline s32 GetNodeTotal(){return nodeTotal;}
	inline u32 GetSpaceRefreshTime(){return spaceRefreshTime;}
	inline u32 GetSpaceRefreshGrainSize(){return spaceRefreshGrainSize;}
	inline string& GetMetaRootPath(){return metaRootPath;}
	inline string& GetDataRootPath(){return dataRootPath;}

	inline s32 GetMetaBakNodeID(){return metaBakNodeID;}

//	inline s32 GetMetaMainNodeID(){return metaMainNodeIDs;}
	inline RddcPolicy GetDataRP(){return dataRP;}

	inline bool IsInstallMetaSvr(){return (isInstMetaSvr == TRUE);}

#ifdef OSD_DEBUG
	inline u32 GetIP(s32 nodeID)
	{
		CStdString ipStr;
		switch (nodeID)
		{
		case 0:
			ipStr = "172.16.149.230";
			return ConvertIPtoDWORD(ipStr);
		case 1:
			ipStr = "172.16.149.231";
			return ConvertIPtoDWORD(ipStr);
		case 2:
			ipStr = "172.16.149.232";
			return ConvertIPtoDWORD(ipStr);
		}
	}

	inline s32 GetMetaBakNodeIDEx(s32 nodeID)
	{
		switch (nodeID)
		{
		case 0:
			return 1;
		case 1:
			return 2;
		case 2:
			return 0;
		}
	}

	void GetNodesInfoFromFmtCfg()
	{
		locNodeID = 2;
		metaBakNodeID = 0;
//		metaMainNodeID = 1;
		nodeTotal = 3;
		metaRootPath = "/meta/";
		dataRootPath = "/data/";
		isInstMetaSvr = TRUE;
		locIP = "172.16.149.232";
	}
#else
	inline u32 GetIP(s32 nodeID)
	{
		vector<string> tp;
		CNodeServersInfo *pNsInfo = CNodeServersInfo::GetInstance();
//		pNsInfo->UpdateInformation();

		pNsInfo->GetNodeInternalIpsBySeq(nodeID, tp);
		RELEASE_ASSERT(tp[0].length() != 0);

		CStdString ipStr(tp[0]);
		return ConvertIPtoDWORD(ipStr);
	}

	inline u32 GetExternIP(s32 nodeID)
	{
		vector<string> tp;
		CNodeServersInfo *pNsInfo = CNodeServersInfo::GetInstance();

		pNsInfo->GetNodeExternalIpsBySeq(nodeID, tp);
		RELEASE_ASSERT(tp[0].length() != 0);

		CStdString ipStr(tp[0]);
		return ConvertIPtoDWORD(ipStr);
	}

	inline s32 GetMetaBakNodeIDEx(s32 nodeID)
	{
		CNodeServersInfo *pNsInfo = CNodeServersInfo::GetInstance();
//		pNsInfo->UpdateInformation();

		return pNsInfo->GetBackNodeSeqBySeq(nodeID);
	}

	void GetNodesInfoFromFmtCfg()
	{
		vector<string> tp;
		CNodeServersInfo *pNsInfo = CNodeServersInfo::GetInstance();
//		pNsInfo->UpdateInformation();

		nodeTotal = pNsInfo->GetNodeNumberOfCluster();
		locNodeID = pNsInfo->GetLocalNodeSeq();
		if (-1 == locNodeID)
		{
			return;
		}
		metaBakNodeID = pNsInfo->GetBackNodeSeqBySeq(locNodeID);
//		metaMainNodeIDs = pNsInfo->GetMainNodeSeqBySeq(locNodeID);
		
		pNsInfo->GetLocalMetaOsdPaths(tp);
		metaRootPath = tp[0];
		if (metaRootPath.length() > 0)
		{
			RELEASE_ASSERT(metaRootPath[metaRootPath.length()-1] == '/');
		}
		pNsInfo->GetLocalDsOsdPaths(tp);
		dataRootPath = tp[0];
		if (dataRootPath.length() > 0)
		{
			RELEASE_ASSERT(dataRootPath[dataRootPath.length()-1] == '/');
		}
		isInstMetaSvr = pNsInfo->IsLocalMdsNode();
		pNsInfo->GetNodeInternalIpsBySeq(locNodeID, tp);
		RELEASE_ASSERT(tp[0].length() != 0)
		locIP = tp[0];
	}
#endif

	string GetZKIP()
	{
		static u32 i = 0;
		i = (zkIPs.size() == i)? 0 : i;
		return zkIPs[i++];
	}

	s32 GetHeartbeat(){return heartbeat;}

private:
	OsdCfg()
	{
//		version = 0;
//
//		metaSliceSize = 8*1024*1024;
//		metaMaxFileNo = 10000000;
//
//		dataSliceSize = 64*1024*1024;
//		dataNodeHashNum = 3;
//		dataRP = FiRAID1;
//		nodeStripPar = 2*1024*1024;
//		dataMaxFileNo = 10000000;
//
//		objStreamThreadNum = 18;
//		netWapperThreadNum = 9;
//		netTimeOut = 6000;
//
//		spaceRefreshTime = 10;
//		spaceRefreshGrainSize = 10*1024*1024;
//
//		heartbeat = 1000;
//
//		zkIPs.push_back("172.16.149.230:2181");
//		zkIPs.push_back("172.16.149.231:2181");
//		zkIPs.push_back("172.16.149.232:2181");

		CFiContext* pCtx = CFiContext::GetInstance();
		version = pCtx->GetNum("/OsdCfg/version");

		metaSliceSize = pCtx->GetNum("/OsdCfg/metaSliceSize");
		metaMaxFileNo = pCtx->GetNum("/OsdCfg/metaMaxFileNo");

		dataSliceSize = pCtx->GetNum("/OsdCfg/dataSliceSize");
		dataNodeHashNum = pCtx->GetNum("/OsdCfg/dataNodeHashNum");
		dataRP = (RddcPolicy)pCtx->GetNum("/OsdCfg/dataRP");
		nodeStripPar = pCtx->GetNum("/OsdCfg/nodeStripPar");
		dataMaxFileNo = pCtx->GetNum("/OsdCfg/dataMaxFileNo");

		objStreamThreadNum = pCtx->GetNum("/OsdCfg/objStreamThreadNum");
		netWapperThreadNum = pCtx->GetNum("/OsdCfg/netWapperThreadNum");
		netTimeOut = pCtx->GetNum("/OsdCfg/netTimeOut");

		spaceRefreshTime = pCtx->GetNum("/OsdCfg/spaceRefreshTime");
		spaceRefreshGrainSize = pCtx->GetNum("/OsdCfg/spaceRefreshGrainSize");

		heartbeat = pCtx->GetNum("/OsdCfg/heartbeat");

		s32 count = pCtx->GetTagCount("/OsdCfg/zkIPs");
		for (s32 i = 0;i<count;++i)
		{
			string ipStr("/OsdCfg/zkIPs/zkServer");
			ipStr += '1' + i;
#if defined(__linux__) || defined(__linux)
			zkIPs.push_back(pCtx->GetString(ipStr.c_str()));
#else
			MCD_STR temp = pCtx->GetString(ipStr.c_str());
			char buf[250]={0};
			UTF16ConvertANSI(temp.GetBuffer(),temp.GetLength()*sizeof(TCHAR),buf,250);
			zkIPs.push_back(std::string(buf));
#endif
		}

		GetNodesInfoFromFmtCfg();

		RELEASE_ASSERT(dataNodeHashNum <= nodeTotal);
	}
	~OsdCfg(){}
	
	u32 version;

	u32 metaSliceSize;		//8*1024*1024
	u32 metaMaxFileNo;		//10000000

	u32 dataSliceSize;		//64*1024*1024
	u32 dataNodeHashNum;	//3(dataNodeHashNum < DATA_NODE_MAX_NUM-2)
	RddcPolicy dataRP;		//FiRAID1
	u32 nodeStripPar;		//2*1024*1024
	u32 dataMaxFileNo;		//10000000

	u32 objStreamThreadNum;		//18
	u32 netWapperThreadNum;		//9
	u32 netTimeOut;				//6000--->6s

	s32 locNodeID;
	s32 metaBakNodeID;
//	vector<s32> metaMainNodeIDs;
	s32 nodeTotal;
//  u32 metaNodesNum;
//  u32 dataNodesNum;

	string locIP;
	BOOL isInstMetaSvr;

	u32 spaceRefreshTime;		//10s
	u32 spaceRefreshGrainSize;	//10*1024*1024

	string metaRootPath;		//"/Meta/"
	string dataRootPath; 		//"/Data/"

	s32 heartbeat;
	vector<string> zkIPs;
};

struct OsdCfgClient
{
	static OsdCfgClient& GetOsdCfgClient(){static GetOsdCfgClient occ;return occ;}
	inline void SetCfgParser(FicsCfgParser *par){fcp = par;}
	inline u32 GetNodeStripPar(){return fcp->getNodeStripPar();}
	inline u32 GetExternIP(s32 nodeID)
	{
		vector<u32> ipAry;
		fcp->getNodeExternIp(nodeID,ipAry);
		return ipAry[0];
	}

private:
	string cfgPath;
	FicsCfgParser *fcp;
	u32 nodeStripPar;
};

void InitOsdCfg();
void InitOsdCfgClient(char *path);

// #else //#if defined(__linux__) || defined(__linux)
// #include <string>
// #include "OSUtility.h"
// #include "../FicsFormatterEx/Formmater/FicsFmtNSInfo.h"
// using namespace std;
// 
// struct OsdCfg
// {
// 	static OsdCfg& GetOsdCfg(){static OsdCfg oc;return oc;}
// 	inline u32 GetVersion(){return version;}
// 	inline u32 GetDataSliceSize(){return dataSliceSize;}
// 	inline u32 GetNodeStripPar(){return nodeStripPar;}
// 	inline u32 GetDataNodeHashNum(){return dataNodeHashNum;}
// 	inline u32 GetNetTimeOut(){return netTimeOut;}
// 	inline u32 GetMetaMaxFileNo(){return metaMaxFileNo;}
// 	inline u32 GetDataMaxFileNo(){return dataMaxFileNo;}
// 	inline u32 GetMetaSliceSize(){return metaSliceSize;}
// 	inline u32 GetObjStreamThreadNum(){return objStreamThreadNum;}
// 	inline u32 GetNetWapperThreadNum(){return netWapperThreadNum;}
// 	inline s32 GetLocNodeID(){return locNodeID;}
// 	inline string GetLocNodeIPStr(){return locIP;}
// 
// 	inline s32 GetNodeTotal(){return nodeTotal;}
// 	inline u32 GetSpaceRefreshTime(){return spaceRefreshTime;}
// 	inline u32 GetSpaceRefreshGrainSize(){return spaceRefreshGrainSize;}
// 	inline string& GetMetaRootPath(){return metaRootPath;}
// 	inline string& GetDataRootPath(){return dataRootPath;}
// 
// 	inline s32 GetMetaBakNodeID(){return metaBakNodeID;}
// 
// 	inline s32 GetMetaMainNodeID(){return metaMainNodeID;}
// 	inline RddcPolicy GetDataRP(){return dataRP;}
// 
// 	inline bool IsInstallMetaSvr(){return (isInstMetaSvr == TRUE);}
// 
// 
// // 	inline u32 GetIP(s32 nodeID)
// // 	{
// // 		std::string  ipStr;
// // 		switch (nodeID)
// // 		{
// // 		case 0:
// // 			ipStr = "172.16.134.15";
// // 			return ConvertIPtoDWORD(ipStr.c_str());
// // 		case 1:
// // 			ipStr = "172.16.133.55";
// // 			return ConvertIPtoDWORD(ipStr.c_str());
// // 		case 2:
// // 			ipStr = "172.16.133.16";
// // 			return ConvertIPtoDWORD(ipStr.c_str());
// // 		}
// // 		return 0;
// // 	}
// 
// // 	inline s32 GetMetaBakNodeIDEx(s32 nodeID)
// // 	{
// // 		switch (nodeID)
// // 		{
// // 		case 0:
// // 			return 1;
// // 		case 1:
// // 			return 2;
// // 		case 2:
// // 			return 0;
// // 		}
// // 	}
// 
// 	inline u32 GetIP(s32 nodeID)
// 	{
// 		vector<string> tp;
// 		CNodeServersInfo *pNsInfo = CNodeServersInfo::GetInstance();
// 		//		pNsInfo->UpdateInformation();
// 
// 		pNsInfo->GetNodeInternalIpsBySeq(nodeID, tp);
// 		RELEASE_ASSERT(tp[0].length() != 0)
// 
// 			CStdString ipStr(tp[0]);
// 		return ConvertIPtoDWORD(ipStr);
// 	}
// 
// 	inline s32 GetMetaBakNodeIDEx(s32 nodeID)
// 	{
// 		CNodeServersInfo *pNsInfo = CNodeServersInfo::GetInstance();
// 		//		pNsInfo->UpdateInformation();
// 
// 		return pNsInfo->GetBackNodeSeqBySeq(nodeID);
// 	}
// 
// 	void GetNodesInfoFromFmtCfg()
// 	{
// 		vector<string> tp;
// 		CNodeServersInfo *pNsInfo = CNodeServersInfo::GetInstance();
// 		//		pNsInfo->UpdateInformation();
// 
// 		locNodeID = pNsInfo->GetLocalNodeSeq();
// 		metaBakNodeID = pNsInfo->GetLocalMdsBakNodeSeq();
// 		metaMainNodeID = pNsInfo->GetLocalMdsMainNodeSeq();
// 		nodeTotal = pNsInfo->GetNodeNumberOfCluster();
// 		pNsInfo->GetLocalMetaOsdPaths(tp);
// 		metaRootPath = tp[0];
// 		if (metaRootPath.length() > 0)
// 		{
// 			RELEASE_ASSERT(metaRootPath[metaRootPath.length()-1] == '/');
// 		}
// 		pNsInfo->GetLocalDsOsdPaths(tp);
// 		dataRootPath = tp[0];
// 		if (dataRootPath.length() > 0)
// 		{
// 			RELEASE_ASSERT(dataRootPath[dataRootPath.length()-1] == '/');
// 		}
// 		isInstMetaSvr = pNsInfo->IsLocalMdsNode();
// 		pNsInfo->GetNodeInternalIpsBySeq(locNodeID, tp);
// 		RELEASE_ASSERT(tp[0].length() != 0)
// 			locIP = tp[0];
// 	}
// 
// // 	void GetNodesInfoFromFmtCfg()
// // 	{
// // 		locNodeID = 2;
// // 		metaBakNodeID = 0;
// // 		metaMainNodeID = 1;
// // 		nodeTotal = 3;
// // 		metaRootPath = "/meta/";
// // 		dataRootPath = "/data/";
// // 		isInstMetaSvr = TRUE;
// // 		locIP = "172.16.133.33";
// // 	}
// 	string GetZKIP()
// 	{
// 		static u32 i = 0;
// 		i = (zkIPs.size() == i)? 0 : i;
// 		return zkIPs[i++];
// 	}
// 
// 	s32 GetHeartbeat(){return heartbeat;}
// 
// private:
// 	OsdCfg()
// 	{
// 		version = 0;
// 
// 		metaSliceSize = 8*1024*1024;
// 		metaMaxFileNo = 10000000;
// 
// 		dataSliceSize = 64*1024*1024;
// 		dataNodeHashNum = 3;
// 		dataRP = FiRAID1;
// 		nodeStripPar = 2*1024*1024;
// 		dataMaxFileNo = 10000000;
// 
// 		objStreamThreadNum = 18;
// 		netWapperThreadNum = 9;
// 		netTimeOut = 6000;
// 
// 		spaceRefreshTime = 10;
// 		spaceRefreshGrainSize = 10*1024*1024;
// 
// 		heartbeat = 1000;
// 
// 		zkIPs.push_back("172.16.134.15:2181");
// 		zkIPs.push_back("172.16.133.55:2181");
// 		zkIPs.push_back("172.16.133.16:2181");
// 		GetNodesInfoFromFmtCfg();
// 
// 		RELEASE_ASSERT(dataNodeHashNum <= nodeTotal);
// 	}
// 	~OsdCfg(){}
// 
// 	u32 version;
// 
// 	u32 metaSliceSize;		//8*1024*1024
// 	u32 metaMaxFileNo;		//10000000
// 
// 	u32 dataSliceSize;		//64*1024*1024
// 	u32 dataNodeHashNum;	//3(dataNodeHashNum < DATA_NODE_MAX_NUM)
// 	RddcPolicy dataRP;		//FiRAID1
// 	u32 nodeStripPar;		//2*1024*1024
// 	u32 dataMaxFileNo;		//10000000
// 
// 	u32 objStreamThreadNum;		//18
// 	u32 netWapperThreadNum;		//9
// 	u32 netTimeOut;				//6000--->6s
// 
// 	s32 locNodeID;
// 	s32 metaBakNodeID;
// 	s32 metaMainNodeID;
// 	s32 nodeTotal;
// 	//  u32 metaNodesNum;
// 	//  u32 dataNodesNum;
// 
// 	string locIP;
// 	BOOL isInstMetaSvr;
// 
// 	u32 spaceRefreshTime;		//10s
// 	u32 spaceRefreshGrainSize;	//10*1024*1024
// 
// 	string metaRootPath;		//"/Meta/"
// 	string dataRootPath; 		//"/Data/"
// 
// 	s32 heartbeat;
// 	vector<string> zkIPs;
// };
// 
// #endif //#endif defined(__linux__) || defined(__linux) 

#endif//#ifndef __OSDCFG_H__
