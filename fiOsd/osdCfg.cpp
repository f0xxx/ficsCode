﻿///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  hunter code ([hunter.f0x] remeber my brothers) email:hunterlhy@163.com /////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "osdCfg.h"

void InitOsdCfg()
{
//  OsdCfg::GetOsdCfg();
}

void InitOsdCfgClient(char *path)
{
	FicsCfgParser *fcp = new FicsCfgParser(path);
	OsdCfgClient::GetOsdCfgClient().SetCfgParser(fcp);
}

