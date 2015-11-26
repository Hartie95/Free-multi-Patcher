#include <3ds.h>

#include <malloc.h>
#include <ctrcommon/input.hpp>
#include <ctrcommon/platform.hpp>

#include "patchManager.h"
#include "menuManager.h"
#include "updater.h"
#include "device.h"
#include "settings.h"
#include "helpers.h"

#include "kernel11.h"

using namespace std;

PatchManager* patchManager;
MenuManager* menuManager;

int applyPatches()
{
    return patchManager->applyPatches();
}

int test()
{
	checkForUpdate();
	return 0;
}

int init(int argc)
{
    if (!platformInit(argc)) {
        return 1;
    }
    httpcInit();
    newsInit();
	cfguInit();

	checkFolder(applicationFolder);
    SaveVersionConstants();
	initDeviceInformations();
	initGlobalSettings();
    
    return 0;
}

int cleanup()
{
    HB_FlushInvalidateCache(); // Just to be sure!

    httpcExit();
    newsExit();
	cfguExit();

    platformCleanup();
    return 0;
}

int main(int argc, char **argv) {
    if(init(argc)!=0) {
    return 0;
    }
   
    short exitLoop=false;
    menuManager = new MenuManager();

    patchManager = new PatchManager();

    patchManager->createPatchPage(menuManager);
	globalSettings->createMenuPage(menuManager);

    while(platformIsRunning()&&exitLoop==false) 
    {
        //Todo: replace with switch case
        exitLoop = menuManager->ManageInput();

        if(inputIsPressed(BUTTON_SELECT))
        {
			test();
        }

        if (inputIsPressed(BUTTON_START)) 
		{
            KernelBackdoor(&applyPatches);
            exitLoop = true;
        }
		
		menuManager->drawMenu();
    }

    cleanup();

    return 0;  
}
