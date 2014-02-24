///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  hunter code ([hunter.f0x] remeber my brothers) email:hunterlhy@163.com /////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __FICSNETSYN_H__
#define __FICSNETSYN_H__
#include "../public/dataBaseType.h"
#include "zkadapter/zkadapter.h"
#include "zkLock/zoo_lock.h"
#include "FiLog.h"
#include "FiErr.h"
#include "error_def.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define NETSYN_LOG_ERR OSD_LOG_ERR
#define NETSYN_LOG_DBG OSD_LOG_DBG
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern string zkServerIP;
extern unsigned int heartbeat;

extern string liveNodePath;

typedef void (*ZKDATABASEFUC)(void *par);
typedef void (*ZKSESSIONWATCHFUC)(int state);

struct listener : public ZKEventListener 
{
	virtual void eventReceived(const ZKEventSource & source,const ZKWatcherEvent & event);
};

struct evtContext
{
	evtContext(ZKDATABASEFUC fucT = NULL, void *parT = NULL) : fuc(fucT), par(parT){ }
	ZKDATABASEFUC fuc;
	void *par;
}; 
typedef vector<evtContext>::iterator ECITR;

struct evtContextPool
{
	static evtContextPool& GetEvtContextPool(){static evtContextPool ecp;return ecp;}

	evtContext* AllocEC()
	{
		evtContext *ret = NULL;

ALLOCECBEGIN:
		l.Lock();
		if (ecPool.empty())
		{
			l.Unlock();
			e.Wait();
			goto ALLOCECBEGIN;
		}

		ret = *(ecPool.end()-1);
		ecPool.erase((ecPool.end()-1));
		if (ecPool.empty())
		{
			e.Reset();
		}
		l.Unlock();

		return ret;
	}

	void ReleaseEC(evtContext *ec)
	{
		bool isSetEvt = false;
		l.Lock();
		if (ecPool.empty())
		{
			isSetEvt = true;
		}

		ec->fuc = NULL;
		ec->par = NULL;
		ecPool.push_back(ec);
		l.Unlock();

		if (isSetEvt)
		{
			e.Set();
		}
	}

	void ResetEC()
	{
		evtContext *ec = NULL;
		s32 num = 0;
		l.Lock();
		num = poolLimitSize - ecPool.size();
		for(s32 i = 0;i < num;++i)
		{
			ec = new evtContext();
			RELEASE_ASSERT(ec != NULL);
			ec->fuc = NULL;
			ec->par = NULL;
			ecPool.push_back(ec);
			ecPoolBak.push_back(ec);
		}
		NETSYN_LOG_ERR("===ResetEC=== ecPoolBak size[%d]",ecPoolBak.size());
		l.Unlock();
		e.Set();
	}

private:
	evtContextPool()
	{
		poolLimitSize = 60;
		evtContext *ec = NULL;
		ecPool.reserve(poolLimitSize);
		for (u32 i = 0;i < poolLimitSize;++i)
		{
			ec = new evtContext();
			RELEASE_ASSERT(ec != NULL);
			ec->fuc = NULL;
			ec->par = NULL;
			ecPool.push_back(ec);
			ecPoolBak.push_back(ec);
			ec = NULL;
		}
		e.SetMode(TRUE);
	}

	struct DelEC
	{
		inline void operator()(evtContext *ec){delete ec;}
	};

	~evtContextPool()
	{
		for_each(ecPoolBak.begin(),ecPoolBak.end(),DelEC());
	}

	vector<evtContext *> ecPool;
	vector<evtContext *> ecPoolBak;
	u32 poolLimitSize;
	OsdLock l;
	FiEvent e;
};

struct nodeChangeInfo
{
	nodeChangeInfo(void *parT, bool isDeadT, int nodeIDT) : par(parT), isDead(isDeadT), nodeID(nodeIDT) {RELEASE_ASSERT(nodeID < 99999);}
	void *par;
	bool isDead;
	int nodeID;
}; 

enum NetSynState
{
	NS_CONNECTED = 0,
	NS_DISCONNECTED
};

struct zkDataBase
{
    friend void csNodeStatusNotify(void *par);
	
