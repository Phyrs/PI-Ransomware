//#include "stdafx.h"
#include "utile.h"
#include <iostream>
#include "stdlib.h"
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <math.h>

string executer(string commande)
{
    char iRes[1024];

    string res = "";
    FILE *pipe = popen(commande.c_str(), "r");

    while (fgets(iRes, 1024, pipe)) res += iRes;
    pclose(pipe);

    return res;
}

//https://github.com/floyernick/fleep-py/blob/master/fleep/__init__.py
string getExtension(string path)
{
    ifstream fExtensions;
    int const nbExtensions = 100;

	fExtensions.open("extensions.json");
	
	if (!fExtensions)
	{
	    cout << "erreur extensions non trouvees" << endl;
	    fExtensions.close();
	    return "";
	}

	fExtensions.seekg(0, ios::beg);
	
	char caractere;
	int offset[nbExtensions];
	string extensions[nbExtensions];
    uint8_t *signature[nbExtensions];
    int taillesSignatures[nbExtensions];

    for (int i=0; i<nbExtensions; i++)
    {
    	extensions[i] = "";
	    for (int j=0; j<7; j++) while (fExtensions.get(caractere) && caractere != '"');
	    while (fExtensions.get(caractere) && caractere != '"') extensions[i] += caractere;

        offset[i] = 0;
	    for (int j=0; j<6; j++) while (fExtensions.get(caractere) && caractere != '"');
	    fExtensions.get(caractere);
	    fExtensions.get(caractere);
	    
	    while (fExtensions.get(caractere) && caractere != ',') offset[i] = offset[i]*10+(caractere-'0');

	    taillesSignatures[i] = 0;
	    for (int j=0; j<3; j++) while (fExtensions.get(caractere) && caractere != '"');
	    long debut = fExtensions.tellg();
	    fExtensions.get(caractere);

	    while (caractere != '"')
	    {
	        while (fExtensions.get(caractere) && caractere != ' ' && caractere != '"');
	        taillesSignatures[i]++;
	    }

        fExtensions.seekg(debut, fExtensions.beg);
        signature[i] = new uint8_t[taillesSignatures[i]];

		for (int j=0; j<taillesSignatures[i]; j++)
	    {
	        string iSignature = "";
	        while (fExtensions.get(caractere) && caractere != ' ' && caractere != '"') iSignature += caractere;

	        signature[i][j] = strtol(iSignature.c_str(), NULL, 16);
	    }
	    
	    while (fExtensions.get(caractere) && caractere != '}');
	}
	
	fExtensions.close();

    ifstream fichier;
	fichier.open(path.c_str(), ios::binary);

	for (int i=0; i<nbExtensions; i++)
	{
	    int j = 0;
	    fichier.seekg(offset[i], ios::beg);

	    while (j < taillesSignatures[i] && fichier.get(caractere) && static_cast<uint8_t>(caractere) == signature[i][j]) j++;
	    
	    if (j == taillesSignatures[i]) return extensions[i];
	}
	
	fichier.close();
	
	return "txt";
}

long nbElementsDans(string path)
{
    long res = -2;

#ifdef WINDOWS
	path += "\\";
    HANDLE hSearch;
	WIN32_FIND_DATA File;
	string pathAdapte = path+"*.*";
    hSearch = FindFirstFile(wstring(pathAdapte.begin(), pathAdapte.end()).c_str(), &File); 

    if (hSearch != INVALID_HANDLE_VALUE) 
    { 
        do
        { 
			res++;
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

string* fichiersEtDossiersDans(string path)
{
	int nbFichiers = nbElementsDans(path);
	string *res = new string[nbFichiers];

#ifdef WINDOWS
	path += "\\";
	HANDLE hSearch;
	WIN32_FIND_DATA File;
	string pathAdapte = path+"*.*";
	FILETIME *dateFichiers = new FILETIME[nbFichiers];
    hSearch = FindFirstFile(wstring(pathAdapte.begin(), pathAdapte.end()).c_str(), &File); 

	int nFichier = 0;

    if (hSearch != INVALID_HANDLE_VALUE) 
    { 
        do
        {
			wstring iFichier = File.cFileName;
			string fichier(iFichier.begin(), iFichier.end());

			if (fichier == "." || fichier == "..") continue;
			fichier = path+fichier;

			HANDLE hFile = CreateFile(wstring(fichier.begin(), fichier.end()).c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
				                      FILE_ATTRIBUTE_NORMAL, NULL);

			GetFileTime(hFile, NULL, NULL, &dateFichiers[nFichier]);
			if (hFile == INVALID_HANDLE_VALUE) continue;

			res[nFichier] = fichier;
			FILETIME dateFichier = dateFichiers[nFichier];

			for (int i=0; i<nFichier; i++)
			{
				if (CompareFileTime(&dateFichier, &dateFichiers[i]) < 0)
				{
					for (int j=nFichier; j>i; j--)
					{
						res[j] = res[j-1];
						dateFichiers[j] = dateFichiers[j-1];
					}

					res[i] = fichier;
					dateFichiers[i] = dateFichier;
					break;
				}
			}

			nFichier++;
        }
        while (FindNextFile(hSearch, &File)); 

        FindClose(hSearch); 
    }
#else
    string iRes = executer("ls -atr \""+path+"\"");

    long i = 0;
    long fin = iRes.find('\n');

    while (fin != -1)
    {
        string element = iRes.substr(0, fin);

        if (element != ".." && element != ".")
        {
            res[i] = path+"/"+iRes.substr(0, fin);
            i++;
        }

        iRes = iRes.substr(fin+1);
        fin = iRes.find('\n');
    }
#endif //WINDOWS

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
