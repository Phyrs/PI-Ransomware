#ifndef HIDDEN_RECOVER_H
#define HIDDEN_RECOVER_H
#include <string>

using namespace std;

class HiddenTearRecoverer
{
    public:
        HiddenTearRecoverer(string pass);
        void decipher(string path) const;

    private:
        string password;
		static const char *EXE_PATH;
};

#endif

