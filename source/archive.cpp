#include "archive.h"

#include <string>
#include <stdio.h>
#include <string.h>
#include "device.h"

/*Thanks to Yellows8 for cver and nver reading*/

u32 NVer_tidlow_regionarray[7] = {
	0x00016202, //JPN
	0x00016302, //USA
	0x00016102, //EUR
	0x00016202, //"AUS"
	0x00016402, //CHN
	0x00016502, //KOR
	0x00016602, //TWN
};

u32 CVer_tidlow_regionarray[7] = {
	0x00017202, //JPN
	0x00017302, //USA
	0x00017102, //EUR
	0x00017202, //"AUS"
	0x00017402, //CHN
	0x00017502, //KOR
	0x00017602 //TWN
};

FS_Archive extdata_archives[TotalExtdataArchives];

Result archive_readfile(Archive archive, char *path, u8 *buffer, u32 size)
{
	Result ret = 0;
	Handle filehandle = 0;
	u32 tmpval = 0;
	FILE *f;

	char filepath[256];

	if (archive == SDArchive)
	{
		memset(filepath, 0, 256);
		strncpy(filepath, path, 255);

		f = fopen(filepath, "r");
		if (f == NULL)return -5;

		tmpval = fread(buffer, 1, size, f);

		fclose(f);

		if (tmpval != size)return -2;

		return 0;
	}

	ret = FSUSER_OpenFile(&filehandle, extdata_archives[archive], fsMakePath(PATH_ASCII, path), FS_OPEN_READ, 0);
	if (ret != 0)return ret;

	ret = FSFILE_Read(filehandle, &tmpval, 0, buffer, size);

	FSFILE_Close(filehandle);

	if (ret == 0 && tmpval != size)ret = -2;

	return ret;
}

Result read_versionbin(FS_Archive archive, FS_Path fileLowPath, u8 *versionbin)
{
	Result ret = 0;
	Handle filehandle = 0;

	ret = FSUSER_OpenFileDirectly(&filehandle, archive, fileLowPath, FS_OPEN_READ, 0x0);
	if (ret != 0)
	{
		printf("Failed to open the RomFS image for *Ver: 0x%08x.\n", (unsigned int)ret);
		return ret;
	}

	ret = romfsInitFromFile(filehandle, 0x0);
	if (ret != 0)
	{
		printf("Failed to mount the RomFS image for *Ver: 0x%08x.\n", (unsigned int)ret);
		return ret;
	}

	ret = archive_readfile(SDArchive, "romfs:/version.bin", versionbin, 0x8);
	romfsExit();

	if (ret != 0)
	{
		printf("Failed to read the *Ver version.bin: 0x%08x.\n", (unsigned int)ret);
		return ret;
	}

	return 0;
}

Result readVerFile(u8* string, u32 size, u32 tidlow)
{
	Result ret = 0;

	FS_Archive archive;
	FS_Path fileLowPath;
	u32 archive_lowpath_data[0x10 >> 2];
	u32 file_lowpath_data[0x14 >> 2];

	memset(archive_lowpath_data, 0, sizeof(file_lowpath_data));
	memset(file_lowpath_data, 0, sizeof(file_lowpath_data));

	memset(string, 0, size);

	archive.id = 0x2345678a;
	archive.lowPath.type = PATH_BINARY;
	archive.lowPath.size = 0x10;
	archive.lowPath.data = (u8*)archive_lowpath_data;

	fileLowPath.type = PATH_BINARY;
	fileLowPath.size = 0x14;
	fileLowPath.data = (u8*)file_lowpath_data;

	archive_lowpath_data[1] = 0x000400DB;

	archive_lowpath_data[0] = tidlow;
	ret = read_versionbin(archive, fileLowPath, string);
	return ret;
}

Result readCver(u8* cverString,u32 size)
{
	return readVerFile(cverString, size, CVer_tidlow_regionarray[device.region]);
}
Result readNver(u8* nverString, u32 size)
{
	return readVerFile(nverString, size, NVer_tidlow_regionarray[device.region]);
}
