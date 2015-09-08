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
#include "collectionEntry.h"


using namespace std;

//TODO move to class
vector<Patch*> loadedPatches;
vector<PatchCollection*> loadedCollections;
string enabledPatches[10];

PatchManager::PatchManager()
{
    checkPatchFolder();
    createDefaultPatches();
    createDefaultCollections();
    loadPatchFiles();
}

void PatchManager::checkPatchFolder()
{
    if(!fsExists(patchesFolder))
        mkdir(patchesFolder.c_str(), 0777);
}

void PatchManager::loadPatchFiles()
{  
    DIR *dir;
    dir = opendir(patchesFolder.c_str());
    if (dir)
    {
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
                }
            }
            else  if(isCollection(currenElement))
            {
                string filepath = patchesFolder + currenElement->d_name;
                FILE* file = fopen(filepath.c_str(), "rb");
                binPatchCollection* tmp = loadCollection(file);

                if (tmp != nullptr)
                {
                    PatchCollection* tmpCollection = new PatchCollection(tmp);
                    
                    loadedCollections.push_back(tmpCollection);
                    free(tmp);
                }
            }
        }
    }
    closedir(dir);
}

int PatchManager::createPatchPage(MenuManager* menuManager)
{
  Menu* page=new Menu(menuManager,menuManager->getMainPage());

  PatchCollection* currentCollection;
  for (std::vector<PatchCollection*>::iterator it = loadedCollections.begin(); it != loadedCollections.end(); ++it)
  {
      currentCollection = (*it);
      CollectionEntry* entry = new CollectionEntry(currentCollection,(MenuManagerM*)menuManager,(MenuM*)page);
      page->addEntry((MenuEntry*)entry);
  }
 
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

bool PatchManager::isType(struct dirent* file, string extension)
{
    u32 nameLength = strlen(file->d_name);
    if (nameLength >= extension.size())
    {
        u32 extensionStart = nameLength - extension.size();
        if (strcmp(file->d_name + extensionStart, extension.c_str()) == 0)
        {
            return true;
        }
    }
    return false;
}

bool PatchManager::isPatch(struct dirent* file)
{
    return isType(file, patchExtension);
}

bool PatchManager::isCollection(struct dirent* file)
{
    return isType(file, patchCollectionExtension);
}

void* PatchManager::loadFile(FILE* file, size_t minSize)
{
    void* loadedFile = nullptr;
    if (file != NULL)
    {
        fseek(file, 0L, SEEK_END);
        u32 fileSize = ftell(file);
        fseek(file, 0L, SEEK_SET);

        if (fileSize < minSize)
            return nullptr;

        loadedFile = (void*)malloc(fileSize);
        if (loadedFile != nullptr)
        {
            fread(loadedFile, 1, fileSize, file);
        }
        fclose(file);
    }
    return loadedFile;
}

binPatch* PatchManager::loadPatch(FILE* file)
{
    binPatch* loadedPatch = (binPatch*)loadFile(file, sizeof(binPatch));
    return loadedPatch;
}

binPatchCollection* PatchManager::loadCollection(FILE* file)
{
    binPatchCollection* loadedCollection = (binPatchCollection*)loadFile(file, (sizeof(binPatch) + sizeof(binPatchCollection)));
    return loadedCollection;
}

int PatchManager::applyPatches()
{  
    PatchCollection* currentCollection;
    for (std::vector<PatchCollection*>::iterator it = loadedCollections.begin(); it != loadedCollections.end(); ++it)
    {
        currentCollection = (*it);
        if (currentCollection->isEnabled()&&checkCompatibility(currentCollection))
        {
            applyPatches(currentCollection->getAllPatches());
        }
    }

    applyPatches(&loadedPatches);

    return 0;
}

void PatchManager::applyPatches(vector<Patch*>* patchList)
{
    Patch* currentPatch = nullptr;
    for (std::vector<Patch*>::iterator it = patchList->begin(); it != patchList->end(); ++it)
    {
        currentPatch = (*it);
        if (currentPatch->isEnabled())
        {
            if (checkCompatibility(currentPatch))
            {
                findAndReplaceCode(currentPatch);
            }
        }
    }
}

int PatchManager::getNumberLoadedPatches()
{
    return loadedPatches.size();
}

void* PatchManager::getProcessAddress(u32 startAddress, u32 processNameSize, const char* processName)
{
    KCodeSet* code_set = FindTitleCodeSet(processName,processNameSize);
    if (code_set == nullptr)
        return nullptr;

    return (void*) FindCodeOffsetKAddr(code_set, startAddress);
}

int PatchManager::findAndReplaceCode(Patch* _patch)
{
    u32 numberOfReplaces = _patch->getNumberOfReplacements();
    u32 codeShift = _patch->getPatchOffset();

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
            destination = startAddressPointer + i + codeShift;
            memcpy(destination, patchCode.code, patchCode.codeSize);
            numberOfFounds++;
        }  
    }  
    return 0;
}

