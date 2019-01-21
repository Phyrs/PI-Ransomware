// That program tries to determine if a file has been ciphered by Vipasana
//

#ifndef VIPASANA_SIGNATURE
#define VIPASANA_SIGNATURE

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <string>
#include <regex>
using namespace std;


string getFileName(char *path_char);

ifstream loadFile(char *path);

long getFileLength(ifstream &file);


long get_beginvipasana_signblock_position(ifstream &file, bool hugefile, long length);


bool check_sign_end_of_file(ifstream &file);

bool check_filename_correspondance(string filename);

void print_report(string filename, bool eof_correspondance, bool filename_correspondance);

#endif