using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Drawing;

using NativeWrappers;

namespace UIAutomationLibraryClientCSharp
{
    class TouchSimulation
    {

        public static int touchTimeResolution = 5;

        public static int touchHoldTime = 1000;

        public static void SimulateTap(Point point)
        {
            PointerTouchInfo[] contacts = new PointerTouchInfo[1];
            contacts[0] = CreateDefaultPointerTouchInfo(point.X, point.Y, 2, 1);

            // Touch down
            contacts[0].PointerInfo.PointerFlags = PointerFlags.DOWN | PointerFlags.INRANGE | PointerFlags.INCONTACT;

            // Initial contact
            bool success = TouchInjector.InjectTouchInput(contacts.Length, contacts);

            // Release contact
            contacts[0].PointerInfo.PointerFlags = PointerFlags.UP;

            success = TouchInjector.InjectTouchInput(contacts.Length, contacts);
        }

        public static void SimulateHold(Point point)
        {
            PointerTouchInfo[] contacts = new PointerTouchInfo[1];
            contacts[0] = CreateDefaultPointerTouchInfo(point.X, point.Y, 2, 1);

            // Touch down
            contacts[0].PointerInfo.PointerFlags = PointerFlags.DOWN | PointerFlags.INRANGE | PointerFlags.INCONTACT;

            // Initial contact
            bool success = TouchInjector.InjectTouchInput(contacts.Length, contacts);

            // Touch update
            contacts[0].PointerInfo.PointerFlags = PointerFlags.UPDATE | PointerFlags.INRANGE | PointerFlags.INCONTACT;

            int steps = touchHoldTime / touchTimeResolution;

            // Move contact
            for (int i = 0; i < steps; i++)
            {
                success = TouchInjector.InjectTouchInput(contacts.Length, contacts);
                new ManualResetEvent(false).WaitOne(touchTimeResolution);
            }

            // Release contact
            contacts[0].PointerInfo.PointerFlags = PointerFlags.UP;

            success = TouchInjector.InjectTouchInput(contacts.Length, contacts);
        }

        public static void SimulateSwipe(Point start, Point end, int step = 1)
        {
            PointerTouchInfo[] contacts = new PointerTouchInfo[1];
            contacts[0] = CreateDefaultPointerTouchInfo(start.X, start.Y, 2, 0);

            // Touch down
            contacts[0].PointerInfo.PointerFlags = PointerFlags.DOWN | PointerFlags.INRANGE | PointerFlags.INCONTACT;

            // Initial contact
            bool success = TouchInjector.InjectTouchInput(contacts.Length, contacts);

            // Touch update
            contacts[0].PointerInfo.PointerFlags = PointerFlags.UPDATE | PointerFlags.INRANGE | PointerFlags.INCONTACT;

            int steps = (int)Math.Sqrt(Math.Pow(start.X - end.X, 2) + Math.Pow(start.Y - end.Y, 2)) / step;

            double deltaXRaw = (double)(end.X - start.X) / steps;

            double deltaYRaw = (double)(end.Y - start.Y) / steps;

            double deltaX = 0;
            double deltaY = 0;

            // Move contact
            for (int i = 0; i < steps; ++i)
            {
                deltaX += deltaXRaw;
                deltaY += deltaYRaw;
                contacts[0].Move((int)deltaX, (int)deltaY);
                success = TouchInjector.InjectTouchInput(contacts.Length, contacts);
                new ManualResetEvent(false).WaitOne(touchTimeResolution);
                deltaX = deltaX - (int)deltaX;
                deltaY = deltaY - (int)deltaY;
            }

            if (deltaX != 0 || deltaY != 0)
            {
                contacts[0].PointerInfo.PtPixelLocation.X = end.X;
                contacts[0].PointerInfo.PtPixelLocation.Y = end.Y;
                success = TouchInjector.InjectTouchInput(contacts.Length, contacts);
                new ManualResetEvent(false).WaitOne(touchTimeResolution);
            }

            // Release contact
            contacts[0].PointerInfo.PointerFlags = PointerFlags.UP;

            success = TouchInjector.InjectTouchInput(contacts.Length, contacts);
        }

        public static void SimulatePinchAndZoom(Point first, Point second, bool zoomOut, int step = 1)
        {
            PointerTouchInfo[] contacts = new PointerTouchInfo[2];
            contacts[0] = CreateDefaultPointerTouchInfo(first.X, first.Y, 2, 1);
            contacts[1] = CreateDefaultPointerTouchInfo(second.X, second.Y, 2, 2);

            // Touch down
            contacts[0].PointerInfo.PointerFlags = PointerFlags.DOWN | PointerFlags.INRANGE | PointerFlags.INCONTACT;
            contacts[1].PointerInfo.PointerFlags = PointerFlags.DOWN | PointerFlags.INRANGE | PointerFlags.INCONTACT;

            // Initial contact
            bool success = TouchInjector.InjectTouchInput(contacts.Length, contacts);

            // Touch update
            contacts[0].PointerInfo.PointerFlags = PointerFlags.UPDATE | PointerFlags.INRANGE | PointerFlags.INCONTACT;
            contacts[1].PointerInfo.PointerFlags = PointerFlags.UPDATE | PointerFlags.INRANGE | PointerFlags.INCONTACT;

            int steps = (int)Math.Sqrt(Math.Pow(first.X - second.X, 2) + Math.Pow(first.Y - second.Y, 2)) / step;

            double deltaXRaw = Math.Abs((double)(first.X - second.X) / steps / 2);

            double deltaYRaw = Math.Abs((double)(first.Y - second.Y) / steps / 2);

            double deltaX = 0;
            double deltaY = 0;

            int direction;

            // if zoomOut move touch points closer
            if (zoomOut) direction = +1;
            else direction = -1;

            // Move contact
            for (int i = 0; i < steps; ++i)
            {
                deltaX += deltaXRaw;
                deltaY += deltaYRaw;
                contacts[0].Move(direction * (int)-deltaX, direction * (int)-deltaY);
                contacts[1].Move(direction * (int)deltaX, direction * (int)deltaY);
                success = TouchInjector.InjectTouchInput(contacts.Length, contacts);
                new ManualResetEvent(false).WaitOne(touchTimeResolution);
                deltaX = deltaX - (int)deltaX;
                deltaY = deltaY - (int)deltaY;
            }

            // Release contacts
            contacts[0].PointerInfo.PointerFlags = PointerFlags.UP;
            contacts[1].PointerInfo.PointerFlags = PointerFlags.UP;

            success = TouchInjector.InjectTouchInput(contacts.Length, contacts);
        }

        public static PointerTouchInfo CreateDefaultPointerTouchInfo(int x, int y, int radius, uint id)
        {
            PointerTouchInfo contact = new PointerTouchInfo();
            contact.PointerInfo.pointerType = PointerInputType.TOUCH;
            contact.TouchFlags = TouchFlags.NONE;
            contact.TouchMasks = TouchMask.CONTACTAREA | TouchMask.ORIENTATION | TouchMask.PRESSURE;
            contact.PointerInfo.PtPixelLocation.X = x;
            contact.PointerInfo.PtPixelLocation.Y = y;
            contact.PointerInfo.PointerId = id;
            contact.ContactArea.left = x - radius;
            contact.ContactArea.right = x + radius;
            contact.ContactArea.top = y - radius;
            contact.ContactArea.bottom = y + radius;
            contact.Orientation = 130; //See the angle changed from default value 90 to 130
            return contact;
        }
    }
}
