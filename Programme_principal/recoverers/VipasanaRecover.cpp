//#include "stdafx.h"
#include "VipasanaRecover.h"
#include <iostream>
#include "stdlib.h"
#include <time.h>
#include <string.h>
#include "etude.h"

unsigned int finRunningValues[12] = {0xac9e61bf, 0x683fdf0c, 0x93ea96d4, 0x8b7e1344,
								     0x041541b0, 0x99d73cce, 0xc358752b, 0xb56a774c,
								     0x0792ba45, 0x0b180141, 0xa55f630e, 0xf392f654};

char finKeystream[96] = {0x62, 0x66, 0x36, 0x31, 0x39, 0x65, 0x61, 0x63, 0x30, 0x63, 0x64, 0x66, 0x33, 0x66, 0x36, 0x38,
						 0x64, 0x34, 0x39, 0x36, 0x65, 0x61, 0x39, 0x33, 0x34, 0x34, 0x31, 0x33, 0x37, 0x65, 0x38, 0x62,
						 0x62, 0x30, 0x34, 0x31, 0x31, 0x35, 0x30, 0x34, 0x63, 0x65, 0x33, 0x63, 0x64, 0x37, 0x39, 0x39,
						 0x32, 0x62, 0x37, 0x35, 0x35, 0x38, 0x63, 0x33, 0x34, 0x63, 0x37, 0x37, 0x36, 0x61, 0x62, 0x35,
						 0x34, 0x35, 0x62, 0x61, 0x39, 0x32, 0x30, 0x37, 0x34, 0x31, 0x30, 0x31, 0x31, 0x38, 0x30, 0x62,
						 0x30, 0x65, 0x36, 0x33, 0x35, 0x66, 0x61, 0x35, 0x35, 0x34, 0x66, 0x36, 0x39, 0x32, 0x66, 0x33}; //Se repete des l'octets 768

VipasanaRecover::VipasanaRecover(string plaintext_path, string ciphered_path, bool isDebug) : isDebug(isDebug)
{
    init(plaintext_path, ciphered_path, true);
}

VipasanaRecover::VipasanaRecover(string plaintext_path, string ciphered_path)
{
    init(plaintext_path, ciphered_path, false);
}

void VipasanaRecover::init(string plaintext_path, string ciphered_path, bool isDebug)
{
    char plain_bytes[30000], ciphered_bytes[30000];

	bool isPlainLoaded = loadFileAndLoadBuffer(plaintext_path.c_str(), plain_bytes, 30000);
	bool isCipherLoaded = loadFileAndLoadBuffer(ciphered_path.c_str(), ciphered_bytes, 30000);
	
	if (!isPlainLoaded || !isCipherLoaded)
	{
	    cout << "Could not load files, maybe too short (< 30000 Bytes)" << endl;
	    return;
	}

	for (int i=0; i<30000; i++) shift[i] = ciphered_bytes[i]-plain_bytes[i];

	findGlobalKey(globalKey, (uint8_t*)plain_bytes, (uint8_t*)ciphered_bytes, 30000, isDebug);
}

char* VipasanaRecover::getGlobalKey()
{
    return globalKey;
}

void VipasanaRecover::printGlobalKey() const
{
	printf("\nGlobal Static Key is : \n");
	
	for(int i=0; i<20; i++)
	{
		printf("Global[%d] : %c\n", i, globalKey[i]);
	}
}

void VipasanaRecover::decipher(string path) const
{
	// Open the file
	ifstream file = loadFile(path.c_str());
	
	long const length3 = pFin(file);
    long const length1 = (length3 > 768)?768:length3;
	long const length2 = (length3 > 30000)?30000:length3;

    string const decipheredPath = path.substr(0, static_cast<int>(strrchr(path.c_str(), '/')-path.c_str()+1))+name(file);
    file.seekg(0, file.beg);

	// Creating new file
	char begin[768];
	ofstream decipheredfile;
	decipheredfile.open(decipheredPath, ios::binary);
	decipheredfile.seekp(0, decipheredfile.beg);

	for (long i=0; i<length1 ; i++)
	{
		file.get(begin[i]);
		begin[i] = reverse_algo(globalKey[i%20], begin[i], i);
	}

	regenerateHeader(decipheredfile, decipheredPath, begin, length3);
	decipheredfile.write(begin, length1);

	for (long i=768; i<length2; i++)
	{
		char buff;
		file.get(buff);
		buff = buff - shift[i];
		decipheredfile << buff;
	}
	
	for (long i=30000; i<length3; i++)
	{
		char buff;
		file.get(buff);
		decipheredfile << buff;
	}
	
	decipheredfile.close();
	file.close();
	
	cout << "File Deciphered : " << decipheredPath << endl;
}

