#ifndef HIDDEN_RECOVER_H
#define HIDDEN_RECOVER_H
#include <string>

using namespace std;

class HiddenTearRecoverer
{
    public:
        HiddenTearRecoverer(string pass);
		HiddenTearRecoverer(string pl, string ciph, int sec);
        void decipher(string path) const;
		void bruteforce();

    private:
        string password, plain, ciphered;
		int seconds_bruteforce;
		static const char *EXE_PATH;
		static const char *EXE_PATH2;
};

#endif

