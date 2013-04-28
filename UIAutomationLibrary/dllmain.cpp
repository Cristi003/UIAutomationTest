// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <objbase.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    HRESULT hrResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    //if(!SUCCEEDED(hrResult)) {
    //    return FALSE;
    //}
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

