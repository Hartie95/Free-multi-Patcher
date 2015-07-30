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

int patchMenu()
{
    // Set generell informations for patching
    static const char * titleId = "menu";
    static const u32 startAddress = 0x00100000;

    // patch Homemenu to show out of region applications
    // 9.0.0 Address: 0x00101B8C;
    static unsigned char originalcode[] = { 0x00, 0x00, 0x55, 0xE3, 0x01, 0x10, 0xA0, 0xE3, 0x11, 0x00, 0xA0, 0xE1, 0x03, 0x00, 0x00, 0x0A };  
    static char patchcode[] = { 0x01, 0x00, 0xA0, 0xE3, 0x70, 0x80, 0xBD, 0xE8 };
    findAndPatchCode(titleId, 4, startAddress, 0x00100000, originalcode, patchcode, sizeof(patchcode));

    return 0;
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
