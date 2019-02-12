/**
 *   That application software provides you some features to help in file system ciphering detection.
 *   It is based on generic detection of ciphered files and some specific ransomwares' signature (Vipasana, HiddenTear and CryPy).
 *  
 *   It also lets you try to recover the files if they have been ciphered by a ransomware whose a recovery module is available for that soft.
 */

#include "stdlib.h"
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>

#include "tools/multithreading.h"
#include "tools/menu.h"
#include "recoverers/VipasanaRecover.h"

#pragma comment(lib, "User32.lib")

using namespace std;

vector <string> recovery_tools;

short askUserForRecoveryTool(){
	short id = -1;
	do{
		printf("\n Please select a recovery tool among the following :\n");
		printf(" 1 - Vipasana Recovery tool (require a plain and a ciphered text)\n");
		cin.clear();
		cin >> id;
		if(cin.fail()){
			cin.clear();
			id = 0;
		}
		id--;
	} while (id < 0 || id > recovery_tools.size()-1);
	return id;
}

void listDirectory(string path){
   WIN32_FIND_DATA ffd;
   TCHAR szDir[MAX_PATH];
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
		  if(filename.compare(".")!=0 && filename.compare("..")!=0)
			listDirectory(dir.c_str());
      }
	  // IT'S A FILE
      else
      {
		 // File analysis to put here
		 launch_thread(dir.c_str());
         //printf("  %s   <FILE>\n", dir.c_str());
      }
   }
   while (FindNextFile(hFind, &ffd) != 0);
   FindClose(hFind);
}

void write_report(){
	ofstream file(cipheredFileName);
	if(!file.is_open()){
		printf("Error while writing log...\n");
		return;
	}
	for(vector<string>::iterator it = ciphered_files_path.begin() ; it != ciphered_files_path.end() ; ++it){
		file << *it;
	}
	file.close();

	printf("%s generated.\n", cipheredFileName);
}

void recover_files(short id_tool_selected){
	ifstream file(cipheredFileName);
	if(!file.is_open()){
		printf("Error while opening %s\n", cipheredFileName);
		return;
	}

	VipasanaRecover *vipasanaRecover;

	// Preparing Vipasana Recovery Files
	if(!string("Vipasana").compare(recovery_tools[id_tool_selected])){
		string plainpath = "";
		string cipheredpath = "";
		printf("You have chosen Vipasana.\n");

		plainpath = askUserForFilePath(true);
		cipheredpath = askUserForFilePath(false);

		vipasanaRecover = new VipasanaRecover(plainpath, cipheredpath);
	}

	// Iterating through the list to recover.
	string line;
	string delimiter("|");
	while(getline(file,line)){
		string ransomware_name = line.substr(0, line.find(delimiter));
		string file_path = line.substr(line.find(delimiter)+1, line.size());

		// Vipasana file, need the Vipasana Recovery Tool
		if(!ransomware_name.compare("Vipasana")){
			vipasanaRecover->decipher(file_path);
		}
	}
	file.close();
}


int _tmain(int argc, _TCHAR* argv[])
{
	// Initializing recovery tool available.
	recovery_tools.push_back("Vipasana");

	short userChoice = 0;
	printFirstMenu();
	do{
		printChoiceMenu();
		userChoice = getUserChoice();

	} while(userChoice != 1 && userChoice != 2) ;

	string path("");
	switch(userChoice){
		case 1:
			path = askUserForPath();
			init_threading();
			listDirectory(path);
			end_threading();
			write_report();
			break;
		case 2:
			// Opening the C:/ciphered_files_list.txt and try to recover files according to the attached ransomware's name.
			short selected_id = askUserForRecoveryTool();
			recover_files(selected_id);
			break;
	}

    while(1);
	return 0;
}

