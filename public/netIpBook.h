#pragma  once
#include "../network/tcpio.h"

bool isLocalNodeIp(vector<u32>& ip);

class FicsCfgParser
{
public:
        FicsCfgParser(MCD_CSTR cfgName);
        ~FicsCfgParser();

        void getNodeInternalIp(u32 node, vector<u32> &ip);

        void getNodeExternIp(u32 node, vector<u32> &ip);

        void getNodeSeq(map<u32, u32> &nodeToBackup);

        u32 getNodeNum();

        u32 getNodeNo(u32 ip);

        u32 getNodeStripPar();

        void getIpSummary(vector<u32> &internalIpLocal, vector<u32> &internalIpOther, vector<u32> &externalIpLocal, vector<u32>& externalIpOther);
private:
        map<u32, vector<u32> > m_nodeToExternalIp;
        map<u32, vector<u32> > m_nodeToInternalIp;
        map<u32, u32> m_ipToNode;
        map<u32, u32> m_nodeToBackup;
        u32 m_nodeNum;
        vector<u32> m_internalIpLocal;
        vector<u32> m_internalIpOther;
        vector<u32> m_externalIpLocal; 
        vector<u32> m_externalIpOther;
		u32 m_nodeStripPar;
};


class ServerIpBook : public NetAddressBook, public FiSingleton<ServerIpBook>
{
        friend class FiSingleton<ServerIpBook>;
public:
        virtual ~ServerIpBook(){}

        virtual void getAllAddress(vector<u32> &myListen, vector< pair<u32, u32> > &peer);

        virtual void getAddressRelation( vector<u32> relationAry[], u32 maxRelations, u32 &realRelations);

        static void setDefaultCfgPath(MCD_CSTR &cfgName);
private:
        ServerIpBook();

private:
        FicsCfgParser m_cfg;
        static MCD_CSTR s_defaultCfgName;
};

class ClientIpBook:public NetAddressBook
{
public:
	ClientIpBook(MCD_CSTR path):m_cfg(path){}
	virtual ~ClientIpBook(){}

	virtual void getAllAddress(vector<u32> &myListen, vector< pair<u32, u32> > &peer);

	virtual void getAddressRelation( vector<u32> relationAry[], u32 maxRelations, u32 &realRelations);
private:
	FicsCfgParser m_cfg;
};


struct NodeConnInfo{
        u32 node;
        bool bConn;
        map<u32, bool> iptoconn;

        NodeConnInfo(){
                node = -1;
                bConn = 0;
        }
                
        const bool operator< (const NodeConnInfo& info) const{
                return node < info.node;
        }
};

class FicsConnQuery
{
public:
        FicsConnQuery(CTcpIo &net, NetAddressBook* book, MCD_CSTR cfgName);

        ~FicsConnQuery();

        //实时查询, 调用的次不建议太多
        void getNodeConnState(set<NodeConnInfo> &connInfo, bool &isFicsNetOk);

public:
        static const u32 s_maxFailedNode = 1; 
private:
        CTcpIo& m_net;
        NetAddressBook* m_book;
        FicsCfgParser m_cfg;
};

namespace FiNetTool{
        void SyncStartFiSvrNet(CTcpIo& net, u32 port, bool bSvr = true, bool bClt = true);
};
