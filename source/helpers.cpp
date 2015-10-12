#include "helpers.h"
#include <ctrcommon/fs.hpp> 
#include "malloc.h"
#include <sys/dirent.h>

using namespace std;

void* loadFile(FILE* file, size_t minSize,size_t* fileSize)
{
	if (fileSize == nullptr)
		fileSize = new size_t();
	*fileSize = 0;
	void* loadedFile = nullptr;
	if (file != NULL)
	{
		fseek(file, 0L, SEEK_END);
		*fileSize = ftell(file);
		fseek(file, 0L, SEEK_SET);

		if (*fileSize < minSize)
			return nullptr;

		loadedFile = (void*)malloc(*fileSize);
		if (loadedFile != nullptr)
		{
			fread(loadedFile, 1, *fileSize, file);
		}
		fclose(file);
	}
	return loadedFile;
}

bool checkFolder(string name)
{
	if (!fsExists(name))
		mkdir(name.c_str(), 0777);
	return true;
}