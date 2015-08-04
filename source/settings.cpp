#include <3ds.h>
#include <ctrcommon/fs.hpp> 
#include <sys/dirent.h>
#include "constants.h"

using namespace std;

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