#include <3ds.h>

#include <ctrcommon/input.hpp>
#include <ctrcommon/service.hpp>
#include "constants.h"
#include <stdio.h>

#define SETTING 	0
#define SAVE 		1
#define PLACEHOLDER 254
#define EXIT 		255

#define ESHOPSPOOF		0
#define REGIONFREE		1
#define NOAUTODL		2
#define SERIALCHANGE 	3

struct menuEntry{
	std::string name;
	std::string description;
	short type;
};

static const menuEntry menu[]={	{"e-shop spoof     ",	"Patches nim for E-Shop access       ", SETTING},
								{"region patch     ",	"Patches the home menu to show out of\nregion games", SETTING},
								{"no auto download ",	"Patches nim to stop automatic update\ndownload\n(Might be unstable)", SETTING},
								{"serial patch",		"Patches the serial to allow E-Shop  \nacces after region change\n(not implemented)", PLACEHOLDER},
								{"save",				"Save current selection for later use", SAVE},
								{"exit",				"Exit without applying patches       ", EXIT}
							};

static const short numberOfEntries = sizeof(menu)/sizeof(menuEntry);

void menuChageSelection(std::string direction);
void menuChangeStatusOfSelection(bool patchlist[]);
short getSelectionType();
short getNumberOfPatches();
void drowTop();
void drawMenu(bool patchlist[]);