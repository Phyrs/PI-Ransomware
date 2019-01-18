//#include "stdafx.h"
#include "etude.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <iostream>

void v1(uint32_t* a, uint32_t b, uint32_t c, uint32_t d, uint32_t staticValue, uint8_t shift, uint32_t baseInteger) { // 004148d8
	*a = rotate_left(f1(b, c, d) + *a + staticValue + baseInteger, shift) + b;
}

void v2(uint32_t* a, uint32_t b, uint32_t c, uint32_t d, uint32_t staticValue, uint8_t shift, uint32_t baseInteger) { // 00414910
	*a = rotate_left(f2(b, c, d) + *a + staticValue + baseInteger, shift) + b;
}

void v3(uint32_t* a, uint32_t b, uint32_t c, uint32_t d, uint32_t staticValue, uint8_t shift, uint32_t baseInteger) { // 00414960
	*a = rotate_left(f3(b, c, d) + *a + staticValue + baseInteger, shift) + b;
}

void v4(uint32_t* a, uint32_t b, uint32_t c, uint32_t d, uint32_t staticValue, uint8_t shift, uint32_t baseInteger) { // 00414980
	*a = rotate_left(f4(b, c, d) + *a + staticValue + baseInteger, shift) + b;
}

void readUint32ArrayFromByteArray(uint32_t* destination, uint8_t* source, uint32_t byteCount) { // 00414a00
	for (; byteCount > 0; byteCount -= 4) {
		destination[((int)(byteCount / 4)) - 1] =
			((source[byteCount - 1] << 24) & 0xff000000) |
			((source[byteCount - 2] << 16) & 0x00ff0000) |
			((source[byteCount - 3] <<  8) & 0x0000ff00) |
			((source[byteCount - 4] <<  0) & 0x000000ff);
	}
}

void updateState1(uint8_t* state) { // 0043547b
	for (uint32_t i = 0; i < 0x200; i++) {
		uint32_t currentValue = (uint32_t)state[i];
		state[i] = (state[i] + 0x80 > 0x100) ? state[i] - 0x80 : state[i] + 0x80;
	}
}

void updateState2(uint8_t* state) { // 004354f8
	for (uint32_t i = 0; i < 0x1ff; i++) {
		state[i] += state[i + 1];
	}
}

void updateState3(uint8_t* state) { // 00435589
	for (uint32_t i = 0; i < 0x200; i++) {
		state[i] *= 2;
	}
}

int isBlocNul(uint8_t* bloc)
{
    int i=0;
    for (i=0; i<512; i++)
    {
        if (bloc[i]) return 0;
    }
    return 1;
}

