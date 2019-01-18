/**
 *   That application software provides you some features to help in file system ciphering detection.
 *   It is based on generic detection of ciphered files and some specific ransomwares' signature (Vipasana, HiddenTear and CryPy).
 *  
 *   It also lets you try to recover the files if they have been ciphered by a ransomware whose a recovery module is available for that soft.
 */

#include "stdafx.h"
#include "stdlib.h"
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#pragma comment(lib, "User32.lib")

using namespace std;

const char *cipheredFileName = "C:/cipheredfiles_list.txt";

void printFirstMenu(){
	printf("---------------------------------------------------------\n");
	printf("                     DiskAnalyzer                        \n");
	printf("---------------------------------------------------------\n");
	printf("\n");
	printf("This tool will analyze your file system in order to detect if it has been ciphered or not.\n");
}

void printChoiceMenu(){
	printf("Please choose an option (enter 1 or 2) :\n");
	printf(" 1 - Analyze Disk\n");
	printf(" 2 - Recover files from a previous analysis (Supposes that you have already analyzed the disk and the file %s has been created.\n", cipheredFileName);
}

short getUserChoice(){
	short choice;
	cin >> choice;
	if(cin.fail()){
		cin.clear();
//		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		return 0;
	}
	return  choice;
}

string askUserForPath(){
	bool isPathOrDir = false;
	string path = "";
	do{
		printf("\n Please enter a specific path (to a directory or a file) to consider (Enter C:/ to analyze the whole disk) :\n");
		cin.clear();
		//cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin >> path;
		struct stat s;
		if(stat(path.c_str(), &s) == 0){
			if(s.st_mode & S_IFDIR || s.st_mode & S_IFREG){
				isPathOrDir = true;
			}
		}
	} while (!isPathOrDir);
	return path;
}

void listDirectory(string path){
   WIN32_FIND_DATA ffd;
   LARGE_INTEGER filesize;
   TCHAR szDir[MAX_PATH];
   size_t length_of_arg;
   HANDLE hFind = INVALID_HANDLE_VALUE;
   DWORD dwError=0;

   TCHAR *param = new TCHAR[path.size()+1];
   param[path.size()]=0;
   copy(path.begin(), path.end(), param);

   StringCchCopy(szDir, MAX_PATH, param);
   StringCchCat(szDir, MAX_PATH, TEXT("/*"));

   hFind = FindFirstFile(szDir, &ffd);
   
   // List all the files in the directory with some info about them.

   do
   {
	  string dir = path;
	  wstring wfilename(ffd.cFileName);
	  string filename(wfilename.begin(), wfilename.end());
	  if(path.at(path.size()-1) == '/' ||  path.at(path.size()-1) == '\\')
		dir.append(filename);
	  else
		dir.append("/").append(filename);

	   // IT'S A DIRECTORY
      if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
		  // Recursive call to put her
          printf("  %s   <DIR>\n", dir.c_str());
      }
	  // IT'S A FILE
      else
      {
		 // File analysis to put here
         printf("  %s   <FILE>\n", dir.c_str());
      }
   }
   while (FindNextFile(hFind, &ffd) != 0);


   FindClose(hFind);
}


int _tmain(int argc, _TCHAR* argv[])
{
	short userChoice = 0;
	printFirstMenu();
	do{
		printChoiceMenu();
		userChoice = getUserChoice();

	} while(userChoice != 1 && userChoice != 2) ;

	string path("");
	switch(userChoice){
		case 1:
			// Asking for a path.
			path = askUserForPath();
			listDirectory(path);

			// Scan general et rapport

			// Scan appliqué aux ransomwares (Vipasana, ...) et rapport

			break;
		case 2:
			// Opening the file wih ciphered files' name and asking for a path
			path = askUserForPath();
			break;
	}

    while(1);
	return 0;
}

