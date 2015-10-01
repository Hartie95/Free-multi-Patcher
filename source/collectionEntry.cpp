#include "collectionEntry.h"
#include "menu.h"
#include "patchEntry.h"
#include "../include/patchEntry.h"

using namespace std;

CollectionEntry::CollectionEntry(PatchCollection* collection,MenuManagerM* manager,MenuM* parentMenu)
{
    this->value = new bool();
    this->collection = collection;
    this->setName(this->collection->getCollectionName());
    this->setDescription(this->collection->getDescription());
    *this->value = this->collection->isEnabled();
    this->manager = manager;
    this->menu = (MenuM*)new Menu((MenuManager*)this->manager,(Menu*)parentMenu);
    
    vector<Patch*>* collectionPatches = collection->getAllPatches();
    
    
    for (std::vector<Patch*>::iterator it = collectionPatches->begin(); it != collectionPatches->end(); ++it)
    {
        Patch* currentPatch = (*it);
        PatchEntry* currentEntry = new PatchEntry(currentPatch);;
        ((Menu*)this->menu)->addEntry((MenuEntry*)currentEntry);
    }
}

int CollectionEntry::sideAction()
{
    if (this->collection == nullptr)
        return 1;
    *this->value = this->collection->changeStatus();
    return 0;
}

string CollectionEntry::getRow()
{
    return this->YesNoMenuEntry::getRow();
}