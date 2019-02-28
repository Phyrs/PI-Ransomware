#ifndef CRYPY_RECOVER_H
#define CRYPY_RECOVER_H

#include <string>
#include "MercenneSlayer.h"

using namespace std;

class CrypyRecover
{
    public:
        CrypyRecover(string pathRacine);
        void recupererIv(string path, uint8_t iv[]) const;

        string gKey();
        void decipher(string pathRacine);
        void decipher(string pathFile, string key);

        static void tester();

    private:
        string extensions[285];
        MercenneSlayer mercenneSlayer;
};


#endif //CRYPY_RECOVER_H

//http://dfir.pro/index.php?link_id=69055
