#include "menu.h"
#include <vector>
#include <string>
#include "stdlib.h"
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>

using namespace std;

string cipheredFileName("C:/cipheredfiles_list.txt");
string reportFileName("C:/analysis_detailed_report.txt");
string hiddenReportFileName("C:/hidden_tear_report.txt");
vector<string> recovery_tools;


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
	printf(" 2 - Analyze Pcap file\n");
	printf(" 3 - Try to bruteforce HiddenTear (Requires a plain/ciphered pair)\n");
	printf(" 4 - Recover files from a previous analysis (Supposes that you have already analyzed the disk and the file %s has been created.)\n", cipheredFileName.c_str());
}

short getUserChoice(){
	int choice;
	cin >> choice;
	if(cin.fail()){
		cin.clear();
		return 0;
	}
	return  choice;
}

string askUserForPath(string text){
	bool isPathADir = false;
	string path;
	fflush(stdin);
	do{
		printf(text.c_str());
		getline(cin,path);
		cin.clear();
		isPathADir = isADirectory(path);
	} while (!isPathADir);
	return path;
}

string askUserForFilePath(string text){
	bool isPathAFile = false;
	string path;
	fflush(stdin);
	do{
		printf(text.c_str());
		getline(cin,path);
		cin.clear();
		isPathAFile = isAFile(path);
	} while (!isPathAFile);
	return path;
}

string askUserForFifteenBytesPass(string text){
	bool isFifteen = false;
	string pass = "";
	do{
		printf(text.c_str());
		cin.clear();
		cin >> pass;
		if(pass.size() == 15)
			isFifteen = true;
	} while (!isFifteen);
	return pass;
}

bool isADirectory(string path){
	struct stat s;
	if(stat(path.c_str(), &s) == 0){
		if(s.st_mode & S_IFDIR){
			return true;
		}
	}
	return false;
}

bool isAFile(string path){
	struct stat s;
	if(stat(path.c_str(), &s) == 0){
		if(s.st_mode & S_IFREG){
			return true;
		}
	}
	return false;
}

short askUserForRecoveryTool(){
	short id = -1;
	do{
		printf("\n Please select a recovery tool among the following :\n");
		printf(" 1 - Vipasana Recovery tool (requires a plain and a ciphered text)\n");
		printf(" 2 - HiddenTear Recovery tool (requires the 15 bytes password from a .pcap analysis or a bruteforce attack)\n");
		printf(" 3 - CryPy Recovery tool (requires CryPy's ciphered files folder's path)\n");
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

void write_report(vector<string> list, string path){
	ofstream file(path);
	if(!file.is_open()){
		printf("Error while writing log...\n");
		return;
	}
	for(vector<string>::iterator it = list.begin() ; it != list.end() ; ++it){
		file << *it;
	}
	file.close();

	printf("%s generated.\n", path.c_str());
}
