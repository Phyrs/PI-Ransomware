#include "pin.h"
#include <iostream>
#include <fstream>

#include "stdio.h"

// Explanation: pin.h and Windows.h shares types that possess the same name; and result in the 'ambiguous type' error showing at compilation
// to avoid that, windows headers files are declared under their own namespace
namespace WINDOWS{
	// Contains windows API functions & structures
	#include <Windows.h>
	// Contains the PEB structure
	#include <winternl.h>
}
// Wide Characters C library
// In C(#), simple characters like ASCII are encoded on 1 byte, and are represented by the 'char' type
// For unicode characters, which are encoded on more than 1 byte, the wchar_t type is used
#include <wchar.h>

using namespace std;

// Delimiters for target program's boundaries in memory (to avoid internal dll calls)
ADDRINT start__= 0;
ADDRINT end__= 0;

/**
 * From given address, returns the associated dll name and subroutine name, based on PEB's circular dll's list
 * @param subroutine_addr {@}
 */
int print_subroutine_name(char* subroutine_addr){
	// -------------------------------------- PART 1 : Find the address of each DLL --------------------------------------

	// PEB: Process Environnment Block
	// Data structure used by the OS
	WINDOWS::PPEB ppeb; // PEB pointer (currently points on nothing, obviously)

	// On Windows 32 bits (x86), the TIB (Thread Informatio Block) is a data structure about the currently running thread.
	// It can be used to get many informations about the process without directly using the Windows API
	// It is accessed from the FS segment register (in 32 bits vers. only)
	// More importantly: FS:[0x30]: linear address of the PEB !!
	__asm{
		mov eax, fs:[0x30]
		mov ppeb, eax
	}
	// The above lines fill our peb ptr with the PEB address contained within the TIB structure

	// One of the PEB's fields is 'Ldr' (type: PPEB_LDR_DATA), which contains informations about the loaded DLLs
	WINDOWS::PPEB_LDR_DATA ldr= ppeb->Ldr;
	// Ldr's 'InMemoryOrderModuleList' field is the head of a double-linked list that lists the loaded DLLs,
	WINDOWS::PLIST_ENTRY first_link= &(ldr->InMemoryOrderModuleList);
	// The 'head' of the circular DLL list is always empty
	WINDOWS::PLIST_ENTRY current= first_link->Flink;
	// (type: _LIST_ENTRY; possess 2 _LIST_ENTRY attributes: *flink, *blink)
	// Each _LIST_ENTRY elt of the list is located within a _LDR_DATA_TABLE_ENTRY structure,
	// which contains informations about the associated DLL

	int i= 0;
	WINDOWS::PLDR_DATA_TABLE_ENTRY pdte; // Current DLL structure
	int found= 0;
	wchar_t* basename;
	current= current->Flink; // cf the first link is always the current program (not a dll)
	while (first_link != current){
		char* p= (char*) current;
		// Get the address of the current pdte (cf documentation, we know that there is only 2 PVOID fields above the PLIST_ENTRY one)
		pdte= (WINDOWS::PLDR_DATA_TABLE_ENTRY) (p - 2*sizeof(WINDOWS::PVOID));
		// Note: in reality there is 3 circular lists; each PVOID field points to a specific type of list
		// (ordered by loading order, by address, third is unknown)

		// To put it shortly: in the _LDR_DATA_TABLE_ENTRY, the field FullDllName is an Unicode String that contains the full name of the DLL
		// Note about the UString structure (also: see Windows documentation):
		// The 4 first bytes represents the length of the string, while the 4 lasts are a pointer on the string

		basename= ((& pdte->FullDllName) + 1)->Buffer;

		// -------------------------------------- PART 2 : Find subroutine in the DLL --------------------------------------

		// DLL base address
		// Note: a DLL is a PE !!
		// The first structure pointed by the base address is the DOS MZ header block.
		// Its size is 30 WORD + 1 DWORD, and the last element (type DWORD) contains the address of the PE block (the third one)
		char* dll = (char*) pdte->DllBase;

		// dpe: relative position of PE block, starting from the base address
		// i.e. ppe == 60 + 4 + (size of DOS stub block)
		unsigned int dpe= *((unsigned int *)(dll + 0x3c));
		char* ppe= dll + dpe;

		// PE block is composed of 3 sub blocks: MAGIC, IMAGE_FILE_HEADER, IMAGE_OPTIONAL_HEADER
		// IMAGE_FILE_HEADER: 3 DWORD and 4 WORD (i.e. size: 20 bytes)
		char* ppe_image_file_header= ppe + 4;

		// IMAGE_OPTIONAL_HEADER
		char* ppe_image_optional_header= ppe + 24;

		// The last field of the IMAGE_OPTIONAL_HEADER consists in an array whose number of elements is stored within the NumberOfRvaAndSizes field
		// The type of the array's elts is IMAGE_DATA_DIRECTORY, and consists in an address and a size
		// The address of the first element of the array points towards the _IMAGE_EXPORT_DIRECTORY structure (in which printf can be found)
		// At address ppe_image_optional_header +92, first elt of the array
		WINDOWS::IMAGE_DATA_DIRECTORY ppe_image_optional_header_data_directory=  *((WINDOWS::IMAGE_DATA_DIRECTORY*) (ppe_image_optional_header + 96));
		// !!! The value given by the field VirtualAddress is RELATIVE TO THE DLL'S BASE (cf windows documentation)
		// The DLL's base address must be added as an offset
		WINDOWS::IMAGE_EXPORT_DIRECTORY export_dir= *((WINDOWS::IMAGE_EXPORT_DIRECTORY*) (dll + (int) ppe_image_optional_header_data_directory.VirtualAddress));
		// The printed value of the NumberOfFunctions field is 0x696, which corresponds to 1686;
		// this is the exact number of functions contained within the MSVCR100D.DLL (cf 010 editor)
		int number_of_functions= export_dir.NumberOfFunctions;
	
		// IMPORTANT NOTE (not precised in the course): the AddressOfFunctions and AddressOfNames fields respectively points towards
		// arrays of functions pointers / string pointers; and ALL THE ADDRESSES ARE RELATIVE
		// i.e. the DLL_BASE address must first be added to the the arrays addresses,
		// and also to each of the addresses contained within these arrays
		int current_list_id= 0;
		WINDOWS::DWORD addr_fn_current_relative;
		WINDOWS::DWORD addr_name_current_relative;
		while(current_list_id<number_of_functions){
			addr_name_current_relative= *((WINDOWS::DWORD*) ((char*)export_dir.AddressOfNames + (int) dll +current_list_id*4));
			addr_fn_current_relative= *((WINDOWS::DWORD*) ((char*)export_dir.AddressOfFunctions + (int) dll +current_list_id*4) );
			if(subroutine_addr == ((char*)addr_fn_current_relative) + (int)dll){
				printf("     %S - %s (%x)\n", basename, ((char*)addr_name_current_relative) + (int) dll, subroutine_addr);
				found= 1;
				break;
			}else{
				current_list_id++;
			}
		}

		//---------------------------------------------------------------------------------------------------------

		if(!found){
			current= current->Flink;
		}else{
			break;
		}
	}
	if(!found)
		printf("     <UNKNOWN> %x\n", subroutine_addr);
	return 0;
}

