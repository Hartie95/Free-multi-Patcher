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

using namespace std;

int checkVersion()
{
    httpcInit();
    int onlineVersion=version;

    httpcContext context;
    Result ret=0;
    u32 statuscode=0;
    char url[] = "http://fmp.hartie95.de/version.txt"; 

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

#define log(...) fprintf(stderr, __VA_ARGS__)

int main(int argc, char **argv) {
  if(!platformInit(argc)) {
    return 0;
  }

  //loadSettings(); 
   
  short exitLoop=false;

  initPatches();
  SaveVersionConstants();
  MenuManager* menu=new MenuManager();
  Menu* testMenu=new Menu(menu,menu->getMainPage());
  Menu* testMenu2=new Menu(menu,testMenu);
  
  createPatchPage(menu);

  bool testbool=true;
  YesNoMenuEntry* testEntryYesNo=new YesNoMenuEntry(&testbool,"testEntryYesNo","This is a testButton\ntestbutton");
  testMenu->addEntry((MenuEntry*)testEntryYesNo);

  menu->addPage(testMenu,menu->getMainPage(),"testpage");
  menu->addPage(testMenu2,testMenu,"testpage2");

  while(platformIsRunning()&&exitLoop==false) {
    //Todo: replace with switch case
    exitLoop=menu->ManageInput();
    if(inputIsPressed(BUTTON_SELECT))
    {
        u32 value=checkForUpdate();
        char intStr[8];
        itoa(value,intStr,16);
        string versionstring=string(intStr);
        Menu* versionMenu=new Menu(menu,menu->getMainPage());
        menu->addPage(versionMenu,menu->getMainPage(),versionstring);
    }
    menu->drawMenu();
  }

  HB_FlushInvalidateCache(); // Just to be sure!
  platformCleanup();
  return 0;  
}