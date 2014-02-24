#include "netIpBook.h"
#include "../include_com/Markup.h"

MCD_CSTR ServerIpBook::s_defaultCfgName = _T("../config/FicsConfig.xml");

bool isLocalNodeIp( vector<u32>& ip )
{
        vector<DWORD> localIp;
        bool bLocal = false;
        GetLocalIP(localIp);
        for (int i = 0; i < ip.size(); i++){
                for (int j = 0; j < localIp.size(); j++){
                        if (localIp[j] == ip[i]){
                                bLocal = true;
                                break;
                        }
                }
        }

        return bLocal;
}


void ServerIpBook::getAllAddress( vector<u32> &myListen, vector< pair<u32, u32> > &peer )
{
        vector<u32> internalIpLocal, internalIpOther,  externalIpLocal, externalIpOther;
        m_cfg.getIpSummary(internalIpLocal, internalIpOther,  externalIpLocal, externalIpOther);

        for (int i = 0; i < internalIpLocal.size(); i++){
                myListen.push_back(internalIpLocal[i]);

                for (int j = 0; j < internalIpOther.size(); j++){
                        peer.push_back(make_pair(internalIpLocal[i], internalIpOther[j]));
                }

                peer.push_back(make_pair(internalIpLocal[i], internalIpLocal[i]));
                
        }

        for (int i = 0; i < externalIpLocal.size(); i++){
                myListen.push_back(externalIpLocal[i]);
        }

}

void ServerIpBook::getAddressRelation( vector<u32> relationAry[], u32 maxRelations, u32 &realRelations )
{
        map<u32, u32> nodes;
        m_cfg.getNodeSeq(nodes);
        for (map<u32, u32>::iterator it = nodes.begin(); it != nodes.end(); it++){
                u32 curnode = it->first;
                vector<u32> ipTmp;
                m_cfg.getNodeInternalIp(curnode, ipTmp);
                relationAry[curnode] = ipTmp;
        }
        realRelations = m_cfg.getNodeNum();
}

ServerIpBook::ServerIpBook() : m_cfg(s_defaultCfgName)
{

}

void ServerIpBook::setDefaultCfgPath( MCD_CSTR &cfgName )
{
        s_defaultCfgName = cfgName;
}



FicsCfgParser::FicsCfgParser( MCD_CSTR cfgName )
{
        m_nodeNum = 0;

        CMarkup xml;
        tstring strTmp;
        const u32 invalidNodeNo = -1;
        u32 curNode = invalidNodeNo, backupNode = invalidNodeNo;

        bool brTmp = false;

        if (!xml.Load(cfgName)) 
        {
                VTRACE(_T("load ficsconfig xml[%s] failed \n"), cfgName);
                return;
        }

        if (xml.FindElem(_T("/fics/NodeServersInfo"))){
                xml.IntoElem();
                
                while (xml.FindElem(_T("OneNodeServer")))
                {
                        xml.IntoElem();
                        brTmp = xml.FindElem(_T("NodeSeq"));
                        assert(brTmp);
                        strTmp = xml.GetData();
                        curNode = MCD_TTOI(strTmp.c_str());

                        //Ω‚ŒˆInternalIPs
                        brTmp = xml.FindElem(_T("InternalIPs"));
                        assert(brTmp);
                        xml.IntoElem();

                        vector<u32> tmp;
                        tmp.clear();
                        while (xml.FindElem(_T("IP")))
                        {
                                strTmp = xml.GetData();
                                CStdString ipTmp = strTmp.c_str();
                                tmp.push_back(ConvertIPtoDWORD(ipTmp));                               
                                m_ipToNode[ConvertIPtoDWORD(ipTmp)] = curNode;
                        }

                        if (isLocalNodeIp(tmp)){
                                m_internalIpLocal.insert(m_internalIpLocal.end(), tmp.begin(), tmp.end());
                        }else
                        {
                                m_internalIpOther.insert(m_internalIpOther.end(), tmp.begin(), tmp.end());
                        }

                        m_nodeToInternalIp[curNode] = tmp;

                        xml.OutOfElem();

                        //Ω‚ŒˆExternalIps
                        brTmp = xml.FindElem(_T("ExternalIps"));
                        assert(brTmp);
                        xml.IntoElem();

                        tmp.clear();
                        while (xml.FindElem(_T("IP")))
                        {
                                strTmp = xml.GetData();
                                CStdString ipTmp = strTmp.c_str();
                                tmp.push_back(ConvertIPtoDWORD(ipTmp));                               
                                m_ipToNode[ConvertIPtoDWORD(ipTmp)] = curNode;
                        }

                        if (isLocalNodeIp(tmp)){
                                m_externalIpLocal.insert(m_externalIpLocal.end(), tmp.begin(), tmp.end());
                        }else
                        {
                                m_externalIpOther.insert(m_externalIpOther.end(), tmp.begin(), tmp.end());
                        }

                        m_nodeToExternalIp[curNode] = tmp;

                        brTmp = xml.FindElem(_T("MdsBackNodeSeq"));
                        if (brTmp){
                                strTmp = xml.GetData();
                                backupNode = MCD_TTOI(strTmp.c_str());
                        }else{
                                backupNode = invalidNodeNo;
                        }

                        m_nodeToBackup[curNode] = backupNode;

                        xml.OutOfElem();

                        xml.OutOfElem();
                }

        }

		if (xml.FindElem(_T("/fics/nodeStripPar")))
		{
			xml.IntoElem();
			strTmp = xmlNode.GetData();
			m_nodeStripPar = atoi(strTmp);
			xml.OutOfElem();
		} else
		{
			m_nodeStripPar = 2 * 1024 * 1024;
		}
}

