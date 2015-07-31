#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <3ds.h>

#include "constants.h"
#include "patches.h"
#include "kernel11.h"
#include "kobjects.h"

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

typedef struct patchStruct
{
    char   version;
    u32   patchNameSize;
    u32   descriptionSize;
    u32   processNameSize;
    u32   originalcodeSize;
    u32   patchcodeSize;
    u32   processType;
    u32     minKernelVersion;
    u32     maxKernelVersion;
    regions regionsSupported;
    nands   nandCompability;
    u8   patchType;
    u32     startAddressProcess;
    u32     startAddressGlobal;
    u32     searchAreaSize;
    u32     numberOfReplacements;
    char binaryData[];
} patch;

patch filePatch;

patch* loadedPatches[256];
u32 numberOfLoadedPatches;

patch* tmpPatch;

int findAndPatchCode( const char* titleId, short titleIdSize,  const u32 startAddress, const u32 area,  unsigned char originalcode[], const char patchcode[],u32 patchcodeSize)
{
    KCodeSet* code_set = FindTitleCodeSet(titleId,titleIdSize);
    if (code_set == nullptr)
        return 1;

    unsigned char * startAddressPointer = (unsigned char*)FindCodeOffsetKAddr(code_set, startAddress);
    unsigned char * destination=nullptr;
    for(unsigned int i = 0; i < area && destination==nullptr; i+=4)
    {  
        //check for the original code position
        if( (*((unsigned int*)(startAddressPointer + i + 0x0)) == *((unsigned int*)&originalcode[0x0])) &&  
            (*((unsigned int*)(startAddressPointer + i + 0x4)) == *((unsigned int*)&originalcode[0x4])) &&  
            (*((unsigned int*)(startAddressPointer + i + 0x8)) == *((unsigned int*)&originalcode[0x8])) &&  
            (*((unsigned int*)(startAddressPointer + i + 0xC)) == *((unsigned int*)&originalcode[0xC])))
        {    
            destination = startAddressPointer + i;
        }  
    }  

    //Apply patches, if the address was found
    if(destination!=nullptr)
        memcpy(destination, patchcode, patchcodeSize);
    else
        return 2;
    
    return 0;
}

int findAndReplace( const char * titleId, short titleIdSize,  const u32 startAddress, const u32 area, short numberOfReplaces,  unsigned char originalcode[],u32 originalcodeSize,const char patchcode[],u32 patchcodeSize)
{
    KCodeSet* code_set = FindTitleCodeSet(titleId,titleIdSize);
    if (code_set == nullptr)
        return 1;

    int numberOfFounds=0;
    unsigned char * startAddressPointer = (unsigned char*)FindCodeOffsetKAddr(code_set, startAddress);
    unsigned char * destination[numberOfReplaces];
    
    for(int i=0;i<numberOfReplaces;i++)
    {
        destination[i]=nullptr;
    }

    for(unsigned int i = 0; i < area && numberOfFounds<=numberOfReplaces; i+=4)
    {  
        //check for the original code position
        bool found=true;
        for(unsigned int x = 0;x<originalcodeSize&&found==true;x+=4)
        {
            if((*((unsigned int*)(startAddressPointer + i + x)) != *((unsigned int*)&originalcode[x])))
                found=false;
        }
        if( found==true)
        {    
            destination[numberOfFounds] = startAddressPointer + i;
            numberOfFounds++;
        }  
    }  

    //Apply patches, if the addresses was found
    for(int i = 0; i < numberOfFounds && destination[i]!=nullptr; i++)
    {  
        memcpy(destination[i], patchcode, patchcodeSize);
    }  
    
    return 0;
}

int patchNimEshop()
{
    // Set generell informations for patching
    static const char * titleId = "nim";
    static const u32 startAddress = 0x00001000;

    // Patch nim to answer, that no update is available
    // 9.0.0 Address: 0x0000DD28
    static unsigned char originalcode[] = { 0x35, 0x22, 0x10, 0xB5, 0xD2, 0x01, 0x80, 0x18, 0x00, 0x79, 0x00, 0x28, 0x03, 0xD0, 0x08, 0x46};
    static const char patchcode[] = { 0x00, 0x20, 0x08, 0x60, 0x70, 0x47 };
    findAndPatchCode(titleId, 3, startAddress, 0x00010000, originalcode, patchcode, sizeof(patchcode));

    return 0;
}

