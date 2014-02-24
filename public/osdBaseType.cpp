#include "osdBaseType.h"
#include "../fiOsd/osdCfg.h"

OsdLock ObjInfoHandle::l;

ObjInfoHandle::ObjInfoHandle(ObjID &fid,s32 nodeID):oi(NULL),cnt(new s32(1))
{
	RELEASE_ASSERT_FASLE;
//	RELEASE_ASSERT(!fid.IsNull());
//
//	oi = new ObjInfo();
//	RELEASE_ASSERT(oi != NULL);
//
//	oi->fileID = fid;
//	ObjInfoSequence *bufTp = (ObjInfoSequence *)oi->objInfoBuf;
//	if (-1 != nodeID)
//	{
//		if (OsdCfg::GetOsdCfg().GetMetaBakNodeID() == nodeID)
//		{
//			bufTp->rev1 = bufTp->metaNode[0] = OsdCfg::GetOsdCfg().GetLocNodeID();
//			bufTp->rev2 = bufTp->metaNode[1] = nodeID;
//		}
//		else
//		{
//			bufTp->rev1 = bufTp->metaNode[0] = nodeID;
//			bufTp->rev2 = bufTp->metaNode[1] = OsdCfg::GetOsdCfg().GetLocNodeID();
////			RELEASE_ASSERT(nodeID == OsdCfg::GetOsdCfg().GetMetaMainNodeID());
//		}
//	}
//	else
//	{
//		bufTp->metaNode[0] = -1;
//		bufTp->metaNode[1] = -1;
//	}
}

