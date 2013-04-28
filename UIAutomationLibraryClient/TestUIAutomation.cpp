#include "StdAfx.h"
#include "UIAutomation.h"
#include "TestUIAutomation.h"
#include "UIAutomationLibraryClient.h"

TestUIAutomation::TestUIAutomation() : _hLinkProcessorListenerReady(NULL),
                                   _hWndMainUI(NULL),
                                   _hThreadBackground(NULL),
                                   _idBackgroundThread(NULL),
                                   _pUIAutomation(NULL)
{
}

TestUIAutomation::~TestUIAutomation()
{
}

HRESULT TestUIAutomation::Initialize(HWND hDlg)
{
    HRESULT hr = E_FAIL; 

    // This sample doesn't expect to enter here with a background thread already initialized.
    if (_hThreadBackground != NULL)
    {
        return hr;
    }

    // The LinkProcessor's background thread will post notifications to the main UI window.
    _hWndMainUI = hDlg;

    // Create an event which will be signaled by the new thread when it's ready to start processing.
    _hLinkProcessorListenerReady = CreateEvent(NULL, TRUE /*Manual reset*/, FALSE /*Initial state*/, NULL);
    if (_hLinkProcessorListenerReady != NULL)
    {
        _hThreadBackground = CreateThread(NULL, 0, s_ListenerThreadProc, this, 0, &_idBackgroundThread);
        if (_hThreadBackground != NULL)
        {
            // Wait for the new background thread to signal the event used to indicate 
            // that the thread is ready to start processing. (Give it 2 seconds.)
            hr = WaitForSingleObject(_hLinkProcessorListenerReady, 2000);
        }
    }

    return S_OK;
}

void TestUIAutomation::Uninitialize()
{
    // Tell the background thread to close down.
    if (_idBackgroundThread != NULL)
    {
        PostThreadMessage(_idBackgroundThread, WM_UIASAMPLE_UITHREADTOBACKGROUNDTHREAD_CLOSEDOWN, 0, 0);

        // Give the thread a couple of seconds to close down gracefully.
        WaitForSingleObject(_hThreadBackground, 2000);
    }
}

void TestUIAutomation::Cleanup()
{
    _pUIAutomation->Release();
    _pUIAutomation = NULL;
}

DWORD WINAPI TestUIAutomation::s_ListenerThreadProc(LPVOID lpParameter)
{
    HRESULT hr = S_OK;

    TestUIAutomation *pThis = (TestUIAutomation*)lpParameter;

    // Make sure the thread's message queue is created.
    MSG msg;
    PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

    // *** Note: The thread on which the UIA calls are made below must be MTA.
    hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (SUCCEEDED(hr))
    {
        // Retrieve a IUIAutomation interface pointer as a starting point for all UIA interaction.
        hr = CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER, 
                              IID_PPV_ARGS(&pThis->_pUIAutomation));
        if (SUCCEEDED(hr))
        {
            // Let the main thread know this thread is ready to start processing.
            SetEvent(pThis->_hLinkProcessorListenerReady);

            BOOL bRet = FALSE;

            // The various WM_UIASAMPLE_UITHREADTO* messages we may receive below are posted 
            // from the app's main UI thread. In response to the messages the background thread
            // will call UIA and if necessary post a message back to the main UI thread later.
            // This means that no UIA calls are being made on the main UI thread.

            // Wait for a message to arrive at the thread message queue.
            while ((bRet = GetMessage( &msg, NULL, 0, 0 )) != 0)
            { 
                if (bRet == -1)
                {
                    // GetMessage() failed.
                    break;
                }
                //else if(msg.message == WM_UIASAMPLE_UITHREADTOLINKPROCESSORTHREAD_BUILDLIST)
                //{                   
                //    pThis->BuildListOfHyperlinksFromWindowInternal((BOOL)msg.wParam, (BOOL)msg.lParam);

                //    // Note: This method demonstrates an alternate approach to getting the hyperlinks from
                //    // the browser window. (However, it does not allow hyperlink relatives to be gathered.)
                //    //  pThis->BuildListOfHyperlinksFromWindowInternalAlternateApproach((BOOL)msg.wParam);
                //}
                //else if(msg.message == WM_UIASAMPLE_UITHREADTOLINKPROCESSORTHREAD_HIGHLIGHTLINK)
                //{
                //    pThis->HighlightLinkInternal((int)msg.wParam, (BOOL)msg.lParam);
                //}
                //else if(msg.message == WM_UIASAMPLE_UITHREADTOLINKPROCESSORTHREAD_INVOKELINK)
                //{
                //    pThis->InvokeLinkInternal((int)msg.wParam, (BOOL)msg.lParam);
                //}
                else if(msg.message == WM_UIASAMPLE_UITHREADTOBACKGROUNDTHREAD_CLOSEDOWN)
                {
                    // The main thread wants this thread to close down.
                    break;
                }
                else
                {
                    TranslateMessage(&msg); 
                    DispatchMessage(&msg); 
                }
            }

            pThis->Cleanup();
        }

        CoUninitialize();
    }

    return 0;
}