/**
 *   That application software provides you some features to help in file system ciphering detection.
 *   It is based on generic detection of ciphered files and some specific ransomwares' signature (Vipasana, HiddenTear and CryPy).
 *  
 *   It also lets you try to recover the files if they have been ciphered by a ransomware whose a recovery module is available for that soft.
 */


#include <stdio.h>
#include "analyzers/Pcap_Investigator.h"
#include "stdlib.h"
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>
#include <tchar.h> 

#include <strsafe.h>
#include "tools/multithreading.h"
#include "tools/menu.h"
#include "recoverers/VipasanaRecover.h"
#include "recoverers/HiddenTearRecoverer.h"

#pragma comment(lib, "User32.lib")

using namespace std;

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

void recover_files(short id_tool_selected){
	ifstream file(cipheredFileName);
	if(!file.is_open()){
		printf("Error while opening %s\n", cipheredFileName.c_str());
		return;
	}

	VipasanaRecover *vipasanaRecover;
	// Preparing Vipasana Recovery tool
	if(!string("Vipasana").compare(recovery_tools[id_tool_selected])){
		string plainpath = "";
		string cipheredpath = "";
		printf("You have chosen Vipasana.\n");

		plainpath = askUserForFilePath(string("Please enter the path of the plaintext file :\n"));
		cipheredpath = askUserForFilePath(string("Now, please enter the path of the cipheredtext file :\n"));

		vipasanaRecover = new VipasanaRecover(plainpath, cipheredpath);
	}

	HiddenTearRecoverer *hiddenTearRecoverer;
	// Preparing HiddenTear Recovery tool
	if(!string("HiddenTearPassword").compare(recovery_tools[id_tool_selected])){
		string password = "";
		printf("You have chosen HiddenTear RecoveryTool.\n");

		password = askUserForFifteenBytesPass(string("Please enter the 15 caracters pass you have discovered (must be 15 carac.)\n"));

		hiddenTearRecoverer = new HiddenTearRecoverer(password);
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
		if(!ransomware_name.compare("HiddenTear")){
			hiddenTearRecoverer->decipher(file_path);
		}
	}
	file.close();
}

int _tmain(int argc, _TCHAR* argv[])
{
	// Initializing recovery tool available.
	recovery_tools.push_back("Vipasana");
	recovery_tools.push_back("HiddenTearPassword");

	short userChoice = 0;
	printFirstMenu();
	do{
		printChoiceMenu();
		userChoice = getUserChoice();

	} while(userChoice != 1 && userChoice != 2 && userChoice != 3) ;

	string path("");
	switch(userChoice){
		case 1:
			path = askUserForPath();
			init_threading(reportFileName);
			listDirectory(path);
			end_threading();
			write_report(ciphered_files_path, cipheredFileName);
			break;
		case 2:
			path = askUserForFilePath(string("Please enter the path to your Pcap file: \n"));
			pcap_parse_file(path);
			break;
		case 3:
			// Opening the C:/ciphered_files_list.txt and try to recover files according to the attached ransomware's name.
			short selected_id = askUserForRecoveryTool();
			recover_files(selected_id);
			break;
	}

    while(1);
	return 0;
}

