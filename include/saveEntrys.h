#pragma once

#include "defaultMenuEntrys.h"
#include "settings.h"
#include "patchManager.h"


class SaveEntry : protected MenuEntry
{
protected:
	Settings* settings;
public:
	SaveEntry(Settings* settings);
	int aAction();
};

class PatchSaveEntry : protected MenuEntry
{
protected:
	PatchManager* patchManager;
public:
	PatchSaveEntry(PatchManager* manager);
	int aAction();
};