    struct AdapterStateListener : public ZKEventListener 
	{
		virtual void eventReceived(const ZKEventSource & source,const ZKWatcherEvent & event)
        {
            if (event.getType() == ZOO_SESSION_EVENT) 
			{
                if (event.getState() == ZOO_CONNECTED_STATE) 
				{
					NETSYN_LOG_ERR("getState ZOO_CONNECTED_STATE");
					vector<ZKSESSIONWATCHFUC>& sna = zkDataBase::GetZkDataBase().GetSessionNotifiedAry();
					for (vector<ZKSESSIONWATCHFUC>::iterator it= sna.begin();it != sna.end();++it)
					{
						(*it)(NS_CONNECTED);
					}
                } 
				else if (event.getState() == ZOO_CONNECTING_STATE) 
				{
					NETSYN_LOG_ERR("getState ZOO_CONNECTING_STATE");
                } 
				else if (event.getState() == ZOO_EXPIRED_SESSION_STATE) 
				{
					NETSYN_LOG_ERR("getState ZOO_EXPIRED_SESSION_STATE");
					vector<ZKSESSIONWATCHFUC>& sna = zkDataBase::GetZkDataBase().GetSessionNotifiedAry();
					for (vector<ZKSESSIONWATCHFUC>::iterator it= sna.begin();it != sna.end();++it)
					{
						(*it)(NS_DISCONNECTED);
					}
					zkDataBase::GetZkDataBase().ReConnect();
                }
				else
				{
					RELEASE_ASSERT_FASLE;
				}
			}
		}
    };

	/**
	 * \获取zkDataBase单件实例 
	 */
	static zkDataBase& GetZkDataBase() {static zkDataBase zkdb(zkServerIP); return zkdb;}

	/**
	 * \获取zookeeper句柄 
	 */
	zhandle_t* GetZkHandle();


	/**
	 * \重新连接到zookeeper服务器 
	 *  
	 * @return true表示成功，false表示失败  
	 */
	FiErr ReConnect();
	/**
	 * \从zookeeper服务器断开
	 */
	void DisConnect();

	/**
	 * \创建一个存储少量string数据的树节点（数据大小不能超过1MB）
	 *  
	 * @param path 树节点路径名，必须以'/'开头，不能用'/'结尾， 
	 *  		  取名不使用特殊字符，尽量使用自己的独立路径前缀
	 *  		  ex：/wl/data1
	 * @param value 树节点中存储的数据
	 * @param flags 树节点中存储的数据类型
	 *  			默认为永久类型，ZOO_EPHEMERAL为临时类型（临时类型的数据在程序结束或飞掉后会自动被
	 *  			删除掉）
	 * @param createAncestors 是否自动创建不存在的父节点
	 *  
	 * @return true表示成功，false表示失败  
	 */
	FiErr CreateNode(const string &path,const char *value = "",int valueLength = 0,int flags = 0,bool createAncestors = true);

	/**
	 * \创建一个存储少量string数据的树节点（数据大小不能超过1MB），创建出的节点会以参数path为前缀自动加上 
	 * 一个全局增大的数字id后缀，ex：创建path为：/wl/data1的数据会生成一个/wl/data1_00000032的节点 
	 *  
	 * @param path 树节点路径名，必须以'/'开头，不能用'/'结尾， 
	 *  		  取名不使用特殊字符，尽量使用自己的独立路径前缀
	 *  		  ex：/wl/data1
	 * @param value 树节点中存储的数据
	 * @param flags 树节点中存储的数据类型
	 *  			默认为永久类型，ZOO_EPHEMERAL为临时类型（临时类型的数据在程序结束或飞掉后会自动被
	 *  			删除掉）
	 * @param createAncestors 是否自动创建不存在的父节点
	 *  
	 * @return 返回后缀数字id表示成功，返回-1表示失败  
	 */
	FiErr CreateSequenceNode(const string &path,int64_t &retId,int flags = 0,const char *value = "",int valueLength = 0,bool createAncestors = true);
	FiErr CreateSequenceNodeEx(const string &path,string &retPath,int flags = 0,const char *value = "",int valueLength = 0,bool createAncestors = true);

