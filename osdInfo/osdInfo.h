///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  hunter code ([hunter.f0x] remeber my brothers) email:hunterlhy@163.com /////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef __OSDINFO_H__
#define __OSDINFO_H__
#include "Python.h"
#include "../public/osdTools.h"
#include <vector>
#include <string>
using namespace std;

enum ficsDataType
{
	OsdInfo_Meta = 0,
	OsdInfo_Data,
	OsdInfo_Unkown,
	OsdInfo_Others
};

enum ficsDiskStatus
{
	OsdInfo_Failed = 0,
	OsdInfo_OK
};

struct DiskInfo
{
	string devName;
	string devSize;
	ficsDataType dataType;
	string devType;
	ficsDiskStatus devStatus;
	s64 devSizeDetail;
};

struct PoolInfo
{
	ficsDataType poolType;
	s64 poolUsed;
	s64 poolLeft;
	s32 usedPct;
	string poolMountPoint;
};

struct OsdInfo
{
	void GetDiskInfo(vector<DiskInfo> &diAry);
	bool MakeMetaPool(vector<DiskInfo> &diAry);
	bool MakeDataPool(vector<DiskInfo> &diAry);

	bool RemovePool();

	void GetPoolUsed(vector<PoolInfo> &piAry);
	
	static OsdInfo& GetOsdInfo(){static OsdInfo oif;return oif;}

private:
	OsdInfo()
	{
		Py_Initialize();
		RELEASE_ASSERT(Py_IsInitialized());
		PyRun_SimpleString("import sys");
        PyRun_SimpleString("sys.path.append('./')");

		// load the module
		PyObject *pName = PyString_FromString("fiOsdInfo");
		PyObject *pyMod = PyImport_Import(pName);
		RELEASE_ASSERT(NULL != pyMod);
		// load the function
		pyFunc_GetDiskInfo = PyObject_GetAttrString(pyMod, "GetDiskInfo"); 
		pyFunc_MakePool = PyObject_GetAttrString(pyMod, "MakePool");
		pyFunc_RemovePool = PyObject_GetAttrString(pyMod, "RemovePool");
		pyFunc_GetPoolUsed = PyObject_GetAttrString(pyMod, "GetPoolUsed");
	}

	~OsdInfo()
	{
		Py_Finalize();    
	}

	PyObject *pyFunc_GetDiskInfo;
	PyObject *pyFunc_MakePool;
	PyObject *pyFunc_RemovePool;
	PyObject *pyFunc_GetPoolUsed;
};

#endif

