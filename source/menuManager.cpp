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

string getModel()
{
    u8 modelID = 0;
    CFGU_GetSystemModel(&modelID);
    string model = "";
    switch (modelID)
    {
    case 0:
    case 1:
        model = "old 3DS";
        break;
    case 3:
        model = "2DS";
        break;
    case 2:
    case 4:
        model = "new 3DS";
        break;
    }
    return model + "\n";
}

string checkFirmwareVersion()
{
    u32 firmware = osGetFirmVersion();
    std::stringstream stream;
    stream << "uk";
    stream << ((firmware)& 0xff);
    stream << "fw: ";
    stream << GET_VERSION_MAJOR(firmware);
    stream << ".";
    stream << GET_VERSION_MINOR(firmware);
    stream << "-";
    stream << GET_VERSION_REVISION(firmware);
    std::string result(stream.str());
    return result + "\n";
}

string checkKernelVersion()
{
    u32 returnValue = osGetKernelVersion();
    kernelVersion* firmware = (kernelVersion*)&returnValue;
    std::stringstream stream;
    stream << "uk";
    stream << (u32)firmware->unknown;
    stream << "kernel: ";
    stream << (u32)firmware->major;
    stream << ".";                       
    stream << (u32)firmware->minor;
    stream << "-";
    stream << (u32)firmware->revision;
    std::string result(stream.str());
    return result+"\n";
}

void MenuManager::drowTop()
{
    string debugOutput = "Model: " + getModel() + checkKernelVersion() + checkFirmwareVersion();
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
    gputDrawString(debugOutput, (gpuGetViewportWidth() - gputGetStringWidth(debugOutput, 8)) / 2, (gpuGetViewportHeight() - gputGetStringHeight(debugOutput, 8)) / 2 + 50, 8, 8, 0, 0, 0);
	gputDrawString(title, (gpuGetViewportWidth() - gputGetStringWidth(title, 12)) / 2, (gpuGetViewportHeight() - gputGetStringHeight(title, 12))/2+25, 12, 12, 0 , 0 , 0); 
	gputDrawString(credit, (gpuGetViewportWidth() - gputGetStringWidth(credit, 8)) / 2, (gpuGetViewportHeight() - gputGetStringHeight(credit, 8))/2+12, 8, 8, 0, 0, 0); 
	gputDrawString(usage, (gpuGetViewportWidth() - gputGetStringWidth(usage, 8)) / 2, (gpuGetViewportHeight() - gputGetStringHeight(usage, 8))/2-75, 8, 8, 0 , 0 , 0); 
	
	gpuFlushBuffer();
}