#include "saveEntrys.h"

using namespace std;


SaveEntry::SaveEntry(Settings* settings) :MenuEntry("save", "")
{
	if (settings != nullptr)
	{
		this->settings = settings;
	}
}

int SaveEntry::aAction()
{
	if(this->settings!=nullptr)
		return this->settings->saveSettings();
	return 2;
}


PatchSaveEntry::PatchSaveEntry(PatchManager* manager) :MenuEntry("save", "")
{
	if (manager != nullptr)
	{
		this->patchManager = manager;
	}
}

int PatchSaveEntry::aAction()
{
	return this->patchManager->saveSettings();
}
