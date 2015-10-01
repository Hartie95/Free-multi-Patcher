#include <stdio.h>
#include <string.h>
#include <ctrcommon/fs.hpp>
#include <stdlib.h>

#include "patchCollections.h"

using namespace std;

binPatch* loadPatch(void* ptr)
{
    binPatch* loadedPatch = nullptr;
    if (ptr != NULL && ptr!=nullptr)
    {
        u32 size = ((binPatch*)ptr)->patchSize;
        if (size < sizeof(binPatch))
            return nullptr;

        loadedPatch = (binPatch*)malloc(size);
        if (loadedPatch != nullptr)
        {
            memcpy(loadedPatch, ptr, size);
        }
    }
    return loadedPatch;
}

PatchCollection::PatchCollection(binPatchCollection* collection)
{
    this->collectionName.assign(collection->binaryData, collection->collectionNameSize);
    this->description.assign(&(collection->binaryData[collection->collectionNameSize]), collection->descriptionSize);

    this->minKernelVersion = collection->minKernelVersion;
    this->maxKernelVersion = collection->maxKernelVersion;
    this->minFirmwareVersion = collection->minFirmwareVersion;
    this->maxFirmwareVersion = collection->maxFirmwareVersion;
    this->devicesSupported = collection->devicesSupported;
    this->regionsSupported = collection->regionsSupported;
    this->nandCompability = collection->nandCompability;

    this->numberOfPatches = collection->numberOfPatches;

    u32 patchPosition = collection->collectionNameSize + collection->descriptionSize+1;
    for (u32 i = 0; i < this->numberOfPatches; i++)
    {
        binPatch *patchPtr = loadPatch(&collection->binaryData[patchPosition]);
        if (patchPtr != nullptr)
        {
            Patch* tmpPatch = new Patch(patchPtr);
            if (tmpPatch != nullptr)
            {
                this->collectionPatches.push_back(tmpPatch);
            }
            patchPosition += patchPtr->patchSize;
            free(patchPtr);
        }
        else
            break;
    }
   
    this->changeStatus(true);
}

PatchCollection::~PatchCollection()
{

}


string PatchCollection::getCollectionName()
{
    return this->collectionName;
}

string PatchCollection::getDescription()
{
    return this->description;
}


kernelVersion PatchCollection::getMinKernelVersion()
{
    return this->minKernelVersion;
}

kernelVersion PatchCollection::getMaxKernelVersion()
{
    return this->maxKernelVersion;
}

firmwareVersion PatchCollection::getMinFirmwareVersion()
{
	return this->minFirmwareVersion;
}

firmwareVersion PatchCollection::getMaxFirmwareVersion()
{
	return this->maxFirmwareVersion;
}

devices PatchCollection::getDevicesSupported()
{
    return this->devicesSupported;
}

regions PatchCollection::getRegionsSupported()
{
    return this->regionsSupported;
}

nands PatchCollection::getNandCompability()
{
    return this->nandCompability;
}

u32     PatchCollection::getNumberOfPatches()
{
    return this->numberOfPatches;
}

std::vector<Patch*>* PatchCollection::getAllPatches()
{
    return &this->collectionPatches;
}

bool    PatchCollection::changeStatus()
{
    return this->changeStatus(!this->enabled);
}

bool    PatchCollection::changeStatus(bool status)
{
    this->enabled = status;
    return this->enabled;
}

bool    PatchCollection::isEnabled()
{
    return this->enabled;
}

