//#include "stdafx.h"
#include "VipasanaAnalyser.h"

#ifdef WINDOWS
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char* argv[])
#endif
{
    VipasanaAnalyser vipasanaAnalyser("Tests/save/lancement3/5.txt", "Tests/save/lancement3/5_chiffre.cbf");
    vipasanaAnalyser.printGlobalKey();
    vipasanaAnalyser.decipher("Tests/lancement3/image_chiffre.cbf");
    testFichier();
    
	return 0;
}

