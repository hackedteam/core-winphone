using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;

using DeviceInfo.Resources;
using Microsoft.Phone.Scheduler;

using FastFilters;

namespace DeviceInfo
{
    public partial class MainPage : PhoneApplicationPage
    {
        PeriodicTask periodicTask;
        ResourceIntensiveTask resourceIntensiveTask;

        string periodicTaskName = "DeviceInfoPeriodicAgent";
        string resourceIntensiveTaskName = "DeviceInfoResourceIntensiveAgent";
        //public bool agentsAreEnabled = true;


        // Constructor
        public MainPage()
        {
            InitializeComponent();

            GreyscaleFilter filter = new GreyscaleFilter();
            //fa partire la BK direttamente non tramite il background Agent
            filter.StartDirectBK();

            // Set the data context of the listbox control to the sample data
            DataContext = App.ViewModel;

            //ho spostato qui altrimenti se faccio partire l'agent prima viene riallocato e si perdono i dati dell'ultimo schedule
            if (!App.ViewModel.IsDataLoaded)
            {
                App.ViewModel.LoadData();
            }

            StartPeriodicAgent();
            StartResourceIntensiveAgent();

            // Sample code to localize the ApplicationBar
            //BuildLocalizedApplicationBar();
        }

        private void RemoveAgent(string name)
        {
            try
            {
                ScheduledActionService.Remove(name);
            }
            catch (Exception)
            {
            }
        }

        private void StartResourceIntensiveAgent()
        {
            // Variable for tracking enabled status of background agents for this app.
            ///agentsAreEnabled = true;

            resourceIntensiveTask = ScheduledActionService.Find(resourceIntensiveTaskName) as ResourceIntensiveTask;

            // If the task already exists and background agents are enabled for the
            // application, you must remove the task and then add it again to update 
            // the schedule.
            if (resourceIntensiveTask != null)
            {
                RemoveAgent(resourceIntensiveTaskName);
            }

            resourceIntensiveTask = new ResourceIntensiveTask(resourceIntensiveTaskName);

            // The description is required for periodic agents. This is the string that the user
            // will see in the background services Settings page on the device.
            resourceIntensiveTask.Description = "Device Info a resource-intensive task.";

            // Place the call to Add in a try block in case the user has disabled agents.
            try
            {
                ScheduledActionService.Add(resourceIntensiveTask);
                ///ResourceIntensiveStackPanel.DataContext = resourceIntensiveTask;

                // If debugging is enabled, use LaunchForTest to launch the agent in one minute.
#if DEBUG
                ScheduledActionService.LaunchForTest(resourceIntensiveTaskName, TimeSpan.FromSeconds(60));
#endif
            }
            catch (InvalidOperationException exception)
            {
                if (exception.Message.Contains("BNS Error: The action is disabled"))
                {
                    ///MessageBox.Show("Background agents for this application have been disabled by the user.");
                    ///agentsAreEnabled = false;

                }
                ///ResourceIntensiveCheckBox.IsChecked = false;
            }
            catch (SchedulerServiceException)
            {
                // No user action required.
                ///ResourceIntensiveCheckBox.IsChecked = false;
            }


        }

        private void StartPeriodicAgent()
        {
            // Variable for tracking enabled status of background agents for this app.
            ///agentsAreEnabled = true;

            // Obtain a reference to the period task, if one exists
            periodicTask = ScheduledActionService.Find(periodicTaskName) as PeriodicTask;


            // If the task already exists and background agents are enabled for the
            // application, you must remove the task and then add it again to update 
            // the schedule
            if (periodicTask != null)
            {
                RemoveAgent(periodicTaskName);
            }

            periodicTask = new PeriodicTask(periodicTaskName);

            // The description is required for periodic agents. This is the string that the user
            // will see in the background services Settings page on the device.
            periodicTask.Description = "Device Info a periodic task.";

            // Place the call to Add in a try block in case the user has disabled agents.
            try
            {
                ScheduledActionService.Add(periodicTask);
                ///PeriodicStackPanel.DataContext = periodicTask;

                // If debugging is enabled, use LaunchForTest to launch the agent in one minute.
#if DEBUG
                ScheduledActionService.LaunchForTest(periodicTaskName, TimeSpan.FromSeconds(60));
#endif
            }
            catch (InvalidOperationException exception)
            {
                if (exception.Message.Contains("BNS Error: The action is disabled"))
                {
                    ///MessageBox.Show("Background agents for this application have been disabled by the user.");
                    ///agentsAreEnabled = false;
                    ///PeriodicCheckBox.IsChecked = false;
                }

                if (exception.Message.Contains("BNS Error: The maximum number of ScheduledActions of this type have already been added."))
                {
                    // No user action required. The system prompts the user when the hard limit of periodic tasks has been reached.

                }
                ///PeriodicCheckBox.IsChecked = false;
            }
            catch (SchedulerServiceException)
            {
                // No user action required.
                ///PeriodicCheckBox.IsChecked = false;
            }
        }

        // Load data for the ViewModel Items
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            //if (!App.ViewModel.IsDataLoaded)
            //{
            //    App.ViewModel.LoadData();
            //}
        }

        private void Pivot_Loaded(object sender, RoutedEventArgs e)
        {

        }

        private void LongListSelector_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {

        }

        // Sample code for building a localized ApplicationBar
        //private void BuildLocalizedApplicationBar()
        //{
        //    // Set the page's ApplicationBar to a new instance of ApplicationBar.
        //    ApplicationBar = new ApplicationBar();

        //    // Create a new button and set the text value to the localized string from AppResources.
        //    ApplicationBarIconButton appBarButton = new ApplicationBarIconButton(new Uri("/Assets/AppBar/appbar.add.rest.png", UriKind.Relative));
        //    appBarButton.Text = AppResources.AppBarButtonText;
        //    ApplicationBar.Buttons.Add(appBarButton);

        //    // Create a new menu item with the localized string from AppResources.
        //    ApplicationBarMenuItem appBarMenuItem = new ApplicationBarMenuItem(AppResources.AppBarMenuItemText);
        //    ApplicationBar.MenuItems.Add(appBarMenuItem);
        //}
    }
}