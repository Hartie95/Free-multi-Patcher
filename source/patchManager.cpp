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

void* stringcpy(void* destination, void* string, size_t stringSize, u32 offset)
{
    if (destination == nullptr || string == nullptr)
        return nullptr;

    if (stringSize == 0)
        return destination;

    u8* destinationByte = (u8*)destination;
    u8* stringByte = (u8*)string;

    for (u32 i = 0; i < stringSize; i++)
    {
        *(destinationByte) = *stringByte;
        stringByte++;
        destinationByte += 1 + offset;
    }
    return destination;
}

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
        if (currentCollection == nullptr)
            continue;
        
        if (!currentCollection->isEnabled() || !checkCompatibility(currentCollection))
            continue;

        vector<Patch*>* patchList = currentCollection->getAllPatches();
        applyPatches(patchList);
    }

    applyPatches(&loadedPatches);

    return 0;
}

void PatchManager::applyPatches(vector<Patch*>* patchList)
{
    if (patchList == nullptr)
        return;

    Patch* currentPatch = nullptr;
    for (std::vector<Patch*>::iterator it = patchList->begin(); it != patchList->end(); ++it)
    {
        currentPatch = (*it);
        if (currentPatch == nullptr || currentPatch==NULL)
            continue;
        
        if (!currentPatch->isEnabled() || !checkCompatibility(currentPatch))
            continue;

        switch (currentPatch->getPatchType())
        {
        case 0:
            this->findAndReplaceCode(currentPatch);
            break;
        case 1:
            this->replaceCodeAt(currentPatch);
            break;
        case 2:
            this->usePointerAndReplaceCode(currentPatch);
            break;
        case 3:
            this->findAndReplaceString(currentPatch);
            break;
        case 4:
            this->replaceStringAt(currentPatch);
            break;
        case 5:
            this->usePointerAndReplaceString(currentPatch);
            break;
        default:
            break;
        }
    }
}

void* PatchManager::getProcessAddress(u32 startAddress, u32 processNameSize, const char* processName)
{
    KCodeSet* code_set = FindTitleCodeSet(processName,processNameSize);
    if (code_set == nullptr)
        return nullptr;

    return (void*) FindCodeOffsetKAddr(code_set, startAddress);
}

void PatchManager::findAndReplaceCode(Patch* _patch)
{
    if (_patch == nullptr)
        return;

    u32 numberOfReplaces = _patch->getNumberOfReplacements();
    u32 codeShift = _patch->getPatchOffset();

    u32 area = _patch->getSearchAreaSize();  

    string processName = _patch->getProcessName();

    code originalCode = _patch->getOriginalCode();
    code patchCode = _patch->getPatchCode();
    
    if (numberOfReplaces<1 || area<1 || originalCode.codeSize < 1 || patchCode.codeSize <1)
        return;
    
    u8 * startAddressPointer = (u8 *)getProcessAddress(_patch->getStartAddressProcess(), processName.size(), processName.c_str());
    
    if(startAddressPointer==nullptr)
        return;
    
    u8 * destination=nullptr;
    u32 numberOfFounds=0;
    
    for(u32 i = 0; i < area && numberOfFounds<numberOfReplaces; i+=4)
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

void PatchManager::usePointerAndReplaceCode(Patch* _patch)
{
    const u32 pointerAddress = _patch->getStartAddressProcess();

    const char* processName = _patch->getProcessName().c_str();
    u32 processNameSize = _patch->getProcessName().size();

    code patchCode = _patch->getPatchCode();
    u32 internalAddress = *(u32 *)getProcessAddress(pointerAddress, processNameSize, processName);
    u8 * destinationPointer = (u8 *)getProcessAddress(internalAddress, processNameSize, processName);
    if (destinationPointer == nullptr)
        return;

    memcpy(destinationPointer, patchCode.code, patchCode.codeSize);

    return;
}

void PatchManager::findAndReplaceString(Patch* _patch)
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
            stringcpy(destination, patchCode.code, patchCode.codeSize,stringCharacterOffset);
            numberOfFounds++;
        }
    }
    return;
}

void PatchManager::replaceStringAt(Patch* _patch)
{
    u32 stringCharacterOffset = _patch->getPatchOffset();
    const u32 destination = _patch->getStartAddressProcess();

    const char* processName = _patch->getProcessName().c_str();
    u32 processNameSize = _patch->getProcessName().size();

    code patchCode = _patch->getPatchCode();

    u8 * destinationPointer = (u8 *)getProcessAddress(destination, processNameSize, processName);
    if (destinationPointer == nullptr)
        return;

    stringcpy(destinationPointer, patchCode.code, patchCode.codeSize, stringCharacterOffset);

    return;
}


void PatchManager::usePointerAndReplaceString(Patch* _patch)
{
    u32 stringCharacterOffset = _patch->getPatchOffset();
    const u32 pointerAddress = _patch->getStartAddressProcess();

    const char* processName = _patch->getProcessName().c_str();
    u32 processNameSize = _patch->getProcessName().size();

    code patchCode = _patch->getPatchCode();
    u32 internalAddress = *(u32 *)getProcessAddress(pointerAddress, processNameSize, processName);
    u8 * destinationPointer = (u8 *)getProcessAddress(internalAddress, processNameSize, processName);
    if (destinationPointer == nullptr)
        return;

    stringcpy(destinationPointer, patchCode.code, patchCode.codeSize, stringCharacterOffset);

    return;
}


