// UIAutomation.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Utilities.h"
#include <objbase.h>
#include <Shobjidl.h>
#include <atlbase.h>
#include <vector>

namespace UIAutomationLibrary {
    HRESULT Utilities::launchApp(const std::wstring& strAppUserModelId, PDWORD processId) {
        CComPtr<::IApplicationActivationManager> spAppActivationManager;
        HRESULT hrResult = E_INVALIDARG;
        if (!strAppUserModelId.empty()) {
            // Instantiate IApplicationActivationManager
            hrResult = CoCreateInstance(__uuidof(::ApplicationActivationManager), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&spAppActivationManager));
            if (SUCCEEDED(hrResult)) {
                // This call ensures that the app is launched as the foreground window
                //hrResult = CoAllowSetForegroundWindow(spAppActivationManager, NULL);
                // Launch the app
                if (SUCCEEDED(hrResult)) {
                    hrResult = spAppActivationManager->ActivateApplication(strAppUserModelId.c_str(), nullptr, AO_NONE | AO_NOERRORUI , processId);
                }
            }
        }
        return hrResult;
    };

    std::vector<HWND> Utilities::getWindows(DWORD processId)
    {
        return std::vector<HWND>();
    }

    ApplicationActivationManager::ApplicationActivationManager() : IApplicationActivationManager()
    {
        CoCreateInstance(__uuidof(::ApplicationActivationManager), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&this->spAppActivationManager));
    };

    ApplicationActivationManager::~ApplicationActivationManager() 
    {
        this->Release();
    };

    HRESULT ApplicationActivationManager::ActivateApplication(LPCWSTR appUserModelId, LPCWSTR arguments, ActivateOptions options, PDWORD processId)
    {
        return this->spAppActivationManager->ActivateApplication(appUserModelId, arguments, (::ACTIVATEOPTIONS) options, processId);
    };

    HRESULT ApplicationActivationManager::ActivateForFile(LPCWSTR appUserModelId, IShellItemArray* itemArray, LPCWSTR verb, PDWORD processId)
    {
        return this->spAppActivationManager->ActivateForFile(appUserModelId, itemArray, verb, processId);
    };

    HRESULT ApplicationActivationManager::ActivateForProtocol(const WCHAR* appUserModelId, IShellItemArray* itemArray, PDWORD processId)
    {
        return this->spAppActivationManager->ActivateForProtocol(appUserModelId, itemArray, processId);
    };

    void ApplicationActivationManager::Release()
    {
        this->spAppActivationManager.Release();
    };
}

/*LPOLESTR lpolestr;
StringFromCLSID(__uuidof(ApplicationActivationManager), &lpolestr);
wprintf_s(L"%s", lpolestr);
StringFromIID(__uuidof(IApplicationActivationManager), &lpolestr);
wprintf_s(L"%s", lpolestr);
CoTaskMemFree(lpolestr);*/

HRESULT APIENTRY launchApp(const wchar_t* strAppUserModelId, PDWORD processId) {
    return UIAutomationLibrary::Utilities::launchApp(std::wstring(strAppUserModelId), processId);
};

UIAutomationLibrary::IApplicationActivationManager* APIENTRY GetApplicationActivationManager() {
    return new UIAutomationLibrary::ApplicationActivationManager();
};

HRESULT APIENTRY SimulateSwipe()
{
    POINTER_TOUCH_INFO contact; 
    BOOL bRet = TRUE;

    // Assume a maximum of 10 contacts and turn touch feedback off 
    InitializeTouchInjection(10, TOUCH_FEEDBACK_INDIRECT);

    // Initialize the touch info structure
    memset(&contact, 0, sizeof(POINTER_TOUCH_INFO));

    long point_x = 400, point_y = 500;
 
    contact.pointerInfo.pointerType = PT_TOUCH; //we're sending touch input
    contact.pointerInfo.pointerId = 0;          //contact 0 
    contact.pointerInfo.ptPixelLocation.x = point_x; 
    contact.pointerInfo.ptPixelLocation.y = point_y; 
    contact.pointerInfo.pointerFlags = POINTER_FLAG_DOWN | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;// | POINTER_FLAG_CONFIDENCE; 
    contact.touchFlags = TOUCH_FLAG_NONE; 
    contact.touchMask = TOUCH_MASK_NONE;//TOUCH_MASK_CONTACTAREA | TOUCH_MASK_ORIENTATION | TOUCH_MASK_PRESSURE; 
    //contact.orientation = 90; 
    //contact.pressure = 32000; 
 
    // Set the contact area depending on thickness 
    contact.rcContact.top = point_y - 2; 
    contact.rcContact.bottom = point_y + 2; 
    contact.rcContact.left = point_x - 2; 
    contact.rcContact.right = point_x + 2; 
 
    bRet = InjectTouchInput(1, &contact);

    // Remove down flag
    contact.pointerInfo.pointerFlags = POINTER_FLAG_UPDATE | POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT;

    // Inject a touch down 
    for (int i = 1; i <= 400; ++i) {
        contact.pointerInfo.ptPixelLocation.x += -1;
        //++contact.pointerInfo.ptPixelLocation.y;
        //++contact.rcContact.top = point_y - 2; 
        //++contact.rcContact.bottom = point_y + 2; 
        contact.rcContact.left += -1;
        contact.rcContact.right += -1;
        bRet = InjectTouchInput(1, &contact);
        //++contact.pointerInfo.ptPixelLocation.x;
        //++contact.rcContact.left; 
        //++contact.rcContact.right;
        Sleep(3);
    }
 
    contact.pointerInfo.pointerFlags = POINTER_FLAG_UP; 
 
    // Inject a touch up 
    InjectTouchInput(1, &contact); 
 
    return 0; 
}