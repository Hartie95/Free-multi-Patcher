#include <stdio.h>
#include <string.h>
#include <citrus/fs.hpp>
#include <stdlib.h>

#include "patches.h"

using namespace std;
using namespace ctr;

    
Patch::Patch(binPatch *_patch)
{
    u32 processNamePosition=_patch->patchNameSize+_patch->descriptionSize;

    this->patchName.assign(_patch->binaryData,_patch->patchNameSize);
    this->description.assign(&(_patch->binaryData[_patch->patchNameSize]),_patch->descriptionSize);
    this->processName.assign(&(_patch->binaryData[processNamePosition]),_patch->processNameSize);

    this->minKernelVersion = _patch->minKernelVersion;
    this->maxKernelVersion = _patch->maxKernelVersion;
    this->minFirmwareVersion = _patch->minFirmwareVersion;
    this->maxFirmwareVersion = _patch->maxFirmwareVersion;
    this->devicesSupported = _patch->devicesSupported;
    this->regionsSupported = _patch->regionsSupported;
    this->nandCompability = _patch->nandCompability;

    this->patchType = _patch->patchType;
    this->startAddressProcess = _patch->startAddressProcess;
    this->startAddressGlobal = _patch->startAddressGlobal;
    this->searchAreaSize = _patch->searchAreaSize;
    this->numberOfReplacements = _patch->numberOfReplacements;
    this->patchOffset = _patch->patchOffset;

    u32 originalcodeSize=_patch->originalcodeSize;
    u8* originalcode = (u8*) malloc(originalcodeSize);
    u32 originalCodePosition=_patch->patchNameSize+_patch->descriptionSize+_patch->processNameSize;
    memcpy(originalcode,&(_patch->binaryData[originalCodePosition]),originalcodeSize);
    this->originalCode={originalcodeSize, originalcode};
    
    u32 patchcodeSize=_patch->patchcodeSize;
    u8* patchcode = (u8*) malloc(patchcodeSize);
    u32 patchCodePosition=originalCodePosition+originalcodeSize;
    memcpy(patchcode,&(_patch->binaryData[patchCodePosition]),patchcodeSize);
    this->patchCode={patchcodeSize, patchcode};

    this->changeStatus(true);
}

Patch::~Patch()
{
    free(this->originalCode.code);
    free(this->patchCode.code);
}

string Patch::getPatchName()
{
    return this->patchName;
}

string Patch::getDescription()
{
    return this->description;
}

string Patch::getProcessName()
{
    return this->processName;
}


kernelVersion Patch::getMinKernelVersion()
{
    return this->minKernelVersion;
}

kernelVersion Patch::getMaxKernelVersion()
{
    return this->maxKernelVersion;
}

firmwareVersion Patch::getMinFirmwareVersion()
{
	return this->minFirmwareVersion;
}

firmwareVersion Patch::getMaxFirmwareVersion()
{
	return this->maxFirmwareVersion;
}

devices Patch::getDevicesSupported()
{
    return this->devicesSupported;
}

regions Patch::getRegionsSupported()
{
    return this->regionsSupported;
}

nands Patch::getNandCompability()
{
    return this->nandCompability;
}


u8 Patch::getPatchType()
{
    return this->patchType;
}

u32 Patch::getStartAddressProcess()
{
    return this->startAddressProcess;
}

u32 Patch::getStartAddressGlobal()
{
    return this->startAddressGlobal;
}

u32 Patch::getSearchAreaSize()
{
    return this->searchAreaSize;
}

u32 Patch::getNumberOfReplacements()
{
    return this->numberOfReplacements;
}

u32 Patch::getPatchOffset()
{
    return this->patchOffset;
}

code Patch::getOriginalCode()
{
    return this->originalCode;
}

code Patch::getPatchCode()
{
    return this->patchCode;
}


bool Patch::changeStatus()
{
    return this->changeStatus(!this->enabled);
}

bool Patch::changeStatus(bool status)
{
    this->enabled=status;
    return this->enabled;
}

bool Patch::isEnabled()
{
    return this->enabled;
}


//-----------------------------------------------------------------------------
/*
u32 self_pid = 0;

int PatchPid()
{
    *(u32*)(curr_kproc_addr + kproc_pid_offset) = 0;
    return 0;
}

int UnpatchPid()
{
    *(u32*)(curr_kproc_addr + kproc_pid_offset) = self_pid;
    return 0;
}

void ReinitSrv()
{
    srvExit();
    srvInit();
}

void PatchSrvAccess()
{
    svcGetProcessId(&self_pid, 0xFFFF8001);
    printf("Current process id: %lu\n", self_pid);

    printf("Patching srv access...");
    KernelBackdoor(PatchPid);
    ReinitSrv();

    u32 new_pid;
    svcGetProcessId(&new_pid, 0xFFFF8001);
    printf("%s\n", new_pid == 0 ? "succeeded!" : "failed!");

    // Cleanup; won't take effect until srv is reinitialized
    KernelBackdoor(UnpatchPid);
}
*/
//-----------------------------------------------------------------------------

/*
Todo:
doesnt work atm(crashes)
int changeSerial()
{
    // Target title id
    //change console Serial
    static const char * titleIdCfg = "cfg";
    static const u32 startAddressCfg = 0x0fefffd5;
    
    static const char * titleIdAct = "act";                           
    static const u32 startAddressAct = 0x0003E74C;

    //todo read from file
    static unsigned char orgSerial[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    static char serial[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    
    findAndReplace(titleIdCfg, 3, startAddressCfg, 0x00000010, 1, orgSerial, sizeof(orgSerial), serial, sizeof(serial));
    findAndReplace(titleIdAct, 3, startAddressAct, 0x00000030, 1, orgSerial, sizeof(orgSerial), serial, sizeof(serial));

    return 0;
}*/

