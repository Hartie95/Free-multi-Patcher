#pragma once

#include <3ds.h>
#include <stdio.h>

#include <string>
#include "workaround.h"

#define MAXNAMELENGTH 29

class MenuEntry 
{
private:
protected:
	std::string name;
	char type;
	std::string description;
    u32 maxNameLength = MAXNAMELENGTH;
    u32 maxDescriptionLength = 40;
	MenuEntry(){

	}
public:
	MenuEntry(std::string name, std::string description);
	virtual int sideAction();
	virtual int aAction();
	virtual std::string getRow();
	void setName(std::string name);
	void setDescription(std::string description);
	std::string getName();
	std::string getDescription();
};

class BackMenuEntry : protected MenuEntry 
{
private:
	MenuManagerM* manager;
public:
	BackMenuEntry(MenuManagerM* manager,std::string name, std::string description);
	int aAction();
	void back();
};

class NavigationMenuEntry : virtual protected MenuEntry 
{
protected:
	MenuM* 		  menu;
	MenuManagerM* manager;
    NavigationMenuEntry(){};
public:
	NavigationMenuEntry(MenuManagerM* manager,MenuM* menu,std::string name, std::string description);
	int aAction();
	void navigate();
};

class YesNoMenuEntry : virtual protected MenuEntry 
{
private:
protected:
	bool* value;
	std::string getValueString(bool value);
	YesNoMenuEntry(){

	}
public:
	YesNoMenuEntry(bool* value,std::string name, std::string description);
	virtual int sideAction();
	virtual std::string getRow();
};