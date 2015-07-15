// dllmain.cpp : Defines the entry point for the DLL application.
#include "windows.h"
extern "C" {
HMODULE g_hModule;
}

#ifdef _DEBUG
#include <stdio.h>
#define SHOW_CONSOLE
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
                      )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        g_hModule = hModule;
#ifdef SHOW_CONSOLE
        {
            AllocConsole(); 
            freopen("CONOUT$","w+t",stdout);
            freopen("CONIN$","r+t",stdin);
            printf("Debug Begin. Using 'printf' to output debug infomation");
        }
#endif
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

