// dllmain.cpp: определяет точку входа для приложения DLL.
#include "stdafx.h"
#include "InjectedDLL.cpp"
#include "../Dll1/Dll1.cpp"

#define SEARCH_STR "TestDataForOSISPThirdLab"
#define REPLACEMENT "Accept the lab, pls :)"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	DWORD pid = GetCurrentProcessId();

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
		ReplaceData(pid, SEARCH_STR, sizeof(SEARCH_STR), REPLACEMENT, sizeof(REPLACEMENT));
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

