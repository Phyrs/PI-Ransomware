#ifndef CRYPY_RECOVER_H
#define CRYPY_RECOVER_H

#include <string>
#include "MercenneSlayer.h"

using namespace std;

class CrypyRecover
{
    public:
        CrypyRecover(string pathRacine);
        string generateKey();
        ~CrypyRecover();

        void decipher(string path) const;
        void recupererIv(string path, uint8_t iv[]) const;

        static void tester();

    private:
        string *keys;
        long nbFichiers;
        string *fichiers;
        string pathDechiffre;
        string extensions[285];
        MercenneSlayer mercenneSlayer;
};


#endif //CRYPY_RECOVER_H

//http://dfir.pro/index.php?link_id=69055
