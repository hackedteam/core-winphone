#include "pch.h"
#include "NativeAudioInterface.h"

using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Phone::Media::Capture;
using namespace Windows::Storage;
using namespace Concurrency;
using namespace Platform;

Windows::Phone::Media::Capture::AudioVideoCaptureDevice ^pAudioVideoCaptureDevice;


/**********************************
// Called each time a preview frame is available
void NativeAudioInterface::Native::CameraCapturePreviewSink::OnFrameAvailable(
	DXGI_FORMAT format,
	UINT width,
	UINT height,
	BYTE* pixels
	)
{
	
	
	// Insert your own code to process each preview frame here
	//OutputDebugString(L"preview frame here: ");
	
	int j=0;
	BYTE tmp;
	//converto da bgra in rgb
	for(int i=0;i<width*height*4;i=i+4)
	{
      tmp=pixels[i];
	  pixels[j]=pixels[i+2];
	  pixels[j+1]=pixels[i+1];
	  pixels[j+2]=tmp;
	  j=j+3;
	}
	
	 
	char nomeFile[20];
	sprintf(nomeFile,"image%ix.rgb",conta);

	fstream filestr;
	//non utilizzo l'incremento delle immagini
	//filestr.open("image.rgb", fstream::out|fstream::binary);
	filestr.open(nomeFile, fstream::out|fstream::binary);
	filestr.seekg (0, ios::beg);
    filestr.write ((const char*)pixels, width*height*3);
	filestr.close();

	//if(contaCameraCapturePreviewSink==0) pAudioVideoCaptureDevice->StopRecordingAsync();
	//pCameraCapturePreviewSink->Release;
	
	
	contaCameraCapturePreviewSink++;
}

******************/
#include <time.h>


#define DTTMFMT "%Y-%m-%d %H:%M:%S "
#define DTTMSZ 21
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
	static int nCamp=0;

	//1MB di buffer
	static BYTE bufferTmp[1024*512];

//	if(fAudio==TRUE)
//	{
//		fAudio=FALSE;
////		pAudioVideoCaptureDevice->StopRecordingAsync();
//	}
//
//
//	return;






/****************
		pos=pos+cbSample;

		if((hnsPresentationTime>(ULONGLONG)30*10000000) && (fAudioCapture==TRUE))
		{
			fAudioCapture=FALSE;
			pAudioVideoCaptureDevice->StopRecordingAsync();
		
			pos=0;
			nCamp++;

		}

		contaCameraCaptureSampleSink++;

		return;

****************/






	char nomeFile[20];
	sprintf(nomeFile,"audio%i.amr",nCamp);

	// hnsPresentationTime aumenta partendo da 0 allo start, 1 sec=10000000
	 // Header= 35, 33, 65, 77, 82, 10
	 // Header= 23  21  41  4D  52  0A


	if(pos==0)
	{
		BYTE HEADER[]={35, 33, 65, 77, 82, 10};
		
		/*
		filestr.open(nomeFile, fstream::out|fstream::binary|fstream::app);
		filestr.seekg (pos, ios::beg);
		filestr.write ((const char*)HEADER, sizeof(HEADER));
		filestr.close();
	*/
		memcpy(bufferTmp,HEADER,sizeof(HEADER));
		pos=sizeof(HEADER);
	}


	memcpy(bufferTmp+pos,pSample,cbSample);
	pos=pos+cbSample;
	
	//catturo solo 20 secondi dallo start anche se la cattura nontinua per 28 sec
