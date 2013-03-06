#include "NativeAudioInterface.h"

using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Phone::Media::Capture;
using namespace Windows::Storage;
using namespace Concurrency;
using namespace Platform;

Windows::Phone::Media::Capture::AudioVideoCaptureDevice ^pAudioVideoCaptureDevice;
IAsyncOperation<AudioVideoCaptureDevice^> ^openOperation;

#include <time.h>


#define DTTMFMT "%Y-%m-%d %H:%M:%S "
#define DTTMSZ 21

//#define DTTMFMTAUD "%Y%m%d%H%M%Sxxxxxx"
#define DTTMFMTAUD "%Y%m%d%H%M%S"
#define DTTMSZAUD 16

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

static int fAudio=FALSE;

// Called each time a captured frame is available	
void NativeAudioInterface::Native::CameraCaptureSampleSink::OnSampleAvailable(
	ULONGLONG hnsPresentationTime,
	ULONGLONG hnsSampleDuration,
	DWORD cbSample,
	BYTE* pSample)
{
	static int pos=0;
	static int nCamp=1;
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
	if (NativeAudioInterface::Native::NativeCapture::fAudioCaptureForceStop==TRUE) return;

	if(pos==0&&nCamp==1)
	{
		BYTE HEADER[]={35, 33, 65, 77, 82, 10};
		
		memcpy(bufferTmp,HEADER,sizeof(HEADER));
		pos=sizeof(HEADER);

		//creo il nome del file come data file (poi vedro' come criptarli)
		char buff[DTTMSZAUD];
   		getDtTmAUD(buff);
		strcpy(nomeFileBase,buff);
	}
	else 
		pos=0;

	
	memcpy(bufferTmp+pos,pSample,cbSample);
	pos=pos+cbSample;

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
	if(NativeAudioInterface::Native::NativeCapture::fAudioCapture==FALSE)
	{
		//dovrei entrare qua dentro solo quando stoppo il processo per cui mi trovo un trunk non completo
		fstream filestr;
		sprintf(nomeFile,"audio%s_%i.amr",nomeFileBase,nCamp);
		filestr.open(nomeFile, fstream::out|fstream::binary|fstream::app);
		filestr.seekg (0, ios::beg);
		filestr.write ((const char*)bufferTmp, pos);
		filestr.close();
		OutputDebugStringA(nomeFile);
		NativeAudioInterface::Native::NativeCapture::fAudioCaptureForceStop=TRUE;
		pos=0;
		nCamp=1;
	}
	else if(hnsPresentationTime>(ULONGLONG)5*nCamp*10000000 )
	{
		//NativeAudioInterface::Native::NativeCapture::fAudioCapture=FALSE;
			
		fstream filestr;
		sprintf(nomeFile,"audio%s_%i.amr",nomeFileBase,nCamp);
		filestr.open(nomeFile, fstream::out|fstream::binary|fstream::app);
		filestr.seekg (0, ios::beg);
		filestr.write ((const char*)bufferTmp, pos);
		filestr.close();
		OutputDebugStringA(nomeFile);
		pos=0;
		nCamp++;
	}

}

void NativeAudioInterface::Native::NativeCapture::StopCapture()
{
	fAudioCapture=FALSE;	
	pAudioVideoCaptureDevice->StopRecordingAsync();
}

#include <thread>
#include <chrono>

	


void NativeAudioInterface::Native::NativeCapture::StartCapture()
{
		fAudioCapture=TRUE;	
		
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
}

NativeAudioInterface::Native::NativeCapture::NativeCapture()
{
	fAudioCapture=FALSE;

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

}
