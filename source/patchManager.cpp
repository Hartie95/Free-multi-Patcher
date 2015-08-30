#include <ctrcommon/gpu.hpp>
#include <ctrcommon/fs.hpp> 

#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "patchManager.h"
#include "constants.h"
#include "kernel11.h"
#include "kobjects.h"

#include "patchEntry.h"


using namespace std;

vector<Patch*> loadedPatches;
u32 numberOfLoadedPatches;
string enabledPatches[10];

void initPatches()
{
    checkPatchFolder();
    createDefaultPatches();
    loadPatchFiles();
}

void checkPatchFolder()
{
    if(!fsExists(patchesFolder))
        mkdir(patchesFolder.c_str(), 0777);
}

void loadPatchFiles()
{  


  DIR *dir;
  dir = opendir(patchesFolder.c_str());
  if (dir)
  {
      //u32 numberOfPatchFiles=getNumberOfPatchFiles(dir);
      //loadedPatches=(patch** )malloc(numberOfPatchFiles*sizeof(patch *));

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
              binPatch* tmp=loadPatch(file);

              if(tmp!=nullptr)
              {
                  loadedPatches.push_back(new Patch(tmp));
                  free(tmp);
                  numberOfLoadedPatches++;
              }
          }
      }
  }
  closedir(dir);
}

int createPatchPage(MenuManager* menuManager)
{
  Menu* page=new Menu(menuManager,menuManager->getMainPage());

  Patch* currentPatch;
  for(std::vector<Patch*>::iterator it = loadedPatches.begin(); it != loadedPatches.end(); ++it)
  {
    currentPatch = (*it);
    PatchEntry* entry=new PatchEntry(currentPatch);
    page->addEntry((MenuEntry*)entry);
  }  
    
  menuManager->addPage(page,page->getParentMenu(),"Manage Patches");
  return 0;
}


bool isPatch(struct dirent* file)
{
    u32 nameLength=strlen(file->d_name);
    if (nameLength >= patchExtension.size() && strcmp(file->d_name + nameLength - patchExtension.size(), patchExtension.c_str()) == 0) {
        return true;
    }

    return false;
}

binPatch* loadPatch(FILE* file)
{
    binPatch* loadedPatch=nullptr;
    if(file != NULL)
    {    
        fseek(file, 0L, SEEK_END);
        u32 fileSize = ftell(file);
        fseek(file, 0L, SEEK_SET);

        loadedPatch=(binPatch*)malloc(fileSize);
        if(loadedPatch!=nullptr)
        {
            fread(loadedPatch,1,fileSize,file);
        }
        fclose(file);
    }
    return loadedPatch;
}

int applyPatches(){

  //PatchSrvAccess();  
  Patch* currentPatch;
  bool ignoreKernelVersion=false;
  for(std::vector<Patch*>::iterator it = loadedPatches.begin(); it != loadedPatches.end(); ++it)
  {
    currentPatch = (*it);
    if(currentPatch->isEnabled())
        if (checkKernelVersion(currentPatch->getMinKernelVersion(), currentPatch->getMaxKernelVersion()) || ignoreKernelVersion)
        {
            findAndReplaceCode(currentPatch);
        }
  }

  return 0;
}

int getNumberLoadedPatches()
{
    return loadedPatches.size();
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

void* getProcessAddress(u32 startAddress,u32 processNameSize,const char* processName)
{
    KCodeSet* code_set = FindTitleCodeSet(processName,processNameSize);
    if (code_set == nullptr)
        return nullptr;

    return (void*) FindCodeOffsetKAddr(code_set, startAddress);
}

int findAndReplaceCode(Patch* _patch)
{
    u32 numberOfReplaces = _patch->getNumberOfReplacements();

    const u32 startAddress = _patch->getStartAddressProcess();
    const u32 area = _patch->getSearchAreaSize();  

    const char* processName = _patch->getProcessName().c_str();
    u32 processNameSize = _patch->getProcessName().size();

    code originalCode = _patch->getOriginalCode();
    code patchCode = _patch->getPatchCode();
    

    u8 * startAddressPointer = (u8 *)getProcessAddress(startAddress, processNameSize, processName);
    if(startAddressPointer==nullptr)
        return 1;
    u8 * destination=nullptr;
    u32 numberOfFounds=0;
    
    for(u32 i = 0; i < area && numberOfFounds<=numberOfReplaces; i+=4)
    {  
        //check for the original code position
        bool found=true;
        for(u32 x = 0; x < originalCode.codeSize && found == true; x+= 4)
        {
            if((*((u32*)(startAddressPointer + i + x)) != *((u32*)&originalCode.code[x])))
                found=false;
        }
        if( found==true)
        {    
            //Apply patches, if the addresses was found  
            destination = startAddressPointer + i;
            memcpy(destination, patchCode.code, patchCode.codeSize);
            numberOfFounds++;
        }  
    }  
    return 0;
}

bool checkKernelVersion(kernelVersion min, kernelVersion max)
{
    u32 kernelValue = osGetKernelVersion();
    bool minBool = false;
    bool maxBool = false;
    kernelVersion* kernel = (kernelVersion*)&kernelValue;
    if (min.major < kernel->major)
        minBool = true;
    else if (min.major == kernel->major)
    {
        if (min.minor < kernel->minor)
            minBool = true;
        else if (min.minor == kernel->minor)
        {
            if (min.revision <= kernel->revision)
                minBool = true;
        }
    }
    if (max.major > kernel->major)
        maxBool = true;
    else if (max.major == kernel->major)
    {
        if (max.minor > kernel->minor)
            minBool = true;
        else if (max.minor == kernel->minor)
        {
            if (max.revision >= kernel->revision)
                minBool = true;
        }

    }
    return (minBool && maxBool);
}