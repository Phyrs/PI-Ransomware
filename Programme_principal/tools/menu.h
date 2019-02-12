#ifndef MENU
#define MENU

#include <string>
using namespace std;

extern string cipheredFileName;

void printFirstMenu();

void printChoiceMenu();

short getUserChoice();

string askUserForPath();

string askUserForFilePath(bool plain);

bool isADirectory(string path);

bool isAFile(string path);

#endif