#ifndef CRYPY_RECOVER_H
#define CRYPY_RECOVER_H

#include <string>
#include "MercenneSlayer.h"

using namespace std;

class CrypyRecover
{
    public:
        CrypyRecover(string pathRacine);
        void decipher(string path) const;

        static void tester();

    private:
        string extensions[274];
        MercenneSlayer mercenneSlayer;
};


#endif //CRYPY_RECOVER_H
