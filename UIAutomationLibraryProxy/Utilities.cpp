// This is the main DLL file.

#include "stdafx.h"
#include <Windows.h>
#include <atlbase.h>
#include <functional>

#include "Utilities.h"
#include "..\UIAutomationLibrary\Utilities.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace UIAutomationLibraryProxy
{
    int Utilities::launchApp(String^ appIdentifier, DWORD %processId) {
        wchar_t* appId = (wchar_t*) Marshal::StringToHGlobalUni(appIdentifier).ToPointer();
        PDWORD procId = (PDWORD) Marshal::AllocHGlobal(sizeof(DWORD)).ToPointer();
        *procId = processId;
        int hrResult =  UIAutomationLibrary::Utilities::launchApp(appId, procId);
        processId = *procId;
        Marshal::FreeHGlobal((IntPtr) appId);
        Marshal::FreeHGlobal((IntPtr) procId);
        return hrResult;
    }
      
    void Utilities::LaunchAppCmdLet::BeginProcessing() {
        PSCmdlet::BeginProcessing();
        //DWORD processId;
        //launchApp(appIdentifier, processId);
        //PSCmdlet::WriteObject(processId);
        HRESULT hrResult;
        typedef std::shared_ptr<UIAutomationLibrary::IApplicationActivationManager> IApplicationActivationManagerPtr;
        IApplicationActivationManagerPtr spAppActivationManager(::GetApplicationActivationManager(), std::mem_fn(&UIAutomationLibrary::IApplicationActivationManager::Release));
        if (spAppActivationManager) {
            DWORD processId;
            LPWSTR appId = (LPWSTR) Marshal::StringToHGlobalUni(AppIdentifier).ToPointer();
            hrResult = spAppActivationManager->ActivateApplication(appId, nullptr, UIAutomationLibrary::ActivateOptions::None , &processId);
            Marshal::FreeHGlobal((IntPtr) appId);
            PSCmdlet::WriteObject(processId);
            //delete spAppActivationManager;
        }
        //CComPtr<::IApplicationActivationManager> spAppActivationManager;
        //CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        //HRESULT hrResult = CoCreateInstance(__uuidof(::ApplicationActivationManager), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&spAppActivationManager));
        //if (SUCCEEDED(hrResult))
        //{
        //    DWORD processId;
        //    LPWSTR appId = (LPWSTR) Marshal::StringToHGlobalUni(appIdentifier).ToPointer();
        //    hrResult = spAppActivationManager->ActivateApplication(appId, nullptr, AO_NONE | AO_NOERRORUI , &processId);
        //    Marshal::FreeHGlobal((IntPtr) appId);
        //    PSCmdlet::WriteObject(processId);
        //}
    }
}