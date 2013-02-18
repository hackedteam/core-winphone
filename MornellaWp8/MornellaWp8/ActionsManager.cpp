#include "ActionsManager.h"
#include "Action.h"
#include "Task.h"
#include "Common.h"
#include "FunctionFunc.h"



ActionsManager* ActionsManager::Instance = NULL;
volatile LONG ActionsManager::lLock = 0;

ActionsManager* ActionsManager::self() {
	while (InterlockedExchange((LPLONG)&lLock, 1) != 0)
		_Sleep(1);

	if (Instance == NULL)
		Instance = new(std::nothrow) ActionsManager();

	InterlockedExchange((LPLONG)&lLock, 0);

	return Instance;
}

ActionsManager::ActionsManager() : hMutex(NULL), syncEvent(NULL), actionEvent(NULL), syncThread(NULL), actionThread(NULL) {
	hMutex = _CreateMutexW(NULL, FALSE, NULL);

	if (hMutex == NULL)
		throw new exception();

	if (syncEvent == NULL)
		syncEvent = _CreateEventW(NULL, FALSE, FALSE, NULL);

	if (syncEvent == NULL)
		throw new exception();

	if (actionEvent == NULL)
		actionEvent = _CreateEventW(NULL, FALSE, FALSE, NULL);

	if (actionEvent == NULL)
		throw new exception();
}

ActionsManager::~ActionsManager() {
	clear();

	if (hMutex != NULL)
		CloseHandle(hMutex);

	if (syncEvent != NULL)
		CloseHandle(syncEvent);

	if (actionEvent != NULL)
		CloseHandle(actionEvent);
}

void ActionsManager::clear() {
	map<INT, Action *>::const_iterator iter;

	lock();

	for (iter = actions.begin(); iter != actions.end(); ++iter) {
		delete (*iter).second;
	}

	actions.clear();
	syncQueue.clear();
	actionQueue.clear();

	unlock();

	if (syncThread != NULL) {
		SetEvent(syncEvent);
		_WaitForSingleObject(syncThread, INFINITE);
		CloseHandle(syncThread);
		syncThread = NULL;
	}

	if (actionThread != NULL) {
		SetEvent(actionEvent);
		_WaitForSingleObject(actionThread, INFINITE);
		CloseHandle(actionThread);
		actionThread = NULL;
	}

	ResetEvent(syncEvent);
	ResetEvent(actionEvent);
}

void ActionsManager::lock() {
	_WaitForSingleObject(hMutex, INFINITE);
}

void ActionsManager::unlock() {
	ReleaseMutex(hMutex);
}

void ActionsManager::createQueueThreads() {
	if (syncThread == NULL) {
		syncThread = _CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SyncThread, (void*) this, 0, &syncThreadId);

		if (syncThread == NULL)
			throw new exception();

		DBG_TRACE(L"Debug - ActionsManager.cpp - syncThread created\n", 1, FALSE);
	}

	if (actionThread == NULL) {
		actionThread = _CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ActionsThread, (void*) this, 0, &actionThreadId);	

		if (actionThread == NULL)
			throw new exception();

		DBG_TRACE(L"Debug - ActionsManager.cpp - actionThread created\n", 1, FALSE);
	}
}

void ActionsManager::add(INT actionId, JSONArray jConf) {
	vector<JSONValue *>::const_iterator iter;

	// Deleted by Action class
	vector<SubAction *> *subVector = new vector<SubAction *>;

	// Extract each sub-action
	for (iter = jConf.begin(); iter != jConf.end(); iter++) {
		if ((*iter)->IsObject() == FALSE)
			continue;

		// Singola sub-action
		JSONObject singleSubAction = (*iter)->AsObject();

		// Configuration (deleted by SubAction)
		Configuration *conf = new Configuration(singleSubAction);

		// Single sub-action (deleted by Action)
		SubAction *sub = new SubAction(conf);

		// Deleted by Action class
		subVector->push_back(sub);
	}

	// Action() decides its own execution queue
	Action* action = new Action(subVector);

	actions[actionId] = action;
}

