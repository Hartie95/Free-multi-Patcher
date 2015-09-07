#include "collectionEntry.h"
#include "menu.h"

using namespace std;

CollectionEntry::CollectionEntry(PatchCollection* collection,MenuManagerM* manager,MenuM* parentMenu)
{
    this->value = new bool();
    this->collection = collection;
    this->YesNoMenuEntry::setName(this->collection->getCollectionName());
    this->YesNoMenuEntry::setDescription(this->collection->getDescription());
    *this->value = this->collection->isEnabled();
    this->manager = manager;
    this->menu = (MenuM*)new Menu((MenuManager*)this->manager,(Menu*)parentMenu);
    
    vector<Patch*>* collectionPatches = collection->getAllPatches();
    Patch* currentPatch = nullptr;
    PatchEntry* currentEntry = nullptr;
    for (std::vector<Patch*>::iterator it = collectionPatches->begin(); it != collectionPatches->end(); ++it)
    {
        currentPatch = (*it);
        currentEntry = new PatchEntry(currentPatch);
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