int patchNimAutoUpdate()
{
    // Set generell informations for patching
    static const char * titleId = "nim";
    static const u32 startAddress = 0x00001000;

    // Patch nim to stop automatic update download(could be unstable)
    // 9.0.0 Address: 0x0000EA00
    static unsigned char originalcode[] = { 0x25, 0x79, 0x0B, 0x99, 0x00, 0x24, 0x00, 0x2D, 0x29, 0xD0, 0x16, 0x4D, 0x2D, 0x68, 0x01, 0x91};
    static char patchcode[] = { 0xE3, 0xA0, 0x00, 0x00 };
    findAndPatchCode(titleId, 3, startAddress, 0x00010000, originalcode, patchcode, sizeof(patchcode));

    return 0;
}

int patchRegionFree()
{
    patchMenu();
    patchNs();
    return 0;
}

int findAndPatchCode_test(patch* _patch)
{
    const u32 startAddress=_patch->startAddressProcess;
    const u32 area=_patch->searchAreaSize;  

    short titleIdSize=_patch->processNameSize;  
    unsigned int titleIdPosition=_patch->patchNameSize+_patch->descriptionSize;
    char titleId[_patch->processNameSize]; 
    memcpy(titleId,&(_patch->binaryData[titleIdPosition]),_patch->processNameSize);

    unsigned char originalcode[_patch->originalcodeSize];
    unsigned int originalCodePosition=_patch->patchNameSize+_patch->descriptionSize+_patch->processNameSize;
    memcpy(originalcode,&(_patch->binaryData[originalCodePosition]),_patch->originalcodeSize);
    
    u32 patchcodeSize=_patch->patchcodeSize;
    char patchcode[patchcodeSize];
    unsigned int patchCodePosition=originalCodePosition+_patch->originalcodeSize;
    memcpy(patchcode,&(_patch->binaryData[patchCodePosition]),patchcodeSize);


    KCodeSet* code_set = FindTitleCodeSet(titleId,titleIdSize);
    if (code_set == nullptr)
        return 1;

    unsigned char * startAddressPointer = (unsigned char*)FindCodeOffsetKAddr(code_set, startAddress);
    unsigned char * destination=nullptr;
    for(unsigned int i = 0; i < area && destination==nullptr; i+=4)
    {  
        //check for the original code position
        if( (*((unsigned int*)(startAddressPointer + i + 0x0)) == *((unsigned int*)&originalcode[0x0])) &&  
            (*((unsigned int*)(startAddressPointer + i + 0x4)) == *((unsigned int*)&originalcode[0x4])) &&  
            (*((unsigned int*)(startAddressPointer + i + 0x8)) == *((unsigned int*)&originalcode[0x8])) &&  
            (*((unsigned int*)(startAddressPointer + i + 0xC)) == *((unsigned int*)&originalcode[0xC])))
        {    
            destination = startAddressPointer + i;
        }  
    }  

    //Apply patches, if the address was found
    if(destination!=nullptr)
        memcpy(destination, patchcode, patchcodeSize);
    /*else
        return 2;
    */
    return 0;
}

int patchMenu()
{
    /*static patch menuPatch=
    // patch Homemenu to show out of region applications
    // 9.0.0 Address: 0x00101B8C;
    { 
        0x00        ,//short   version;
        17          ,//short   patchNameSize;
        49          ,//short   descriptionSize;
        4           ,//short   processNameSize
        16          ,//short   originalcodeSize;
        8           ,//short   patchcodeSize;
        0x01        ,//short   processType;
        0x00        ,//u32     minKernelVersion;
        0xffffffff  ,//u32     maxKernelVersion;
        {1,1,1,1,1,1,1,1},//regions regionssupported;
        0x00        ,//short   nandCompability;
        0x00        ,//short   patchType;
        0x00100000  ,//u32     startAddressProcess;
        0x26A00000  ,//u32     startAddressGlobal;
        0x00100000  ,//u32     searchAreaSize;
        0x01        ,//u32     numberOfReplacements
        
        //    /*patchName*/   //"region patch menu"
        //    /*description*/ "Patches the home menu to show out of region games"
         //   /*processname*/ "menu"
         //   /*OriginalCode*/ "\x00\x00\x55\xE3\x01\x10\xA0\xE3\x11\x00\xA0\xE1\x03\x00\x00\x0A" 
        //    /*patchBegin*/  "\x01\x00\xA0\xE3\x70\x80\xBD\xE8"
        
        // BYTE    binaryData[];
    //};


   // memcpy(menuPatch.binaryData,binary,sizeof(binary));
    // Set generell informations for patching
   /* static const char * titleId = "menu";
    static const u32 startAddress = 0x00100000;

    // patch Homemenu to show out of region applications
    // 9.0.0 Address: 0x00101B8C;
    static unsigned char originalcode[] = { 0x00, 0x00, 0x55, 0xE3, 0x01, 0x10, 0xA0, 0xE3, 0x11, 0x00, 0xA0, 0xE1, 0x03, 0x00, 0x00, 0x0A };  
    static char patchcode[] = { 0x01, 0x00, 0xA0, 0xE3, 0x70, 0x80, 0xBD, 0xE8 };
    findAndPatchCode(titleId, 4, startAddress, 0x00100000, originalcode, patchcode, sizeof(patchcode));*/
    /* const u32 startAddress=_patch->startAddressProcess;
    const u32 area=_patch->searchAreaSize;  

    short titleIdSize=_patch->patchNameSize;  
    unsigned int titleIdPosition=_patch->patchNameSize+_patch->descriptionSize;

    unsigned int originalCodePosition=_patch->patchNameSize+_patch->descriptionSize+_patch->processNameSize;
    
    u32 patchcodeSize=_patch->patchcodeSize;
    unsigned int patchCodePosition=originalCodePosition+_patch->originalcodeSize;

*/
    //findAndPatchCode(&_patch->binaryData[titleIdPosition], 4, _patch->startAddressProcess, 0x00100000, &_patch->binaryData[originalCodePosition], &_patch->binaryData[patchCodePosition], _patch->patchcodeSize);
    for(u32 i = 0; i<numberOfLoadedPatches;i++)
    {
        findAndPatchCode_test(loadedPatches[i]);
    }
    return 0;
}
#include <ctrcommon/gpu.hpp>
#include <ctrcommon/fs.hpp> 

