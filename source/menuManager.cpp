#include <sstream>
#include <ctrcommon/gpu.hpp>

#include "menuManager.h"
#include "patchManager.h"
#include "Kernel11.h"

using namespace std;

MenuManager::MenuManager()
{
	this->mainPage=new Menu(this,nullptr);
	this->currentPage=this->mainPage;
	
	this->menuPages.push_back(this->mainPage);

	this->exit=false;
}

bool MenuManager::ManageInput()
{
	inputPoll();

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
        currentPage->selectionDoAAction();
    }

    if(inputIsPressed(BUTTON_B)) {
    	this->back();
    }

    if(inputIsPressed(BUTTON_START)) {
      	KernelBackdoor(&applyPatches);
      	this->exit = true;
    }

    return this->exit;
}

void MenuManager::back()
{
	this->setActivePage(this->currentPage->back());
}

void MenuManager::navigateTo(MenuM* targetPage)
{
	navigateTo((Menu*) targetPage);
}

void MenuManager::navigateTo(Menu* targetPage)
{
	this->currentPage=targetPage;
}

void MenuManager::addPage(Menu* page, string name)
{
	addPage(page,this->mainPage,name);
}

void MenuManager::addPage(Menu* page, Menu* parent, string name)
{
	this->menuPages.push_back(page);
    NavigationMenuEntry* newPage= new NavigationMenuEntry((MenuManagerM*)this,(MenuM*)page,name,"");
    parent->addEntry((MenuEntry*)newPage);
}

void MenuManager::setActivePage(Menu* page)
{
	if(page==nullptr)
		this->exit=true;
	else
		this->currentPage=page;
}

Menu* MenuManager::getMainPage()
{
	return this->mainPage;
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