FicsCfgParser::~FicsCfgParser()
{

}

void FicsCfgParser::getNodeInternalIp( u32 node, vector<u32> &ip ){
        if (m_nodeToInternalIp.find(node) != m_nodeToInternalIp.end()){
                ip = m_nodeToInternalIp[node];
        }
}

void FicsCfgParser::getNodeExternIp( u32 node, vector<u32> &ip )
{
        if (m_nodeToExternalIp.find(node) != m_nodeToExternalIp.end()){
                ip = m_nodeToExternalIp[node];
        }
}

u32 FicsCfgParser::getNodeNum()
{
        return m_nodeToBackup.size();
}

u32 FicsCfgParser::getNodeNo( u32 ip )
{
        return m_ipToNode[ip];
}

u32 FicsCfgParser::getNodeStripPar()
{
        return m_nodeStripPar;
}

void FicsCfgParser::getIpSummary( vector<u32> &internalIpLocal, vector<u32> &internalIpOther, vector<u32> &externalIpLocal, vector<u32>& externalIpOther )
{
        internalIpLocal = m_internalIpLocal;
        internalIpOther = m_internalIpOther;
        externalIpLocal = m_externalIpLocal;
        externalIpOther = m_externalIpOther;
}

void FicsCfgParser::getNodeSeq( map<u32, u32> &nodeToBackup )
{
        nodeToBackup = m_nodeToBackup;
}

void ClientIpBook::getAllAddress( vector<u32> &myListen, vector< pair<u32, u32> > &peer )
{
        std::vector<MCD_STR> localip;//=CFiContext::GetStringArr(_T("/fiScm"));
		//CFiContext::GetString(_T("/fiScm/LocalIp"));
		localip.push_back(CFiContext::GetString(_T("/fiScm/LocalIp")));
        vector<u32> internalIpLocal;
        vector<u32> internalIpOther;
        vector<u32> externalIpLocal;
        vector<u32> externalIpOther;
        m_cfg.getIpSummary(internalIpLocal,internalIpOther,externalIpLocal,externalIpOther);
        for (std::vector<MCD_STR>::iterator it=localip.begin();
                it!=localip.end();++it)
        {
                for (vector<u32>::iterator looper=externalIpOther.begin();looper!=externalIpOther.end();
                        ++looper)
                {
                        CStdString temp = (tstring)(*it);
                        peer.push_back(make_pair(ConvertIPtoDWORD(temp),*looper));
                }
        }
}

void ClientIpBook::getAddressRelation( vector<u32> relationAry[], u32 maxRelations, u32 &realRelations )
{
        map<u32, u32> nodes;
        m_cfg.getNodeSeq(nodes);
        for (map<u32, u32>::iterator it = nodes.begin(); it != nodes.end(); it++){
                u32 curnode = it->first;
                vector<u32> ipTmp;
                ipTmp.clear();
                m_cfg.getNodeExternIp(curnode, ipTmp);
                relationAry[curnode].insert(relationAry[curnode].end(), ipTmp.begin(), ipTmp.end());

        }
        realRelations = m_cfg.getNodeNum();
}

FicsConnQuery::FicsConnQuery( CTcpIo &net, NetAddressBook* book, MCD_CSTR cfgName ) : m_net(net), m_book(book), m_cfg(cfgName)
{

        
}

FicsConnQuery::~FicsConnQuery()
{

}

void FicsConnQuery::getNodeConnState( set<NodeConnInfo> &connInfo, bool &isFicsNetOk)
{
        const u32 maxRelations = 500;
        vector<u32> relationAry[maxRelations];
        u32 realRelations = 0;
        m_book->getAddressRelation(relationAry, maxRelations, realRelations);

        u32 connNode = 0;
        for (int i = 0; i < realRelations; i++){
                assert(relationAry[i].size() > 0);
                NodeConnInfo info;
                info.node = m_cfg.getNodeNo(relationAry[i][0]);

                info.bConn = m_net.isNodeConnByIp(relationAry[i][0]);
                if (info.bConn){
                        connNode++;
                }
                
                for (int j = 0; j < relationAry[i].size(); j++){
                        u32 ip = relationAry[i][j];
                        info.iptoconn[ ip ] = m_net.isIpConn(ip);
                }
        
                connInfo.insert(info);
        }

        if (connNode >= m_cfg.getNodeNum() - s_maxFailedNode){
                isFicsNetOk = true; 
        }else{
                isFicsNetOk = false;
        }
        
}

namespace FiNetTool{

void SyncStartFiSvrNet( CTcpIo& net, u32 port, bool bSvr, bool bClt)
{
        assert(bSvr || bClt);
        if (bSvr && bClt){
                net.Start(port);
        }else if (bSvr){
                net.StartServer(port);
        }else if (bClt){
                net.StartClient(port);
        }

        FicsConnQuery qry(net, ServerIpBook::GetInstance(), _T("../FicsConfig.xml"));
        while (true)
        {
                set<NodeConnInfo> connInfo;
                bool isFicsNetOk;
                qry.getNodeConnState(connInfo, isFicsNetOk);
                if (!isFicsNetOk){
                        FiSleep(1000);
                        continue;
                }
                break;
        }

}

};