#include <sys/dirent.h>
#include <sys/stat.h>
using namespace std;

static const string patchesFolder="sdmc:/fmp/patches/";
static const string patchExtension=".patch";

void createDefaultPatches()
{
    // check if the patches Folder exists
    if(!fsExists(patchesFolder))
        mkdir(patchesFolder.c_str(), 0777);

    // create the default patch files
    // patch Homemenu to show out of region applications
    // 9.0.0 Address: 0x00101B8C;
    char menuBytes[]=/*patchName*/  "region patch menu"
                    /*description*/ "Patches the home menu to show out of region games"
                    /*processname*/ "menu"
                    /*OriginalCode*/ "\x00\x00\x55\xE3\x01\x10\xA0\xE3\x11\x00\xA0\xE1\x03\x00\x00\x0A" 
                    /*patchBegin*/  "\x01\x00\xA0\xE3\x70\x80\xBD\xE8";

    static patch* menuPatch=(patch*)malloc(sizeof(patch)+sizeof(menuBytes));
    
    menuPatch->version              = 0x00;
    menuPatch->patchNameSize        = 17;
    menuPatch->descriptionSize      = 49;
    menuPatch->processNameSize      = 4;
    menuPatch->originalcodeSize     = 16;
    menuPatch->patchcodeSize        = 8;
    menuPatch->processType          = 0x01;
    menuPatch->minKernelVersion     = 0x0;
    menuPatch->maxKernelVersion     = 0x0;
    menuPatch->regionsSupported     = {1,1,1,1,1,1,1,0};
    menuPatch->nandCompability      = {1,1,0};
    menuPatch->patchType            = 0x00;
    menuPatch->startAddressProcess  = 0x00100000;
    menuPatch->startAddressGlobal   = 0x26A00000;
    menuPatch->searchAreaSize       = 0x00100000;
    menuPatch->numberOfReplacements = 0x01;

    memcpy(menuPatch->binaryData, menuBytes, sizeof(menuBytes));

    string menuPatchFileName="menu"+patchExtension;

    string filepath=patchesFolder+menuPatchFileName;
    FILE *file = fopen(filepath.c_str(),"w"); 
    if (file == NULL) 
    {
        file = fopen(filepath.c_str(),"c"); 
    }
    fwrite(menuPatch,1,(sizeof(patch)+sizeof(menuBytes)),file);
    fclose(file);



    // Patch nim to stop automatic update download(could be unstable)
    // 9.0.0 Address: 0x0000EA00    
    char nimBytes[]=/*patchName*/  "e-shop spoof"
                    /*description*/ "Patches nim for E-Shop access"
                    /*processname*/ "nim"
                    /*OriginalCode*/ "\x25\x79\x0B\x99\x00\x24\x00\x2D\x29\xD0\x16\x4D\x2D\x68\x01\x91" 
                    /*patchBegin*/  "\xE3\xA0\x00\x00";

    static patch* nimPatch=(patch*)malloc(sizeof(patch)+sizeof(nimBytes));
    
    nimPatch->version              = 0x00;
    nimPatch->patchNameSize        = 12;
    nimPatch->descriptionSize      = 29;
    nimPatch->processNameSize      = 3;
    nimPatch->originalcodeSize     = 16;
    nimPatch->patchcodeSize        = 2;
    nimPatch->processType          = 0x01;
    nimPatch->minKernelVersion     = 0x0;
    nimPatch->maxKernelVersion     = 0xFFFFFFFF;
    nimPatch->regionsSupported     = {1,1,1,1,1,1,1,0};
    nimPatch->nandCompability      = {1,1,0};
    nimPatch->patchType            = 0x00;
    nimPatch->startAddressProcess  = 0x00001000;
    nimPatch->startAddressGlobal   = 0x26A00000;
    nimPatch->searchAreaSize       = 0x00100000;
    nimPatch->numberOfReplacements = 0x01;

    memcpy(nimPatch->binaryData, nimBytes, sizeof(nimBytes));

    string nimPatchFileName="nim"+patchExtension;

    filepath=patchesFolder+nimPatchFileName;
    file = fopen(filepath.c_str(),"w"); 
    if (file == NULL) 
    {
        file = fopen(filepath.c_str(),"c"); 
    }
    fwrite(nimPatch,1,(sizeof(patch)+sizeof(nimBytes)),file);
    fclose(file);
}

