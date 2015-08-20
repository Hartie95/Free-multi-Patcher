#pragma once

#ifndef MENU_H
#define MENU_H

#include <3ds.h>

#include <ctrcommon/input.hpp>
#include <ctrcommon/service.hpp>
#include "constants.h"
#include <stdio.h>
#include <vector>
#include <memory>

#include "defaultMenuEntrys.h"
//#include "menuManager.h"

class MenuManager;

class Menu : MenuM
{
private:
	Menu* parentMenu;
	MenuManager* parentManager;
	unsigned int currentSelection;
	std::vector<MenuEntry*> menuEntrys;
public:
	Menu(MenuManager* parentManager,Menu* parentMenu);
	MenuManager* getParentManager();
	Menu* getParentMenu();
	void addEntry(MenuEntry* entry);

	void menuChangeSelection(std::string direction);
	short getNumberOfEntrys();
	void selectionDoSiteAction();
	void selectionDoAAction();
	Menu* back();

	void drawMenu();
};

#endif