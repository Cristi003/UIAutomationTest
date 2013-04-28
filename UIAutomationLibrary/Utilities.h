// Utilities.h

#ifndef _UIAUTOMATIONLIBRARYL_H_
#define _UIAUTOMATIONLIBRARY_H_

#ifdef UIAUTOMATIONLIBRARY_EXPORTS
#define UIAUTOMATIONLIBRARY_API __declspec(dllexport) 
#else
#define UIAUTOMATIONLIBRARY_API __declspec(dllimport) 
#endif

#include "stdafx.h"
#include <string>
#include <Windows.h>
#include <memory>
#include <ShObjIdl.h>
#include <atlbase.h>

namespace UIAutomationLibrary {
    namespace Utilities {
        HRESULT UIAUTOMATIONLIBRARY_API launchApp(const std::wstring& appIdentifier, PDWORD processID);
        //HRESULT UIAUTOMATIONLIBRARY_API launchApp(const wchar_t* appIdentifier, PDWORD processID);
    };

    enum ActivateOptions
    {
        None = 0x00000000,  // No flags set
        DesignMode = 0x00000001,  // The application is being activated for design mode, and thus will not be able to
        // to create an immersive window. Window creation must be done by design tools which
        // load the necessary components by communicating with a designer-specified service on
        // the site chain established on the activation manager.  The splash screen normally
        // shown when an application is activated will also not appear.  Most activations
        // will not use this flag.
        NoErrorUI = 0x00000002,  // Do not show an error dialog if the app fails to activate.                                
        NoSplashScreen = 0x00000004,  // Do not show the splash screen when activating the app.
    };

    struct IApplicationActivationManager
    {
        // Activates the specified immersive application for the "Launch" contract, passing the provided arguments
        // string into the application.  Callers can obtain the process Id of the application instance fulfilling this contract.
        virtual HRESULT ActivateApplication(LPCWSTR appUserModelId, LPCWSTR arguments, ActivateOptions options, PDWORD processId) = 0;
        virtual HRESULT ActivateForFile(LPCWSTR appUserModelId, IShellItemArray* itemArray, LPCWSTR verb, PDWORD processId) = 0;
        virtual HRESULT ActivateForProtocol(LPCWSTR appUserModelId, IShellItemArray* itemArray, PDWORD processId) = 0;
        virtual void Release() = 0;
    };

    class ApplicationActivationManager : public IApplicationActivationManager {
    private:
        ATL::CComPtr<::IApplicationActivationManager> spAppActivationManager;
    public:
        ApplicationActivationManager();
        ~ApplicationActivationManager();
        virtual HRESULT ActivateApplication(LPCWSTR appUserModelId, LPCWSTR arguments, ActivateOptions options, PDWORD processId) override;
        virtual HRESULT ActivateForFile(LPCWSTR appUserModelId, IShellItemArray* itemArray, LPCWSTR verb, PDWORD processId) override;
        virtual HRESULT ActivateForProtocol(LPCWSTR appUserModelId, IShellItemArray* itemArray, PDWORD processId) override;
        virtual void Release() override;
    };
};

extern "C" UIAUTOMATIONLIBRARY_API HRESULT APIENTRY launchApp(const wchar_t* appIdentifier, PDWORD processID);
extern "C" UIAUTOMATIONLIBRARY_API UIAutomationLibrary::IApplicationActivationManager* APIENTRY GetApplicationActivationManager();
extern "C" UIAUTOMATIONLIBRARY_API HRESULT APIENTRY SimulateSwipe();

#endif