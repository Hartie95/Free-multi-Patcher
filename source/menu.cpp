#include <sstream>
#include "menu.h"
#include <ctrcommon/gpu.hpp>

using namespace std;

static int selection = 0;

string getValueString(bool value)
{
	if(value==true)
		return "on ";
	else
		return "off";	
}

void menuChageSelection(string direction)
{
	if(direction=="up")
	{
		if(selection>0)
			selection --;
	}
	else if(direction=="down")
	{
		if(selection<numberOfEntries-1)
			selection ++;
	}
}

void menuChangeStatusOfSelection(bool patchlist[])
{
	if(menu[selection].type==SETTING)
	{
		patchlist[selection]=!patchlist[selection];
	}
}

short getSelectionType()
{
  return menu[selection].type;
}

short getNumberOfPatches()
{
  short numberOfPatches = 0;
  for (int i = 0; i < numberOfEntries; i++)
  {
    if(menu[i].type==SETTING)
    {
      numberOfPatches++;
    }
  }
  return numberOfPatches;
}

void drowTop()
{
  const string title  = "Eshop/Region spoofer by hartie95";
  const string credit = "based on Ygw eshop spoofer by felipejfc";
  stringstream usageStream;
  usageStream << "Usage:\n" << "\n";
  usageStream << "Start      - Apply patches and exit" << "\n";
  usageStream << "A          - Use selection" << "\n";
  usageStream << "Up/Down    - Change selection" << "\n";
  usageStream << "Left/Right - Modify selection" << "\n";
  
  string usage = usageStream.str(); 

  gpuClear(); 
  gpuViewport(TOP_SCREEN, 0, 0, TOP_WIDTH, TOP_HEIGHT);
  gputOrtho(0, TOP_WIDTH, 0, TOP_HEIGHT, -1, 1);
  gpuClearColor(0xFF, 0xFF, 0xFF, 0xFF);  
  gputDrawString(title, (gpuGetViewportWidth() - gputGetStringWidth(title, 12)) / 2, (gpuGetViewportHeight() - gputGetStringHeight(title, 12))/2+25, 12, 12, 0 , 0 , 0); 
  gputDrawString(credit, (gpuGetViewportWidth() - gputGetStringWidth(credit, 8)) / 2, (gpuGetViewportHeight() - gputGetStringHeight(credit, 8))/2+12, 8, 8, 0, 0, 0); 
  gputDrawString(usage, (gpuGetViewportWidth() - gputGetStringWidth(usage, 8)) / 2, (gpuGetViewportHeight() - gputGetStringHeight(usage, 8))/2-75, 8, 8, 0 , 0 , 0); 
  
  gpuFlushBuffer();
}

void drawMenu(bool patchlist[])
{
  stringstream menuStream;
  for(int i = 0; i < numberOfEntries; i++)
  {
  	if(i==selection)
  		menuStream << "-> ";
  	else
  		menuStream << "   ";

  	menuStream << menu[i].name;
 		if(menu[i].type==SETTING)
  	{
  		menuStream << "   ";	
      menuStream << getValueString(patchlist[i]);
  	}
  	menuStream << "\n";
  }

  stringstream descriptionStream;
  descriptionStream << menu[selection].description;
  
  string menu = menuStream.str();
  string description = descriptionStream.str();

  gpuClear(); 
  gpuViewport(BOTTOM_SCREEN, 0, 0, BOTTOM_WIDTH, BOTTOM_HEIGHT);
  gputOrtho(0, BOTTOM_WIDTH, 0, BOTTOM_HEIGHT, -1, 1);
  gpuClearColor(0xFF, 0xFF, 0xFF, 0xFF);
  gputDrawString(menu, (gpuGetViewportWidth() ) / 8, (gpuGetViewportHeight() - gputGetStringHeight(menu, 8))/2 +50 , 8, 8, 0, 0, 0);     
  gputDrawString(description, (gpuGetViewportWidth() - gputGetStringWidth(description, 8)) / 2, (gpuGetViewportHeight() - gputGetStringHeight(description, 8))/2 -75 , 8, 8, 0, 0, 0);     

  gpuFlushBuffer();
}