/**
 * Muse be used on a (supposedly) far call instruction (that begins with 0xff, however other instructions such as push also begins with 0xff, thoses must be discarded)
 * Based on the second byte of the call bytecode (0x15, 0xd0), fetch the @ of the called subroutine, and uses print_subroutine_name to get its 'human readable' name
 * @param ip: current instruction
 * @param pcode -> not used anymore
 * @param ctx {CONTEXT}: processor context, contains registers values
 */
VOID call_tracer(VOID* ip, VOID *pcode, CONTEXT* ctx){
	char* called_subroutine; // @ of subroutine
	PIN_REGISTER regval;
	switch(((char*)ip)[1] & 0xff){ // First oppcode is 0xff (can be the beginning of an absolute call)
		case 0x15: // call to ptr in imports section
			called_subroutine= (char*)(*((long*)(*((long*)((char*)ip+2)))));
			printf("%x : call from import section:\n", ip);
			print_subroutine_name(called_subroutine);
			break;
		case 0xd0: // call to eax
			PIN_GetContextRegval(ctx, REG_EAX, reinterpret_cast<UINT8*>(&regval));
			printf("%x : call from eax:\n", ip);
			called_subroutine= (char*)(*((long*)(&regval)));
			print_subroutine_name(called_subroutine);
			break;
		case 0xd3: // call to ebx
			PIN_GetContextRegval(ctx, REG_EBX, reinterpret_cast<UINT8*>(&regval));
			printf("%x : call from ebx:\n", ip);
			called_subroutine= (char*)(*((long*)(&regval)));
			print_subroutine_name(called_subroutine);
			break;
		case 0xd1: // call to ecx
			PIN_GetContextRegval(ctx, REG_ECX, reinterpret_cast<UINT8*>(&regval));
			printf("%x : call from ecx:\n", ip);
			called_subroutine= (char*)(*((long*)(&regval)));
			print_subroutine_name(called_subroutine);
			break;
		case 0xd2: // call to edx
			PIN_GetContextRegval(ctx, REG_EDX, reinterpret_cast<UINT8*>(&regval));
			printf("%x : call from edx:\n", ip);
			called_subroutine= (char*)(*((long*)(&regval)));
			print_subroutine_name(called_subroutine);
			break;
		case 0xd6: // call to esi
			PIN_GetContextRegval(ctx, REG_ESI, reinterpret_cast<UINT8*>(&regval));
			printf("%x : call from esi:\n", ip);
			called_subroutine= (char*)(*((long*)(&regval)));
			print_subroutine_name(called_subroutine);
			break;
		case 0xd7: // call to edi
			PIN_GetContextRegval(ctx, REG_EDI, reinterpret_cast<UINT8*>(&regval));
			printf("%x : call from edi:\n", ip);
			called_subroutine= (char*)(*((long*)(&regval)));
			print_subroutine_name(called_subroutine);
			break;
	}
}

