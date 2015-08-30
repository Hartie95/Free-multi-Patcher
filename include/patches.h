#pragma once

#include <3ds.h>
#include "constants.h"

/*int PatchPid();
int UnpatchPid();
void ReinitSrv();
void PatchSrvAccess();*/
//int patchDlp();
//int  changeSerial();
void createDefaultPatches();

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

typedef struct nandTypesStruct
{
    u32 System:1;
    u32 Emu:1;
    u32 placeholder:6;
} nands;

typedef struct codeStruct
{
    u32 codeSize;
    u8* code;
} code;

typedef struct patchStruct
{
    char   version;
    u32   patchNameSize;
    u32   descriptionSize;
    u32   processNameSize;
    u32   originalcodeSize;
    u32   patchcodeSize;
    u32   processType;
    kernelVersion     minKernelVersion;
    kernelVersion     maxKernelVersion;
    regions regionsSupported;
    nands   nandCompability;
    u8      patchType;
    u32     startAddressProcess;
    u32     startAddressGlobal;
    u32     searchAreaSize;
    u32     numberOfReplacements;
    char binaryData[];
} binPatch;

class Patch
{
private:
    std::string  patchName;
    std::string  description;
    std::string  processName;

    kernelVersion     minKernelVersion;
    kernelVersion     maxKernelVersion;
    regions regionsSupported;
    nands   nandCompability;

    u8      patchType;
    u32     startAddressProcess;
    u32     startAddressGlobal;
    u32     searchAreaSize;
    u32     numberOfReplacements;
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
    code    getOriginalCode();
    code    getPatchCode();

    bool    changeStatus();
    bool    changeStatus(bool status);
    bool    isEnabled();
};