//#include "stdafx.h"
#include "utile.h"
#include <iostream>
#include "stdlib.h"
#include <stdint.h>
#include <time.h>
#include <string.h>

ifstream loadFile(char const *path){
	ifstream file;
	file.open(path, ios::binary);

	if (!file) {
		cerr << "Error while opening files...";
		exit(1);   // call system to stop
	} else {
		return file;
	}
}

bool loadFileAndLoadBuffer(char const *path, char *dst, int size){
	ifstream file;
	file.open(path, ios::binary);

	if (!file) {
		cerr << "Error while opening files...";
		file.close();
		return false;   // call system to stop
	} else {
		if(file.read (dst, size))
			printf("%s has been loaded...\n", path);
		else {
		    file.close();
		    return false;
	    }
	}
	file.close();
	
	return true;
}

