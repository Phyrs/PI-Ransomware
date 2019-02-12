//#include "stdafx.h"

#include <iostream>
#include "MercenneSlayer.h"

using namespace std;

#ifdef WINDOWS
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char* argv[])
#endif
{
    MercenneSlayer::tester();
    MercenneSlayer mercenneSlayer(32, 624, 397, 31, 0x9908B0DF, 11, 0xFFFFFFFF, 7, 0x9D2C5680, 15, 0xEFC60000, 18);

	return 0;
}

