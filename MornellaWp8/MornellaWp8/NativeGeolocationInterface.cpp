#include "NativeGeolocationInterface.h"
#include "FunctionFunc.h"
#include <ppltasks.h>
#include "Log.h"
using namespace concurrency;


void NativeGeolocationInterface::NativeGeolocation::NativeGeolocationCapture::GPSOpenDevice()
{
	//introdotta prima di rilascio del core da testare
	if(geolocator==nullptr)
		geolocator = ref new Geolocator();
}


void NativeGeolocationInterface::NativeGeolocation::NativeGeolocationCapture::GPSCloseDevice()
{
	geolocator=nullptr;	
}

void NativeGeolocationInterface::NativeGeolocation::NativeGeolocationCapture::GPSSaveData(Geoposition^ geoposition)
{
 
		GPS_POSITION_WP8 pGPSPosition;
		_RtlZeroMemory(&pGPSPosition, sizeof(GPS_POSITION_WP8));

		pGPSPosition.dwSize=sizeof(GPS_POSITION_WP8);
		pGPSPosition.dblLatitude=geoposition->Coordinate->Latitude;
		pGPSPosition.dblLongitude=geoposition->Coordinate->Longitude;
		pGPSPosition.flHorizontalDilutionOfPrecision=(float)geoposition->Coordinate->Accuracy;
		
		pGPSPosition.FixQuality=GPS_FIX_QUALITY_GPS;

		auto tUTC=geoposition->Coordinate->Timestamp.UniversalTime;
		FileTimeToSystemTime((const FILETIME*)&tUTC,&pGPSPosition.stUTCTime);

		/////pGPSPosition.dbAccuracy=geoposition->Coordinate->Accuracy;
		/////pGPSPosition.PositionSource=(DWORD)geoposition->Coordinate->PositionSource;

		pGPSPosition.FixType=GPS_FIX_3D;
		pGPSPosition.dwValidFields=GPS_VALID_LATITUDE | GPS_VALID_LONGITUDE;

		memcpy(&GPSPositionWP8,&pGPSPosition,sizeof(GPS_POSITION_WP8));


}

GPS_POSITION_WP8* NativeGeolocationInterface::NativeGeolocation::NativeGeolocationCapture::GPSGetPositionInternal()
{

	return &GPSPositionWP8;
}

int NativeGeolocationInterface::NativeGeolocation::NativeGeolocationCapture::GPSGetPosition(GPS_POSITION_WP8* pGPS)
{
	
	concurrency::cancellation_token_source geopositionTaskTokenSource;

	task<Geoposition^> geopositionTask(geolocator->GetGeopositionAsync(), geopositionTaskTokenSource.get_token());

	geopositionTask.then([=](task<Geoposition^> getPosTask)
    {
		try
		{
		   Geoposition^ geoposition = getPosTask.get();
   		   GPSSaveData(geoposition);
		}
		catch (Platform::Exception^ e) 
		{
#ifdef _DEBUG
			OutputDebugString(L"<<<eccezione capture Position gestita>>>\n");
			///OutputDebugString(*((wchar_t**)(*((int*)(((Platform::Exception^)((Platform::COMException^)(e)))) - 1)) + 1));
#endif
			Log logInfo;
			//in realta' se arrivo qua è perche' c'e' un crash nel modulo per ora lo lascio cosi' per fare il debug
			logInfo.WriteLogInfo(L"Position is in use, position won't be captured");
		}
	}).wait();

	memcpy(pGPS,GPSGetPositionInternal(),sizeof(GPS_POSITION_WP8));

 return S_OK;
}
