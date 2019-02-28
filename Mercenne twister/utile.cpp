//#include "stdafx.h"
#include "utile.h"
#include <iostream>
#include "stdlib.h"
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <dirent.h>
#include <bits/stdc++.h> 

string executer(string commande)
{
    char iRes[1024];
    string res = "";
    FILE *pipe = popen(commande.c_str(), "r");

    while (fgets(iRes, 1024, pipe)) res += iRes;
    pclose(pipe);

    return res;
}

long nbElementsDans(string path)
{
    long res = -2;

#ifdef WINDOWS
    WIN32_FIND_DATA File; 
    HANDLE hSearch; 
    hSearch = FindFirstFile("*.*", &File); 

    if (hSearch != INVALID_HANDLE_VALUE) 
    { 
        do
        { 
            printf("%s\n", File.cFileName); 
        }
        while (FindNextFile(hSearch, &File)); 
  
        FindClose(hSearch); 
    }
#else
    DIR *dossier = opendir(path.c_str()); 
  
    if (dossier) 
    {
        struct dirent *lien; 
        while ((lien = readdir(dossier))) res++;
        closedir(dossier);
    }
#endif

    return res;
}

stack<string> fichiersEtDossiersDans(string path)
{
    string iRes = executer("ls -at \""+path+"\"");
    stack<string> res;

    long fin = iRes.find('\n');

    while (fin != -1)
    {
        string element = iRes.substr(0, fin);
        if (element != ".." && element != ".") res.push(path+"/"+iRes.substr(0, fin));

        iRes = iRes.substr(fin+1);
        fin = iRes.find('\n');
    }

    return res;
}

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

void intToChars(int number, char res[4])
{
    res[3] = number >> 24;
    res[2] = (number%(1<<24)) >> 16;
    res[1] = (number%(1<<16)) >> 8;
    res[0] = number%(1<<8);
}

void regenerateHeader(ofstream &file, string path, char begin[768], long length)
{
    long iPos = file.tellp();
    string extension = path.substr(static_cast<int>(strrchr(path.c_str(), '.')-path.c_str()+1));

    if (extension == "bmp")
    {
        begin[0] = 0x42;
        begin[1] = 0x4d;
        
        intToChars(length, begin+2); //Length of the file
        
        //Start of the pixels
        begin[10] = 0x7a;
        begin[11] = 0x00;
        begin[12] = 0x00;
        begin[13] = 0x00;
        
        //Length of the header
        begin[14] = 0x6c;
        begin[15] = 0x00;
        begin[16] = 0x00;
        begin[17] = 0x00;

        int const nbPxs = (length-122)/3;
        
        //A determiner selon nbPxs (produit qui tombe juste)
        int const tx = 640;
        int const ty = 400;

        intToChars(tx, begin+18);
        intToChars(ty, begin+22);
    }
    else if (extension == "jpg" || extension == "jpeg")
    {
        //static
        begin[0] = 0xff;
        begin[1] = 0xd8;
        begin[2] = 0xff;
        begin[3] = 0xe0;
        
        //segment length
        begin[4] = 0x00;
        begin[5] = 0x10;
        
        //static id
        begin[6] = 0x4a;
        begin[7] = 0x46;
        begin[8] = 0x49;
        begin[9] = 0x46;
        begin[10] = 0x00;
        
        //version
        begin[11] = 0x01;
        begin[12] = 0x01;

        begin[13] = 0x01; //type px/cm
        
        //width px/cm
        begin[14] = 0x00;
        begin[15] = 0x48;
        
        //height px/cm
        begin[16] = 0x00;
        begin[17] = 0x48;
        
        //width, height of icon
        begin[18] = 0x00;
        begin[19] = 0x00;
        
        begin[20] = 0xff;
    }
    
    file.seekp(iPos, file.beg);
}

void testFichier()
{
    ifstream file = loadFile("Tests/save/lancement3/image.jpg");
    //ifstream file = loadFile("/home/phyrs/Modèles 3D/fourtout/Slasher.jpg");

	file.seekg(0, file.beg);
	
	for (long i=0; i<100; i++)
	{
        char buff;
	    file.get(buff);
	    printf("%d %02x\n", i, buff);
	}
	
	char iLength[4];
	intToChars(768122, iLength);
    printf("%02x %02x %02x %02x\n", iLength[0], iLength[1], iLength[2], iLength[3]);
    
    intToChars(640, iLength);
    printf("%02x %02x %02x %02x\n", iLength[0], iLength[1], iLength[2], iLength[3]);

    intToChars(400, iLength);
    printf("%02x %02x %02x %02x\n", iLength[0], iLength[1], iLength[2], iLength[3]);
}