//	if(NativeAudioInterface::Native::NativeCapture::fAudioCapture==FALSE)
	if((hnsPresentationTime>(ULONGLONG)20*10000000) && (NativeAudioInterface::Native::NativeCapture::fAudioCapture==TRUE))
	{
		NativeAudioInterface::Native::NativeCapture::fAudioCapture=FALSE;
			fstream filestr;
			filestr.open(nomeFile, fstream::out|fstream::binary|fstream::app);
			filestr.seekg (0, ios::beg);
			filestr.write ((const char*)bufferTmp, pos);
			filestr.close();

			pos=0;
			nCamp++;
	}
	


	/*
	filestr.open(nomeFile, fstream::out|fstream::binary|fstream::app);
	filestr.seekg (pos, ios::beg);
	filestr.write ((const char*)pSample, cbSample);
	filestr.close();
	pos=pos+cbSample;
	////nCamp++;

	*/

	

	/*
	wchar_t msgW[128];
	char msgA[128];
	swprintf_s(msgW,L"hnsPresentationTime,hnsSampleDuration,cbSample=(%i,%i,%i)\n", hnsPresentationTime,hnsSampleDuration,cbSample );
	wcstombs(msgA, msgW, wcslen(msgW)+1);

	
	char buff[DTTMSZ];
	filestr.open ("timeaudio.txt", fstream::out|fstream::app);
	filestr << getDtTm (buff) << std::endl;
	filestr << msgA << std::endl;
	filestr << std::endl;
	filestr.close();

	*/

	/***************
	if(fCapture==TRUE) 
	{
		fCapture=FALSE;
		pAudioVideoCaptureDevice->StopRecordingAsync();
//		NativeAudioInterface::Native::pAudioVideoCaptureDevice->StopRecordingAsync();
	}
	*************/
	/*
	if((hnsPresentationTime>(ULONGLONG)30*10000000) && (fAudioCapture==TRUE))
	{
		fAudioCapture=FALSE;
		pAudioVideoCaptureDevice->StopRecordingAsync();
		
		pos=0;
		nCamp++;
		
		
//		NativeAudioInterface::Native::pAudioVideoCaptureDevice->StopRecordingAsync();
	}


	contaCameraCaptureSampleSink++;
	*/
}

void NativeAudioInterface::Native::NativeCapture::StopCapture()
{
	fAudioCapture=FALSE;	
	pAudioVideoCaptureDevice->StopRecordingAsync();
	/////////fCapture=FALSE;
	
	
}

void NativeAudioInterface::Native::NativeCapture::StartCapture()
{
	//salto il primo perche' non è detto che l'audio sia gia' inizializzato
	//if(conta !=0)
	{	
		fAudioCapture=TRUE;	
	///	fAudio=TRUE;
		pAudioVideoCaptureDevice->StartRecordingToSinkAsync();
		
	}
}

void NativeAudioInterface::Native::NativeCapture::WriteFile( String^ strFile, String^ strContent )
{
    auto folder = ApplicationData::Current->LocalFolder;
    task<StorageFile^> getFileTask(folder->CreateFileAsync( strFile, CreationCollisionOption::ReplaceExisting));

    // Create a local to allow the DataReader to be passed between lambdas.
    auto writer = std::make_shared<Streams::DataWriter^>(nullptr);

    getFileTask.then([](StorageFile^ file)
    {
        return file->OpenAsync(FileAccessMode::ReadWrite);
    }).then([this, writer, strContent](Streams::IRandomAccessStream^ stream)
    {
        Streams::DataWriter^ state = ref new Streams::DataWriter(stream);
        *writer = state;

        unsigned int codeUnits = state->MeasureString(strContent);
        state->WriteUInt32(codeUnits);
        state->WriteString(strContent);

        return state->StoreAsync();
    }).then([writer](uint32 count)
    {
        return (*writer)->FlushAsync();
    }).then([this, writer](bool flushed)
    {
        delete (*writer);
    });
}

void NativeAudioInterface::Native::NativeCapture::LoadFile(String^ strFile)
{
    auto folder = ApplicationData::Current->LocalFolder;
    task<StorageFile^> getFileTask(folder->GetFileAsync(strFile));

    // Create a local to allow the DataReader to be passed between lambdas.
    auto reader = std::make_shared<Streams::DataReader^>(nullptr);
    getFileTask.then([this, reader](task<StorageFile^> fileTask)
    {
        try
        {
            StorageFile^ file = fileTask.get();

            task<Streams::IRandomAccessStreamWithContentType^> (file->OpenReadAsync()).then([reader](Streams::IRandomAccessStreamWithContentType^ stream)
            {
                *reader = ref new Streams::DataReader(stream);
                return (*reader)->LoadAsync(static_cast<uint32>(stream->Size));
            }).then([this, reader](uint32 bytesRead)
            {

                Streams::DataReader^ state = (*reader);
                try
                {
                        unsigned int codeUnits = state->ReadUInt32();
                        Platform::String^ strContent = state->ReadString(codeUnits);

                }
                catch (Platform::Exception^ e)
                {
                    // Do nothing.
                }
            });;
        }
        catch (Platform::Exception^ e)
        {

        }
    });
}