bool isPatch(struct dirent* file)
{
    u32 nameLength=strlen(file->d_name);
    if (nameLength >= patchExtension.size() && strcmp(file->d_name + nameLength - patchExtension.size(), patchExtension.c_str()) == 0) {
        return true;
    }

    return false;
}

int getNumberOfPatchFiles(DIR* dir)
{
    u32 numberOfFiles=0;
    struct dirent *currenElement;
    while ((currenElement = readdir(dir)) != NULL)
    {
        if(isPatch(currenElement))
            numberOfFiles++;
    }
    seekdir(dir,SEEK_SET);
    return numberOfFiles;
}

patch* loadPatch(FILE* file)
{
    patch* loadedPatch=nullptr;
    if(file != NULL)
    {    
        fseek(file, 0L, SEEK_END);
        u32 fileSize = ftell(file);
        fseek(file, 0L, SEEK_SET);

        loadedPatch=(patch*)malloc(fileSize);
        if(loadedPatch!=nullptr)
        {
            fread(loadedPatch,1,fileSize,file);
        }
        fclose(file);
    }
    return loadedPatch;
}

void test()
{
    createDefaultPatches();

    DIR *dir;
    dir = opendir(patchesFolder.c_str());
    if (dir)
    {
        u32 numberOfPatchFiles=getNumberOfPatchFiles(dir);
        FILE* patchFiles[numberOfPatchFiles];

        closedir(dir);
        dir = opendir(patchesFolder.c_str());
        numberOfLoadedPatches=0;
        struct dirent *currenElement;
        while ((currenElement = readdir(dir)) != NULL)
        {
            if(isPatch(currenElement))
            {
                string filepath=patchesFolder+currenElement->d_name;
                FILE* file = fopen(filepath.c_str(),"rb");  
                patch* tmp=loadPatch(file);
                if(tmp!=nullptr)
                {
                    loadedPatches[numberOfLoadedPatches]=tmp;
                    numberOfLoadedPatches++;
                }
            }
        }
        //numberOfLoadedPatches=1;
        //tmpPatch=loadPatch(file);
        //loadedPatches=(patch**)malloc(sizeof(patch*)*numberOfPatchFiles);
        
        /*for(u32 i=0;i<numberOfPatchFiles;i++)
        {
            patch* tmp=loadPatch(patchFiles[i]);
            if(tmp!=nullptr)
                tmpPatch=tmp;
        }*/
    }
    closedir(dir);
}

int patchNs()
{
    // Set generell informations for patching
    static const char * titleId = "ns";
    static const u32 startAddress = 0x00018000;

    // patch NS to return update doesnt need to be installed intead of CVer not found error code after Update Check
    // 9.0.0 Addresses: 0x00102acc, 0x001894f4;
    static char patchcode[] = { 0x0B, 0x18, 0x21, 0xC8 };  
    static unsigned char originalcode[] = { 0x0C, 0x18, 0xE1, 0xD8 };
    findAndReplace(titleId, 2, startAddress, 0x00010000, 2, originalcode, sizeof(originalcode), patchcode, sizeof(patchcode));

    return 0;
}
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
//doesnt work atm(crashes)*/
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
}
