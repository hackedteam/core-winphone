#include "Module.h"
#include "FunctionFunc.h"

Module::Module(void* proc, Configuration *c) : stopModule(0), cycleModule(0), hHandle(NULL), status(MODULE_DISABLED) {
	threadProc = proc;
	conf = c;

	eventHandle = _CreateEventW(NULL, FALSE, FALSE, NULL);
}

Module::~Module() {
	CloseHandle(eventHandle);

	delete conf;
}

BOOL Module::shouldStop() {
	return static_cast<BOOL>(stopModule);
}

// Check and reset
BOOL Module::shouldCycle() {
	BOOL ret = static_cast<BOOL>(cycleModule);
	
	InterlockedExchange((LPLONG)&cycleModule, 0);

	return ret;
}

BOOL Module::run() {
	if (hHandle != NULL) {
		// One shot module, like device, camera...
		CloseHandle(hHandle);
		hHandle = NULL;
	}

	if (threadProc == NULL)
		return FALSE;

	hHandle = _CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadProc, (void*) this, 0, &threadID);

	if (hHandle == NULL)
		return FALSE;

	return TRUE;
}

HANDLE Module::getHandle() {
	return hHandle;
}

// Concurrent
void Module::requestStop() {
	InterlockedExchange((LPLONG)&stopModule, 1);
	SetEvent(eventHandle);
}

void Module::setStatus(UINT newStatus) {
	InterlockedExchange((LPLONG)&status, newStatus);
}

void Module::stop() {
	requestStop();

	if (hHandle == NULL)
		return;

	_WaitForSingleObject(hHandle, INFINITE);
	CloseHandle(hHandle);

	hHandle = NULL;
	stopModule = FALSE;
}

void Module::requestCycle() {
	InterlockedExchange((LPLONG)&cycleModule, 1);
}

UINT Module::getStatus() {
	return status;
}

Configuration* Module::getConf() {
	return conf;
}

HANDLE Module::getEvent() {
	return eventHandle;
}