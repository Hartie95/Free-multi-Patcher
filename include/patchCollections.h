#pragma once

#include <3ds.h>
#include <vector>

#include "patches.h"
#include "binaryLayouts.h"

void createDefaultCollections();

class PatchCollection
{
private:
    std::string  collectionName;
    std::string  description;

    kernelVersion     minKernelVersion;
    kernelVersion     maxKernelVersion;
    firmwareVersion   minFirmwareVersion;
    firmwareVersion   maxFirmwareVersion;
    regions regionsSupported;
    nands   nandCompability;

    u32     numberOfPatches;

    std::vector<Patch*> collectionPatches;

    bool    enabled;

public:
    PatchCollection(binPatchCollection* collection);
    ~PatchCollection();
    std::string  getCollectionName();
    std::string  getDescription(); 

    kernelVersion     getMinKernelVersion();
    kernelVersion     getMaxKernelVersion();
    regions getRegionsSupported();
    nands   getNandCompability();

    u8      getPatchType();
    u32     getNumberOfPatches();
    Patch*  getPatch(int position);
    std::vector<Patch*>* getAllPatches();

    bool    changeStatus();
    bool    changeStatus(bool status);
    bool    isEnabled();
};