//////
//////void MyAsyncOperationCompletedHandlerCamera(IAsyncOperation<Geoposition^>^ operation,Windows::Foundation::AsyncStatus status)
////void MyAsyncOperationCompletedHandlerCamera(IAsyncOperation<PhotoCaptureDevice^> ^operation, Windows::Foundation::AsyncStatus status)
////{
////		auto captureDevice = operation->GetResults();
////
////		// Save the reference to the opened video capture device
////		pPhotoCaptureDevice= captureDevice;
////
////
////		// Retrieve the native ICameraCaptureDeviceNative interface from the managed video capture device
////		ICameraCaptureDeviceNative *iCameraCaptureDeviceNative = NULL; 
////		HRESULT hr = reinterpret_cast<IUnknown*>(captureDevice)->QueryInterface(__uuidof(ICameraCaptureDeviceNative), (void**) &iCameraCaptureDeviceNative);
////
////		// Save the pointer to the native interface
////		pCameraCaptureDeviceNative = iCameraCaptureDeviceNative;
////
////
////		 Windows::Foundation::Size actualResolution = captureDevice->PreviewResolution;
////
////	//Windows::Devices::Geolocation::Geolocator^ geolocator;
////	//geolocator = ref new Geolocator();
////
////		 //Platform::WriteOnlyArray<int,1U>^ inputBuffer= ref new Platform::WriteOnlyArray<int,1U>;
////
////		 //Platform::WriteOnlyArray<int, 1U>^ pixels = ref new  Platform::WriteOnlyArray<int,1U>(12,88);
////		  
////		 Platform::Array<int, 1U>^ pixels;
////
////		 pixels= ref new Platform::Array<int, 1U>(640*480*4);
////		  
////
////
////		 captureDevice->GetPreviewBufferArgb(pixels);
////
////
////		// Retrieve IAudioVideoCaptureDeviceNative native interface from managed projection.
/////***
////		IAudioVideoCaptureDeviceNative *iAudioVideoCaptureDeviceNative = NULL;
////		hr = reinterpret_cast<IUnknown*>(captureDevice)->QueryInterface(__uuidof(IAudioVideoCaptureDeviceNative), (void**) &iAudioVideoCaptureDeviceNative);
////
////		// Save the pointer to the IAudioVideoCaptureDeviceNative native interface
////		pAudioVideoCaptureDeviceNative = iAudioVideoCaptureDeviceNative;
////***/
////
////		// Set sample encoding format to ARGB. See the documentation for further values.
///////		pAudioVideoCaptureDevice->VideoEncodingFormat = CameraCaptureVideoFormat::Argb;
////		////////////////////pAudioVideoCaptureDevice->AudioEncodingFormat = CameraCaptureAudioFormat::
////		// Initialize and set the CameraCaptureSampleSink class as sink for captures samples
///////		MakeAndInitialize<CameraCaptureSampleSink>(&pCameraCaptureSampleSink);
///////		pAudioVideoCaptureDeviceNative->SetVideoSampleSink(pCameraCaptureSampleSink);
////
////		// Start recording (only way to receive samples using the ICameraCaptureSampleSink interface
////		if(conta < 3)
////		{
///////			pAudioVideoCaptureDevice->StartRecordingToSinkAsync();
////			fCapture=TRUE;
////		}
////
////		/*
////	    auto result = operation->GetResults(); 
////        auto x= result->Coordinate->Latitude;
////		auto y= result->Coordinate->Longitude;
////	
////		printf("x=%f\n",x);
////		printf("y=%f\n",y);
////
////		HINSTANCE LibHandle;
////
////		typedef HANDLE   (__stdcall  *FunctionFuncGetStdHandle)(DWORD);
////		LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
////		FunctionFuncGetStdHandle  _GetStdHandle=  (FunctionFuncGetStdHandle)GetProcAddress(LibHandle,"GetStdHandle");
////
////		HANDLE handle = _GetStdHandle(STD_OUTPUT_HANDLE);
////
////		wchar_t msgW[128];
////		char msgA[128];
////		swprintf_s(msgW,L"x,y=(%f,%f)\n", x,y );
////		wcstombs(msgA, msgW, wcslen(msgW)+1);
////		WriteFile(handle, msgA, strlen(msgA)+1, NULL, NULL);
////
////		fstream filestr;
////		char buff[DTTMSZ];
////		filestr.open ("test.txt", fstream::out|fstream::app);
////		filestr << getDtTm (buff) << std::endl;
////		filestr << msgA << std::endl;
////		filestr << std::endl;
////		filestr.close();
////		*/
////}


