// cry.cpp : définit le point d'entrée pour l'application console.
//
//#include "Analyzer_Chi_Squared.h"
#include "stdafx.h"
#include "stdlib.h"
#include <iostream>
#include <fstream>
#include <string>
#include <regex>

using namespace std;

string getFileName(char *path_char){
	string path(path_char);

	char sep = '/';

	size_t i = path.rfind(sep, path.length());
	if(i != string::npos)
		return path.substr(i+1, path.length() - i);

	return "";
}

ifstream loadFile(char *path){
	ifstream file;
	file.open(path, ios::binary);

	if (!file) {
		cerr << "Error while opening file, please check the name...";
		cin >> path;
		exit(1);   // call system to stop
	} else {
		return file;
	}
}

long getFileLength(ifstream &file){
	file.seekg(0, file.end);
	long length = file.tellg();
	file.clear();
	file.seekg(0, file.beg);
	return length;
}



bool check_filename_correspondance(string filename){
	regex crypy_name(".sinta");

	bool res = false;
	if(regex_search(filename, crypy_name)){
		res = true;
	}

	return res;
}

void print_report(string filename, bool filename_correspondance){
	printf("------------------------------------------\n");
	printf("         REPORT of %s analysis            \n", filename.c_str());
	printf("------------------------------------------\n");
	printf("\n");
	printf(" - Filename corresponds to the Crypy's typical filename : %s\n", (filename_correspondance ? "yes" : "no"));
	//printf(" - File corresponds to a khi square repartition : %s\n", (khi_square ? "yes" : "no"));
}


/**
* The argument is a path to a file that has been supposedly encrypted with vipasana.
*/
int _tmain(int argc, _TCHAR* argv[])
{
	//analyzer = (AnalyzerChiSquared *) ana;
	//bool khi_square = ana->is_ciphered_by_ransomware();
	char *file_path = "C:/Documents and Settings/Administrateur/Bureau/8E6cO4jiEVFx5JGJXAcMGD9YXsphvbz54tlX.sinta";
	string filename = getFileName(file_path);

	ifstream file = loadFile(file_path);

	// Checking for the filename name
	printf("Checking filename correspondance...\n");
	bool filename_correspondance = check_filename_correspondance(filename);

	file.close();

	print_report(filename, filename_correspondance);

	//cin >> argc;
	while(1){};
	return 0;
}
