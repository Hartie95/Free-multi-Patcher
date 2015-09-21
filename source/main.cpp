#include <3ds.h>

#include <ctrcommon/input.hpp>
#include <ctrcommon/platform.hpp>
#include <ctrcommon/service.hpp>

#include "patchManager.h"
#include "menuManager.h"
#include "updater.h"

#include "kernel11.h"

using namespace std;

/*Todo  move update stuff to own class*/
PatchManager* patchManager;
MenuManager* menuManager;

int applyPatches()
{
    return patchManager->applyPatches();
}

int init(int argc)
{
    if (!platformInit(argc)) {
        return 1;
    }
    httpcInit();
    newsInit();
    initCfgu();

    SaveVersionConstants();

    CFGU_GetSystemModel(&modelID);

    u32 kernelValue = osGetKernelVersion();
    kernelversion = *(kernelVersion*)&kernelValue;
    
    return 0;
}

int cleanup()
{
    HB_FlushInvalidateCache(); // Just to be sure!

    httpcExit();
    newsExit();
    exitCfgu();

    platformCleanup();

    return 0;
}


int main(int argc, char **argv) {
  
  if(init(argc)!=0) {
    return 0;
  }
  
  //loadSettings(); 
   
  short exitLoop=false;
  patchManager= new PatchManager();
  menuManager = new MenuManager();
  
  patchManager->createPatchPage(menuManager);

  while(platformIsRunning()&&exitLoop==false) 
  {
    //Todo: replace with switch case
    exitLoop = menuManager->ManageInput();

    if(inputIsPressed(BUTTON_SELECT))
    {
        checkForUpdate();
    }

    if (inputIsPressed(BUTTON_START)) {
        KernelBackdoor(&applyPatches);
        exitLoop = true;
    }
    menuManager->drawMenu();
  }

  cleanup();

  return 0;  
}
