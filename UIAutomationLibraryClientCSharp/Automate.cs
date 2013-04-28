using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Automation;

namespace UIAutomationLibraryClientCSharp
{
    class Automate
    {
        public static void start(uint processId) {
            //var x = AutomationElement.RootElement.FindFirst(TreeScope.Children, new PropertyCondition(AutomationElement.NameProperty, (int) processId));
            var x = AutomationElement.RootElement.FindFirst(TreeScope.Children, new PropertyCondition(AutomationElement.NameProperty, "GesturesApp JS sample"));
            //(TreeScope.Descendants, new PropertyCondition(AutomationElement.NameProperty,
            //"GestureRecognizer Ball in Eight", PropertyConditionFlags.IgnoreCase));
            x.GetCurrentPropertyValue(AutomationElement.ClassNameProperty, true);
        }
    }
}
