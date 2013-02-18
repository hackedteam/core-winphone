#include "Event.h"
#include "Conf.h"
#include "FunctionFunc.h"

Event::Event(void* proc, Configuration *c) : stopModule(0), threadHandle(NULL), status(MODULE_DISABLED),
startAction(-1), endAction(-1), repeatAction(-1) {
	threadProc = proc;
	conf = c;

	try {
		startAction = conf->getInt(L"start");
	} catch (...) {
		
	}

	try {
		endAction = conf->getInt(L"end");
	} catch (...) {

	}

	try {
		repeatAction = conf->getInt(L"repeat");
	} catch (...) {

	}

	actions = ActionsManager::self();

	eventHandle = _CreateEventW(NULL, FALSE, FALSE, NULL);
}

Event::~Event() {
	CloseHandle(eventHandle);

	delete conf;
}

// Only called from inside the thread
BOOL Event::shouldStop() {
	return static_cast<BOOL>(stopModule);
}

BOOL Event::run() {
	if (threadHandle != NULL || threadProc == NULL)
		return FALSE;

	threadHandle = _CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadProc, (void*) this, 0, &threadID);

	if (threadHandle == NULL)
		return FALSE;

	return TRUE;
}

HANDLE Event::getHandle() {
	return threadHandle;
}

// Concurrent, called through EventManager
void Event::requestStop() {
	InterlockedExchange((LPLONG)&stopModule, 1);
	SetEvent(eventHandle);
}

// Concurrent, called from inside the thread
void Event::setStatus(UINT newStatus) {
	InterlockedExchange((LPLONG)&status, newStatus);
}

// Called by EventManager
void Event::stop() {
	requestStop();

	if (threadHandle == NULL)
		return;

	_WaitForSingleObject(threadHandle, INFINITE);
	CloseHandle(threadHandle);

	threadHandle = NULL;
	stopModule = FALSE;
}

UINT Event::getStatus() {
	return status;
}

// Called by EventManager
BOOL Event::isEnabled() {
	return conf->getBool(L"enabled");
}

const wstring Event::getName() {
	try {
		return conf->getString(L"event");
	} catch (...) {
		return L"unknown";
	}
}

// Only called from inside the thread
void Event::triggerStart() {
	if (startAction == CONF_ACTION_NULL)
		return;

	actions->trigger(startAction);
}

// Only called from inside the thread
void Event::triggerEnd() {
	if (endAction == CONF_ACTION_NULL)
		return;

	actions->trigger(endAction);
}

// Only called from inside the thread
void Event::triggerRepeat() {
	if (repeatAction == CONF_ACTION_NULL)
		return;
	
	actions->trigger(repeatAction);
}

Configuration* Event::getConf() {
	return conf;
}

HANDLE Event::getEvent() {
	return eventHandle;
}