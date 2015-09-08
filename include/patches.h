#pragma once

#include <3ds.h>
#include "constants.h"

/*int PatchPid();
int UnpatchPid();
void ReinitSrv();
void PatchSrvAccess();*/

void createDefaultPatches();

/*Patchlayout defined by https://github.com/hartmannaf/Free-multi-Patcher/wiki/patchlayout */

//Australia is not used, instead it uses European region
typedef struct regionsStruct
{
    u32 japan:1;
    u32 northAmerica:1;
    u32 europe:1;
    u32 australia:1;
    u32 china:1;
    u32 korea:1;
    u32 taiwan :1;
    u32 placeholder :1;
} regions;

//placeholder for possible later types of emunand
typedef struct nandTypesStruct
{
    u32 System:1;
    u32 Emu:1;
    u32 placeholder:6;
} nands;

typedef struct patchStruct
{
    u8    version;              //0 for development, 1 after first release
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
    regions regionsSupported;
    nands   nandCompability;
    u8      patchType;          //0->search code, 1->overwrite code, 2->search String, 3->overwrite string
    u32     startAddressProcess;
    u32     startAddressGlobal;
    u32     searchAreaSize;     //0 overwrite all apearences
    u32     patchOffset;             //strings offset between each character, code offset wer to patch based on originalcode  
    u32     numberOfReplacements;
    char binaryData[];
} binPatch;


//for application internal use
typedef struct codeStruct
{
    u32 codeSize;
    u8* code;
} code;

class Patch
{
private:
    std::string  patchName;
    std::string  description;
    std::string  processName;

    kernelVersion     minKernelVersion;
    kernelVersion     maxKernelVersion;
    firmwareVersion   minFirmwareVersion;
    firmwareVersion   maxFirmwareVersion;
    regions regionsSupported;
    nands   nandCompability;

    u8      patchType;
    u32     startAddressProcess;
    u32     startAddressGlobal;
    u32     searchAreaSize;
    u32     numberOfReplacements;
    u32     patchOffset;
    code    originalCode;
    code    patchCode;

    bool    enabled;

public:
    Patch(binPatch*);
    ~Patch();
    std::string  getPatchName();
    std::string  getDescription();
    std::string  getProcessName();

    kernelVersion     getMinKernelVersion();
    kernelVersion     getMaxKernelVersion();
    regions getRegionsSupported();
    nands   getNandCompability();

    u8      getPatchType();
    u32     getStartAddressProcess();
    u32     getStartAddressGlobal();
    u32     getSearchAreaSize();
    u32     getNumberOfReplacements();
    u32     getPatchOffset();
    code    getOriginalCode();
    code    getPatchCode();

    bool    changeStatus();
    bool    changeStatus(bool status);
    bool    isEnabled();
};