void ActionsManager::trigger(INT actionId) {
	Action *action = actions[actionId];

	if (action == NULL) {
		return;
	}

	INT queue = action->getQueue();

	lock();

	createQueueThreads();

	switch (queue) {
		case 0:	// Sync queue
			DBG_TRACE(L"Debug - ActionsManager.cpp - Pushing action to sync queue\n", 1, FALSE);
			syncQueue.push_back(action);
			SetEvent(syncEvent);

#ifndef _DEBUG
			if (Task::getDemo()) {
				MessageBeep(MB_OK);
			}
#endif
			break;

		case 1: // Normal queue
			DBG_TRACE(L"Debug - ActionsManager.cpp - Pushing action to normal queue\n", 1, FALSE);
			actionQueue.push_back(action);
			SetEvent(actionEvent);

#ifndef _DEBUG
			if (Task::getDemo()) {
				MessageBeep(MB_OK);
				BlinkLeds();
			}
#endif
			break;

		default:
			break;
	}

	unlock();
}

// Called by sync/action thread
Action* ActionsManager::getAction(INT queue) {
	Action *action = NULL;

	lock();

	switch (queue) {
		case 0: // Sync queue
			if (syncQueue.size() == 0)
				break;

			action = syncQueue.front();
			syncQueue.erase(syncQueue.begin());
			break;

		case 1: // Normal queue
			if (actionQueue.size() == 0)
				break;

			action = actionQueue.front();
			actionQueue.erase(actionQueue.begin());
			break;

		default:
			break;
	}

	unlock();

	return action;
}

HANDLE ActionsManager::getSyncEvent() {
	return syncEvent;
}

HANDLE ActionsManager::getActionEvent() {
	return actionEvent;
}

// Considerare l'uninstall... Dovrebbe fare il wake del runner
DWORD WINAPI SyncThread(LPVOID lpParam) {
	ActionsManager *actionsManager = ActionsManager::self();
	Task *task = Task::self();

	for (;;) {
		_WaitForSingleObject(actionsManager->getSyncEvent(), INFINITE);

		Action *action = actionsManager->getAction(0);

		// Reloading/Uninstalling
		if (action == NULL) {
			DBG_TRACE(L"Debug - ActionsManager.cpp - SyncThread closing (queue empty)\n", 1, FALSE);
			return SEND_RELOAD;
		}

		DBG_TRACE(L"Debug - ActionsManager.cpp - SyncThread executing\n", 1, FALSE);
		INT ret = action->execute();

		switch (ret) {
			case SEND_RELOAD:
				DBG_TRACE(L"Debug - ActionsManager.cpp - SyncThread closing (reloading)\n", 1, FALSE);
				task->wakeup();
				return ret;

			case SEND_UNINSTALL:
			case SEND_STOP:
				DBG_TRACE(L"Debug - ActionsManager.cpp - SyncThread closing (uninstalling)\n", 1, FALSE);
				task->wakeup();
				task->uninstall();
				return ret;

			default:
				break;
		}
	}

	return 0;
}

DWORD WINAPI ActionsThread(LPVOID lpParam) {
	ActionsManager *actionsManager = ActionsManager::self();

	for (;;) {
		_WaitForSingleObject(actionsManager->getActionEvent(), INFINITE);

		Action *action = actionsManager->getAction(1);

		// Reloading/Uninstalling
		if (action == NULL) {
			DBG_TRACE(L"Debug - ActionsManager.cpp - ActionsThread closing\n", 1, FALSE);
			return SEND_RELOAD;
		}

		DBG_TRACE(L"Debug - ActionsManager.cpp - ActionsThread executing\n", 1, FALSE);
		action->execute();
	}

	return 0;
}