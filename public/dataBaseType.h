#ifndef __DATABASETYPE_H__
#define __DATABASETYPE_H__
#include "osdBaseType.h"

#define DCS_NET_MOD_ID 1119
#define DCS_NET_PORT 31119

#pragma pack(push ,1)

struct DataCltHead
{
	FiErrData ret;
	u32 len;
	u64 offset;
	u8 isWrite;
	ObjID fileID;
	ObjInfoSequence ois;
};

#pragma pack(pop)

#endif 
