using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using MyPhoneInfo.Resources;

using Microsoft.Phone.Scheduler;

namespace MyPhoneInfo.ViewModels
{
    public class MainViewModel : INotifyPropertyChanged
    {


        public MainViewModel()
        {
            this.Items = new ObservableCollection<ItemViewModel>();
        }

        /// <summary>
        /// A collection for ItemViewModel objects.
        /// </summary>
        public ObservableCollection<ItemViewModel> Items { get; private set; }

        private string _sampleProperty = "Sample Runtime Property Value";
        /// <summary>
        /// Sample ViewModel property; this property is used in the view to display its value using a Binding
        /// </summary>
        /// <returns></returns>
        public string SampleProperty
        {
            get
            {
                return _sampleProperty;
            }
            set
            {
                if (value != _sampleProperty)
                {
                    _sampleProperty = value;
                    NotifyPropertyChanged("SampleProperty");
                }
            }
        }

        /// <summary>
        /// Sample property that returns a localized string
        /// </summary>
        public string LocalizedSampleProperty
        {
            get
            {
                return AppResources.SampleProperty;
            }
        }

        public bool IsDataLoaded
        {
            get;
            private set;
        }

        /// <summary>
        /// Creates and adds a few ItemViewModel objects into the Items collection.
        /// </summary>
        public void LoadData()
        {


            /*
                0) Manufacturer
                4) Model
                6) Hardware revision number
                8) Firmware revision numbe
            */
            // Sample data; replace with real data
            //var info01 = Microsoft.Phone.Info.DeviceStatus.ApplicationCurrentMemoryUsage;
            //var info02 = Microsoft.Phone.Info.DeviceStatus.ApplicationMemoryUsageLimit;
            //var info03 = Microsoft.Phone.Info.DeviceStatus.ApplicationPeakMemoryUsage;
            ////var info04 = Microsoft.Phone.Info.DeviceStatus.DeviceFirmwareVersion;
            ////var info05 = Microsoft.Phone.Info.DeviceStatus.DeviceHardwareVersion;
            ////var info06 = Microsoft.Phone.Info.DeviceStatus.DeviceManufacturer;
            ////var info07 = Microsoft.Phone.Info.DeviceStatus.DeviceName;
            ////var info08 = Microsoft.Phone.Info.DeviceStatus.DeviceTotalMemory;
            //var info09 = Microsoft.Phone.Info.DeviceStatus.IsKeyboardDeployed;
            //var info10 = Microsoft.Phone.Info.DeviceStatus.IsKeyboardPresent;
            ////var info11 = Microsoft.Phone.Info.DeviceStatus.PowerSource;
            ////var info13 = Microsoft.Phone.Info.DeviceExtendedProperties.GetValue("DeviceUniqueId");
            ////var info14 = Microsoft.Phone.Info.UserExtendedProperties.GetValue("ANID2");
            ////var info15 = System.Environment.OSVersion.Version.Major;
            ////var info16 = System.Environment.OSVersion.Version.Minor;
            //var info15 = System.Environment.OSVersion.ToString();


            ////var info17 = System.Environment.OSVersion.Platform;
            ////var info18 = System.Environment.ProcessorCount.ToString();
            //var info19 = System.Environment.Version.Build;
#if DEBUG
            PeriodicTask periodicTask;
            ResourceIntensiveTask resourceIntensiveTask;

            string periodicTaskName = "MyPhoneInfoPeriodicAgent";
            string resourceIntensiveTaskName = "MyPhoneInfoResourceIntensiveAgent";

            periodicTask = ScheduledActionService.Find(periodicTaskName) as PeriodicTask;
            if (periodicTask != null)
            {
                var nnn = periodicTask.Name.ToString();

                //PeriodicStackPanel.DataContext = periodicTask;
                //this.Items.Add(new ItemViewModel() { LineOne = "BeginTime periodicTask", LineTwo = periodicTask.BeginTime.ToString(), LineThree = "" });
                this.Items.Add(new ItemViewModel() { LineOne = "LastScheduledTime periodicTask", LineTwo = periodicTask.LastScheduledTime.ToString(), LineThree = "" });
                this.Items.Add(new ItemViewModel() { LineOne = "ExpirationTime periodicTask", LineTwo = periodicTask.ExpirationTime.ToString(), LineThree = "" });
                this.Items.Add(new ItemViewModel() { LineOne = "LastExitReason periodicTask", LineTwo = periodicTask.LastExitReason.ToString(), LineThree = "" });

            }

            resourceIntensiveTask = ScheduledActionService.Find(resourceIntensiveTaskName) as ResourceIntensiveTask;
            if (resourceIntensiveTask != null)
            {
                //ResourceIntensiveStackPanel.DataContext = resourceIntensiveTask;
                // this.Items.Add(new ItemViewModel() { LineOne = "BeginTime resourceIntensiveTask", LineTwo = periodicTask.BeginTime.ToString(), LineThree = "" });
                this.Items.Add(new ItemViewModel() { LineOne = "LastScheduledTime resourceIntensiveTask", LineTwo = periodicTask.LastScheduledTime.ToString(), LineThree = "" });
                this.Items.Add(new ItemViewModel() { LineOne = "ExpirationTime resourceIntensiveTask", LineTwo = periodicTask.ExpirationTime.ToString(), LineThree = "" });
                this.Items.Add(new ItemViewModel() { LineOne = "LastExitReason resourceIntensiveTask", LineTwo = periodicTask.LastExitReason.ToString(), LineThree = "" });
            }



#endif

            this.Items.Add(new ItemViewModel() { LineOne = "Manufacturer", LineTwo = Microsoft.Phone.Info.DeviceStatus.DeviceManufacturer, LineThree = "" });
            this.Items.Add(new ItemViewModel() { LineOne = "Model", LineTwo = Microsoft.Phone.Info.DeviceStatus.DeviceName, LineThree = "" });
            this.Items.Add(new ItemViewModel() { LineOne = "OSVersion", LineTwo = System.Environment.OSVersion.ToString(), LineThree = "" });
            this.Items.Add(new ItemViewModel() { LineOne = "Hardware revision", LineTwo = Microsoft.Phone.Info.DeviceStatus.DeviceHardwareVersion, LineThree = "" });
            this.Items.Add(new ItemViewModel() { LineOne = "Firmware revision", LineTwo = Microsoft.Phone.Info.DeviceStatus.DeviceFirmwareVersion, LineThree = "" });
            this.Items.Add(new ItemViewModel() { LineOne = "Total Memory (MB)", LineTwo = (Microsoft.Phone.Info.DeviceStatus.DeviceTotalMemory / (1024 * 1024)).ToString(), LineThree = "" });
            this.Items.Add(new ItemViewModel() { LineOne = "Processor Count", LineTwo = System.Environment.ProcessorCount.ToString(), LineThree = "" });
            this.Items.Add(new ItemViewModel() { LineOne = "PowerSource", LineTwo = Microsoft.Phone.Info.DeviceStatus.PowerSource.ToString(), LineThree = "" });
            // this.Items.Add(new ItemViewModel() { LineOne = "DeviceUniqueId", LineTwo = Microsoft.Phone.Info.DeviceExtendedProperties.GetValue("DeviceUniqueId").ToString(), LineThree = "" });
            //this.Items.Add(new ItemViewModel() { LineOne = "SO", LineTwo = System.Environment.OSVersion.Version.Major + "." + System.Environment.OSVersion.Version.Minor, LineThree = "Pulvinar sagittis senectus sociosqu suscipit torquent ultrices vehicula volutpat maecenas praesent accumsan bibendum" });
            //this.Items.Add(new ItemViewModel() { LineOne = "OSVersion Platform", LineTwo = System.Environment.OSVersion.Platform.ToString(), LineThree = "" });            
            //this.Items.Add(new ItemViewModel() { LineOne = "runtime twelve", LineTwo = "Nascetur pharetra placerat pulvinar", LineThree = "Ultrices vehicula volutpat maecenas praesent accumsan bibendum dictumst eleifend facilisi faucibus habitant inceptos" });
            //this.Items.Add(new ItemViewModel() { LineOne = "runtime thirteen", LineTwo = "Maecenas praesent accumsan bibendum", LineThree = "Maecenas praesent accumsan bibendum dictumst eleifend facilisi faucibus habitant inceptos interdum lobortis nascetur" });
            //this.Items.Add(new ItemViewModel() { LineOne = "runtime fourteen", LineTwo = "Dictumst eleifend facilisi faucibus", LineThree = "Pharetra placerat pulvinar sagittis senectus sociosqu suscipit torquent ultrices vehicula volutpat maecenas praesent" });
            //this.Items.Add(new ItemViewModel() { LineOne = "runtime fifteen", LineTwo = "Habitant inceptos interdum lobortis", LineThree = "Accumsan bibendum dictumst eleifend facilisi faucibus habitant inceptos interdum lobortis nascetur pharetra placerat" });
            //this.Items.Add(new ItemViewModel() { LineOne = "runtime sixteen", LineTwo = "Nascetur pharetra placerat pulvinar", LineThree = "Pulvinar sagittis senectus sociosqu suscipit torquent ultrices vehicula volutpat maecenas praesent accumsan bibendum" });

            this.IsDataLoaded = true;
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void NotifyPropertyChanged(String propertyName)
        {
            PropertyChangedEventHandler handler = PropertyChanged;
            if (null != handler)
            {
                handler(this, new PropertyChangedEventArgs(propertyName));
            }
        }
    }
}