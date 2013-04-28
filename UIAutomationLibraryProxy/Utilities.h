// Utilities.h

#pragma once
#include "Windows.h"
#include <ShObjIdl.h>

using namespace System;
using namespace System::Management::Automation;
using namespace System::Runtime::InteropServices;
using namespace System::Runtime::CompilerServices;

namespace UIAutomationLibraryProxy {

	public ref class Utilities
	{
    public:
		static int launchApp(System::String^ appIdentifier, DWORD %processId);       

        [FlagsAttribute] 
        enum class ActivationOptions
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

        [CmdletAttribute(VerbsCommon::Open, "StoreApp")]
        ref class LaunchAppCmdLet : public PSCmdlet
        {
        public:
            [ParameterAttribute(Mandatory=true)]
            [ValidateNotNullOrEmpty]
            property String^ AppIdentifier;

            [ParameterAttribute(Mandatory=false)]
            [ValidateNotNullOrEmpty]
            property ActivationOptions Options 
            {
                ActivationOptions get()
                {
                    return options;
                }
        
                void set(ActivationOptions options)
                {
                    options = options;
                }
            };
        protected:
            virtual void BeginProcessing() override;
        private:
            static ActivationOptions options = ActivationOptions::None;
        };
	};
}
