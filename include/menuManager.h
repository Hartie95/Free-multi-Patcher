#include <3ds.h>
#include <ctrcommon/input.hpp>
#include <ctrcommon/service.hpp>
#include "constants.h"
#include <stdio.h>
#include <vector>

#include "menu.h"

class MenuManager
{
private:
	std::vector<Menu*> MenuPages;
	Menu* mainMenu;
	Menu* currentPage;

	bool setActivePage(Menu* page);
public:
	MenuManager();

	bool ManageInput();
	void drawMenu();
	void drowTop();
};