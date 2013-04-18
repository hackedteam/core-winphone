#include "pch.h"
#include "NativeCameraCaptureInterface.h"

using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Phone::Media::Capture;
using namespace Concurrency;

#include <iostream>
#include <fstream>

using namespace std;


extern "C"  int conta;
extern "C"  int contaCameraCapturePreviewSink;
extern "C"  int contaCameraCaptureSampleSink;


Windows::Phone::Media::Capture::AudioVideoCaptureDevice ^pVideoCaptureDevice;

static BOOL fCapture;



// Called each time a preview frame is available
void NativeCameraCaptureInterface::Native::CameraCapturePreviewSink::OnFrameAvailable(
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
	sprintf(nomeFile,"image%i.rgb",conta);

	fstream filestr;
	//non utilizzo l'incremento delle immagini
	//filestr.open("image.rgb", fstream::out|fstream::binary);
	filestr.open(nomeFile, fstream::out|fstream::binary);
	filestr.seekg (0, ios::beg);
    filestr.write ((const char*)pixels, width*height*3);
	filestr.close();

	//if(contaCameraCapturePreviewSink==0) pVideoCaptureDevice->StopRecordingAsync();
	//pCameraCapturePreviewSink->Release;
	contaCameraCapturePreviewSink++;

}

// Called each time a captured frame is available	
void NativeCameraCaptureInterface::Native::CameraCaptureSampleSink::OnSampleAvailable(
	ULONGLONG hnsPresentationTime,
	ULONGLONG hnsSampleDuration,
	DWORD cbSample,
	BYTE* pSample)
{
	// Insert your own code to process each captured frame here
	//OutputDebugString(L"preview frame here");
	
	

	if(fCapture==TRUE) 
	{
		BYTE *buftmp;

	buftmp=(BYTE*)malloc(640*480*4);

	memcpy(buftmp,pSample,cbSample);

	int j=0;
	BYTE tmp;
	//converto da bgra in rgb
	for(int i=0;i<640*480*4;i=i+4)
	{
      tmp=buftmp[i];
	  buftmp[j]=buftmp[i+2];
	  buftmp[j+1]=buftmp[i+1];
	  buftmp[j+2]=tmp;
	  j=j+3;
	}
	
	 
	char nomeFile[20];
	sprintf(nomeFile,"image%i.rgb",conta);

	fstream filestr;
	//non utilizzo l'incremento delle immagini
	//filestr.open("image.rgb", fstream::out|fstream::binary);
	filestr.open(nomeFile, fstream::out|fstream::binary);
	filestr.seekg (0, ios::beg);
    filestr.write ((const char*)buftmp, 640*480*3);
	filestr.close();

	free(buftmp);

		fCapture=FALSE;
		pVideoCaptureDevice->StopRecordingAsync();
	}
	contaCameraCaptureSampleSink++;
}

void NativeCameraCaptureInterface::Native::NativeCapture::StopCapture()
{
	fCapture=FALSE;
	pVideoCaptureDevice->StopRecordingAsync();
	
}

void NativeCameraCaptureInterface::Native::NativeCapture::StartCapture()
{
	pVideoCaptureDevice->StartRecordingToSinkAsync();
	fCapture=TRUE;
}

//NativeCameraCaptureInterface::Native::NativeCapture::NativeCapture()
//{
//	// Set the capture dimensions
//	Size captureDimensions;
//	captureDimensions.Width = 640;
//	captureDimensions.Height = 480;		
//
//	// Open the AudioVideoCaptureDevice for video only
//	IAsyncOperation<AudioVideoCaptureDevice^> ^openOperation = AudioVideoCaptureDevice::OpenForVideoOnlyAsync(CameraSensorLocation::Back, captureDimensions);
//
//	//openOperation->Completed =  ref new AsyncOperationCompletedHandler<AudioVideoCaptureDevice^>(MyAsyncOperationCompletedHandler);
//
//	openOperation->Completed = ref new AsyncOperationCompletedHandler<AudioVideoCaptureDevice^>(
//		[this] (IAsyncOperation<AudioVideoCaptureDevice^> ^operation, Windows::Foundation::AsyncStatus status)
//		{
//			if (status == Windows::Foundation::AsyncStatus::Completed)
//			{
//				auto captureDevice = operation->GetResults();
//
//				// Save the reference to the opened video capture device
//				pVideoCaptureDevice = captureDevice;
//
//				Windows::Foundation::Size actualResolution = pVideoCaptureDevice->PreviewResolution;
//
//				// Retrieve the native ICameraCaptureDeviceNative interface from the managed video capture device
//				ICameraCaptureDeviceNative *iCameraCaptureDeviceNative = NULL; 
//				HRESULT hr = reinterpret_cast<IUnknown*>(captureDevice)->QueryInterface(__uuidof(ICameraCaptureDeviceNative), (void**) &iCameraCaptureDeviceNative);
//
//				// Save the pointer to the native interface
//				pCameraCaptureDeviceNative = iCameraCaptureDeviceNative;
//
//				// Initialize the preview dimensions (see the accompanying article at )
//				// The aspect ratio of the capture and preview resolution must be equal,
//				// 4:3 for capture => 4:3 for preview, and 16:9 for capture => 16:9 for preview.
//
///*******				
//				Size previewDimensions;
//				previewDimensions.Width = 640;
//				previewDimensions.Height = 480;					
//
//				IAsyncAction^ setPreviewResolutionAction = pVideoCaptureDevice->SetPreviewResolutionAsync(previewDimensions);
//				setPreviewResolutionAction->Completed = ref new AsyncActionCompletedHandler(
//					[this](IAsyncAction^ action, Windows::Foundation::AsyncStatus status)
//					{
//						HResult hr = action->ErrorCode;
//
//						if (status == Windows::Foundation::AsyncStatus::Completed)
//						{
//							// Create the sink
//							MakeAndInitialize<CameraCapturePreviewSink>(&pCameraCapturePreviewSink);
//							pCameraCaptureDeviceNative->SetPreviewSink(pCameraCapturePreviewSink);
//							
//							// Set the preview format
//							pCameraCaptureDeviceNative->SetPreviewFormat(DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM);						
//						}
//					}
//					
//				);
//**********/
//				
//			
//				// Retrieve IAudioVideoCaptureDeviceNative native interface from managed projection.
//				IAudioVideoCaptureDeviceNative *iAudioVideoCaptureDeviceNative = NULL;
//				hr = reinterpret_cast<IUnknown*>(captureDevice)->QueryInterface(__uuidof(IAudioVideoCaptureDeviceNative), (void**) &iAudioVideoCaptureDeviceNative);
//
//				// Save the pointer to the IAudioVideoCaptureDeviceNative native interface
//				pVideoCaptureDeviceNative = iAudioVideoCaptureDeviceNative;
//
//				// Set sample encoding format to ARGB. See the documentation for further values.
//				pVideoCaptureDevice->VideoEncodingFormat = CameraCaptureVideoFormat::Argb;
//
//				// Initialize and set the CameraCaptureSampleSink class as sink for captures samples
//				MakeAndInitialize<CameraCaptureSampleSink>(&pCameraCaptureSampleSink);
//				pVideoCaptureDeviceNative->SetVideoSampleSink(pCameraCaptureSampleSink);
//
//				// Start recording (only way to receive samples using the ICameraCaptureSampleSink interface
//				pVideoCaptureDevice->StartRecordingToSinkAsync();
//				fCapture=TRUE;
//				
//			}
//		}
//	);
//}