void scrambleValues(uint32_t* toUpdate, uint8_t* baseBytes) { // 00414a58
	uint32_t oldValues[] = {
		toUpdate[0],
		toUpdate[1],
		toUpdate[2],
		toUpdate[3]
	};

	uint32_t baseIntegers[16];
	readUint32ArrayFromByteArray(baseIntegers, baseBytes, 0x40);

	v1(oldValues + 0, oldValues[1], oldValues[2], oldValues[3], 0xd76aa478, 0x07, baseIntegers[ 0]);
	v1(oldValues + 3, oldValues[0], oldValues[1], oldValues[2], 0xe8c7b756, 0x0c, baseIntegers[ 1]);
	v1(oldValues + 2, oldValues[3], oldValues[0], oldValues[1], 0x242070db, 0x11, baseIntegers[ 2]);
	v1(oldValues + 1, oldValues[2], oldValues[3], oldValues[0], 0xc1bdceee, 0x16, baseIntegers[ 3]);
	v1(oldValues + 0, oldValues[1], oldValues[2], oldValues[3], 0xf57c0faf, 0x07, baseIntegers[ 4]);
	v1(oldValues + 3, oldValues[0], oldValues[1], oldValues[2], 0x4787c62a, 0x0c, baseIntegers[ 5]);
	v1(oldValues + 2, oldValues[3], oldValues[0], oldValues[1], 0xa8304613, 0x11, baseIntegers[ 6]);
	v1(oldValues + 1, oldValues[2], oldValues[3], oldValues[0], 0xfd469501, 0x16, baseIntegers[ 7]);
	v1(oldValues + 0, oldValues[1], oldValues[2], oldValues[3], 0x698098d8, 0x07, baseIntegers[ 8]);
	v1(oldValues + 3, oldValues[0], oldValues[1], oldValues[2], 0x8b44f7af, 0x0c, baseIntegers[ 9]);
	v1(oldValues + 2, oldValues[3], oldValues[0], oldValues[1], 0xffff5bb1, 0x11, baseIntegers[10]);
	v1(oldValues + 1, oldValues[2], oldValues[3], oldValues[0], 0x895cd7be, 0x16, baseIntegers[11]);
	v1(oldValues + 0, oldValues[1], oldValues[2], oldValues[3], 0x6b901122, 0x07, baseIntegers[12]);
	v1(oldValues + 3, oldValues[0], oldValues[1], oldValues[2], 0xfd987193, 0x0c, baseIntegers[13]);
	v1(oldValues + 2, oldValues[3], oldValues[0], oldValues[1], 0xa679438e, 0x11, baseIntegers[14]);
	v1(oldValues + 1, oldValues[2], oldValues[3], oldValues[0], 0x49b40821, 0x16, baseIntegers[15]);

	v2(oldValues + 0, oldValues[1], oldValues[2], oldValues[3], 0xf61e2562, 0x05, baseIntegers[ 1]);
	v2(oldValues + 3, oldValues[0], oldValues[1], oldValues[2], 0xc040b340, 0x09, baseIntegers[ 6]);
	v2(oldValues + 2, oldValues[3], oldValues[0], oldValues[1], 0x265e5a51, 0x0e, baseIntegers[11]);
	v2(oldValues + 1, oldValues[2], oldValues[3], oldValues[0], 0xe9b6c7aa, 0x14, baseIntegers[ 0]);
	v2(oldValues + 0, oldValues[1], oldValues[2], oldValues[3], 0xd62f105d, 0x05, baseIntegers[ 5]);
	v2(oldValues + 3, oldValues[0], oldValues[1], oldValues[2], 0x02441453, 0x09, baseIntegers[10]);
	v2(oldValues + 2, oldValues[3], oldValues[0], oldValues[1], 0xd8a1e681, 0x0e, baseIntegers[15]);
	v2(oldValues + 1, oldValues[2], oldValues[3], oldValues[0], 0xe7d3fbc8, 0x14, baseIntegers[ 4]);
	v2(oldValues + 0, oldValues[1], oldValues[2], oldValues[3], 0x21e1cde6, 0x05, baseIntegers[ 9]);
	v2(oldValues + 3, oldValues[0], oldValues[1], oldValues[2], 0xc33707d6, 0x09, baseIntegers[14]);
	v2(oldValues + 2, oldValues[3], oldValues[0], oldValues[1], 0xf4d50d87, 0x0e, baseIntegers[ 3]);
	v2(oldValues + 1, oldValues[2], oldValues[3], oldValues[0], 0x455a14ed, 0x14, baseIntegers[ 8]);
	v2(oldValues + 0, oldValues[1], oldValues[2], oldValues[3], 0xa9e3e905, 0x05, baseIntegers[13]);
	v2(oldValues + 3, oldValues[0], oldValues[1], oldValues[2], 0xfcefa3f8, 0x09, baseIntegers[ 2]);
	v2(oldValues + 2, oldValues[3], oldValues[0], oldValues[1], 0x676f02d9, 0x0e, baseIntegers[ 7]);
	v2(oldValues + 1, oldValues[2], oldValues[3], oldValues[0], 0x8d2a4c8a, 0x14, baseIntegers[12]);

	v3(oldValues + 0, oldValues[1], oldValues[2], oldValues[3], 0xfffa3942, 0x04, baseIntegers[ 5]);
	v3(oldValues + 3, oldValues[0], oldValues[1], oldValues[2], 0x8771f681, 0x0b, baseIntegers[ 8]);
	v3(oldValues + 2, oldValues[3], oldValues[0], oldValues[1], 0x6d9d6122, 0x10, baseIntegers[11]);
	v3(oldValues + 1, oldValues[2], oldValues[3], oldValues[0], 0xfde5380c, 0x17, baseIntegers[14]);
	v3(oldValues + 0, oldValues[1], oldValues[2], oldValues[3], 0xa4beea44, 0x04, baseIntegers[ 1]);
	v3(oldValues + 3, oldValues[0], oldValues[1], oldValues[2], 0x4bdecfa9, 0x0b, baseIntegers[ 4]);
	v3(oldValues + 2, oldValues[3], oldValues[0], oldValues[1], 0xf6bb4b60, 0x10, baseIntegers[ 7]);
	v3(oldValues + 1, oldValues[2], oldValues[3], oldValues[0], 0xbebfbc70, 0x17, baseIntegers[10]);
	v3(oldValues + 0, oldValues[1], oldValues[2], oldValues[3], 0x289b7ec6, 0x04, baseIntegers[13]);
	v3(oldValues + 3, oldValues[0], oldValues[1], oldValues[2], 0xeaa127fa, 0x0b, baseIntegers[ 0]);
	v3(oldValues + 2, oldValues[3], oldValues[0], oldValues[1], 0xd4ef3085, 0x10, baseIntegers[ 3]);
	v3(oldValues + 1, oldValues[2], oldValues[3], oldValues[0], 0x04881d05, 0x17, baseIntegers[ 6]);
	v3(oldValues + 0, oldValues[1], oldValues[2], oldValues[3], 0xd9d4d039, 0x04, baseIntegers[ 9]);
	v3(oldValues + 3, oldValues[0], oldValues[1], oldValues[2], 0xe6db99e5, 0x0b, baseIntegers[12]);
	v3(oldValues + 2, oldValues[3], oldValues[0], oldValues[1], 0x1fa27cf8, 0x10, baseIntegers[15]);
	v3(oldValues + 1, oldValues[2], oldValues[3], oldValues[0], 0xc4ac5665, 0x17, baseIntegers[ 2]);

	v4(oldValues + 0, oldValues[1], oldValues[2], oldValues[3], 0xf4292244, 0x06, baseIntegers[ 0]);
	v4(oldValues + 3, oldValues[0], oldValues[1], oldValues[2], 0x432aff97, 0x0a, baseIntegers[ 7]);
	v4(oldValues + 2, oldValues[3], oldValues[0], oldValues[1], 0xab9423a7, 0x0f, baseIntegers[14]);
	v4(oldValues + 1, oldValues[2], oldValues[3], oldValues[0], 0xfc93a039, 0x15, baseIntegers[ 5]);
	v4(oldValues + 0, oldValues[1], oldValues[2], oldValues[3], 0x655b59c3, 0x06, baseIntegers[12]);
	v4(oldValues + 3, oldValues[0], oldValues[1], oldValues[2], 0x8f0ccc92, 0x0a, baseIntegers[ 3]);
	v4(oldValues + 2, oldValues[3], oldValues[0], oldValues[1], 0xffeff47d, 0x0f, baseIntegers[10]);
	v4(oldValues + 1, oldValues[2], oldValues[3], oldValues[0], 0x85845dd1, 0x15, baseIntegers[ 1]);
	v4(oldValues + 0, oldValues[1], oldValues[2], oldValues[3], 0x6fa87e4f, 0x06, baseIntegers[ 8]);
	v4(oldValues + 3, oldValues[0], oldValues[1], oldValues[2], 0xfe2ce6e0, 0x0a, baseIntegers[15]);
	v4(oldValues + 2, oldValues[3], oldValues[0], oldValues[1], 0xa3014314, 0x0f, baseIntegers[ 6]);
	v4(oldValues + 1, oldValues[2], oldValues[3], oldValues[0], 0x4e0811a1, 0x15, baseIntegers[13]);
	v4(oldValues + 0, oldValues[1], oldValues[2], oldValues[3], 0xf7537e82, 0x06, baseIntegers[ 4]);
	v4(oldValues + 3, oldValues[0], oldValues[1], oldValues[2], 0xbd3af235, 0x0a, baseIntegers[11]);
	v4(oldValues + 2, oldValues[3], oldValues[0], oldValues[1], 0x2ad7d2bb, 0x0f, baseIntegers[ 2]);
	v4(oldValues + 1, oldValues[2], oldValues[3], oldValues[0], 0xeb86d391, 0x15, baseIntegers[ 9]);

	toUpdate[0] += oldValues[0];
	toUpdate[1] += oldValues[1];
	toUpdate[2] += oldValues[2];
	toUpdate[3] += oldValues[3];
}