NativeAudioInterface::Native::NativeCapture::NativeCapture()
{
	// Set the capture dimensions

	fAudioCapture=0;

	Size captureDimensions;
	captureDimensions.Width = 640;
	captureDimensions.Height = 480;		



	//store
				

				//StorageFolder^ localFolder = ApplicationData::Current->LocalFolder;
				//StorageFolder dataFolder; //= NULL;

	/****************
				StorageFolder^  folder = ApplicationData::Current->LocalFolder;


				task<StorageFile^> getFileTask(folder->CreateFileAsync( "test.amr", CreationCollisionOption::ReplaceExisting));


				 // Create a local to allow the DataReader to be passed between lambdas.
				auto writer = std::make_shared<Streams::DataWriter^>(nullptr);

				getFileTask.then([](StorageFile^ file)
				{
					return file->OpenAsync(FileAccessMode::ReadWrite);
				}).then([this, writer](Streams::IRandomAccessStream^ stream)
				{
					Streams::DataWriter^ state = ref new Streams::DataWriter(stream);
					*writer = state;



					//unsigned int codeUnits = state->MeasureString(strContent);
					//state->WriteUInt32(codeUnits);
					//state->WriteString(strContent);

					return state->StoreAsync();
				}).then([writer](uint32 count)
				{
					return (*writer)->FlushAsync();
				}).then([this, writer](bool flushed)
				{
					delete (*writer);
				});

				******************/
/*

				StorageFolder ^localFolder = ApplicationData::Current->LocalFolder;
				concurrency::task<StorageFile^> fileOperation = localFolder->CreateFileAsync(L"data.txt", CreationCollisionOption::ReplaceExisting);


				fileOperation.then([this](StorageFile^ sampleFile)
				{
					::Globalization::DateTimeFormatting::DateTimeFormatter("longtime");
					Platform::String ^str; // other str stuff here
	 
					return FileIO::WriteTextAsync(sampleFile, str);
				}).then([this](concurrency::task<void> previousOperation) {
				try {
						previousOperation.get();
					} 
				catch (Platform::Exception^) 
				{
				}
				});


				concurrency::task<bool> TestFileExists::MainPage::Testme(String^ fileName)
				{
					using namespace Windows::Storage;
					using namespace concurrency;

					StorageFolder^ item =  ApplicationData::Current->LocalFolder; 

					return create_task(item->GetFileAsync(fileName)).then([this](task<StorageFile^> t)
					{
							bool fileExists = true;

							try {
									StorageFile^ file = t.get();
							}
							catch(Platform::Exception^ exp)
							{
									fileExists = false;
							}

							return (fileExists);
					});
				}

*/




	IAsyncOperation<AudioVideoCaptureDevice^> ^openOperation = AudioVideoCaptureDevice::OpenForAudioOnlyAsync();


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

				// Set sample encoding format to ARGB. See the documentation for further values.
				pAudioVideoCaptureDevice->AudioEncodingFormat = CameraCaptureAudioFormat::Amr;
				
				////////////////////pAudioVideoCaptureDevice->AudioEncodingFormat = CameraCaptureAudioFormat::
				// Initialize and set the CameraCaptureSampleSink class as sink for captures samples
				MakeAndInitialize<CameraCaptureSampleSink>(&pCameraCaptureSampleSink);
				pAudioVideoCaptureDeviceNative->SetAudioSampleSink(pCameraCaptureSampleSink);

				//Start recording (only way to receive samples using the ICameraCaptureSampleSink interface
				//fAudio=TRUE;
				//pAudioVideoCaptureDevice->StartRecordingToSinkAsync();
				//////////fAudioCapture=TRUE;

				
			}
		}
	);





	// SOLO VIDEO BYGIO
