#ifndef VIPASANA_RECOVER_H
#define VIPASANA_RECOVER_H

#include "utile.h"
#include <stdint.h>

using namespace std;

class VipasanaRecover
{
    public:
        VipasanaRecover(string plaintext_path, string ciphered_path, bool isDebug);
        VipasanaRecover(string plaintext_path, string ciphered_path);
        void decipher(string path) const;
        void printGlobalKey() const;
        char* getGlobalKey();

    private:
        long pFin(ifstream &file) const;
        string name(ifstream &file) const;
        void init(string plaintext_path, string ciphered_path, bool isDebug);
        
        bool isDebug;
        char shift[30000];  //static difference between all plain and cipher after the 768 first bytes
        char globalKey[20];
};

char reverse_algo(char global_key_byte, char ciphered, int nByte);
void findGlobalKey(char *globalKey, uint8_t *plain, uint8_t *ciphered, int tailleCiphered, bool isDebug);

#endif //VIPASANA_ANALYSER_H

