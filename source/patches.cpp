#include <stdio.h>
#include <string.h>
#include <ctrcommon/fs.hpp> 
//#include <sys/stat.h>
#include <stdlib.h>

#include "patches.h"

using namespace std;

    

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
    this->regionsSupported = _patch->regionsSupported;
    this->nandCompability = _patch->nandCompability;

    patchType = _patch->patchType;
    startAddressProcess = _patch->startAddressProcess;
    startAddressGlobal = _patch->startAddressGlobal;
    searchAreaSize = _patch->searchAreaSize;
    numberOfReplacements = _patch->numberOfReplacements;

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

u32   Patch::getStartAddressProcess()
{
    return this->startAddressProcess;
}

u32   Patch::getStartAddressGlobal()
{
    return this->startAddressGlobal;
}

u32     Patch::getSearchAreaSize()
{
    return this->searchAreaSize;
}

u32     Patch::getNumberOfReplacements()
{
    return this->numberOfReplacements;
}

code   Patch::getOriginalCode()
{
    return this->originalCode;
}

code   Patch::getPatchCode()
{
    return this->patchCode;
}



bool    Patch::changeStatus()
{
    return this->changeStatus(!this->enabled);
}

bool    Patch::changeStatus(bool status)
{
    this->enabled=status;
    return this->enabled;
}

bool    Patch::isEnabled()
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
Todo: find offsets
int patchDlp()
{
    // Set generell informations for patching
    static const char * titleId = "dlp";
    static const u32 startAddress = 0x00008000;

    // patch NS to return update doesnt need to be installed intead of CVer not found error code after Update Check
    // 9.0.0 Addresses: 0x00102acc, 0x001894f4;
    static char patchcode[] = { 0x0B, 0x18, 0x21, 0xC8 };  
    static unsigned char originalcode[] = { 0x0C, 0x18, 0xE1, 0xD8 };
    findAndReplace(titleId, 2, startAddress, 0x00010000, 2, originalcode, sizeof(originalcode), patchcode, sizeof(patchcode));

    return 0;
}*/

