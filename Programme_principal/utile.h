#ifndef UTILE_H
#define UTILE_H

//#define WINDOWS

#ifdef WINDOWS
    #include "stdafx.h"
#endif

#include <string>
#include <fstream>

using namespace std;

void testFichier();
ifstream loadFile(char const *path);
void regenerateHeader(ofstream &file, string path, char begin[768], long length);
bool loadFileAndLoadBuffer(char const *path, char *dst, int size);

#endif //ETUDE_H
