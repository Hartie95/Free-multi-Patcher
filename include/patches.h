#pragma once

#include <3ds.h>

int PatchPid();
int UnpatchPid();
void ReinitSrv();
void PatchSrvAccess();
int changeSerial();
int patchNimEshop();
int patchNimAutoUpdate();
int patchMenu();