void generateKeyFromBlock(uint8_t* block, uint8_t* key) { // 004355d2
	void(*blockUpdaters[])(uint8_t*) = {
		updateState1,
		updateState2,
		updateState3
	};

    int ii = 0;
    for (ii=0; ii<512; ii++) printf("%d", block[ii]);
    printf("\nbloc");
    
	uint8_t finalBlock[64];
	memset(finalBlock, 0, 64);
	finalBlock[0] = 0x80; // seems statically defined
	finalBlock[57] = 0x10; // is the length of the block (always 0x200) transformed deterministically

	uint32_t nextBlockUpdater = 0;
	for (uint32_t keyBytesGenerated = 0; keyBytesGenerated < 0x3ab0; keyBytesGenerated += 16) {
		uint32_t runningValues[] = {
			0x67452301,
			0xefcdab89,
			0x98badcfe,
			0x10325476
		}; // initial in memory: 01 23 45 67 89 ab cd ef fe dc ba 98 76 54 32 10

//de 0 15024 -> 939 boucles -> 313 tours complet d'updater
//block = 0 aprs 8*3=24 tours, donc 384 boucles
//%restant = 384*0.6/15024 = 0.327

		for (uint32_t blockPos = 0; blockPos < 0x200; blockPos += 0x40) {
			scrambleValues(runningValues, block + blockPos);
		}
		//printf("%d \n", runningValues[0]);
		
		//if (isBlocNul(block)) printf("nul %d\n", keyBytesGenerated);
		
		scrambleValues(runningValues, finalBlock);

		memcpy(key + keyBytesGenerated, runningValues, 16);
		blockUpdaters[nextBlockUpdater](block);
		
		nextBlockUpdater = (nextBlockUpdater + 1) % 3;
	}
}

