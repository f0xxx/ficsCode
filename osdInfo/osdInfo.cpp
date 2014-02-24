///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  hunter code ([hunter.f0x] remeber my brothers) email:hunterlhy@163.com /////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "osdInfo.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  usage  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void usage()
{
	vector<DiskInfo> diAry;
	OsdInfo::GetOsdInfo().GetDiskInfo(diAry);

	vector<PoolInfo> piAry;
	OsdInfo::GetOsdInfo().GetPoolUsed(piAry);

	DiskInfo di;
	di.devName = "sdd";
	diAry.push_back(di);
	di.devName = "sde";
	diAry.push_back(di);
	OsdInfo::GetOsdInfo().MakeMetaPool(diAry);

	OsdInfo::GetOsdInfo().RemovePool();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  OsdInfo  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OsdInfo::GetDiskInfo(vector<DiskInfo> &diAry)
{
    // test the function is callable  
    if (pyFunc_GetDiskInfo && PyCallable_Check(pyFunc_GetDiskInfo))  
    {
		string dataType;
		string devStatus;
		char *str1,*str2,*str3,*str4,*str5;
		s64 sll1;
		PyObject *pyRet = PyObject_CallObject(pyFunc_GetDiskInfo,NULL);

		PyObject *pyRetItem = NULL;
		DiskInfo di;
		for (u32 i = 0;i<PyList_Size(pyRet);++i)
		{
			pyRetItem = PyList_GetItem(pyRet,i);
			PyArg_Parse(pyRetItem, "(sssssL)", &str1, &str2 , &str3, &str4, &str5, &sll1);
			OSD_LOG_ERR("===GetDiskInfo=== %s,%s,%s,%s,%s,%lld\n",str1,str2,str3,str4,str5,sll1);
			di.devName = str1;
			di.devSize = str2;
			dataType = str3;
			if (dataType == "Meta")
			{
				di.dataType = OsdInfo_Meta;
			}
			else if (dataType == "Data")
			{
				di.dataType = OsdInfo_Data;
			}
			else if (dataType == "have partition")
			{
				di.dataType = OsdInfo_Others;
			}
			else
			{
				di.dataType = OsdInfo_Unkown;
			}
			di.devType = str4;
			devStatus = str5;
			if (devStatus == "OK")
			{
				di.devStatus = OsdInfo_OK;
			}
			else
			{
				di.devStatus = OsdInfo_Failed;
			}
			di.devSizeDetail = sll1;
			diAry.push_back(di);
		}
    }
}

bool OsdInfo::MakeMetaPool(vector<DiskInfo> &diAry)
{
	s32 sl1;
    if (pyFunc_MakePool && PyCallable_Check(pyFunc_MakePool))  
	{
		string par;
		for (vector<DiskInfo>::iterator it = diAry.begin();it != diAry.end();++it)
		{
			par += "/dev/" + it->devName + " ";
		}

		char *str1,*str2;
		PyObject *pyPar,*pyRet;
		u32 i = 0;
		pyPar = Py_BuildValue("(ss)","meta", par.c_str());
		pyRet = PyObject_CallObject(pyFunc_MakePool,pyPar);
		PyArg_Parse(pyRet, "(lss)", &sl1, &str1 , &str2);
		OSD_LOG_ERR("===MakeMetaPool===ret[%d],retStr[%s],cmd[%s]\n",sl1,str2,str1);
	}

	if (sl1 == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool OsdInfo::MakeDataPool(vector<DiskInfo> &diAry)
{
	s32 sl1;
    if (pyFunc_MakePool && PyCallable_Check(pyFunc_MakePool))  
	{
		string par;
		for (vector<DiskInfo>::iterator it = diAry.begin();it != diAry.end();++it)
		{
			par += "/dev/" + it->devName + " ";
		}

		char *str1,*str2;
		PyObject *pyPar,*pyRet;
		u32 i = 0;
		pyPar = Py_BuildValue("(ss)","data", par.c_str());
		pyRet = PyObject_CallObject(pyFunc_MakePool,pyPar);
		PyArg_Parse(pyRet, "(lss)", &sl1, &str1 , &str2);
		OSD_LOG_ERR("===MakeDataPool===ret[%d],retStr[%s],cmd[%s]\n",sl1,str2,str1);
	}

	if (sl1 == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool OsdInfo::RemovePool()
{
	s32 sl1;
    if (pyFunc_RemovePool && PyCallable_Check(pyFunc_RemovePool))  
	{
		FiEvent evt;
		evt.Wait(1000);
		char *str1;
		PyObject *pyRet = PyObject_CallObject(pyFunc_RemovePool,NULL);
		PyArg_Parse(pyRet, "(ls)", &sl1, &str1);
		OSD_LOG_ERR("===RemovePool===ret[%d],retStr[%s]\n",sl1,str1);
	}

	if (sl1 == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}


void OsdInfo::GetPoolUsed(vector<PoolInfo> &piAry)
{
    if (pyFunc_GetPoolUsed && PyCallable_Check(pyFunc_GetPoolUsed))
	{
		string poolType;
		char *str1,*str2;
		s64 sll1,sll2;
		s32 sl1;
		PyObject *pyRet = PyObject_CallObject(pyFunc_GetPoolUsed,NULL);

		PyObject *pyRetItem = NULL;
		PoolInfo pi;
		for (u32 i = 0;i<PyList_Size(pyRet);++i)
		{
			pyRetItem = PyList_GetItem(pyRet,i);
			PyArg_Parse(pyRetItem, "(sLLls)", &str1, &sll1, &sll2, &sl1, &str2);
			OSD_LOG_ERR("===GetPoolUsed=== %s,%lld,%lld,%d,%s\n",str1,sll1,sll2,sl1,str2);
			poolType = str1;
			if (poolType == "Meta")
			{
				pi.poolType = OsdInfo_Meta;
			}
			else if (poolType == "Data")
			{
				pi.poolType = OsdInfo_Data;
			}
			else
			{
				pi.poolType = OsdInfo_Unkown;
			}
			pi.poolMountPoint = str2;
			pi.poolUsed = sll1;
			pi.poolLeft = sll2;
			pi.usedPct = sl1;
			piAry.push_back(pi);
		}
	}
}

