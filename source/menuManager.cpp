#include <sstream>
#include <ctrcommon/gpu.hpp>

#include "menuManager.h"
#include "patchManager.h"
#include "device.h"

using namespace std;

MenuManager::MenuManager(bool* exit)
{
	this->mainPage=new Menu(this,nullptr);
	this->currentPage=this->mainPage;
	
	this->menuPages.push_back(this->mainPage);

	//Generates Version String for the UI
	this->versionString = generateVersionString(version);

	this->exit=exit;
}

void MenuManager::ManageInput()
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
		*this->exit=true;
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
    string model = "";
    switch (device.modelID)
    {
    case 0:
        model = "old 3DS";
        break;
    case 1:
        model = "old 3DS XL";
        break;
    case 3:
        model = "2DS";
        break;
    case 2:
        model = "new 3DS";
        break;
    case 4:
        model = "new 3DS XL";
        break;
    }
    return model + "\n";
}

string getRegion()
{
	string region = "";
	switch (device.region)
	{
	case 0:
		region = "J";
		break;
	case 1:
		region = "U";
		break;
	case 2:
		region = "E";
		break;
	case 3:
		region = "AUS";
		break;
	case 4:
		region = "C";
		break;
	case 5:
		region = "K";
		break;
	case 6:
		region = "T";
		break;
	default:
		break;
	}
	return region;
}

string checkFirmwareVersion()
{
    std::stringstream stream;
    stream << "fw: ";
    stream << (u32)device.firmwareversion.major;
    stream << ".";
    stream << (u32)device.firmwareversion.minor;
    stream << ".";
    stream << (u32)device.firmwareversion.revision;
	stream << "-";
	stream << (u32)device.firmwareversion.nver;
	stream << " ";
	stream << getRegion();
    std::string result(stream.str());
    return result + "\n";
}

string checkKernelVersion()
{
    std::stringstream stream;
    stream << "kernel: ";
    stream << (u32)device.kernelversion.major;
    stream << ".";                       
    stream << (u32)device.kernelversion.minor;
    stream << "-";
    stream << (u32)device.kernelversion.revision;
    std::string result(stream.str());
    return result+"\n";
}

void MenuManager::drowTop()
{
    string deviceInformations = "Model: " + getModel() + checkKernelVersion()+checkFirmwareVersion();
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
	gputDrawString(title, (gpuGetViewportWidth() - gputGetStringWidth(title, 12)) / 2, (gpuGetViewportHeight() - gputGetStringHeight(title, 12))/2+55, 12, 12, 0 , 0 , 0); 
	gputDrawString(credit, (gpuGetViewportWidth() - gputGetStringWidth(credit, 8)) / 2, (gpuGetViewportHeight() - gputGetStringHeight(credit, 8))/2+42, 8, 8, 0, 0, 0);
	gputDrawString(deviceInformations, (gpuGetViewportWidth() - gputGetStringWidth(deviceInformations, 8)) / 2, (gpuGetViewportHeight() - gputGetStringHeight(deviceInformations, 8)) / 2-20, 8, 8, 10, 10, 10);
	gputDrawString(usage, (gpuGetViewportWidth() - gputGetStringWidth(usage, 8)) / 2, (gpuGetViewportHeight() - gputGetStringHeight(usage, 8))/2-75, 8, 8, 0 , 0 , 0); 
	gputDrawString(versionString, gpuGetViewportWidth() - gputGetStringWidth(versionString, 8)-5, gputGetStringHeight(versionString, 8), 8, 8, 0, 0, 0);

	gpuFlushBuffer();
}