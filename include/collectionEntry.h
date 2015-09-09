#pragma once

#include "patchCollections.h"
#include "defaultMenuEntrys.h"


class CollectionEntry : protected YesNoMenuEntry, protected NavigationMenuEntry
{
protected:
    PatchCollection* collection;
public:
    CollectionEntry(PatchCollection* collection, MenuManagerM* manager,MenuM* parentMenu);
    int sideAction();
    virtual std::string getRow();
};