/**
 * Called at each instruction
 */
VOID instrumente(INS ins, VOID *p){

	if(INS_Address(ins) >= start__ && INS_Address(ins) < end__){ // to only work on the main program's instructions

		string* code= new string(INS_Disassemble(ins));
		ADDRINT ip= INS_Address(ins);

		if(((char*)ip)[0] == '\xFF') // (supposedly) Call Instruction
			INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)call_tracer, IARG_INST_PTR, IARG_PTR, code, IARG_CONTEXT, IARG_END); // call_tracer: will be called before executing the current instruction
	}	
}

/**
 * Called only when a dll is loaded
 */
VOID module(IMG img, VOID *p){
	//cout << IMG_Name(img) << " " << IMG_LowAddress(img) << " " << IMG_HighAddress(img) << "\n"; // high, low addr : boundaries of the module
	// Tip: get boundaries of targetted .exe (always first in the dll list) so that only its instructions are monitored
	if(! start__)
		start__= IMG_LowAddress(img);
	if(! end__)
		end__= IMG_HighAddress(img);
}

// Called only at the end
/*VOID end_(INT32 c, VOID *p){
	
}*/

// Pin.exe takes 2 paths as arguments:
// 1 - the present dll (who explains how to instrument the targetted .exe)
// 2 - targetted .exe
int main(int argc, char* argv[]){
	PIN_Init(argc, argv);
	// Below is the command to put in the windows cmdr
	// "C:\pin-2.13-62732-msvc10-windows\pin.exe" -t "C:\Documents and Settings\Administrateur\Mes documents\Visual Studio 2010\Projects\Exemple\Release\PintoolExemple1.dll" -- "C:\Documents and Settings\Administrateur\Mes documents\Visual Studio 2010\Projects\Pin_Test\Release\Pin_Test.exe"
	INS_AddInstrumentFunction(instrumente, 0);
	IMG_AddInstrumentFunction(module, 0); // called only when a dll is loaded
	//PIN_AddFiniFunction(end_, 0); // called at the end of target's execution
	PIN_StartProgram(); // launch target program (with path in arg)
	return 0;
}