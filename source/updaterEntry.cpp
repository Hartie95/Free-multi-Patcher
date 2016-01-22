#include "updaterEntry.h"

UpdaterMenuEntry::UpdaterMenuEntry(actionFunction function, Updater* updater, std::string name, std::string description) : MenuEntry(name, description)
{
	this->aFunction = function;
	this->updater = updater;
}

int UpdaterMenuEntry::aAction()
{
	Result res = 0;
	if (this->aFunction != nullptr)
		res = (this->updater->*this->aFunction)();
	return res;
}
