#pragma once
#include <3ds.h>
#include <stdio.h>
#include <map>
#include "menuManager.h"

#define SETTINGSMAP std::map<std::string, u32>

bool initGlobalSettings();

#define ALREADYEXIST 1
#define ADDED 2
#define ERROR 3
#define EMPTYKEY 4

class Settings
{
private:
	SETTINGSMAP settings;
	std::string name;
public:
	Settings(std::string configName);
	bool loadSettings(std::string configName);
	Result addElement(std::string elementName, u32 value);
	Result updateElement(std::string elementName, u32 value);
	bool removeElement(std::string elementName);
	bool saveSettings();
	bool hasElement(std::string key);
	u32 getValue(std::string elementName);
	u32* getValuePointer(std::string elementName);
	u32 getNumberOfElements();
	bool createMenuPage(MenuManager* menuManager);
};

extern Settings* globalSettings;
