
/*! @file
 *  This is an example of the PIN tool that demonstrates some basic PIN APIs 
 *  and could serve as the starting point for developing your first PIN tool
 */

#include "pin.H"
#include <iostream>
#include <fstream>

using namespace std;

char *exe;
ADDRINT fin;
ADDRINT debut = 0;
unsigned int nombre = 0;
unsigned int nombre2 = 0;

void f(void *ip, void *pcode)
{
	unsigned int p = ((char*)((debut+fin)/2))[0];
	cout << "vue" << ip << " " << *((string*)pcode) << " -- " << p << endl;
	nombre++;
}

void g (void *p)
{
	cout << "Read at " << p << " " << ((unsigned int*)p)[0] << endl;
}

void instrumente(INS ins, void *p)
{
	if (INS_Address(ins) >= debut && INS_Address(ins) < fin)
	{
		nombre2++;
		string *code = new string(INS_Disassemble(ins));
		cout << INS_Address(ins) << " : " << INS_Disassemble(ins) << endl;

		ADDRINT ip = INS_Address(ins);

		if (memcmp(exe+(ip-debut), (char*)ip, INS_Size(ins)))
		{
			cout << "automodification" << endl;
			//cout << "At " << ip << " we had " << INS_Disassemble((INS)exe+ip-debut) << " updated" << INS_Disassemble(ins) << endl;
		}

		//if (((char*) ip)[0] == '\xc3')
		{
			INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)f, IARG_INST_PTR, IARG_PTR, code, IARG_END); //BEFORE, AFTER, BRANCH
		
			for (UINT32 i=0; i<INS_MemoryOperandCount(ins); i++)
			{
				if (INS_MemoryOperandIsRead(ins, i))
				{
					INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)g, IARG_MEMORYOP_EA, i, IARG_END);
				}
			}
		}

		delete code;
	}
}

void fini(INT32 c, void *p)
{
	cout << "number of instructions " << nombre << " " << nombre2 << endl;
}

void module(IMG img, void *p)
{
	//cout << IMG_LowAddress(img) << " -> " << IMG_HighAddress(img) << " : " << IMG_Name(img) << endl;

	if (!debut)
	{
		debut = IMG_LowAddress(img);
		fin = IMG_HighAddress(img);

		exe = new char[fin-debut];
		memcpy(exe, (void*)debut, fin-debut);
	}
}

int main(int argc, char *argv[])
{
	cout << argv[0] << endl;
	return 0;
	PIN_Init(argc, argv);
	INS_AddInstrumentFunction(instrumente, 0);
	IMG_AddInstrumentFunction(module, 0);
	PIN_AddFiniFunction(fini, 0);

	PIN_StartProgram();

	return 0;
}