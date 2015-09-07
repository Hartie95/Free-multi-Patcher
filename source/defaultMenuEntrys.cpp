#include "defaultMenuEntrys.h"

using namespace std;

MenuEntry::MenuEntry(std::string name, std::string description)
{
	this->name=name;
	this->description = description;
}

int MenuEntry::sideAction() 
{ 
	return 0;
}

int MenuEntry::aAction() 
{
	return 0;
}
void MenuEntry::setName(string name)
{
	this->name=name;
}
void MenuEntry::setDescription(string description)
{
	this->description=description;
}


string MenuEntry::getName() 
{
	return this->name;
}

string MenuEntry::getDescription() 
{
	return this->description;
}
string MenuEntry::getRow()
{
	return this->name;
}

BackMenuEntry::BackMenuEntry(MenuManagerM* manager,string name, string description)
{
	this->manager = manager;
	this->name = name;
	this->description = description;
}

int BackMenuEntry::aAction() 
{
	this->back();
	return 0;
}

void BackMenuEntry::back() 
{
	this->manager->back();
}

NavigationMenuEntry::NavigationMenuEntry(MenuManagerM* manager,MenuM* menu,std::string name, std::string description)
{
	this->manager = manager;
	this->menu = menu;
	this->name = name;
	this->description = description;
}

int NavigationMenuEntry::aAction() 
{
	this->navigate();
	return 0;
}

void NavigationMenuEntry::navigate()
{
	this->manager->navigateTo(this->menu);
}


YesNoMenuEntry::YesNoMenuEntry(bool* value,std::string name, std::string description)
{
	if(value==nullptr)
		value=new bool(true);
	this->value=value;
	this->name=name;
	this->description=description;
}

string YesNoMenuEntry::getValueString(bool value)
{
    if(value==true)
        return "on ";
    else
        return "off";  
}

int YesNoMenuEntry::sideAction()
{
	if(this->value==nullptr)
		return 1;
	*this->value=!*this->value;
	return 0;
}

string YesNoMenuEntry::getRow()
{
	int length=this->name.size();
	string returnString=this->name;

	for(u32 i = length; i < this->maxNameLength; i++)
	{
		returnString+=" ";
	}
	returnString+="  "+getValueString(*this->value);
	return returnString;
}