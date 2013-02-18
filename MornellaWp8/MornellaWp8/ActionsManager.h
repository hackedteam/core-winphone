#include "Common.h"
#include <string>
#include <exception>
#include "JSON/JSON.h"
#include "Manager.h"

using namespace std;

#ifndef __ActionsManager_h__
#define __ActionsManager_h__

DWORD WINAPI ActionsThread(LPVOID lpParam);
DWORD WINAPI SyncThread(LPVOID lpParam);

class Action;

class ActionsManager : public Manager {
private: 
	static ActionsManager *Instance;
	static volatile LONG lLock;

	HANDLE hMutex;
	HANDLE syncThread, actionThread;
	HANDLE syncEvent, actionEvent;
	DWORD syncThreadId, actionThreadId;
	map<INT, Action *> actions;
	vector<Action *> syncQueue, actionQueue;

private:
	ActionsManager();
	~ActionsManager();

	void lock();
	void unlock();
	void createQueueThreads();

public:
	static ActionsManager* self();

	// Called by events
	void trigger(INT actionId);

	// Empty internal maps, close queue threads
	void clear();
	void add(INT actionId, JSONArray jConf);

	HANDLE getSyncEvent();
	HANDLE getActionEvent();
	Action* getAction(INT queue);
};

#endif