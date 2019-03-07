#ifndef UTILE_H
#define UTILE_H

#define WINDOWS

#ifdef WINDOWS
	#include <windows.h>
	#include <stdint.h>
	#include <sstream>

    #define popen _popen
	#define pclose _pclose
#else
    #include <bits/stdc++.h> 
    #include <dirent.h>
#endif

#include <stack>
#include <string>
#include <fstream>
#include <sys/stat.h>

using namespace std;

void testFichier();
string executer(string commande);
string getExtension(string path);
long nbElementsDans(string path);
ifstream loadFile(char const *path);
string* fichiersEtDossiersDans(string path);
bool loadFileAndLoadBuffer(char const *path, char *dst, int size);
void regenerateHeader(ofstream &file, string path, char begin[768], long length);

#endif //UTILE_H
