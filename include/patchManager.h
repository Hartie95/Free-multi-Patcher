#pragma once
#include <3ds.h>

#include <sys/dirent.h>
#include "patches.h"
#include "patchCollections.h"
#include "menuManager.h"


class PatchManager
{
private:
    bool isType(struct dirent* file, std::string extension);
    void* loadFile(FILE* file, size_t minSize);
    bool isPatch(struct dirent* file);
    bool isCollection(struct dirent* file);

    void applyPatches(std::vector<Patch*>* patchList);
    int findAndReplaceCode(Patch* _patch);
    void replaceCodeAt(Patch* _patch);
    void findAndRepalaceString(Patch* _patch);
    void* getProcessAddress(u32 startAddress, u32 processNameSize, const char* processName);

    
    void* getProcessAddress(u32 startAddress, u32 processNameSize, char* processName);

    bool checkCompatibility(Patch* _patch);
    bool checkCompatibility(PatchCollection* _collection);

    bool checkKernelVersion(kernelVersion min, kernelVersion max);
protected:
public:
    PatchManager();
    void checkPatchFolder();
    void loadPatchFiles();
    int createPatchPage(MenuManager* menuManager);


    binPatch* loadPatch(FILE* file);
    binPatchCollection* loadCollection(FILE* file);

    int  applyPatches();

    int getNumberLoadedPatches();   
};
