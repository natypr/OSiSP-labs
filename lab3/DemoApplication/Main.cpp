#include <stdio.h>
#include "stdafx.h"
#include <windows.h>
#include <string>
#include <vector>
#include <tlhelp32.h>
#include <conio.h>
#include <fstream>
#include <iostream>


#define FUNCTION_NAME "_ReplaceData@20"

#define DLL_IMPORT __declspec(dllimport)

#define SEARCH_STR "TestDataForOSISPThirdLab"
#define REPLACEMENT "Accept the lab, pls :)"
#define PATH_TO_DLL "Dll1.dll"
#define PATH_TO_DLL_FOR_INJECTION "InjectedDLL.dll"

#define ARGS DWORD pid, const char* data, size_t len, const char* replacement, size_t replacementLength



typedef HMODULE(WINAPI *LPLoadLibrary)(LPCSTR);
typedef HMODULE(WINAPI *LPGetProcAddress)(HMODULE, LPCSTR);

typedef void _stdcall TReplaceData(ARGS);


extern "C" DLL_IMPORT void _stdcall ReplaceData(ARGS);



int callFunctionInStaticDll(DWORD pid);
int callFunctionInDynamicDll(DWORD pid);
TReplaceData* staticImport();
TReplaceData* dynamicImport();
void injectLibrary(DWORD procID);




int main(int argc, char* argv[])
{
	#define value "TestDataForOSISPThirdLab"

	const char data1[] = value;
	const char data2[] = value;

	const char replacement[] = "Accept the lab, pls :)";

	DWORD pid = GetCurrentProcessId();

	printf("PID: %d\n", pid);

	printf("Please, select type of dll injection (0 - dynamic, 1 - static, 2 - via remote thread, anything else - do nothing)\n");
	char a = getchar();


	switch (a)
	{
		case '0':
			callFunctionInDynamicDll(pid);
			break;
		case '1':
			callFunctionInStaticDll(pid);
			break;
		case '2':
			DWORD injectionPid;
			printf("Enter pid\n");
			scanf("%d", &injectionPid);
			injectLibrary(injectionPid);
			break;
	default:
		break;
	}


	printf("Original Data 1 value: %s. Actual data 1 value: %s\n", value, data1);
	printf("Original Data 2 value: %s. Actual data 2 value: %s\n", value, data2);


	
	system("pause");
	return 0;

}

int makeCall(TReplaceData func, DWORD pid) {
	if (func != NULL)
	{
		func(pid, SEARCH_STR, sizeof(SEARCH_STR), REPLACEMENT, sizeof(REPLACEMENT));
		return 1;
	}
	else
	{
		puts("DLL is not found");
		return 0;
	}
}


// STATIC

int callFunctionInStaticDll(DWORD pid)
{
	TReplaceData* func = NULL;
	func = staticImport();
	return makeCall(func, pid);
}

TReplaceData *staticImport()
{
	return (TReplaceData*)ReplaceData;
}

// DYNAMIC 

int callFunctionInDynamicDll(DWORD pid)
{
	TReplaceData* func = NULL;
	func = dynamicImport();
	return makeCall(func, pid);
}

TReplaceData* dynamicImport() 
{
	HMODULE hDll = LoadLibraryA(PATH_TO_DLL);
	if (hDll == NULL)
		return NULL;
	TReplaceData* func = (TReplaceData*)GetProcAddress(hDll, FUNCTION_NAME);
	FreeLibrary(hDll);
	return func;
}



// INJECTION 

void injectLibrary(DWORD procID)
{


	HMODULE hdll = LoadLibraryA("Kernel32.dll");

	LPLoadLibrary LoadLibraryA = (LPLoadLibrary)GetProcAddress(hdll, "LoadLibraryA");


	HANDLE hProc = OpenProcess(PROCESS_VM_READ | PROCESS_VM_OPERATION | PROCESS_VM_WRITE |
		PROCESS_CREATE_THREAD | PROCESS_CREATE_PROCESS,
		FALSE, procID);

	LPVOID path = VirtualAllocEx(hProc, NULL, strlen(PATH_TO_DLL_FOR_INJECTION) + 1,
		MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);


	int er = WriteProcessMemory(hProc, path, PATH_TO_DLL_FOR_INJECTION,
		strlen(PATH_TO_DLL_FOR_INJECTION) + 1, NULL);

	DWORD threadID;

	HANDLE HThread = CreateRemoteThread(hProc, NULL, NULL,
		(LPTHREAD_START_ROUTINE)LoadLibraryA, (LPVOID)path, NULL, &threadID);

	Sleep(1000);



	if (HThread == NULL)
	{
		printf("error\n");
	}
	else {
		// Join Thread
		WaitForSingleObject(HThread, INFINITE);
	}


	FreeLibrary(hdll);
	CloseHandle(hProc);

}