	/**
	 * \删除一个树节点
	 *  
	 * @param path 树节点路径名，必须以'/'开头，不能用'/'结尾， 
	 *  		  取名不使用特殊字符，尽量使用自己的独立路径前缀
	 *  		  ex：/wl/data1
	 * @param recursive 是否自动递归删除所有子节点
	 * @param version 使用默认值
	 *  
	 * @return true表示成功，false表示失败  
	 */
	FiErr DeleteNode(const string &path, bool recursive = false, int version = -1);


	/**
	 * \判断一个树节点是否已经存在
	 *  
	 * @param path 树节点路径名，必须以'/'开头，不能用'/'结尾， 
	 *  		  取名不使用特殊字符，尽量使用自己的独立路径前缀
	 *  		  ex：/wl/data1
	 * @param callBackFuc 监控回调函数，在一个节点被删除的时候被调用 
	 * 					  注：此监控回调函数一次调用之后失效,同一路径只支持一个观察函数 
	 * @param callBackFucPar 监控回调函数参数
	 *  
	 * @return true表示存在，false表示不存在
	 */
	FiErr NodeExists(const string &path,ZKDATABASEFUC callBackFuc = NULL,void *callBackFucPar = NULL,Stat *stat = NULL);
//  FiErr NodeExistsEx(const string &path,watcher_fn callBackFuc = NULL,void *callBackFucPar = NULL,Stat *stat = NULL);
	//支持同一路径多个观察函数
//	FiErr NodeExistsMutiWatch(const string &path,listener *ltn = NULL,Stat *stat = NULL);

	/**
	 * \获取一个父节点的下一层子节点
	 *  
	 * @param children 获取的子节点
	 * @param path 树节点路径名，必须以'/'开头，不能用'/'结尾， 
	 *  		  取名不使用特殊字符，尽量使用自己的独立路径前缀
	 *  		  ex：/wl/data1
	 * @param callBackFuc 监控回调函数 
	 *  				  在子节点被删除或添加的时候被调用
	 *  				  注：此监控回调函数一次调用之后失效,同一路径只支持一个观察函数
	 * @param callBackFucPar 监控回调函数参数
	 *  
	 * @return true表示成功，false表示失败
	 */
	FiErr GetNodeChildren(vector<string> &children,const string &path,ZKDATABASEFUC callBackFuc = NULL,void *callBackFucPar = NULL);
	FiErr GetNodeChildrenEx(String_vector *children,const string &path,ZKDATABASEFUC callBackFuc = NULL,void *callBackFucPar = NULL);
	//支持同一路径多个观察函数
//	FiErr GetNodeChildrenMutiWatch(vector<string> &children,const string &path,listener *ltn = NULL);

	/**
	 * \获取一个树节点的数据
	 *  
	 * @param data 获取的数据
	 * @param path 树节点路径名，必须以'/'开头，不能用'/'结尾， 
	 *  		  取名不使用特殊字符，尽量使用自己的独立路径前缀
	 *  		  ex：/wl/data1
	 * @param callBackFuc 监控回调函数 
	 *  				  在子节点数据变化的时候被调用
	 *  				  注：此监控回调函数一次调用之后失效,同一路径只支持一个观察函数
	 * @param callBackFucPar 监控回调函数参数
	 * @param stat 使用默认值
	 *  
	 * @return true表示成功，false表示失败
	 */
    FiErr GetNodeData(char *value ,int valueLength ,const string &path,ZKDATABASEFUC callBackFuc = NULL,void *callBackFucPar = NULL,Stat *stat = NULL);
    FiErr GetNodeDataEx(char *value ,int &valueLength ,const string &path,ZKDATABASEFUC callBackFuc = NULL,void *callBackFucPar = NULL,Stat *stat = NULL);
	//支持同一路径多个观察函数
//    FiErr GetNodeDataMutiWatch(char *value ,int &valueLength ,const string &path,listener *ltn = NULL,Stat *stat = NULL);

