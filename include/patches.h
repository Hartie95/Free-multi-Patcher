#pragma once

#include <3ds.h>

/*int PatchPid();
int UnpatchPid();
void ReinitSrv();
void PatchSrvAccess();*/
int patchNimEshop();
int patchNimAutoUpdate();
int patchRegionFree();
int patchMenu();
int patchNs();
//int patchDlp();
int changeSerial();