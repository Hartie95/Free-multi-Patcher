#include "defaultMenuEntrys.h"

using namespace std;

MenuEntry::MenuEntry(std::string name, std::string description)
{
	this->type = ' ';
	this->setName(name);
	this->setDescription(description);
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
    int length = name.size();
    string returnString = name;

    for (u32 i = length; i < this->maxNameLength; i++)
    {
        returnString += " ";
    }
    this->name = returnString;
}

void MenuEntry::setDescription(string description)
{
    u32 lastSpace = 0;
    u32 lastBreak = 0;
    u32 size = description.size();
    for (u32 i = 0; i < size; i++)
    {
        if (description.at(i) == ' ')
        {
            lastSpace = i;
        }
        if (i - lastBreak >= this->maxDescriptionLength)
        {
            lastBreak = lastSpace;
            description[lastSpace] = '\n';
        }
    }
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
	string returnString="";
	returnString += this->type;
	returnString += " ";
	returnString += this->name;
	return returnString;
}

BackMenuEntry::BackMenuEntry(MenuManagerM* manager,string name, string description):MenuEntry(name, description)
{
    this->manager = manager;
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

NavigationMenuEntry::NavigationMenuEntry(MenuManagerM* manager,MenuM* menu,std::string name, std::string description):MenuEntry(name, description)
{
	this->manager = manager;
    this->menu = menu;
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


YesNoMenuEntry::YesNoMenuEntry(bool* value,std::string name, std::string description) :MenuEntry(name, description)
{
	if(value==nullptr)
		value=new bool(true);
    this->value = value;
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
	string returnString = MenuEntry::getRow();
	
	returnString+=" "+getValueString(*this->value);
	return returnString;
}