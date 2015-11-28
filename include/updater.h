#pragma once

#include <3ds.h>
#include <string>
#include "menuManager.h"
#include "settings.h"

#define SETTINGS_BOOT_CHECK "CheckForUpdateAtBoot"
#define SETTINGS_UPDATE_NOTIFICATION "EnableUpdateNotifications"
#define SETTINGS_LAST_NOTIFICATION "lastVersionNotification"

class Updater
{
private:
	Menu* menuPage;
	Settings* updaterSettings;
	bool* exitLoop;
	u32 onlineVersion;
	std::string onlineVersionString;
	bool installEntryAdded;

	Result createMenuPage(MenuManager* manager);
	Result checkVersion();
	Result createUpdateNotification();
	Result downloadUpdate();
	Result installUpdate();
	
	Result download(std::string* url,size_t* filesize,u8** file);

public:
	Updater(MenuManager* manager,bool* exitLoop);
	Result checkForUpdate();
	Result updateApplication();
};
