#include <ctrcommon/input.hpp>
//#include <ctrcommon/gpu.hpp>
#include <ctrcommon/platform.hpp>
#include <ctrcommon/service.hpp>
#include <3ds.h>
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>

#include "kernel11.h"
#include "kobjects.h"
#include "menu.h"
#include "patchManager.h"
#include "menuManager.h"
#include <sstream>

using namespace std;


PatchManager* patchManager;
MenuManager* menuManager;

int applyPatches()
{
    return patchManager->applyPatches();
}

int checkVersion()
{
    int onlineVersion=version;
    httpcContext context;
    Result ret=0;
    u32 statuscode=0;
    char url[] = "http://fmp.hartie95.de/version.txt"; 

    httpcInit();

    ret = httpcOpenContext(&context, url , 0); 
    if(ret==0)
    {
        ret = httpcBeginRequest(&context);
        if(ret==0)
        {
            ret = httpcGetResponseStatusCode(&context, &statuscode, 0); 
            if(ret==0&&statuscode==200)
            {
                u32 contentsize=0; 
                ret=httpcGetDownloadSizeState(&context, NULL, &contentsize);
                if(ret==0)
                {
                    u8* buf=(u8*)malloc(contentsize);
                    memset(buf, 0, contentsize);
                    ret = httpcDownloadData(&context, buf, contentsize, NULL);
                    if(ret==0) 
                    {
                        char versionString[contentsize+1];
                        for(u32 i = 0; i<contentsize;i++)
                        {  
                            versionString[i] = (char)buf[i];
                        }
                        versionString[contentsize]='\0';
                        onlineVersion = (int)strtol(versionString, NULL, 16);
                        httpcCloseContext(&context);
                    }
                    free(buf);
                }
            }
        }
    }
    httpcExit();
    return onlineVersion;
}

int checkForUpdate()
{   
    u32 onlineVersion=checkVersion(); 
    if(version<onlineVersion)
    {
        const char title8[]={"A New Version is available"};
        const char message8[]={"A new version is available, please download it from http://fmp.hartie95.de/releases"};
        u16 title16[sizeof(title8)];
        u16 message16[sizeof(message8)];
        for (u32 i=0;i<sizeof(title8);i++)
        {
            title16[i]=(u16) title8[i];
        }
        for (u32 i=0;i<sizeof(message8);i++)
        {
            message16[i]=(u16) message8[i];
        }
        newsInit();
        NEWSU_AddNotification(title16,sizeof(title8), message16, sizeof(message8),nullptr, 0, false);
        newsExit();
    }
    return onlineVersion;
}


int main(int argc, char **argv) {
  if(!platformInit(argc)) {
    return 0;
  }

  //loadSettings(); 
   
  short exitLoop=false;
  initCfgu();
  patchManager= new PatchManager();
  SaveVersionConstants();
  menuManager = new MenuManager();
  Menu* testMenu = new Menu(menuManager, menuManager->getMainPage());
  Menu* testMenu2 = new Menu(menuManager, testMenu);
  
  patchManager->createPatchPage(menuManager);

  bool testbool=true;
  YesNoMenuEntry* testEntryYesNo=new YesNoMenuEntry(&testbool,"testEntryYesNo","This is a testButton\ntestbutton");
  testMenu->addEntry((MenuEntry*)testEntryYesNo);

  menuManager->addPage(testMenu, menuManager->getMainPage(), "testpage");
  menuManager->addPage(testMenu2, testMenu, "testpage2");

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

  HB_FlushInvalidateCache(); // Just to be sure!
  exitCfgu();
  platformCleanup();
  return 0;  
}