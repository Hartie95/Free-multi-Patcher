#pragma once
#include <3ds.h>

#include <sys/dirent.h>
#include "patches.h"
#include "patchCollections.h"
#include "menuManager.h"
#include "device.h"
#include "settings.h"


class PatchManager
{
private:
    std::vector<Patch*> loadedPatches;
    std::vector<PatchCollection*> loadedCollections;
	Settings* patchSettings;

    bool isType(struct dirent* file, std::string extension);
    bool isPatch(struct dirent* file);
    bool isCollection(struct dirent* file);

    void applyPatches(std::vector<Patch*>* patchList);
    void findAndReplaceCode(Patch* _patch);
    void replaceCodeAt(Patch* _patch);
    void usePointerAndReplaceCode(Patch* _patch);
    void findAndReplaceString(Patch* _patch);
    void replaceStringAt(Patch* _patch);
    void usePointerAndReplaceString(Patch* _patch);

    void* getProcessAddress(u32 startAddress, u32 processNameSize, const char* processName);
    void* getProcessAddress(u32 startAddress, u32 processNameSize, char* processName);

    bool checkCompatibility(Patch* _patch);
    bool checkCompatibility(PatchCollection* _collection);

    bool checkKernelVersion(kernelVersion min, kernelVersion max);
	bool checkFirmwareVersion(firmwareVersion min, firmwareVersion max);
	bool isRegionSupported(regions _regions);
    bool isDeviceTypeSupported(devices _devices);

public:
    PatchManager();
    void checkPatchFolder();
    void loadPatchFiles();
    int createPatchPage(MenuManager* menuManager);

    binPatch* loadPatch(FILE* file);
    binPatchCollection* loadCollection(FILE* file);

    int applyPatches(); 
	int saveSettings();
	int loadSettings();
};