void PatchManager::replaceCodeAt(Patch* _patch)
{
    const u32 destination = _patch->getStartAddressProcess();

    const char* processName = _patch->getProcessName().c_str();
    u32 processNameSize = _patch->getProcessName().size();

    code patchCode = _patch->getPatchCode();

    u8 * destinationPointer = (u8 *)getProcessAddress(destination, processNameSize, processName);
    if (destinationPointer == nullptr)
        return;
            
    memcpy(destinationPointer, patchCode.code, patchCode.codeSize);

    return;
}

void PatchManager::findAndRepalaceString(Patch* _patch)
{
    u32 numberOfReplaces = _patch->getNumberOfReplacements(); 
    u32 stringCharacterOffset = _patch->getPatchOffset();

    const u32 startAddress = _patch->getStartAddressProcess();
    const u32 area = _patch->getSearchAreaSize();

    const char* processName = _patch->getProcessName().c_str();
    u32 processNameSize = _patch->getProcessName().size();

    code originalCode = _patch->getOriginalCode();
    code patchCode = _patch->getPatchCode();


    u8 * startAddressPointer = (u8 *)getProcessAddress(startAddress, processNameSize, processName);
    if (startAddressPointer == nullptr)
        return;
    u8 * destination = nullptr;
    u32 numberOfFounds = 0;

    for (u32 i = 0; i < area && numberOfFounds <= numberOfReplaces; i++)
    {
        //check for the original code position
        bool found = true;
        for (u32 x = 0; x < originalCode.codeSize && found == true; x += 1 + stringCharacterOffset)
        {
            if ((*((startAddressPointer + i + x)) != *(&originalCode.code[x])))
                found = false;
        }
        if (found == true)
        {
            //Apply patches, if the addresses was found  /*TODO*/
            destination = startAddressPointer + i;
            memcpy(destination, patchCode.code, patchCode.codeSize);
            numberOfFounds++;
        }
    }
    return;
}


//todo manage settings
bool ignoreFirmware = true;
bool ignoreKernel = true;
bool ignoreRegion = true;
bool ignoreFirmwareCollection = true;
bool ignoreKernelCollection = true;
bool ignoreRegionCollection = true;

bool PatchManager::checkCompatibility(Patch* _patch)
{
    if (_patch == nullptr)
        return false;

    bool compatibleFirmware = true;
    bool compatibleKernel = checkKernelVersion(_patch->getMinKernelVersion(), _patch->getMaxKernelVersion());
    bool compatibleRegion = true;

    bool compatible = (compatibleFirmware || ignoreFirmware)
        && (compatibleKernel || ignoreKernel)
        && (compatibleRegion || ignoreRegion);

    return compatible;
}

bool PatchManager::checkCompatibility(PatchCollection* _collection)
{
    if (_collection == nullptr)
        return false;

    bool compatibleFirmware = true;
    bool compatibleKernel = checkKernelVersion(_collection->getMinKernelVersion(), _collection->getMaxKernelVersion());
    bool compatibleRegion = true;

    bool compatible = (compatibleFirmware || ignoreFirmware)
        && (compatibleKernel || ignoreKernel)
        && (compatibleRegion || ignoreRegion);

    return compatible;
}

bool PatchManager::checkKernelVersion(kernelVersion min, kernelVersion max)
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
            maxBool = true;
        else if (max.minor == kernel->minor)
        {
            if (max.revision >= kernel->revision)
                maxBool = true;
        }

    }
    return (minBool && maxBool);
}