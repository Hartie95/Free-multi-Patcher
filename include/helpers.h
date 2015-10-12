#pragma once

#include <3ds.h>
#include <stdio.h>
#include <sstream>

void* loadFile(FILE* file, size_t minSize, size_t* fileSize);
bool checkFolder(std::string name);