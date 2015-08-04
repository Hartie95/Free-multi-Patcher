#pragma once

#include <3ds.h>

#include <ctrcommon/input.hpp>
#include <ctrcommon/service.hpp>
#include "constants.h"
#include <stdio.h>
#include <vector>

#define MENU 		0
#define SETTING 	1
#define SAVE 		2
#define PLACEHOLDER 254
#define EXIT 		255

#define ESHOPSPOOF		0
#define REGIONFREE		1
#define NOAUTODL		2
#define SERIALCHANGE 	3

typedef struct menuEntryStruct{
	std::string name;
	std::string description;
	int (*sideAction)(std::string);
	short type;
}menuEntry;

static const menuEntry menu[]={	
	{"save",				"Save current selection for later use",nullptr, SAVE},
	
};



static const short numberOfEntries = sizeof(menu)/sizeof(menuEntry);
class Menu
{
private:
	Menu* parent;
	int currentSelection;
	//menuEntrys entrys[255];
	std::string getValueString(bool value);
	std::vector<menuEntry> menuEntrys;
public:
	Menu();
	void menuChangeSelection(std::string direction);
	void menuChangeStatusOfSelection(bool patchlist[]);
	short getSelectionType();
	short getNumberOfEntrys();
	void selectionDoSiteAction();
	Menu* back();

	void drawMenu();
};