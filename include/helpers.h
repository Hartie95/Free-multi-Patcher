#pragma once

#include <3ds.h>
#include <stdio.h>

void* loadFile(FILE* file, size_t minSize, size_t* fileSize);
bool checkFolde();