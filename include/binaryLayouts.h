#pragma once

#include <3ds.h>
#include "constants.h"

/*Patchlayout defined by https://github.com/hartmannaf/Free-multi-Patcher/wiki/patchlayout */

//Australia is not used, instead it uses European region
typedef struct regionsStruct
{
    u32 japan : 1;
    u32 northAmerica : 1;
    u32 europe : 1;
    u32 australia : 1;
    u32 china : 1;
    u32 korea : 1;
    u32 taiwan : 1;
    u32 placeholder : 1;
} regions;

//placeholder for possible later types of emunand
typedef struct nandTypesStruct
{
    u32 System : 1;
    u32 Emu : 1;
    u32 placeholder : 6;
} nands;

typedef struct patchStruct
{
    u32   version;              //0 for development, 1 after first release
    u32   patchSize;
    u32   patchNameSize;
    u32   descriptionSize;
    u32   processNameSize;
    u32   originalcodeSize;
    u32   patchcodeSize;
    u32   processType;          //ARM9 = 0, ARM11 = 1
    kernelVersion       minKernelVersion;
    kernelVersion       maxKernelVersion;
    firmwareVersion     minFirmwareVersion;
    firmwareVersion     maxFirmwareVersion;
    devices devicesSupported;
    regions regionsSupported;
    nands   nandCompability;
    u32     patchType;          //0->search code, 1->overwrite code, 2->search String, 3->overwrite string
    u32     startAddressProcess;
    u32     startAddressGlobal;
    u32     searchAreaSize;     //0 overwrite all apearences
    u32     patchOffset;        //strings offset between each character, code offset where to patch based on originalcode  
    u32     numberOfReplacements;
    char binaryData[];
} binPatch;


/*PatchCollectionlayout defined by https://github.com/hartmannaf/Free-multi-Patcher/wiki/patchCollectionlayout */

typedef struct patchCollectionStruct
{
    u32   version;
    u32   collectionNameSize;
    u32   descriptionSize;
    u32   numberOfPatches;
    u32   processType;
    kernelVersion     minKernelVersion;
    kernelVersion     maxKernelVersion;
    firmwareVersion   minFirmwareVersion;
    firmwareVersion   maxFirmwareVersion;
    devices devicesSupported;
    regions regionsSupported;
    nands   nandCompability;
    char    binaryData[];
} binPatchCollection;