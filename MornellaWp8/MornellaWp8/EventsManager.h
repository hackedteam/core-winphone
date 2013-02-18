#include "Common.h"
#include <string>
#include <exception>
#include "JSON/JSON.h"
#include "Manager.h"

using namespace std;

#ifndef __EventsManager_h__
#define __EventsManager_h__

class Event;

class EventsManager : public Manager {
private: 
	static EventsManager *Instance;
	static volatile LONG lLock;
	HANDLE hMutex;

	vector<Event *> eventsList;

private:
	EventsManager();
	~EventsManager();

	void lock();
	void unlock();

public:
	static EventsManager* self();

	// Init module configuration
	BOOL add(const wstring& moduleName, JSONObject jConf, void* threadProc);

	// Empty internal maps
	void clear();

	BOOL startAll();
	BOOL stopAll();
	BOOL enable(INT eventIndex);
	BOOL disable(INT eventIndex);

#ifdef _DEBUG
	void dumpEvents();
#endif
};

#endif