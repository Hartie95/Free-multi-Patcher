#include "helpers.h"
#include <citrus/fs.hpp> 
#include "malloc.h"
#include <sys/dirent.h>

using namespace std;
using namespace ctr;

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
	if (!fs::exists(name))
		mkdir(name.c_str(), 0777);
	return true;
}


std::string getStringFromDownload(size_t downloadSize, u8* downloadResult)
{
	string resultString = "";
	char urlString[downloadSize + 1];
	for (u32 i = 0; i<downloadSize; i++)
	{
		urlString[i] = (char)downloadResult[i];
	}
	urlString[downloadSize] = '\0';
	resultString = string(urlString);
	return resultString;
}