#include "patchEntry.h"

using namespace std;

PatchEntry::PatchEntry(Patch* patch)
{
	this->value=new bool();
	this->patch=patch;
	this->setName(this->patch->getPatchName());
	this->setDescription(this->patch->getDescription());
	*this->value=this->patch->isEnabled();
}

int PatchEntry::sideAction()
{
	if(this->patch==nullptr)
		return 1;
	*this->value=this->patch->changeStatus();
	return 0;
}