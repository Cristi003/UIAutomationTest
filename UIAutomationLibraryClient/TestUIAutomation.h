// Utilities.h

#ifndef _TestUIAutomation_H_
#define _TestUIAutomation_H_

#include "stdafx.h"
#include <UIAutomation.h>

class TestUIAutomation {
public:
    TestUIAutomation();
    ~TestUIAutomation();
    // The Initialize() and Uninitialize() methods are responsible for the 
    // management of the background thread which makes all the calls to UIA.
    HRESULT Initialize(HWND hDlg);
    void Uninitialize();
private:
    // s_ListenerThreadProc is the thread proc for the background MTA thread
    // on which calls to UIA are made.
    static DWORD WINAPI s_ListenerThreadProc(__in LPVOID lpParameter);

    // Cleanup() is called to release any objects which were set up when the 
    // background thread started. It does not release anything which was set
    // up on the main UI thread.
    void Cleanup();

    // The following are used as part of managing the LinkProcessor's background thread.
    IUIAutomation *_pUIAutomation; // IUIAutomation interface used to add the LinkProcessor's background thread.
    HWND           _hWndMainUI; // Handle to sample's main UI window.
    HANDLE         _hThreadBackground; // Handle to background MTA thread.
    DWORD          _idBackgroundThread; // Id of background thread.
    HANDLE         _hLinkProcessorListenerReady; // Event used by background thread to indicate that it's ready to start processing messages.
};

#endif