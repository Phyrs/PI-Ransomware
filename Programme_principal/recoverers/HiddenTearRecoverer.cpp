#include "HiddenTearRecoverer.h"
#include <string.h>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

using namespace std;

const char *HiddenTearRecoverer::EXE_PATH = "C:\\hidden_tear\\HiddenTearNetworkAtk.exe";
const char *HiddenTearRecoverer::EXE_PATH2 = "C:\\hidden_tear\\HiddenTearBruteforce.exe";

HiddenTearRecoverer::HiddenTearRecoverer(string pass)
{
    password = pass;
}

HiddenTearRecoverer::HiddenTearRecoverer(string pl, string ciph, int sec, int t)
{
    plain = pl;
	ciphered = ciph;
	seconds_bruteforce = sec;
	thread_number = t;
}

void HiddenTearRecoverer::decipher(string path) const
{
	string cmd(EXE_PATH);
	cmd.append(" ");
	cmd.append(password);
	cmd.append(" \"");
	cmd.append(path);
	cmd.append("\"");

	STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

	
	wchar_t wtext[2000];
	mbstowcs(wtext, cmd.c_str(), strlen(cmd.c_str())+1);
	LPWSTR ptr = wtext;

	wchar_t wtext2[2000];
	mbstowcs(wtext2, EXE_PATH, strlen(EXE_PATH)+1);
	LPWSTR ptr2 = wtext2;

    // Start the child process. 
    if( !CreateProcess( ptr2,   // No module name (use command line)
        ptr,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    ) 
    {
        printf( "CreateProcess failed (%d).\n", GetLastError() );
        return;
    }

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

	printf("%s : deciphering attempt with %s.\n", path.c_str(), password.c_str());
}

void HiddenTearRecoverer::bruteforce()
{
	string cmd(EXE_PATH2);
	cmd.append(" \"");
	cmd.append(plain);
	cmd.append("\" \"");
	cmd.append(ciphered);
	cmd.append("\" ");
	char number[50];
	sprintf(number, "%d", seconds_bruteforce);
	cmd.append(number);
	cmd.append(" ");
	char number2[50];
	sprintf(number2, "%d", thread_number);
	cmd.append(number2);

	STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

	wchar_t wtext[2000];
	mbstowcs(wtext, cmd.c_str(), strlen(cmd.c_str())+1);
	LPWSTR ptr = wtext;

	wchar_t wtext2[2000];
	mbstowcs(wtext2, EXE_PATH2, strlen(EXE_PATH2)+1);
	LPWSTR ptr2 = wtext2;

    // Start the child process. 
    if( !CreateProcess( ptr2,   // No module name (use command line)
        ptr,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    ) 
    {
        printf( "CreateProcess failed (%d).\n", GetLastError() );
        return;
    }

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}