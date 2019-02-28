#ifndef UTILE_H
#define UTILE_H

//#define WINDOWS

#ifdef WINDOWS
    #include "stdafx.h"
    #define popen _popen
#endif

#include <stack>
#include <string>
#include <fstream>
#include <sys/stat.h>

using namespace std;

void testFichier();
string executer(string commande);
long nbElementsDans(string path);
ifstream loadFile(char const *path);
stack<string> fichiersEtDossiersDans(string path);
bool loadFileAndLoadBuffer(char const *path, char *dst, int size);
void regenerateHeader(ofstream &file, string path, char begin[768], long length);

#endif //UTILE_H
