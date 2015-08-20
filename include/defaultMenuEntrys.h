#pragma once

#include <3ds.h>
#include <stdio.h>

#include <ctrcommon/input.hpp>
#include <ctrcommon/service.hpp>
#include "workaround.h"


//#include "menuManager.h"


class MenuEntry 
{
private:
protected:
	std::string name;
	std::string description;
	MenuEntry(){

	}
public:
	MenuEntry(std::string name, std::string description);
	virtual int sideAction();
	virtual int aAction();
	virtual std::string getRow();
	std::string getName();
	std::string getDescription();
};

class BackMenuEntry : MenuEntry 
{
private:
	MenuManagerM* manager;
public:
	BackMenuEntry(MenuManagerM* manager,std::string name, std::string description);
	int aAction();
	void back();
};

class NavigationMenuEntry : MenuEntry 
{
private:
	MenuM* 		  menu;
	MenuManagerM* manager;
public:
	NavigationMenuEntry(MenuManagerM* manager,MenuM* menu,std::string name, std::string description);
	int aAction();
	void navigate();
};

class YesNoMenuEntry : MenuEntry 
{
private:
protected:
	bool* value;
	std::string getValueString(bool value);
public:
	YesNoMenuEntry(bool* value,std::string name, std::string description);
	int sideAction();
	std::string getRow();
};