/*
Todo:
//doesnt work atm(crashes)*//*
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

    // create the default patch files
    // patch Homemenu to show out of region applications
    // 9.0.0 Address: 0x00101B8C;
    char menuBytes[]=/*patchName*/  "region patch menu"
                    /*description*/ "Patches the home menu to show out of region games"
                    /*processname*/ "menu"
                    /*OriginalCode*/"\x00\x00\x55\xE3\x01\x10\xA0\xE3\x11\x00\xA0\xE1\x03\x00\x00\x0A" 
                    /*patchBegin*/  "\x01\x00\xA0\xE3\x70\x80\xBD\xE8";

    binPatch* menuPatch=(binPatch*)malloc(sizeof(binPatch)+sizeof(menuBytes));
    
    menuPatch->version              = 0x00;
    menuPatch->patchNameSize        = 17;
    menuPatch->descriptionSize      = 49;
    menuPatch->processNameSize      = 4;
    menuPatch->originalcodeSize     = 16;
    menuPatch->patchcodeSize        = 8;
    menuPatch->processType          = 0x01;
    menuPatch->minKernelVersion     = {0x00, 0x00, 0x00, 0x00};
    menuPatch->maxKernelVersion     = {0xFF, 0xFF, 0xFF, 0xFF};
    menuPatch->minFirmwareVersion   = { 4, 0, 0, 0 };
    menuPatch->maxFirmwareVersion   = { 9, 9, 0, 255 };
    menuPatch->regionsSupported     = {1,1,1,1,1,1,1,0};
    menuPatch->nandCompability      = {1,1,0};
    menuPatch->patchType            = 0x00;
    menuPatch->startAddressProcess  = 0x00100000;
    menuPatch->startAddressGlobal   = 0x26960000;
    menuPatch->searchAreaSize       = 0x001A0000;
    menuPatch->numberOfReplacements = 0x01;

    memcpy(menuPatch->binaryData, menuBytes, sizeof(menuBytes));

    string menuPatchFileName="regionMenu"+patchExtension;

    string filepath=patchesFolder+menuPatchFileName;
    file = fopen(filepath.c_str(),"w"); 
    if (file == NULL) 
    {
        file = fopen(filepath.c_str(),"c"); 
    }
    fwrite(menuPatch,1,(sizeof(binPatch)+sizeof(menuBytes)),file);
    fclose(file);
    free(menuPatch);

    // patch NS to return update doesnt need to be installed intead of CVer not found error code after Update Check
    // 9.0.0 Addresses: 0x00102acc, 0x001894f4; 
    char nsBytes[]= /*patchName*/   "region patch menu"
                    /*description*/ "Patches the home menu to show out of region games"
                    /*processname*/ "ns"
                    /*OriginalCode*/"\x0C\x18\xE1\xD8" 
                    /*patchBegin*/  "\x0B\x18\x21\xC8";

    binPatch* nsPatch=(binPatch*)malloc(sizeof(binPatch)+sizeof(nsBytes));
    
    nsPatch->version              = 0x00;
    nsPatch->patchNameSize        = 17;
    nsPatch->descriptionSize      = 49;
    nsPatch->processNameSize      = 2;
    nsPatch->originalcodeSize     = 4;
    nsPatch->patchcodeSize        = 4;
    nsPatch->processType          = 0x01;
    nsPatch->minKernelVersion     = { 0, 4, 33, 2 };
    nsPatch->maxKernelVersion     = { 255, 1, 50, 2 };
    nsPatch->minFirmwareVersion   = {0x00, 0x00, 0x00, 0x00};
    nsPatch->maxFirmwareVersion   = {0xFF, 0xFF, 0xFF, 0xFF};
    nsPatch->regionsSupported     = {1,1,1,1,1,1,1,0};
    nsPatch->nandCompability      = {1,1,0};
    nsPatch->patchType            = 0x00;
    nsPatch->startAddressProcess  = 0x00018000;
    nsPatch->startAddressGlobal   = 0x26A00000;
    nsPatch->searchAreaSize       = 0x00100000;
    nsPatch->numberOfReplacements = 0x02;

    memcpy(nsPatch->binaryData, nsBytes, sizeof(nsBytes));

    string nsPatchFileName="regionNs"+patchExtension;

    filepath=patchesFolder+nsPatchFileName;
    file = fopen(filepath.c_str(),"w"); 
    if (file == NULL) 
    {
        file = fopen(filepath.c_str(),"c"); 
    }
    fwrite(nsPatch,1,(sizeof(binPatch)+sizeof(nsBytes)),file);
    fclose(file);
    free(nsPatch);


    // Patch nim to answer, that no update is available(doesnt affect updating in systemsettings)
    // 9.0.0 Address: 0x0000DD28  
    char nimSpoofBytes[]=   /*patchName*/   "e-shop spoof"
                            /*description*/ "Patches nim for E-Shop access"
                            /*processname*/ "nim"
                            /*OriginalCode*/"\x35\x22\x10\xB5\xD2\x01\x80\x18\x00\x79\x00\x28\x03\xD0\x08\x46" 
                            /*patchBegin*/  "\x00\x20\x08\x60\x70\x47";

    binPatch* nimSpoofPatch=(binPatch*)malloc(sizeof(binPatch)+sizeof(nimSpoofBytes));
    
    nimSpoofPatch->version              = 0x00;
    nimSpoofPatch->patchNameSize        = 12;
    nimSpoofPatch->descriptionSize      = 29;
    nimSpoofPatch->processNameSize      = 3;
    nimSpoofPatch->originalcodeSize     = 16;
    nimSpoofPatch->patchcodeSize        = 6;
    nimSpoofPatch->processType          = 0x01;
    nimSpoofPatch->minKernelVersion     = {0x00, 0x00, 0x00, 0x00};
    nimSpoofPatch->maxKernelVersion     = {0xFF, 0xFF, 0xFF, 0xFF};
    menuPatch->minFirmwareVersion       = { 9, 0, 0, 20 };
    menuPatch->maxFirmwareVersion       = { 9, 9, 0, 26 };
    nimSpoofPatch->regionsSupported     = {1,1,1,1,1,1,1,0};
    nimSpoofPatch->nandCompability      = {1,1,0};
    nimSpoofPatch->patchType            = 0x00;
    nimSpoofPatch->startAddressProcess  = 0x00001000;
    nimSpoofPatch->startAddressGlobal   = 0x26A00000;
    nimSpoofPatch->searchAreaSize       = 0x00100000;
    nimSpoofPatch->numberOfReplacements = 0x01;

    memcpy(nimSpoofPatch->binaryData, nimSpoofBytes, sizeof(nimSpoofBytes));

    string nimSpoofPatchFileName="nimSpoof"+patchExtension;

    filepath=patchesFolder+nimSpoofPatchFileName;
    file = fopen(filepath.c_str(),"w"); 
    if (file == NULL) 
    {
        file = fopen(filepath.c_str(),"c"); 
    }
    fwrite(nimSpoofPatch,1,(sizeof(binPatch)+sizeof(nimSpoofBytes)),file);
    fclose(file);
    free(nimSpoofPatch);


    // Patch nim to stop automatic update download(could be unstable)
    // 9.0.0 Address: 0x0000EA00    
    char nimUpdateBytes[]=   /*patchName*/  "no auto download"
                            /*description*/ "Patch nim to stop automatic update download(could be unstable)"
                            /*processname*/ "nim"
                            /*OriginalCode*/"\x25\x79\x0B\x99\x00\x24\x00\x2D\x29\xD0\x16\x4D\x2D\x68\x01\x91" 
                            /*patchBegin*/  "\xE3\xA0\x00\x00";

    binPatch* nimUpdatePatch=(binPatch*)malloc(sizeof(binPatch)+sizeof(nimUpdateBytes));
    
    nimUpdatePatch->version              = 0x00;
    nimUpdatePatch->patchNameSize        = 16;
    nimUpdatePatch->descriptionSize      = 62;
    nimUpdatePatch->processNameSize      = 3;
    nimUpdatePatch->originalcodeSize     = 16;
    nimUpdatePatch->patchcodeSize        = 4;
    nimUpdatePatch->processType          = 0x01;
    nimUpdatePatch->minKernelVersion     = {0x00, 0x00, 0x00, 0x00};
    nimUpdatePatch->maxKernelVersion     = {0xFF, 0xFF, 0xFF, 0xFF};
    menuPatch->minFirmwareVersion        = { 4, 0, 0, 0 };
    menuPatch->maxFirmwareVersion        = { 9, 9, 0, 255 };
    nimUpdatePatch->regionsSupported     = {1,1,1,1,1,1,1,0};
    nimUpdatePatch->nandCompability      = {1,1,0};
    nimUpdatePatch->patchType            = 0x00;
    nimUpdatePatch->startAddressProcess  = 0x00001000;
    nimUpdatePatch->startAddressGlobal   = 0x26A00000;
    nimUpdatePatch->searchAreaSize       = 0x00010000;
    nimUpdatePatch->numberOfReplacements = 0x01;

    memcpy(nimUpdatePatch->binaryData, nimUpdateBytes, sizeof(nimUpdateBytes));

    string nimUpdatePatchFileName="nimUpdate"+patchExtension;

    filepath=patchesFolder+nimUpdatePatchFileName;
    file = fopen(filepath.c_str(),"w"); 
    if (file == NULL) 
    {
        file = fopen(filepath.c_str(),"c"); 
    }
    fwrite(nimUpdatePatch,1,(sizeof(binPatch)+sizeof(nimUpdateBytes)),file);
    fclose(file);
    free(nimUpdatePatch);

}