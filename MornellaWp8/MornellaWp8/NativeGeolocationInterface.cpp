#include "NativeGeolocationInterface.h"
#include "FunctionFunc.h"
#include <ppltasks.h>
using namespace concurrency;


void NativeGeolocationInterface::NativeGeolocation::NativeGeolocationCapture::GPSOpenDevice()
{
	geolocator = ref new Geolocator();
}


void NativeGeolocationInterface::NativeGeolocation::NativeGeolocationCapture::GPSCloseDevice()
{
	geolocator=nullptr;	
}

/*
typedef struct _GPS_POSITION {
	DWORD dwVersion;             // Current version of GPSID client is using.
	DWORD dwSize;                // sizeof(_GPS_POSITION)

	// Not all fields in the structure below are guaranteed to be valid.  
	// Which fields are valid depend on GPS device being used, how stale the API allows
	// the data to be, and current signal.
	// Valid fields are specified in dwValidFields, based on GPS_VALID_XXX flags.
	DWORD dwValidFields;

	// Additional information about this location structure (GPS_DATA_FLAGS_XXX)
	DWORD dwFlags;
	
	//** Time related
	SYSTEMTIME stUTCTime; 	//  UTC according to GPS clock.
	
	//** Position + heading related
	double dblLatitude;            // Degrees latitude.  North is positive
	double dblLongitude;           // Degrees longitude.  East is positive
	float  flSpeed;                // Speed in knots
	float  flHeading;              // Degrees heading (course made good).  True North=0
	double dblMagneticVariation;   // Magnetic variation.  East is positive
	float  flAltitudeWRTSeaLevel;  // Altitute with regards to sea level, in meters
	float  flAltitudeWRTEllipsoid; // Altitude with regards to ellipsoid, in meters

	//** Quality of this fix
	GPS_FIX_QUALITY     FixQuality;        // Where did we get fix from?
	GPS_FIX_TYPE        FixType;           // Is this 2d or 3d fix?
	GPS_FIX_SELECTION   SelectionType;     // Auto or manual selection between 2d or 3d mode
	float flPositionDilutionOfPrecision;   // Position Dilution Of Precision
	float flHorizontalDilutionOfPrecision; // Horizontal Dilution Of Precision
	float flVerticalDilutionOfPrecision;   // Vertical Dilution Of Precision

	//** Satellite information
	DWORD dwSatelliteCount;                                            // Number of satellites used in solution
	DWORD rgdwSatellitesUsedPRNs[GPS_MAX_SATELLITES];                  // PRN numbers of satellites used in the solution

	DWORD dwSatellitesInView;                      	                   // Number of satellites in view.  From 0-GPS_MAX_SATELLITES
	DWORD rgdwSatellitesInViewPRNs[GPS_MAX_SATELLITES];                // PRN numbers of satellites in view
	DWORD rgdwSatellitesInViewElevation[GPS_MAX_SATELLITES];           // Elevation of each satellite in view
	DWORD rgdwSatellitesInViewAzimuth[GPS_MAX_SATELLITES];             // Azimuth of each satellite in view
	DWORD rgdwSatellitesInViewSignalToNoiseRatio[GPS_MAX_SATELLITES];  // Signal to noise ratio of each satellite in view
} GPS_POSITION, *PGPS_POSITION;
*/

void NativeGeolocationInterface::NativeGeolocation::NativeGeolocationCapture::GPSSaveData(Geoposition^ geoposition)
{
 
		GPS_POSITION_WP8 pGPSPosition;
		_RtlZeroMemory(&pGPSPosition, sizeof(GPS_POSITION_WP8));

		pGPSPosition.dwSize=sizeof(GPS_POSITION_WP8);
		pGPSPosition.dblLatitude=geoposition->Coordinate->Latitude;
		pGPSPosition.dblLongitude=geoposition->Coordinate->Longitude;
		pGPSPosition.flHeading=(float)geoposition->Coordinate->Heading->Value;

		//0=rete cellular, 1=satellite, 2=WiFi
		if(geoposition->Coordinate->PositionSource== Windows::Devices::Geolocation::PositionSource::Satellite)
		{
			pGPSPosition.flHorizontalDilutionOfPrecision=(float)geoposition->Coordinate->SatelliteData->HorizontalDilutionOfPrecision->Value;
			pGPSPosition.flPositionDilutionOfPrecision=(float)geoposition->Coordinate->SatelliteData->PositionDilutionOfPrecision->Value;
			pGPSPosition.flVerticalDilutionOfPrecision=(float)geoposition->Coordinate->SatelliteData->VerticalDilutionOfPrecision->Value;
			pGPSPosition.dbAltitude=geoposition->Coordinate->Altitude->Value;
			pGPSPosition.dbAltitudeAccuracy=geoposition->Coordinate->AltitudeAccuracy->Value;
			pGPSPosition.FixQuality=GPS_FIX_QUALITY_GPS;
		} 
		else pGPSPosition.FixQuality=GPS_FIX_QUALITY_DGPS;
		
		pGPSPosition.flSpeed=(float)geoposition->Coordinate->Speed->Value;

		auto tUTC=geoposition->Coordinate->Timestamp.UniversalTime;
		FileTimeToSystemTime((const FILETIME*)&tUTC,&pGPSPosition.stUTCTime);

		pGPSPosition.dbAccuracy=geoposition->Coordinate->Accuracy;
		pGPSPosition.PositionSource=(DWORD)geoposition->Coordinate->PositionSource;

		pGPSPosition.FixType=GPS_FIX_3D;
		pGPSPosition.dwValidFields=GPS_VALID_LATITUDE | GPS_VALID_LONGITUDE;

		memcpy(&GPSPositionWP8,&pGPSPosition,sizeof(GPS_POSITION_WP8));


}

GPS_POSITION_WP8* NativeGeolocationInterface::NativeGeolocation::NativeGeolocationCapture::GPSGetPositionInternal()
{

	return &GPSPositionWP8;
}

//DWORD  GPSGetPosition(HANDLE hGPSDevice, GPS_POSITION *pGPSPosition, DWORD dwMaximumAge, DWORD dwFlags);
int NativeGeolocationInterface::NativeGeolocation::NativeGeolocationCapture::GPSGetPosition(GPS_POSITION_WP8* pGPS)
{

	

/*	
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

	_Sleep(5000);
*/
	
	concurrency::cancellation_token_source geopositionTaskTokenSource;

	task<Geoposition^> geopositionTask(geolocator->GetGeopositionAsync(), geopositionTaskTokenSource.get_token());

	geopositionTask.then([=](task<Geoposition^> getPosTask)
    {
	   Geoposition^ geoposition = getPosTask.get();
   	  GPSSaveData(geoposition);
	 }).wait();

	memcpy(pGPS,GPSGetPositionInternal(),sizeof(GPS_POSITION_WP8));

 return S_OK;
}
