//#include "stdafx.h"
#include "VipasanaAnalyser.h"

#ifdef WINDOWS
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char* argv[])
#endif
{
    VipasanaAnalyser vipasanaAnalyser("Tests/save/lancement2/5.txt", "Tests/save/lancement2/5_chiffre.cbf");
    vipasanaAnalyser.printGlobalKey();
    vipasanaAnalyser.decipher("Tests/lancement2/4_chiffre.cbf");

	return 0;
}