////////////	// Open the AudioVideoCaptureDevice for video only
////////////	IAsyncOperation<AudioVideoCaptureDevice^> ^openOperation = AudioVideoCaptureDevice::OpenForVideoOnlyAsync(CameraSensorLocation::Back, captureDimensions);
////////////
////////////	//openOperation->Completed =  ref new AsyncOperationCompletedHandler<AudioVideoCaptureDevice^>(MyAsyncOperationCompletedHandler);
////////////
////////////	openOperation->Completed = ref new AsyncOperationCompletedHandler<AudioVideoCaptureDevice^>(
////////////		[this] (IAsyncOperation<AudioVideoCaptureDevice^> ^operation, Windows::Foundation::AsyncStatus status)
////////////		{
////////////			if (status == Windows::Foundation::AsyncStatus::Completed)
////////////			{
////////////				auto captureDevice = operation->GetResults();
////////////
////////////				// Save the reference to the opened video capture device
////////////				pAudioVideoCaptureDevice = captureDevice;
////////////
////////////
////////////				// Retrieve the native ICameraCaptureDeviceNative interface from the managed video capture device
////////////				ICameraCaptureDeviceNative *iCameraCaptureDeviceNative = NULL; 
////////////				HRESULT hr = reinterpret_cast<IUnknown*>(pAudioVideoCaptureDevice)->QueryInterface(__uuidof(ICameraCaptureDeviceNative), (void**) &iCameraCaptureDeviceNative);
////////////
////////////				// Save the pointer to the native interface
////////////				pCameraCaptureDeviceNative = iCameraCaptureDeviceNative;
////////////
////////////				// Initialize the preview dimensions (see the accompanying article at )
////////////				// The aspect ratio of the capture and preview resolution must be equal,
////////////				// 4:3 for capture => 4:3 for preview, and 16:9 for capture => 16:9 for preview.
////////////
/////////////*******				
////////////				Size previewDimensions;
////////////				previewDimensions.Width = 640;
////////////				previewDimensions.Height = 480;					
////////////
////////////				IAsyncAction^ setPreviewResolutionAction = pAudioVideoCaptureDevice->SetPreviewResolutionAsync(previewDimensions);
////////////				setPreviewResolutionAction->Completed = ref new AsyncActionCompletedHandler(
////////////
////////////					[this](IAsyncAction^ action, Windows::Foundation::AsyncStatus status)
////////////					{
////////////						HResult hr = action->ErrorCode;
////////////
////////////						if (status == Windows::Foundation::AsyncStatus::Completed)
////////////						{
////////////							// Create the sink
////////////							MakeAndInitialize<CameraCapturePreviewSink>(&pCameraCapturePreviewSink);
////////////							pCameraCaptureDeviceNative->SetPreviewSink(pCameraCapturePreviewSink);
////////////							
////////////							// Set the preview format
////////////							pCameraCaptureDeviceNative->SetPreviewFormat(DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM);						
////////////						}
////////////					}
////////////
////////////					
////////////				);
////////////**********/
////////////				
////////////			
////////////				// Retrieve IAudioVideoCaptureDeviceNative native interface from managed projection.
////////////				IAudioVideoCaptureDeviceNative *iAudioVideoCaptureDeviceNative = NULL;
////////////				hr = reinterpret_cast<IUnknown*>(pAudioVideoCaptureDevice)->QueryInterface(__uuidof(IAudioVideoCaptureDeviceNative), (void**) &iAudioVideoCaptureDeviceNative);
////////////
////////////				// Save the pointer to the IAudioVideoCaptureDeviceNative native interface
////////////				pAudioVideoCaptureDeviceNative = iAudioVideoCaptureDeviceNative;
////////////
////////////				// Set sample encoding format to ARGB. See the documentation for further values.
////////////				pAudioVideoCaptureDevice->VideoEncodingFormat = CameraCaptureVideoFormat::Argb;
////////////				////////////////////pAudioVideoCaptureDevice->AudioEncodingFormat = CameraCaptureAudioFormat::
////////////				// Initialize and set the CameraCaptureSampleSink class as sink for captures samples
////////////				MakeAndInitialize<CameraCaptureSampleSink>(&pCameraCaptureSampleSink);
////////////				pAudioVideoCaptureDeviceNative->SetVideoSampleSink(pCameraCaptureSampleSink);
////////////
////////////				// Start recording (only way to receive samples using the ICameraCaptureSampleSink interface
////////////				/////////pAudioVideoCaptureDevice->StartRecordingToSinkAsync();
////////////				/////////fCapture=TRUE;
////////////
////////////				
////////////			}
////////////		}
////////////	);
	
	 //m_camera = await PhotoCaptureDevice.OpenAsync(CameraSensorLocation.Back, resolution);
	//IAsyncOperation<PhotoCaptureDevice^> ^openOperation = PhotoCaptureDevice::OpenAsync(CameraSensorLocation::Back, captureDimensions);

	//openOperation->Completed =  ref new AsyncOperationCompletedHandler<PhotoCaptureDevice^>(MyAsyncOperationCompletedHandlerCamera);



}
