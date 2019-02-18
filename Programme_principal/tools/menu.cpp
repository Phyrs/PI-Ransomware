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
	printf(" 2 - Recover files from a previous analysis (Supposes that you have already analyzed the disk and the file %s has been created.\n", cipheredFileName.c_str());
}

short getUserChoice(){
	short choice;
	cin >> choice;
	if(cin.fail()){
		cin.clear();
		return 0;
	}
	return  choice;
}

string askUserForPath(){
	bool isPathADir = false;
	string path = "";
	do{
		printf("\n Please enter a specific path (to a directory) to consider (Enter C:/ to analyze the whole disk) :\n");
		cin.clear();
		cin >> path;
		isPathADir = isADirectory(path);
	} while (!isPathADir);
	return path;
}

string askUserForFilePath(bool plain){
	bool isPathAFile = false;
	string path = "";
	do{
		if(plain)
			printf("Please enter the path of the plaintext file :\n");
		else
			printf("Now, please enter the path of the cipheredtext file :\n");
		cin.clear();
		cin >> path;
		isPathAFile = isAFile(path);
	} while (!isPathAFile);
	return path;
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

