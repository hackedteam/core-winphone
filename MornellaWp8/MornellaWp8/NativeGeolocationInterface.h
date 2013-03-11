#pragma once

#include "pchNative.h"

///using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Phone::Media::Capture;
using namespace Windows::Devices::Geolocation;







namespace NativeGeolocationInterface
{
	namespace NativeGeolocation
	{

		public ref class NativeGeolocationCapture sealed
		{
			

			public:
				int GPSGetPosition();
				void GPSOpenDevice();
				void GPSCloseDevice();		
				

				

			private:
				
				Windows::Devices::Geolocation::Geolocator^ geolocator;
				double GPSGetLatitude();
				void GPSSaveData(double,double,double);
				double latitude;
				double longitude;
				double accuracy;
			
		};

	}
	
}
