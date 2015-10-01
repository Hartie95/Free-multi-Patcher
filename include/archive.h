#pragma once

#include <3ds.h>

typedef enum {
	HomeMenu_Extdata,
	Theme_Extdata,
	TotalExtdataArchives,
	SDArchive = TotalExtdataArchives
} Archive;

Result archive_readfile(Archive archive, char *path, u8 *buffer, u32 size);
Result read_versionbin(FS_archive archive, FS_path fileLowPath, u8 *versionbin);

Result readVerFile(u8* cverString, u32 size, u32 tidlow);

Result readCver(u8* cverString, u32 size);
Result readNver(u8* nverString, u32 size);