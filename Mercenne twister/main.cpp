//#include "stdafx.h"

#include <iostream>
#include "CrypyRecover.h"
#include "MercenneSlayer.h"

using namespace std;

#ifdef WINDOWS
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char* argv[])
#endif
{
    //CrypyRecover::tester();
    MercenneSlayer::tester();

	return 0;
}

