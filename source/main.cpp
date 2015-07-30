#include <ctrcommon/input.hpp>
#include <ctrcommon/gpu.hpp>
#include <ctrcommon/platform.hpp>
#include <ctrcommon/service.hpp>
#include <ctrcommon/fs.hpp> 
#include <3ds.h>
#include "constants.h"
#include <stdio.h>
#include "patches.h"
#include "kernel11.h"
#include "kobjects.h"
#include "menu.h"
#include <sys/dirent.h>

using namespace std;

#define log(...) fprintf(stderr, __VA_ARGS__)

static const string settingsFolder="sdmc:/fmp/";
static const string settingsFileName="settings.cfg";

bool applyPatches(bool patchlist[]){
  SaveVersionConstants();
  //PatchSrvAccess();
  gputDrawString("srv patched", (gpuGetViewportWidth() - gputGetStringWidth("srv patched", 8)) / 2, 130, 8, 8, 0 ,0 ,0);

  if(patchlist[ESHOPSPOOF]==true)
  {
    if(!KernelBackdoor(patchNimEshop)){
      gputDrawString("patch applied!", (gpuGetViewportWidth() - gputGetStringWidth("patch applied!", 8)) / 2, 70, 8, 8, 0 ,0 ,0);
    }
  }

  if(patchlist[REGIONFREE]==true)
  {
    if(!KernelBackdoor(patchRegionFree)){
      gputDrawString("patch applied!", (gpuGetViewportWidth() - gputGetStringWidth("patch applied!", 8)) / 2 + 25, 70, 8, 8, 0 ,0 ,0);
    }
  }

  if(patchlist[NOAUTODL]==true)
  {
    if(!KernelBackdoor(patchNimAutoUpdate)){
      gputDrawString("patch applied!", (gpuGetViewportWidth() - gputGetStringWidth("patch applied!", 8)) / 2 + 50, 70, 8, 8, 0 ,0 ,0);
    }
  }
  
  //will crash
  /*if(patchlist[SERIALCHANGE]==true)
  {
    if(!KernelBackdoor(changeSerial)){
      gputDrawString("patch applied!", (gpuGetViewportWidth() - gputGetStringWidth("patch applied!", 8)) / 2 + 50, 70, 8, 8, 0 ,0 ,0);
    }
  }*/

  HB_FlushInvalidateCache(); // Just to be sure!

  return true;
}

bool loadSettings(bool patchlist[],short numberOfPatches)
{
  string filepath=settingsFolder+settingsFileName;
  FILE *file = fopen(filepath.c_str(),"rb"); 
  if(file != NULL)
  {
    fread(patchlist,1,sizeof(patchlist)/sizeof(bool),file);
    fclose(file);
  }
  return true;
}

bool saveSettings(bool patchlist[])
{
  if(!fsExists(settingsFolder))
    mkdir(settingsFolder.c_str(), 0777);

  string filepath=settingsFolder+settingsFileName;
  FILE *file = fopen(filepath.c_str(),"w"); 
  if (file == NULL) 
  {
    file = fopen(filepath.c_str(),"c"); 
  }
  fwrite(patchlist,1,sizeof(patchlist)/sizeof(bool),file);
  fclose(file);
  return true;
}

int main(int argc, char **argv) {
  if(!platformInit()) {
    return 0;
  }
  short numberOfPatches=getNumberOfPatches();
  bool patchlist[numberOfPatches];

  for (int i = 0; i < numberOfPatches; i++)
  {
    patchlist[i]=true;
  }

  loadSettings(patchlist,numberOfPatches); 
   
  short exitLoop=false;

  while(platformIsRunning()&&exitLoop==false) {
    //Todo: replace with switch case
    inputPoll();

    if(inputIsPressed(BUTTON_UP))
    {
      menuChageSelection("up");
    }
    else if(inputIsPressed(BUTTON_DOWN))
    {
      menuChageSelection("down");
    }

    if(inputIsPressed(BUTTON_LEFT)||inputIsPressed(BUTTON_RIGHT))
    {
      menuChangeStatusOfSelection(patchlist);
    }

    if(inputIsPressed(BUTTON_A)) {
      short selectionType=getSelectionType();
      switch(selectionType)
      {
        case SAVE:
          saveSettings(patchlist);
          break;
        case EXIT:
          exitLoop=true;
          break;
      }      
    }

    if(inputIsPressed(BUTTON_START)) {
      applyPatches(patchlist);
      exitLoop=true;
    }

    drowTop();
    drawMenu(patchlist);
    gpuSwapBuffers(true);
  }

  platformCleanup();
  return 0;  
}