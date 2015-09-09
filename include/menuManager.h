#pragma once
#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include <3ds.h>
#include <ctrcommon/input.hpp>
#include <ctrcommon/service.hpp>
#include <stdio.h>
#include <vector>

//class Menu;
#include "menu.h"
#include "workaround.h"


class MenuManager : MenuManagerM
{
private:
	std::vector<Menu*> menuPages;
	Menu* mainPage;
	Menu* currentPage;
	bool  exit;

	void setActivePage(Menu* page);

public:
	MenuManager();

	bool ManageInput();
	
	void back();
	void navigateTo(MenuM* targetPage);
	void navigateTo(Menu* targetPage);
	void addPage(Menu* Page, std::string name);
	void addPage(Menu* Page, Menu* parent, std::string name);
	Menu* getMainPage();

	void drawMenu();
	void drowTop();
};

#endif