using System;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using System.Diagnostics;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;

using NativeWrappers;
using ComWrappers;

namespace UIAutomationLibraryClientCSharp
{
    class Program
    {
        //[DllImport("kernel32.dll", CharSet = CharSet.Auto)]
        //static extern IntPtr LoadLibrary(string lpFileName);

        [DllImport("UIAutomationLibrary.dll", EntryPoint = "SimulateSwipe", CallingConvention = CallingConvention.StdCall)]
        public static extern int SimulateSwipeNative();

        static int launchAppPInvoke(string appName, ref uint processId)
        {
            // PInvoke call on the native dll
            return Mine.launchApp(appName, ref processId);
        }

        static int launchAppCLIWrapper(string appName, ref uint processId)
        {
            // Call to C++/CLI dll wrapper around the native dll
            return UIAutomationLibraryProxy.Utilities.launchApp(appName, ref processId);
        }

        static void Main(string[] args)
        {
            // Ensure current directory is exe directory
            //Environment.CurrentDirectory = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);

            //string dllPath = Path.GetFullPath(@"C:\Users\Cristi\Documents\Visual Studio 2012\Projects\UIAutomation\x64\Debug\UIAutomationLibrary.dll");//..\..\..\Debug\UIAutomationLibrary.dll");
            //int hrResult = LoadLibrary(dllPath);
            string appName = @"Microsoft.SDKSamples.GesturesApp.CS_8wekyb3d8bbwe!App";
            uint processId = 0;
            int result = 0;
            result = Mine.launchApp(appName, ref processId); //launchAppPInvoke(appName, ref processId);
            Console.WriteLine("Indirect PInvoke result : " + result + " -> App launched with process id " + processId);
            //result = launchAppCLIWrapper(appName, ref processId);
            //Console.WriteLine("Indirect C++/CLI Wrapper result : " + result + " -> App launched with process id " + processId);
            //IApplicationActivationManager appActiveManager = new ApplicationActivationManager();//Class not registered
            //result = appActiveManager.ActivateApplication(appName, null, ActivateOptions.None, out processId).ToInt32();
            //Console.WriteLine("Direct Com Wrapper result : " + result + " -> App launched with process id " + processId);
            TouchInjector.InitializeTouchInjection(10, TouchFeedback.INDIRECT);
            // Wait for app to initialize
            new ManualResetEvent(false).WaitOne(1000);
            //Automate.start(processId);
            TouchSimulation.SimulateSwipe(new Point { X = 1900, Y = 600 }, new Point { X = 100, Y = 600 }, 700);
            // Give time for app to finish any animation like snapping
            new ManualResetEvent(false).WaitOne(1000);
            TouchSimulation.SimulateTap(new Point { X = 500, Y = 600 });
            TouchSimulation.SimulatePinchAndZoom(new Point { X = 400, Y = 300 }, new Point { X = 300, Y = 200 }, true);
            // Give time for app to finish any animation like snapping
            new ManualResetEvent(false).WaitOne(1000);
            TouchSimulation.SimulatePinchAndZoom(new Point { X = 400, Y = 300 }, new Point { X = 300, Y = 200 }, false);
            // Give time for app to finish any animation like snapping
            new ManualResetEvent(false).WaitOne(1000);
            OpenCharm();
        }

        public static void OpenCharm()
        {
            TouchSimulation.SimulateSwipe(new Point { X = Screen.PrimaryScreen.Bounds.Right - 1, Y = Screen.PrimaryScreen.Bounds.Bottom / 2 }
                , new Point { X = Screen.PrimaryScreen.Bounds.Right - 101, Y = Screen.PrimaryScreen.Bounds.Bottom / 2 }, 500);
        }
    }
}
