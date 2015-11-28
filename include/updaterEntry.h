#pragma once

#include "defaultMenuEntrys.h"
#include "updater.h"

typedef Result(Updater::*actionFunction)();

class UpdaterMenuEntry : protected MenuEntry
{
private:
	actionFunction aFunction;
	Updater* updater;
public:
	UpdaterMenuEntry(actionFunction function, Updater* updater, std::string name, std::string description);
	int aAction();
};