long VipasanaRecover::pFin(ifstream &file) const
{
    file.seekg(-16, file.end);
    
    char byte;
    file.get(byte);

    int iExposant = 1;
    long dEnteteFin = 0; //Taille du bloc a la fin du fichier
    
    while (byte != '{')
    {
        dEnteteFin = dEnteteFin+(byte-'0')*iExposant;
        iExposant *= 10;
        
        file.seekg(-2, ios_base::cur);
        file.get(byte);
    }
    
	long res = file.tellg();
	res = res - dEnteteFin;
	res = res - 1l;

    return res;
}

string VipasanaRecover::name(ifstream &file) const
{
    //On se place au debut du nom
    char byte;
    file.seekg(pFin(file), ios_base::beg);

    for (short i=0; i<5; i++) 
    {
        while (byte != '}') file.get(byte);
        file.get(byte);
    }
    file.get(byte);
    
    //on récupere le nom
    string res = "";

    while (byte != '}')
    {
        res += byte;
        file.get(byte);
    }

    return res;
}

//Usefull functions
void findGlobalKey(char *globalKey, uint8_t *plain, uint8_t *ciphered, int tailleCiphered, bool isDebug)
{
	short candidatsGlobalKey[20][10];
	for (int i=0; i<20; i++) for (int j=0; j<10; j++) candidatsGlobalKey[i][j] = 0;

	for (int i=768; i<tailleCiphered; i++)
	{
		short value = -1;
		uint8_t const plainByte = plain[i];
		uint8_t const cipheredByte = ciphered[i];
		uint8_t const keystreamByte = finKeystream[(i-768)%96];

		if (cipheredByte == (uint8_t)(plainByte+keystreamByte)) value = 0;
		if (cipheredByte == (uint8_t)(plainByte-keystreamByte)) value = 1;
		if (cipheredByte == (uint8_t)(plainByte+keystreamByte+0x32)) value = 2;
		if (cipheredByte == (uint8_t)(plainByte+keystreamByte+0x03)) value = 3;
		if (cipheredByte == (uint8_t)(plainByte-keystreamByte-0x34)) value = 4;
		if (cipheredByte == (uint8_t)(plainByte-keystreamByte-0x05)) value = 5;
		if (cipheredByte == (uint8_t)(plainByte+0x06)) value = 6;
		if (cipheredByte == (uint8_t)(plainByte-0x07)) value = 7;
		if (cipheredByte == (uint8_t)(plainByte+0x38)) value = 8;
		if (cipheredByte == (uint8_t)(plainByte-0x39)) value = 9;

		if (value != -1) candidatsGlobalKey[i%20][value]++;
		else if (isDebug) printf("Probleme globalKey octet : %d, char : %c, \n", i, plainByte);
	}

    if (isDebug)
    {
	    printf("Table de probabilite pour la globalKey :\n");

	    for (short i=0; i<20; i++)
	    {
		    printf("[%d] : [", i);
		    for (short j=0; j<10; j++) printf("%d ", candidatsGlobalKey[i][j]);
		    printf("]\n");
	    }
	}

	for (short i=0; i<20; i++)
	{
		short value = 0;
		for (short j=1; j<10; j++) if (candidatsGlobalKey[i][j] > candidatsGlobalKey[i][value]) value = j;

		globalKey[i] = '0'+value;
	}
}

char reverse_algo(char global_key_byte, char ciphered, int nByte){
	char value = ciphered;
	char keystreamByte = finKeystream[(nByte-768)%96];

	switch (global_key_byte)
	{
		case '0':
			value -= keystreamByte;
			break;
		case '1':
			value += keystreamByte;
			break;
		case '2':
			value -= keystreamByte;
			value -= 0x32;
			break;
		case '3':
			value -= keystreamByte;
			value -= 0x03;
			break;
		case '4':
			value += keystreamByte;
			value += 0x34;
			break;
		case '5':
			value += keystreamByte;
			value += 0x05;
			break;
		case '6':
			value -= 0x06;
			break;
		case '7':
			value += 0x07;
			break;
		case '8':
			value -= 0x38;
			break;
		case '9':
			value += 0x39;
			break;
	}

	return value;
}

void inverseV1(uint32_t* a, uint32_t b, uint32_t c, uint32_t d, uint32_t staticValue, uint8_t shift, uint32_t baseInteger)
{
	*a = rotate_right(*a - b, shift) - baseInteger - staticValue - f1(b, c, d);
}

void inverseV2(uint32_t* a, uint32_t b, uint32_t c, uint32_t d, uint32_t staticValue, uint8_t shift, uint32_t baseInteger)
{
	*a = rotate_right(*a - b, shift) - baseInteger - staticValue - f2(b, c, d);
}

void inverseV3(uint32_t* a, uint32_t b, uint32_t c, uint32_t d, uint32_t staticValue, uint8_t shift, uint32_t baseInteger)
{
	*a = rotate_right(*a - b, shift) - baseInteger - staticValue - f3(b, c, d);
}

void inverseV4(uint32_t* a, uint32_t b, uint32_t c, uint32_t d, uint32_t staticValue, uint8_t shift, uint32_t baseInteger)
{
	*a = rotate_right(*a - b, shift) - baseInteger - staticValue - f4(b, c, d);
}
