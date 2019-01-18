#ifndef ETUDE_H
#define ETUDE_H

#include "utile.h"
#include <stdint.h>

#define rotate_right(value, count) (((value) >> count) | ((value) << (0x20 - count)))
#define rotate_left(value, count) (((value) << count) | ((value) >> (0x20 - count))) // 004148b8
#define f1(a,b,c) ((a & b) | ((~a) & c)) // 00414888
#define f2(a,b,c) ((a & c) | ((~c) & b)) // 00414898; == f1(c, a, b)
#define f3(a,b,c) (a ^ b ^ c) // 004148a4
#define f4(a,b,c) (((~c) | a) ^ b) // 004148ac

void generateRandomString(char* output, size_t length, const char *allowedCharacters);
void encryptFileContent(uint8_t* fileContent, char* keystream, char *globalAlgoKey); // 004355f5
void byteArrayToString(uint8_t *byteArray, size_t byteArrayLength, char* output);
void generateKeyFromBlock(uint8_t* block, uint8_t* key); // 004355d2

#endif //ETUDE_H