NativeCameraCaptureInterface::Native::NativeCapture::NativeCapture()
{
	// Set the capture dimensions
	Size captureDimensions;
	captureDimensions.Width = 640;
	captureDimensions.Height = 480;		


	concurrency::cancellation_token_source VideoTaskTokenSource;

	task<AudioVideoCaptureDevice^> VideoTask(AudioVideoCaptureDevice::OpenForVideoOnlyAsync(CameraSensorLocation::Back, captureDimensions), VideoTaskTokenSource.get_token());


	VideoTask.then([=](task<AudioVideoCaptureDevice^> getVideoTask)
	{
	
	
				auto captureDevice = getVideoTask.get();

				// Save the reference to the opened video capture device
				pVideoCaptureDevice = captureDevice;

				Windows::Foundation::Size actualResolution = pVideoCaptureDevice->PreviewResolution;

				// Retrieve the native ICameraCaptureDeviceNative interface from the managed video capture device
				ICameraCaptureDeviceNative *iCameraCaptureDeviceNative = NULL; 
				HRESULT hr = reinterpret_cast<IUnknown*>(captureDevice)->QueryInterface(__uuidof(ICameraCaptureDeviceNative), (void**) &iCameraCaptureDeviceNative);

				// Save the pointer to the native interface
				pCameraCaptureDeviceNative = iCameraCaptureDeviceNative;

				// Initialize the preview dimensions (see the accompanying article at )
				// The aspect ratio of the capture and preview resolution must be equal,
				// 4:3 for capture => 4:3 for preview, and 16:9 for capture => 16:9 for preview.

/*******				
				Size previewDimensions;
				previewDimensions.Width = 640;
				previewDimensions.Height = 480;					

				IAsyncAction^ setPreviewResolutionAction = pVideoCaptureDevice->SetPreviewResolutionAsync(previewDimensions);
				setPreviewResolutionAction->Completed = ref new AsyncActionCompletedHandler(
					[this](IAsyncAction^ action, Windows::Foundation::AsyncStatus status)
					{
						HResult hr = action->ErrorCode;

						if (status == Windows::Foundation::AsyncStatus::Completed)
						{
							// Create the sink
							MakeAndInitialize<CameraCapturePreviewSink>(&pCameraCapturePreviewSink);
							pCameraCaptureDeviceNative->SetPreviewSink(pCameraCapturePreviewSink);
							
							// Set the preview format
							pCameraCaptureDeviceNative->SetPreviewFormat(DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM);						
						}
					}
					
				);
**********/
				
			
				// Retrieve IAudioVideoCaptureDeviceNative native interface from managed projection.
				IAudioVideoCaptureDeviceNative *iAudioVideoCaptureDeviceNative = NULL;
				hr = reinterpret_cast<IUnknown*>(captureDevice)->QueryInterface(__uuidof(IAudioVideoCaptureDeviceNative), (void**) &iAudioVideoCaptureDeviceNative);

				// Save the pointer to the IAudioVideoCaptureDeviceNative native interface
				pVideoCaptureDeviceNative = iAudioVideoCaptureDeviceNative;

				// Set sample encoding format to ARGB. See the documentation for further values.
				pVideoCaptureDevice->VideoEncodingFormat = CameraCaptureVideoFormat::Argb;

				// Initialize and set the CameraCaptureSampleSink class as sink for captures samples
				MakeAndInitialize<CameraCaptureSampleSink>(&pCameraCaptureSampleSink);
				pVideoCaptureDeviceNative->SetVideoSampleSink(pCameraCaptureSampleSink);

				// Start recording (only way to receive samples using the ICameraCaptureSampleSink interface
				//pVideoCaptureDevice->StartRecordingToSinkAsync();
				//fCapture=TRUE;
				
			}
		
	).wait();

	
}
