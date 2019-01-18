#ifndef UTILE_H
#define UTILE_H

//#define WINDOWS

#ifdef WINDOWS
    #include "stdafx.h"
#endif

#include <string>
#include <fstream>

using namespace std;

ifstream loadFile(char const *path);
bool loadFileAndLoadBuffer(char const *path, char *dst, int size);

#endif //ETUDE_H
