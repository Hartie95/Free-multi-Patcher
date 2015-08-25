#pragma once
#include <3ds.h>

#include <sys/dirent.h>
#include "patches.h"
#include "menuManager.h"


void initPatches();
void checkPatchFolder();
void loadPatchFiles();
int createPatchPage(MenuManager* menuManager);

bool isPatch(struct dirent* file);
int  getNumberOfPatchFiles(DIR* dir);
void* getProcessAddress(u32 startAddress,u32 processNameSize,char* processName);
int findAndReplaceCode(Patch* _patch);



binPatch* loadPatch(FILE* file);

int  applyPatches();