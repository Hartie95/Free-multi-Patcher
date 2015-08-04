#include <sstream>
#include <ctrcommon/gpu.hpp>

#include "menuManager.h"
#include "patchManager.h"
#include "Kernel11.h"

using namespace std;

MenuManager::MenuManager()
{
	this->mainMenu=new Menu();
}

bool MenuManager::ManageInput()
{
	inputPoll();
	bool exitValue=false;

    if(inputIsPressed(BUTTON_UP))
    {
      currentPage->menuChangeSelection("up");
    }
    else if(inputIsPressed(BUTTON_DOWN))
    {
      currentPage->menuChangeSelection("down");
    }

    if(inputIsPressed(BUTTON_LEFT)||inputIsPressed(BUTTON_RIGHT))
    {
      currentPage->selectionDoSiteAction();
    }

    if(inputIsPressed(BUTTON_A)) {
      short selectionType=currentPage->getSelectionType();
      switch(selectionType)
      {
        case SAVE:
          //saveSettings(patchlist);
          break;
        case EXIT:
        	exitValue = this->setActivePage(this->currentPage->back());
          break;
      }      
    }

    if(inputIsPressed(BUTTON_B)) {
    	exitValue = true;
    }

    if(inputIsPressed(BUTTON_START)) {
      KernelBackdoor(&applyPatches);
      
      exitValue = true;
    }

    return exitValue;
}

bool MenuManager::setActivePage(Menu* page)
{
	if(page==nullptr)
		return true;
	else
		this->currentPage=page;
	return false;
}

void MenuManager::drawMenu()
{
	this->drowTop();
	this->currentPage->drawMenu();
}

void MenuManager::drowTop()
{
	const string title  = "Free Multi Patcher by hartie95";
	const string credit = "based on Ygw eshop spoofer by felipejfc";
	stringstream usageStream;
	usageStream << "Usage:\n" << "\n";
	usageStream << "Start      - Apply patches and exit" << "\n";
	usageStream << "A          - Use selection" << "\n";
	usageStream << "B          - Go Back" << "\n";
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