//todo manage settings
bool ignoreFirmware = true;
bool ignoreKernel = true;
bool ignoreRegion = true;
bool ignoreDeviceType = true;
bool ignoreFirmwareCollection = true;
bool ignoreKernelCollection = true;
bool ignoreRegionCollection = true;
bool ignoreDeviceTypeCollection = true;

bool PatchManager::checkCompatibility(Patch* _patch)
{
    if (_patch == nullptr)
        return false;

    bool compatibleDevice = isDeviceTypeSupported(_patch->getDevicesSupported());
    bool compatibleFirmware = checkFirmwareVersion(_patch->getMinFirmwareVersion(), _patch->getMaxFirmwareVersion());
    bool compatibleKernel = checkKernelVersion(_patch->getMinKernelVersion(), _patch->getMaxKernelVersion());
    bool compatibleRegion = isRegionSupported(_patch->getRegionsSupported());

    bool compatible = (compatibleDevice || ignoreDeviceType)
        && (compatibleFirmware || ignoreFirmware)
        && (compatibleKernel || ignoreKernel)
        && (compatibleRegion || ignoreRegion);

    return compatible;
}

bool PatchManager::checkCompatibility(PatchCollection* _collection)
{
    if (_collection == nullptr)
        return false;

    bool compatibleDevice = isDeviceTypeSupported(_collection->getDevicesSupported());
    bool compatibleFirmware = checkFirmwareVersion(_collection->getMinFirmwareVersion(), _collection->getMaxFirmwareVersion());
    bool compatibleKernel = checkKernelVersion(_collection->getMinKernelVersion(), _collection->getMaxKernelVersion());
    bool compatibleRegion = isRegionSupported(_collection->getRegionsSupported());

    bool compatible = (compatibleDevice || ignoreDeviceTypeCollection)
        && (compatibleFirmware || ignoreFirmwareCollection)
        && (compatibleKernel || ignoreKernelCollection)
        && (compatibleRegion || ignoreRegionCollection);

    return compatible;
}

bool checkMin(u8 numbers[], u32 numbersSize, u8 minNumbers[], u32 minNumbersSize)
{
    if (numbersSize != minNumbersSize)
        return false;

    if (numbers == nullptr || minNumbers == nullptr)
        return false;

    for (u32 i = 0; i < numbersSize;i++)
    {
        if (numbers[i] > minNumbers[i])
            return true;
        if (numbers[i] < minNumbers[i])
            return false;
    }
    return true;
}

bool checkMax(u8 numbers[], u32 numbersSize, u8 maxNumbers[], u32 maxNumbersSize)
{
    if (numbersSize != maxNumbersSize)
        return false;

    if (numbers == nullptr || maxNumbers == nullptr)
        return false;

    for (u32 i = 0; i < numbersSize; i++)
    {
        if (numbers[i] < maxNumbers[i])
            return true;
        if (numbers[i] > maxNumbers[i])
            return false;
    }
    return true;
}

bool PatchManager::checkKernelVersion(kernelVersion min, kernelVersion max)
{
    u8 kernelVersionArray[3] = { device.kernelversion.major, device.kernelversion.minor, device.kernelversion.revision};
    u8 minVersionArray[3] = { min.major, min.minor, min.revision };
    u8 maxVersionArray[3] = { max.major, max.minor, max.revision };
    return checkMin(kernelVersionArray, 3, minVersionArray, 3) && checkMax(kernelVersionArray, 3, maxVersionArray, 3);
    
}

bool PatchManager::checkFirmwareVersion(firmwareVersion min, firmwareVersion max)
{
	u8 cverVersionArray[3] = { device.firmwareversion.major, device.firmwareversion.minor, device.firmwareversion.revision };
	u8 minVersionArray[3] = { min.major, min.minor, min.revision };
	u8 maxVersionArray[3] = { max.major, max.minor, max.revision };
	bool ret= checkMin(cverVersionArray, 3, minVersionArray, 3) && checkMax(cverVersionArray, 3, maxVersionArray, 3);
	ret= ret&checkMin(&device.firmwareversion.nver, 1, &min.nver, 1) && checkMax(&device.firmwareversion.nver, 1, &max.nver, 1);
	return ret;
}

bool PatchManager::isRegionSupported(regions _regions)
{
	bool supported = false;
	switch (device.region)
	{
	case 0:
		supported = _regions.japan;
		break;
	case 1:
		supported = _regions.northAmerica;
		break;
	case 2:
		supported = _regions.europe;
		break;
	case 3:
		supported = _regions.australia;
		break;
	case 4:
		supported = _regions.china;
		break;
	case 5:
		supported = _regions.korea;
		break;
	case 6:
		supported = _regions.taiwan;
		break;
	default:
		break;
	}
	return supported;

}

bool PatchManager::isDeviceTypeSupported(devices _devices)
{
    bool supported = false;
    switch (device.modelID)
    {
        case 0:
            supported = _devices.old3DS;
            break;
        case 1:
            supported = _devices.old3DSXL;
            break;
        case 3:
            supported = _devices.old2DS;
            break;
        case 2:
            supported = _devices.new3DS;
            break;
        case 4:
            supported = _devices.new3DSXL;
            break;
        default:
            break;
    }
    return supported;
}