void createDefaultCollections()
{
    FILE *file;

    // create the default patch files
    // patch Homemenu to show out of region applications
    // 9.0.0 Address: 0x00101B8C;
    char menuBytes[] =/*patchName*/  "region patch menu"
        /*description*/ "Patches the home menu to show out of region games"
        /*processname*/ "menu"
        /*OriginalCode*/"\x00\x00\x55\xE3\x01\x10\xA0\xE3\x11\x00\xA0\xE1\x03\x00\x00\x0A"
        /*patchBegin*/  "\x01\x00\xA0\xE3\x70\x80\xBD\xE8";

    u32 menuPatchSize = sizeof(binPatch) + sizeof(menuBytes);
    binPatch* menuPatch = (binPatch*)malloc(menuPatchSize);

    menuPatch->version = 0x00;
    menuPatch->patchSize = menuPatchSize;
    menuPatch->patchNameSize = 17;
    menuPatch->descriptionSize = 49;
    menuPatch->processNameSize = 4;
    menuPatch->originalcodeSize = 16;
    menuPatch->patchcodeSize = 8;
    menuPatch->processType = 0x01;
    menuPatch->minKernelVersion = { 0x00, 0x00, 0x00, 0x00 };
    menuPatch->maxKernelVersion = { 0xFF, 0xFF, 0xFF, 0xFF };
    menuPatch->minFirmwareVersion = { 4, 0, 0, 0 };
    menuPatch->maxFirmwareVersion = { 9, 9, 0, 255 };
    menuPatch->devicesSupported = {1,1,1,1,1,0};
    menuPatch->regionsSupported = {1,1,1,1,1,1,1,0};
    menuPatch->nandCompability = { 1, 1, 0 };
    menuPatch->patchType = 0x00;
    menuPatch->startAddressProcess = 0x00100000;
    menuPatch->startAddressGlobal = 0x26960000;
    menuPatch->searchAreaSize = 0x001A0000;
    menuPatch->patchOffset = 0x00;
    menuPatch->numberOfReplacements = 0x01;
    menuPatch->patchOffset = 0;

    memcpy(menuPatch->binaryData, menuBytes, sizeof(menuBytes));
    

    // patch NS to return update doesnt need to be installed intead of CVer not found error code after Update Check
    // 9.0.0 Addresses: 0x00102acc, 0x001894f4; 
    char nsBytes[] = /*patchName*/   "region patch ns"
        /*description*/ "Patches the ns to ignore out of region updates"
        /*processname*/ "ns"
        /*OriginalCode*/"\x0C\x18\xE1\xD8"
        /*patchBegin*/  "\x0B\x18\x21\xC8";

    u32 nsPatchSize = sizeof(binPatch) + sizeof(nsBytes);
    binPatch* nsPatch = (binPatch*)malloc(nsPatchSize);

    nsPatch->version = 0x00;
    nsPatch->patchSize = nsPatchSize;
    nsPatch->patchNameSize = 15;
    nsPatch->descriptionSize = 46;
    nsPatch->processNameSize = 2;
    nsPatch->originalcodeSize = 4;
    nsPatch->patchcodeSize = 4;
    nsPatch->processType = 0x01;
    nsPatch->minKernelVersion = { 0, 4, 33, 2 };
    nsPatch->maxKernelVersion = { 255, 1, 50, 2 };
    nsPatch->minFirmwareVersion = { 0x00, 0x00, 0x00, 0x00 };
    nsPatch->maxFirmwareVersion = { 0xFF, 0xFF, 0xFF, 0xFF };
    nsPatch->devicesSupported = { 1, 1, 1, 1, 1, 0 };
    nsPatch->regionsSupported = { 1, 1, 1, 1, 1, 1, 1, 0 };
    nsPatch->nandCompability = { 1, 1, 0 };
    nsPatch->patchType = 0x00;
    nsPatch->patchOffset = 0x00;
    nsPatch->startAddressProcess = 0x00018000;
    nsPatch->startAddressGlobal = 0x26A00000;
    nsPatch->searchAreaSize = 0x00050000;
    nsPatch->numberOfReplacements = 0x02;
    nsPatch->patchOffset = 0;

    memcpy(nsPatch->binaryData, nsBytes, sizeof(nsBytes));
    

    char collectionBytes[] =/*patchName*/  "regionfree collection"
        /*description*/ "Patches the home menu and ns to allow the usage of out of region games";
    
    u32 regionfreeSize = sizeof(binPatchCollection) + sizeof(collectionBytes) + menuPatchSize + nsPatchSize;
    binPatchCollection* regionfree = (binPatchCollection*)malloc(regionfreeSize);

    regionfree->version = 0x00;
    regionfree->collectionNameSize = 21;
    regionfree->descriptionSize = 70;
    regionfree->numberOfPatches = 2;
    regionfree->processType = 0x01;
    regionfree->minKernelVersion = { 0, 4, 33, 2 };
    regionfree->maxKernelVersion = { 255, 1, 50, 2 };
    regionfree->minFirmwareVersion = { 4, 0, 0, 0 };
    regionfree->maxFirmwareVersion = { 9, 9, 0, 255 };
    regionfree->devicesSupported = { 1, 1, 1, 1, 1, 0 };
    regionfree->regionsSupported = { 1, 1, 1, 1, 1, 1, 1, 0 };
    regionfree->nandCompability = { 1, 1, 0 };

    memcpy(regionfree->binaryData, collectionBytes, sizeof(collectionBytes));
    memcpy(&regionfree->binaryData[sizeof(collectionBytes)], menuPatch, menuPatchSize);
    memcpy(&regionfree->binaryData[sizeof(collectionBytes) + menuPatchSize], nsPatch, nsPatchSize);

    string collectionFileName = "regionFree" + patchCollectionExtension;

    string filepath = patchesFolder + collectionFileName;
    file = fopen(filepath.c_str(), "wb");
    if (file == NULL)
    {
        file = fopen(filepath.c_str(), "cb");
    }
    fwrite(regionfree, 1, regionfreeSize, file);
    fclose(file);


    free(nsPatch);
    free(menuPatch);
    free(regionfree);
}