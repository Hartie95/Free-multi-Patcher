#include <ctrcommon/input.hpp>
//#include <ctrcommon/gpu.hpp>
#include <ctrcommon/platform.hpp>
#include <ctrcommon/service.hpp>
#include <3ds.h>
#include "constants.h"
#include <stdio.h>
#include "kernel11.h"
#include "kobjects.h"
#include "menu.h"
#include "patchManager.h"
#include "menuManager.h"

using namespace std;

#define log(...) fprintf(stderr, __VA_ARGS__)

int main(int argc, char **argv) {
  if(!platformInit(0)) {
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
    menu->drawMenu();
  }

  HB_FlushInvalidateCache(); // Just to be sure!
  platformCleanup();
  return 0;  
}