void createDefaultPatches()
{
    FILE *file ;
    string filepath="";
    
    // Patch nim to answer, that no update is available(doesnt affect updating in systemsettings)
    // 9.0.0 Address: 0x0000DD28  
    char nimSpoofBytes[]=   /*patchName*/   "e-shop spoof"
                            /*description*/ "Patches nim for E-Shop access"
                            /*processname*/ "nim"
                            /*OriginalCode*/"\x35\x22\x10\xB5\xD2\x01\x80\x18\x00\x79\x00\x28\x03\xD0\x08\x46" 
                            /*patchBegin*/  "\x00\x20\x08\x60\x70\x47";

    u32 nimSpoofSize = sizeof(binPatch) + sizeof(nimSpoofBytes);
    binPatch* nimSpoofPatch = (binPatch*)malloc(nimSpoofSize);
    
    nimSpoofPatch->version              = 0x00;
    nimSpoofPatch->patchSize            = nimSpoofSize;
    nimSpoofPatch->patchNameSize        = 12;
    nimSpoofPatch->descriptionSize      = 29;
    nimSpoofPatch->processNameSize      = 3;
    nimSpoofPatch->originalcodeSize     = 16;
    nimSpoofPatch->patchcodeSize        = 6;
    nimSpoofPatch->processType          = 0x01;
    nimSpoofPatch->minKernelVersion     = {0x00, 0x00, 0x00, 0x00};
    nimSpoofPatch->maxKernelVersion     = {0xFF, 0xFF, 0xFF, 0xFF};
    nimSpoofPatch->minFirmwareVersion   = { 9, 0, 0, 20 };
    nimSpoofPatch->maxFirmwareVersion   = {0xFF, 0xFF, 0xFF, 0xFF};
    nimSpoofPatch->devicesSupported     = {1,1,1,1,1,0};
    nimSpoofPatch->regionsSupported     = {1,1,1,1,1,1,1,0};
    nimSpoofPatch->nandCompability      = {1,1,0};
    nimSpoofPatch->patchType            = 0x00;
    nimSpoofPatch->startAddressProcess  = 0x00001000;
    nimSpoofPatch->startAddressGlobal   = 0x26A00000;
    nimSpoofPatch->searchAreaSize       = 0x00100000;
    nimSpoofPatch->numberOfReplacements = 0x01;
    nimSpoofPatch->patchOffset          = 0;

    memcpy(nimSpoofPatch->binaryData, nimSpoofBytes, sizeof(nimSpoofBytes));

    string nimSpoofPatchFileName="nimSpoof"+patchExtension;

    filepath=patchesFolder+nimSpoofPatchFileName;
    file = fopen(filepath.c_str(),"wb"); 
    if (file == NULL) 
    {
        file = fopen(filepath.c_str(),"cb"); 
    }
    fwrite(nimSpoofPatch, 1, (nimSpoofSize), file);
    fclose(file);
    free(nimSpoofPatch);


    // Patch nim to stop automatic update download(could be unstable)
    // 9.0.0 Address: 0x0000EA00    
    char nimUpdateBytes[]=   /*patchName*/  "no auto download"
                            /*description*/ "Patch nim to stop automatic update download(could be unstable)"
                            /*processname*/ "nim"
                            /*OriginalCode*/"\x25\x79\x0B\x99\x00\x24\x00\x2D\x29\xD0\x16\x4D\x2D\x68\x01\x91" 
                            /*patchBegin*/  "\xE3\xA0\x00\x00";

    u32 nimUpdateSize = sizeof(binPatch) + sizeof(nimUpdateBytes);
    binPatch* nimUpdatePatch = (binPatch*)malloc(nimUpdateSize);
    
    nimUpdatePatch->version              = 0x00;
    nimUpdatePatch->patchSize            = nimUpdateSize;
    nimUpdatePatch->patchNameSize        = 16;
    nimUpdatePatch->descriptionSize      = 62;
    nimUpdatePatch->processNameSize      = 3;
    nimUpdatePatch->originalcodeSize     = 16;
    nimUpdatePatch->patchcodeSize        = 4;
    nimUpdatePatch->processType          = 0x01;
    nimUpdatePatch->minKernelVersion     = {0x00, 0x00, 0x00, 0x00};
    nimUpdatePatch->maxKernelVersion     = {0xFF, 0xFF, 0xFF, 0xFF};
    nimUpdatePatch->minFirmwareVersion   = { 4, 0, 0, 0 };
    nimUpdatePatch->maxFirmwareVersion   = {0xFF, 0xFF, 0xFF, 0xFF};
    nimUpdatePatch->devicesSupported     = {1,1,1,1,1,0};
    nimUpdatePatch->regionsSupported     = {1,1,1,1,1,1,1,0};
    nimUpdatePatch->nandCompability      = {1,1,0};
    nimUpdatePatch->patchType            = 0x00;
    nimUpdatePatch->startAddressProcess  = 0x00001000;
    nimUpdatePatch->startAddressGlobal   = 0x26A00000;
    nimUpdatePatch->searchAreaSize       = 0x00010000;
    nimUpdatePatch->numberOfReplacements = 0x01;
    nimUpdatePatch->patchOffset          = 0;

    memcpy(nimUpdatePatch->binaryData, nimUpdateBytes, sizeof(nimUpdateBytes));

    string nimUpdatePatchFileName="nimUpdate"+patchExtension;

    filepath=patchesFolder+nimUpdatePatchFileName;
    file = fopen(filepath.c_str(),"wb"); 
    if (file == NULL) 
    {
        file = fopen(filepath.c_str(),"cb"); 
    }
    fwrite(nimUpdatePatch, 1, (nimUpdateSize), file);
    fclose(file);
    free(nimUpdatePatch);
}
