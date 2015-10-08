#include "saveEntrys.h"

using namespace std;


SaveEntry::SaveEntry(Settings* settings)
{
	if (settings != nullptr)
	{
		this->name = "save";
		this->settings = settings;
	}
}

int SaveEntry::aAction()
{
	if(this->settings!=nullptr)
		return this->settings->saveSettings();
	return 2;
}


PatchSaveEntry::PatchSaveEntry(PatchManager* manager)
{
	if (manager != nullptr)
	{
		this->name = "save";
		this->patchManager = manager;
	}
}

int PatchSaveEntry::aAction()
{
	return this->patchManager->saveSettings();
}
