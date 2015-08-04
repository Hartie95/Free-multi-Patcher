#pragma once
#include <3ds.h>

#include <sys/dirent.h>
#include "patches.h"

void initPatches();
void checkPatchFolder();
void loadPatchFiles();

bool isPatch(struct dirent* file);
int  getNumberOfPatchFiles(DIR* dir);
void* getProcessAddress(u32 startAddress,u32 processNameSize,char* processName);
int findAndReplaceCode(Patch* _patch);

patch* loadPatch(FILE* file);

int  applyPatches();