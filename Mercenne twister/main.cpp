//#include "stdafx.h"
#include "utile.h"
#include <iostream>

using namespace std;

#ifdef WINDOWS

#include "../Mercenne twister/CrypyRecover.h"
#include "../Mercenne twister/MercenneSlayer.h"

int _tmain(int argc, _TCHAR* argv[])
#else

#include "CrypyRecover.h"
#include "MercenneSlayer.h"
int main(int argc, char* argv[])
#endif
{
    CrypyRecover::tester();
    //MercenneSlayer::tester();

	return 0;
}


