#include <3ds.h>

#include <malloc.h>
#include <citrus/hid.hpp>
#include <citrus/core.hpp>

#include "patchManager.h"
#include "menuManager.h"
#include "updater.h"
#include "device.h"
#include "settings.h"
#include "helpers.h"

#include "kernel11.h"

using namespace std;
using namespace ctr;

PatchManager* patchManager;
MenuManager* menuManager;
Updater* updater;

int applyPatches()
{
    return patchManager->applyPatches();
}

int test()
{
	return 0;
}

int init(int argc)
{
    if (!core::init(argc)) {
        return 1;
    }
    httpcInit();
    newsInit();
	cfguInit();
	pmInit();

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
	pmExit();

    core::exit();
    return 0;
}

int main(int argc, char **argv) {
    if(init(argc)!=0) {
    return 0;
    }
   
    bool exitLoop=false;
    menuManager = new MenuManager(&exitLoop);

    patchManager = new PatchManager();

    patchManager->createPatchPage(menuManager);
	globalSettings->createMenuPage(menuManager);
	updater = new Updater(menuManager,&exitLoop);

    while(core::running()&&exitLoop==false) 
    {
        //Todo: replace with switch case
        menuManager->ManageInput();

        if(hid::pressed(hid::BUTTON_SELECT))
        {
			test();
        }

        if (hid::pressed(hid::BUTTON_START))
		{
            KernelBackdoor(&applyPatches);
            exitLoop = true;
        }
		
		menuManager->drawMenu();
    }

    cleanup();

    return 0;  
}
