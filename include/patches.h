#pragma once

#include <3ds.h>
#include "constants.h"
#include "binaryLayouts.h"

/*int PatchPid();
int UnpatchPid();
void ReinitSrv();
void PatchSrvAccess();*/

void createDefaultPatches();

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