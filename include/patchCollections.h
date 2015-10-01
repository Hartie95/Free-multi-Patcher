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
    devices devicesSupported;
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
	firmwareVersion     getMinFirmwareVersion();
	firmwareVersion     getMaxFirmwareVersion();
    devices getDevicesSupported();
    regions getRegionsSupported();
    nands   getNandCompability();

    u32     getNumberOfPatches();
    std::vector<Patch*>* getAllPatches();

    bool    changeStatus();
    bool    changeStatus(bool status);
    bool    isEnabled();
};