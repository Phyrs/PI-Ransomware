#ifndef MENU
#define MENU

#include <string>
#include <vector>
using namespace std;

extern string cipheredFileName;
extern string reportFileName;
extern vector<string> recovery_tools;

void printFirstMenu();

void printChoiceMenu();

short getUserChoice();

string askUserForPath();

string askUserForFilePath(bool plain);

bool isADirectory(string path);

bool isAFile(string path);

short askUserForRecoveryTool();

#endif