	/**
	 * \设置一个树节点的数据
	 *  
	 * @param path 树节点路径名，必须以'/'开头，不能用'/'结尾， 
	 *  		  取名不使用特殊字符，尽量使用自己的独立路径前缀
	 *  		  ex：/wl/data1
	 * @param value 设置树节点的数据
	 * @param version 使用默认值
	 *  
	 * @return true表示成功，false表示失败
	 */
	FiErr SetNodeData(const string &path, const char *value ,int valueLength, int version = -1);

	/**
	 * \设置监控函数观察集群中是否有节点添加或者删除
	 *  
	 * @param fuc 监控函数
	 * @param par 监控函数参数 
	 *  
	 * @return true表示成功，false表示失败
	 */
	void AddCSNodeWatch(ZKDATABASEFUC fuc,void *par = NULL);

	void AddSessionWatch(ZKSESSIONWATCHFUC fuc);

	bool IsLocNodeDead();

	/**
	 * \获取集群中正常工作的节点
	 *   
	 * @return 节点集合
	 */
	vector<string>& GetLiveNodeAry(){return liveNodeAry;}
	u32 GetLiveNodePathPreLen(){return liveNodePath.length()+1;}
	OsdLock& GetLiveNodeAryLock(){return liveNodeAryLock;}

	/**
	 * \useless
	 */
	inline vector<evtContext>& GetNotifiedAry(){return notifiedAry;}
	inline vector<ZKSESSIONWATCHFUC>& GetSessionNotifiedAry(){return sessionNotifiedAry;}
	string& GetLocNodeIPStr(){return locIPStr;}

	/**
	 * \useless
	 */
	bool MarkMeAlive();

	inline void setReconnectStatus(bool par){zka->setReconnectStatus(par);}
	inline string& GetLocNodeIDStr(){return locNodeIDStr;}

private:
	zkDataBase(string &zkServerIP);
	~zkDataBase();

private:
	ZooKeeperAdapter *zka;
	ZooKeeperConfig config;

	AdapterStateListener asl;
	vector<string> liveNodeAry;
	OsdLock liveNodeAryLock;

	vector<evtContext> notifiedAry;
	vector<ZKSESSIONWATCHFUC> sessionNotifiedAry;

	listener ltn;

	string locNodeIDStr;
	string locIPStr;
	int err;
};

struct zkLock
{
	/**
	 * \分布式锁的构造函数，可以在相关zookeeper集群中对不同节点进行同步
	 * 
	 * @param path 锁的名字，必须以'/'开头，不能用'/'结尾， 
	 *  		   取名不使用特殊字符，尽量使用自己的独立路径前缀
	 * 			   ex：/wl/lock1 
	 * @param zkHandle 使用默认参数 
	 * @param completion 回调函数，使用默认参数 
	 * @param completionData 回调函数参数，使用默认参数 
	 */
	zkLock(char *path, zkr_lock_completion cpt = NULL, void *cptd = NULL);
	~zkLock(); 

	/**
	 * \获取分布式锁
	 */
	void GetZKLock();
	bool TryGetZKLock();
	/**
	 * \释放分布式锁
	 */
	void ReleaseZKLock();
private:
	zkr_lock_mutex_t mutex;
	string lockPath;
	zkr_lock_completion completion;
	void *completionData;
	static u32 cnt;
	static OsdLock l;
};

struct NetSynInitHelper
{
	NetSynInitHelper(string &zkServerIPT,unsigned int heartbeatT);
	~NetSynInitHelper(){}
	static NetSynInitHelper& GetNetSynInitHelper(string &zkServerIPT,unsigned int heartbeatT){static NetSynInitHelper nsih(zkServerIPT,heartbeatT);return nsih;}
};

/**
 * \fics网络一致性初始化接口，在主初始化函数中单线程同步调用
 * 
 * @param zkServerIPT 连接的zookeeper服务器ip端口
 * 					  ex：zkServerIP("127.0.0.1:2183")
 * @param heartbeatT 心跳时间，单位毫秒 
 *  				 ex：如果心跳为1s，那么当某个节点崩溃2s之内其他节点会收到通知
 *  
 * @return true表示成功，false表示失败 
 */
void InitNetSyn(); 

#endif

