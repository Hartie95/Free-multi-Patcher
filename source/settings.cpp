#include <3ds.h>
#include <ctrcommon/fs.hpp> 
#include "settings.h"
#include "constants.h"
#include "helpers.h"
#include "stdlib.h"

#include "saveEntrys.h"

using namespace std;

Settings* globalSettings;

//placeholder for possible later types of emunand
typedef struct settingsElementStruct
{
	u32 value;
	char name[MAXNAMELENGTH+1];
}settingsElement;


bool initGlobalSettings()
{
	globalSettings = new Settings(globalSettingsFileName);
	globalSettings->addElement("ignoreFirmware",false);
	globalSettings->addElement("ignoreKernel", false);
	globalSettings->addElement("ignoreRegion", false);
	globalSettings->addElement("ignoreDeviceType", false);
	globalSettings->addElement("ignoreFirmwareCollection", false);
	globalSettings->addElement("ignoreKernelCollection", false);
	globalSettings->addElement("ignoreRegionCollection", false);
	globalSettings->addElement("ignoreDeviceTypeCollection", false);
	globalSettings->loadSettings(globalSettingsFileName);
	return true;
}

Settings::Settings(std::string configName)
{
	this->name = configName;
	loadSettings(configName);
}

bool Settings::loadSettings(std::string configName)
{
	if (configName == "")
		return false;

	if (!checkFolder(settingsFolder))
		return false;

	this->name = configName;

	string filepath = settingsFolder+configName+settingsExtension;
	FILE *file = fopen(filepath.c_str(), "rb");
	if (file != NULL)
	{
		size_t fileSize = 0;
		settingsElement* elements= (settingsElement*) loadFile(file, 0, &fileSize);
		u32 numberOfElements = fileSize / sizeof(settingsElement);
		for (u32 i = 0;i < numberOfElements;i++)
		{
			string* tmp = new string(elements[i].name);
			this->updateElement(*tmp, elements[i].value);
		}
		free(elements);
	}
	return true;
}

Result Settings::addElement(std::string key, u32 value)
{
	if (key != "")
	{
		if (!hasElement(key))
		{
			settings.insert({ key, value });
			if (hasElement(key))
				return ADDED;
			else
				return ERROR;
		}
		else
		{
			return ALREADYEXIST;
		}
	}
	return EMPTYKEY;
}

bool Settings::hasElement(string key)
{
	SETTINGSMAP::iterator it = settings.find(key);
	if (it == settings.end())
		return false;
	else
		return true;
}

Result Settings::updateElement(std::string key, u32 value)
{
	if (key != "")
	{
		if (hasElement(key))
		{
			SETTINGSMAP::iterator it = settings.find(key);
			it->second = value;
			return true;
		}
		else
			return addElement(key, value);
	}
	return ERROR;
}

bool Settings::removeElement(std::string key)
{
	settings.erase(key);
	return true;
}

bool Settings::saveSettings()
{
	if (!checkFolder(settingsFolder))
		return false;

	string filepath = settingsFolder + this->name + settingsExtension;
	FILE *file = fopen(filepath.c_str(), "wb");
	if (file == NULL)
	{
		file = fopen(filepath.c_str(), "cb");
	}
	if (file == NULL)
		return false;
	u32 elementsSize = settings.size()*sizeof(settingsElement);
	settingsElement* elements = (settingsElement*) malloc(elementsSize);
	u32 i = 0;
	memset(elements, 0, elementsSize);
	for (SETTINGSMAP::iterator it = settings.begin(); it != settings.end(); ++it)
	{
		if (it->first == "")
			continue;
		memcpy(&elements[i].name, it->first.c_str(), it->first.size());
		elements[i].value = it->second;
		i++;
	}

	fwrite(elements, 1, elementsSize, file);
	fclose(file);
	free(elements);
	return true;
}

u32 Settings::getValue(std::string elementName)
{
	if(this->hasElement(elementName))
		return settings.find(elementName)->second;
	else return 0;
}

u32* Settings::getValuePointer(std::string elementName)
{
	if (this->hasElement(elementName))
		return &settings.find(elementName)->second;
	else return nullptr;
}

u32 Settings::getNumberOfElements()
{
	return this->settings.size();
}

bool Settings::createMenuPage(MenuManager* menuManager)
{
	Menu* page = new Menu(menuManager, menuManager->getMainPage());

	for (SETTINGSMAP::iterator it = settings.begin(); it != settings.end(); ++it)
	{
		YesNoMenuEntry* entry = new YesNoMenuEntry((bool*)&(it->second), it->first, "");
		page->addEntry((MenuEntry*)entry);
	}
	page->addEntry((MenuEntry*)new SaveEntry(this));


	menuManager->addPage(page, page->getParentMenu(), "Settings");

	return true;
}
