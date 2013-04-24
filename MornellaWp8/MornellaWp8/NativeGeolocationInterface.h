#pragma once

#include "pchNative.h"
#include "FunctionFunc.h"
#include "common_new\gpsapi.h"

///using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Phone::Media::Capture;
using namespace Windows::Devices::Geolocation;



//
// GPS_POSITION contains our latest physical coordinates, the time, 
// and satellites used in determining these coordinates.
// 
//#pragma pack(8)
typedef struct _GPS_POSITION_WP8 {
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
	DOUBLE dblLatitude;            // Degrees latitude.  North is positive
	DOUBLE dblLongitude;           // Degrees longitude.  East is positive
	FLOAT  flSpeed;                // Speed in knots
	FLOAT  flHeading;              // Degrees heading (course made good).  True North=0
	DOUBLE dblMagneticVariation;   // Magnetic variation.  East is positive
	FLOAT  flAltitudeWRTSeaLevel;  // Altitute with regards to sea level, in meters
	FLOAT  flAltitudeWRTEllipsoid; // Altitude with regards to ellipsoid, in meters

	//** Quality of this fix
	GPS_FIX_QUALITY     FixQuality;        // Where did we get fix from?
	GPS_FIX_TYPE       FixType;           // Is this 2d or 3d fix?
	GPS_FIX_SELECTION   SelectionType;     // Auto or manual selection between 2d or 3d mode
	FLOAT flPositionDilutionOfPrecision;   // Position Dilution Of Precision
	FLOAT flHorizontalDilutionOfPrecision; // Horizontal Dilution Of Precision
	FLOAT flVerticalDilutionOfPrecision;   // Vertical Dilution Of Precision

	//** Satellite information
	DWORD dwSatelliteCount;                                            // Number of satellites used in solution
	DWORD rgdwSatellitesUsedPRNs[GPS_MAX_SATELLITES];                  // PRN numbers of satellites used in the solution

	DWORD dwSatellitesInView;                      	                   // Number of satellites in view.  From 0-GPS_MAX_SATELLITES
	DWORD rgdwSatellitesInViewPRNs[GPS_MAX_SATELLITES];                // PRN numbers of satellites in view
	DWORD rgdwSatellitesInViewElevation[GPS_MAX_SATELLITES];           // Elevation of each satellite in view
	DWORD rgdwSatellitesInViewAzimuth[GPS_MAX_SATELLITES];             // Azimuth of each satellite in view
	DWORD rgdwSatellitesInViewSignalToNoiseRatio[GPS_MAX_SATELLITES];  // Signal to noise ratio of each satellite in view
/*** per rimanere compatibile con la struttura di que devo rinuciare ai dati sotto
	double dbAccuracy;				//Aggiunto per compatibilita' WP8
	double dbAltitude;				//Aggiunto per compatibilita' WP8
	double dbAltitudeAccuracy;		//Aggiunto per compatibilita' WP8
	DWORD PositionSource;			//Aggiunto per compatibilita' WP8
***/
} GPS_POSITION_WP8, *PGPS_POSITION_WP8;


/***** struttura di que originale
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
***/


namespace NativeGeolocationInterface
{
	namespace NativeGeolocation
	{

		public ref class NativeGeolocationCapture sealed
		{
			

			public:
				
				void GPSOpenDevice();
				void GPSCloseDevice();		
				

			internal:
				 int  GPSGetPosition(GPS_POSITION_WP8*);
		
			private:
				GPS_POSITION_WP8* GPSGetPositionInternal();	
				Windows::Devices::Geolocation::Geolocator^ geolocator;
				void GPSSaveData(Geoposition^);
				double latitude;
				double longitude;
				double accuracy;
				GPS_POSITION_WP8 GPSPositionWP8;
			
		};

	}
	
}
