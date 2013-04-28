// UIAutomationClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <comdef.h>
#include <vector>

#include "..\UIAutomationLibrary\Utilities.h"
#include "TestUIAutomation.h"
#include "UIAutomationLibraryClient.h"
#include "UIAutomation.h"

using namespace std;

// Global variables used between the .cpp files in this sample.
TestUIAutomation *g_automation = nullptr;

HRESULT InitializeUIAutomation(HWND hWnd)
{
    IUIAutomation *pUIAutomation;

    HRESULT hr = CoCreateInstance(__uuidof(CUIAutomation8), nullptr,
                CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pUIAutomation));

    IUIAutomationElement *rootElement = nullptr;

    pUIAutomation->GetRootElement(&rootElement);

    IUIAutomationTreeWalker* pContentWalker = nullptr;

    pUIAutomation->get_ContentViewWalker(&pContentWalker);

    IUIAutomationElement* childElement = nullptr;

    pContentWalker->GetFirstChildElement(rootElement, &childElement);

    while (childElement)
    {
        BSTR windowName;
        childElement->get_CurrentName(&windowName);
        wcout << L"   " << windowName << endl;
        SysFreeString(windowName);

        IUIAutomationElement* childElementNext;
        pContentWalker->GetNextSiblingElement(childElement, &childElementNext);
        childElement->Release();
        childElement = childElementNext;
    }

    IUIAutomationElement *firstChild;

    pContentWalker->GetFirstChildElement(rootElement, &firstChild);

    IUIAutomationElement *windowElement;

    pUIAutomation->ElementFromHandle(static_cast<UIA_HWND>(hWnd) , __out &windowElement);

    BSTR windowName;
    windowElement->get_CurrentClassName(__out &windowName);

    wcout << "Window class name: " << windowName << endl;

    SysFreeString(windowName);

    BSTR autoId;

    firstChild->get_CurrentAutomationId(&autoId);

    wcout << "Automation id: " << autoId << endl;

    SysFreeString(autoId);

    firstChild->Release();

    rootElement->Release();
    pUIAutomation->Release();

   /* g_automation = new TestUIAutomation();
    g_automation->Initialize(nullptr);*/

    return hr;
}

class ProcessHWNDs
{
public:
    ProcessHWNDs(DWORD processId) : processId(processId)
    {
    }
    vector<HWND> hTopWindows;
    DWORD processId;

};

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	if(!hWnd) return false;
    ProcessHWNDs *p = reinterpret_cast<ProcessHWNDs*>(lParam);
    DWORD processId;
	GetWindowThreadProcessId(hWnd, &processId);
    if (p->processId == processId)
	{
        p->hTopWindows.push_back(hWnd);
    }

	return true;

}

vector<HWND> getProcessWindows(DWORD processId)
{
	// Now need to run a loop to get the correct window for process.
	bool bFound = false;
	HWND prevWindow = 0;
    vector<HWND> result;

	while ( ! bFound ) {
		HWND desktopWindow = ::GetDesktopWindow();
		if ( ! desktopWindow )
			break;

		HWND nextWindow = ::FindWindowEx( desktopWindow, prevWindow, nullptr, nullptr );
		if ( ! nextWindow )
			break;

		// Check whether window belongs to the correct process.
		DWORD procId;
		::GetWindowThreadProcessId( nextWindow, &procId );

		if ( procId == processId ) {// Add additional checks. In my case, I had to bring the window to front so these checks were necessary.
			result.push_back(nextWindow);         
		}

		prevWindow = nextWindow;
	}

	return result;
}

HWND getMainMetroWindow(vector<HWND> hWnds)
{
    for (HWND hWnd: hWnds)
    {
        std::vector<wchar_t> buffer(1024, 0);
		if ( ::IsWindowVisible( hWnd ) && ! IsIconic( hWnd ) && ::GetWindowText(hWnd, &buffer[0], buffer.size())
			&& ! ::GetParent( hWnd ) )
            return hWnd;
    }
    return nullptr;
}

double PCFreq = 0.0;
__int64 CounterStart = 0;

void StartCounter()
{
    LARGE_INTEGER li;
    QueryPerformanceFrequency(&li);

    PCFreq = double(li.QuadPart)/1000000.0;

    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
}
double GetCounter()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return double(li.QuadPart-CounterStart)/PCFreq;
}

int _tmain(int argc, _TCHAR* argv[])
{
	HRESULT hrResult = S_OK;
    if (SUCCEEDED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
    {
        if (argc == 2)
        {
            DWORD dwProcessId = 0;
            ++argv;
            hrResult = UIAutomationLibrary::Utilities::launchApp(*argv, &dwProcessId);
            if (SUCCEEDED(hrResult)) {
                wcout << "App launches in process id " << dwProcessId << endl;
                // Wait for app to initialize
                Sleep(1000);
                ProcessHWNDs processHwnds(dwProcessId);
                ::EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&processHwnds));
                for (HWND hWnd : processHwnds.hTopWindows)
                {
                    vector<wchar_t> buffer(1024, 0);
                    ::GetWindowText(hWnd, &buffer[0], buffer.size());
                    wstring windowTitle(&buffer[0]);
                }
                HWND mainMetroWindow = getMainMetroWindow(getProcessWindows(dwProcessId));
                //HWND window = new HWND__();
                //window->unused = 0x23095C;
                InitializeUIAutomation(mainMetroWindow);
                /*SimulateSwipe();*/
            }
        }
        else
        {
            hrResult = E_INVALIDARG;
        }

        CoUninitialize();
    }
    _com_error err(hrResult);
    wcout << "App exited with message: " << err.ErrorMessage() << endl;
    return hrResult;
}