void byteArrayToString(uint8_t *byteArray, size_t byteArrayLength, char* output)
{
	for (int i = 0; i < byteArrayLength; i++) {
		sprintf(output + i * 2, "%02x", byteArray[i]);
	}
}

void encryptFileContent(uint8_t* fileContent, char* keystream, char *globalAlgoKey) // 004355f5
{
    //printf("debut\n");
    
	for (int i = 0; i < 0x7530; i++)
	{ // only the first 7530 byte are actually encrypted!
		char algo = globalAlgoKey[i % 20];
		char keystreamByte = (uint8_t)keystream[i];
		/*
        if (i >=768 && i<1000)
        {
            if (i%16 == 0) printf("\n");
            if (i%4 == 0) printf(" ");
            printf("%02x", keystreamByte);
        }*/
        
		//if (i >= 768 && i<800) printf("reference -> Plain : %02x, key : %c, stream: %02x\n", fileContent[i], algo, keystreamByte);

		switch (algo)
		{
		case '0':
			fileContent[i] += keystreamByte;
			break;
		case '1':
			fileContent[i] -= keystreamByte;
			break;
		case '2':
			fileContent[i] += keystreamByte;
			fileContent[i] += 0x32;
			break;
		case '3':
			fileContent[i] += keystreamByte;
			fileContent[i] += 0x03;
			break;
		case '4':
			fileContent[i] -= keystreamByte;
			fileContent[i] -= 0x34;
			break;
		case '5':
			fileContent[i] -= keystreamByte;
			fileContent[i] -= 0x05;
			break;
		case '6':
			fileContent[i] += 0x06;
			break;
		case '7':
			fileContent[i] -= 0x07;
			break;
		case '8':
			fileContent[i] += 0x38;
			break;
		case '9':
			fileContent[i] -= 0x39;
			break;
		}

		//if (i >= 768 && i<800) printf("reference -> Cipher : %02x\n", fileContent[i]);
	}
}

