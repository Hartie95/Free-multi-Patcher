#include <sstream>
#include <ctrcommon/gpu.hpp>

#include "menu.h"
#include "patches.h"

using namespace std;

//static int selection = 0;
Menu::Menu()
{
  this->currentSelection=0;  
  menuEntry back = {"exit",        "Exit without applying patches       ",nullptr, EXIT};
  this->menuEntrys.push_back(back);
  this->menuEntrys.push_back(back);
  this->menuEntrys.push_back(back);

}

string Menu::getValueString(bool value)
{
  if(value==true)
    return "on ";
  else
    return "off";  
}

void Menu::menuChangeSelection(string direction)
{
  if(direction=="up")
  {
    if(this->currentSelection>0)
      this->currentSelection --;
  }
  else if(direction=="down")
  {
    if(this->currentSelection<numberOfEntries-1)
      this->currentSelection ++;
  }
}

void Menu::selectionDoSiteAction()
{
  if(this->menuEntrys.at(this->currentSelection).sideAction != nullptr)
  {
    (this->menuEntrys.at(this->currentSelection).sideAction)(this->menuEntrys.at(this->currentSelection).name);
  }
}

Menu* Menu::back()
{
  return this->parent;
}

short Menu::getSelectionType()
{
  return menu[this->currentSelection].type;
}

short Menu::getNumberOfEntrys()
{
  return this->menuEntrys.size();
}

void Menu::drawMenu()
{
  stringstream menuStream;
  int i=0;
  for(std::vector<menuEntry>::iterator it = menuEntrys.begin(); it != menuEntrys.end(); ++it)
  {
    /*if(i == this->currentSelection)
      menuStream << "-> ";
    else
      menuStream << "   ";

    menuStream << (*it).name;
     if((*it).type==SETTING)
    {
      menuStream << "   ";  
      menuStream << this->getValueString(true);
    }
    menuStream << "\n";
    i++;*/
  }
/*
  stringstream descriptionStream;
  descriptionStream << menuEntrys.at(this->currentSelection).description;
  
  string menu = menuStream.str();
  string description = descriptionStream.str();

  gpuClear(); 
  gpuViewport(BOTTOM_SCREEN, 0, 0, BOTTOM_WIDTH, BOTTOM_HEIGHT);
  gputOrtho(0, BOTTOM_WIDTH, 0, BOTTOM_HEIGHT, -1, 1);
  gpuClearColor(0xFF, 0xFF, 0xFF, 0xFF);
  gputDrawString(menu, (gpuGetViewportWidth() ) / 8, (gpuGetViewportHeight() - gputGetStringHeight(menu, 8))/2 +50 , 8, 8, 0, 0, 0);     
  gputDrawString(description, (gpuGetViewportWidth() - gputGetStringWidth(description, 8)) / 2, (gpuGetViewportHeight() - gputGetStringHeight(description, 8))/2 -75 , 8, 8, 0, 0, 0);     
*/
  gpuFlushBuffer();

  gpuSwapBuffers(true);
}