#include "updater.h"
#include "constants.h"
#include "settings.h"
#include <stdlib.h>
#include <ctrcommon\app.hpp>
#include "saveEntrys.h"
#include "updaterEntry.h"


using namespace std;

Updater::Updater(MenuManager* manager, bool* exitLoop)
{
	this->onlineVersion = 0;
	this->onlineVersionString = "";
	this->menuPage = nullptr;
	this->exitLoop = exitLoop;
	

	this->updaterSettings = new Settings("Updater");
	this->updaterSettings->addElement(SETTINGS_BOOT_CHECK, false);
	this->updaterSettings->addElement(SETTINGS_UPDATE_NOTIFICATION, false);
	this->updaterSettings->addElement(SETTINGS_LAST_NOTIFICATION, version);
	this->updaterSettings->loadSettings("Updater");

	this->createMenuPage(manager);

	if (this->updaterSettings->getValue(SETTINGS_BOOT_CHECK))
		this->checkForUpdate();
}


Result Updater::checkForUpdate()
{
	Result ret = checkVersion();
	if (this->onlineVersion > 0)
	{
		if (this->updaterSettings->getValue(SETTINGS_UPDATE_NOTIFICATION))
			this->createUpdateNotification();
		if (this->installEntryAdded == false)
		{
			MenuEntry* entry = (MenuEntry*) new UpdaterMenuEntry(&Updater::updateApplication, this, "Update application", "Downloads and installs Updates.\nThe application will close itself after sucess.");
			menuPage->addEntry((MenuEntry*)entry);
			installEntryAdded = true;
		}
	}
	return ret;
}


Result Updater::updateApplication()
{
	Result res = 0;
	if (this->onlineVersion == 0)
		this->checkVersion();
	if (this->onlineVersion>0)
	{
		res = this->downloadUpdate();
		if (res == 0)
			res = this->installUpdate();
	}
	return res;
}


Result Updater::createMenuPage(MenuManager* manager)
{
	this->menuPage = new Menu(manager, manager->getMainPage());
	MenuEntry* entry= (MenuEntry*)new YesNoMenuEntry((bool*)this->updaterSettings->getValuePointer(SETTINGS_BOOT_CHECK),
												"Auto check for Updates", 
												"Always check online for new versions at start");
	menuPage->addEntry((MenuEntry*)entry);

	entry = (MenuEntry*)new YesNoMenuEntry((bool*)this->updaterSettings->getValuePointer(SETTINGS_UPDATE_NOTIFICATION),
								"Create update notifications",
								"Create update notifikations in the homemenu for new Versions");
	menuPage->addEntry((MenuEntry*)entry);

	entry = (MenuEntry*) new SaveEntry(this->updaterSettings);
	menuPage->addEntry((MenuEntry*)entry);

	entry = (MenuEntry*) new UpdaterMenuEntry(&Updater::checkForUpdate, this, "Check for Updates", "Check online for new Updates");
	menuPage->addEntry((MenuEntry*)entry);

	manager->addPage(this->menuPage, "Updater");

	return 0;
}


Result Updater::checkVersion()
{
	this->onlineVersion = 0;
	this->onlineVersionString = "";
	size_t downloadSize = 0;
	u8* downloadResult = nullptr;
	Result res = download((string*)&versionCheckUrl, &downloadSize, &downloadResult);
	if (res == 0)
	{
		char versionString[downloadSize + 1];
		for (u32 i = 0; i<downloadSize; i++)
		{
			versionString[i] = (char)downloadResult[i];
		}
		versionString[downloadSize] = '\0';
		this->onlineVersionString = string(versionString);
		this->onlineVersion = (int)strtol(versionString, NULL, 16);
		free(downloadResult);
	}
	return res;
}


Result Updater::createUpdateNotification()
{
	if (this->updaterSettings->getValue(SETTINGS_LAST_NOTIFICATION) < this->onlineVersion)
	{
		const char title8[] = { "A New Version is available" };
		string message = "A new version is available:\n" + generateVersionString(this->onlineVersion) + "\n\nPlease install it directly inside of the fpm or download it from http://fmp.hartie95.de/releases";
		u32 messageSize = message.size();
		u16 title16[sizeof(title8)];
		u16 message16[messageSize + 1];
		for (u32 i = 0; i < sizeof(title8); i++)
		{
			title16[i] = (u16)title8[i];
		}
		for (u32 i = 0; i < messageSize; i++)
		{
			message16[i] = (u16)message.at(i);
		}
		message16[messageSize] = (u16)'\0';

		NEWS_AddNotification(title16, sizeof(title8), message16, messageSize, nullptr, 0, false);
		this->updaterSettings->updateElement(SETTINGS_LAST_NOTIFICATION, this->onlineVersion);
		this->updaterSettings->saveSettings();
	}
	return 0;
}


Result Updater::downloadUpdate()
{
	size_t downloadSize = 0;
	u8* downloadResult = nullptr;
	string url = mainDownloadUrl + this->onlineVersionString + ".txt";
	Result res = download(&url,&downloadSize,&downloadResult);

	if (res == 0)
	{
		string ciaUrl = "";
		char urlString[downloadSize + 1];
		for (u32 i = 0; i<downloadSize; i++)
		{
			urlString[i] = (char)downloadResult[i];
		}
		urlString[downloadSize] = '\0';
		ciaUrl = string(urlString);
		free(downloadResult);
		res = download(&ciaUrl, &downloadSize, &downloadResult);
		if (res == 0)
		{
			string UpdateFileName = this->onlineVersionString + ".cia";

			string filepath = applicationFolder + UpdateFileName;
			FILE* file = fopen(filepath.c_str(), "wb");
			if (file == NULL)
			{
				file = fopen(filepath.c_str(), "cb");
			}
			fwrite(downloadResult, 1, (downloadSize), file);
			fclose(file);
			free(downloadResult);
		}
	}

	return 0;
}


Result Updater::installUpdate()
{
	string UpdateFileName = this->onlineVersionString + ".cia";

	string filepath = applicationFolder + UpdateFileName;
	/*App applicationInformation = appGetCiaInfo(filepath, SD);*/
	Result res = appInstallFile(SD, filepath, NULL);
	if (res == 0)
		*exitLoop = true;
	return res;
}


Result Updater::download(std::string* url, size_t* filesize, u8** file)
{
	httpcContext context;
	Result ret = 0;
	Result res = 1;
	u32 statuscode = 0;
	char * cUrl = (char*)url->c_str();
	ret = httpcOpenContext(&context, cUrl, 0);
	if (ret == 0)
	{
		ret = httpcBeginRequest(&context);
		if (ret == 0)
		{
			ret = httpcGetResponseStatusCode(&context, &statuscode, 0);
			if (ret == 0 && statuscode == 200)
			{
				u32 contentsize = 0;
				ret = httpcGetDownloadSizeState(&context, NULL, &contentsize);
				if (ret == 0)
				{
					*file = (u8*)malloc(contentsize);
					memset(*file, 0, contentsize);
					ret = httpcDownloadData(&context, *file, contentsize, NULL);
					if (ret == 0)
					{
						*filesize = contentsize;
						res = 0;
						httpcCloseContext(&context);
					}
					else
					{
						free(*file);
					}
				}
			}
		}
	}
	return res;
}
