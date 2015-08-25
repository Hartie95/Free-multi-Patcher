#pragma once

#include "defaultMenuEntrys.h"
#include "patches.h"


class PatchEntry : protected YesNoMenuEntry
{
protected:
	Patch* patch;
public:
	PatchEntry(Patch* patch);
	int sideAction();
};