void generateRandomString(char* output, size_t length, const char *allowedCharacters) 
{
	for (int i = 0; i < length; i++) {
		output[i] = allowedCharacters[rand() % strlen(allowedCharacters)];
	}
}

void printBytes(uint8_t* bytes, size_t length)
{
	for (int i = 0; i < 10; i++) {
		printf("%02x", bytes[i]);
	}
	printf("\n");
	for (int i = 0; i < 10; i++) {
		printf("%c", (char)bytes[i]);
	}
	printf("\n...\n");
	for (int i = 380; i < 390; i++) {
		printf("%02x", bytes[i]);
	}
	printf("\n...\n");
	for (int i = length - 1 - 10; i < length; i++) {
		printf("%c", (char)bytes[i]);
	}
	printf("\n");
}
/*
void dechiffrer(uint8_t* fileContent, size_t length, char cle[20])
{
    	for (int i = 0; i < 0x7530; i++) { // only the first 7530 byte are actually encrypted!
		char algo = globalAlgoKey[i % 20];
		char keystreamByte = (uint8_t)keystream[i];

		switch (fileContent[i])
		{
		    case 0x41+0x06:
		        cle[i%20] = '6';
			    break;
		    case -0x07:
			    cle[i%20] = '7';
			    break;
		    case +0x38:
			    cle[i%20] = '8';
			    break;
		    case -0x39:
			    cle[i%20] = '9';
			    break;
		}
	}
}*/
/*
int main()
{
	srand(time(NULL));
	// first, we generate a global algo key. This one is fixed for every file on the system.
	// it can be cracked with a known-plaintext-attack
	char globalAlgoKey[20];
	generateRandomString(globalAlgoKey, 20, "0123456789");

	// next, we create a random filekeyInitializationBlock. This block is generated from a field
	// of A-Za-z0-9 (which is encrypted via RSA) and a set of 512 positions. Both are unique for
	// each file. We simplify this by just saying "the filekeyInitializationBlock is in itself random".
	char filekeyInitializationBlock[512];
	generateRandomString(filekeyInitializationBlock, 512, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
	
	// sample file content
	uint8_t fileContent[0x7530 + 46];
	memset(fileContent, 0x41, 0x7530);
	strcpy((char *)fileContent + 0x7530, "from here on the file is no longer encrypted!");

    //sfileContent[0]= 'a';

	// print our sample file
	printBytes(fileContent, 0x7530 + 45);


	// now we encrypt the file with the Vipasana encryption algorithm.
	// 1. create the 15.000 byte long keystream bytes
	uint8_t keystreamBytes[0x3ab0];
	generateKeyFromBlock((uint8_t *)filekeyInitializationBlock, keystreamBytes);
	printBytes(keystreamBytes, 0x3ab0);

	// 2. the keystream is the stringified keystreamBytes
	char keystream[0x3ab0 * 2 + 1];
	byteArrayToString(keystreamBytes, 0x3ab0, keystream);

	// 3. only 0x7530 byte are actually used
	keystream[0x7530 + 1] = '\0';

    printf("Chiffrer\n");

	int i;
    for (i=736-16*5; i<1000; i+=16)
    {
        printf("%d ", i);
        
        int j;
        for (j=0; j<4; j++) printf("%02x", keystream[i+j]);
        printf(" ");
        for (j=4; j<8; j++) printf("%02x", keystream[i+j]);
        printf(" ");
        for (j=8; j<12; j++) printf("%02x", keystream[i+j]);
        printf(" ");
        for (j=12; j<16; j++) printf("%02x", keystream[i+j]);
        printf("\n");
	}

	// 4. now we encrypt our file
	encryptFileContent(fileContent, keystream, globalAlgoKey);

	// display the results
	printBytes(fileContent, 0x7530 + 45);

	return 0;
}
*/
