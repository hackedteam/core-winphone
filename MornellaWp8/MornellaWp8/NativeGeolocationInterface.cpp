#include "NativeGeolocationInterface.h"


void NativeGeolocationInterface::NativeGeolocation::NativeGeolocationCapture::GPSOpenDevice()
{
	geolocator = ref new Geolocator();
}


void NativeGeolocationInterface::NativeGeolocation::NativeGeolocationCapture::GPSCloseDevice()
{
		
		
}

void NativeGeolocationInterface::NativeGeolocation::NativeGeolocationCapture::GPSSaveData(double _latitude,double _longitude,double _accuracy)
{
 
	latitude=_latitude;
	longitude=_longitude;
	accuracy=_accuracy;

}

double NativeGeolocationInterface::NativeGeolocation::NativeGeolocationCapture::GPSGetLatitude()
{
	return latitude;
}

//DWORD  GPSGetPosition(HANDLE hGPSDevice, GPS_POSITION *pGPSPosition, DWORD dwMaximumAge, DWORD dwFlags);
int NativeGeolocationInterface::NativeGeolocation::NativeGeolocationCapture::GPSGetPosition()
{
	
	IAsyncOperation<Geoposition^> ^operation = geolocator->GetGeopositionAsync();

	operation->Completed = ref new AsyncOperationCompletedHandler<Geoposition^>(
        [this] (IAsyncOperation<Geoposition^>^ asyncOperation, Windows::Foundation::AsyncStatus status) mutable
        {
            if(status != Windows::Foundation::AsyncStatus::Error)
            {
               Geoposition^ geoposition = asyncOperation->GetResults();

               // use the location information
               double latitude = geoposition->Coordinate->Latitude;
               double longitude = geoposition->Coordinate->Longitude;
               double accuracy = geoposition->Coordinate->Accuracy;
			   //NativeGeolocationInterface::NativeGeolocation::NativeGeolocationCapture::GPSSaveData(latitude,longitude,accuracy);
			   GPSSaveData(latitude,longitude,accuracy);

            }
            else
            {
				if(asyncOperation->ErrorCode.Value == E_ABORT)
				{
					// The user has disable location in the phone settings
				}
				else
				{
					// There was another error
				}
            }
        });
		
	double r1=GPSGetLatitude();
	
 return S_OK;
}
