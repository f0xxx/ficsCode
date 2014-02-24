///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  hunter code ([hunter.f0x] remeber my brothers) email:hunterlhy@163.com /////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __FIOSDIF_H__
#define __FIOSDIF_H__
#include <vector>
#include "../public/osdBaseType.h"
#include "../FicsFormatterEx/Formmater/IFicsFormatter.h"
using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct MetaIF
{
	MetaIF(){};

	FiErr WriteMeta(ObjInfoHandle &obj, void *buf, u32 &len, u64 offset);
	FiErr ReadMeta(ObjInfoHandle &obj, void *buf, u32 &len, u64 offset);
	FiErr ScanningReadMeta(ObjInfoHandle &obj, void *buf, u32 &len, u64 offset);

	FiErr AllocMetalObj(ObjInfoHandle &obj,bool isLocalAlloc = false,RddcPolicy rp = FiDefault);
	FiErr AllocMetalObjEx(ObjInfoHandle &obj,s32 mainNode,RddcPolicy rp = FiDefault);
	FiErr DelMeta(ObjInfoHandle &obj);
	
	FiErr TruncateMeta(ObjInfoHandle &obj, u64 offset, u64 oriSize = -1);

private:
};

struct DataIF
{
	DataIF(){};

	FiErr WriteData(ObjInfoHandle &obj, void *buf, u32 &len, u64 offset);
	FiErr ReadData(ObjInfoHandle &obj, void *buf, u32 &len, u64 offset);

//	FiErr AllocDataObj(ObjInfoHandle &obj){};
//	FiErr DelData(ObjInfoHandle &obj){};

	FiErr TruncateData(ObjInfoHandle &obj, u64 offset, u64 oriSize = -1);
	FiErr DistributeTruncateData(char *ioPar);

private:
};

struct JournalIF
{
	JournalIF(){};

	FiErr WriteJournal(ObjInfoHandle &obj, void *buf, u32 &len, u64 offset);
	FiErr ReadJournal(ObjInfoHandle &obj, void *buf, u32 &len, u64 offset);
	FiErr ScanningReadJournal(ObjInfoHandle &obj, void *buf, u32 &len, u64 offset);

	//只在格式化的时候调用
	FiErr AllocJournalObj(ObjInfoHandle &obj);
	FiErr DelJournal(ObjInfoHandle &obj);
	
	FiErr TruncateJournal(ObjInfoHandle &obj, u64 len, u64 oriSize = -1);

private:
};

class FiOsdFormat:public IFormatter{
public:
        FiOsdFormat(){}
        virtual FiErr StandardFormat(CNodeServersInfo* pNSsInfo, char* &outBuffer, size_t &bufSize);
private:
};

enum OsdLayerStatus
{
	OsdLayerOK = 0,
	OsdLayerErr
};

typedef void (*NotifyStatusCallBack)(OsdLayerStatus s);

void RegisterNotifyStatusFuc(NotifyStatusCallBack f);

extern bool gIsScanMode;
struct OsdInitHelper
{
	OsdInitHelper(bool isScanMode = false);
	~OsdInitHelper(){}
	void WaitOsdInitDone(){evt.Wait();}
	static OsdInitHelper& GetOsdInitHelper(){static OsdInitHelper oih(gIsScanMode);return oih;}
private:
	FiEvent evt;
};
void InitOsdLayer(bool isScanMode = false,bool isUseFiState = false);
void InitNetWapper();

#endif

