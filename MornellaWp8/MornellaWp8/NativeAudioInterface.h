#pragma once

#include "pchNative.h"

using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Phone::Media::Capture;




namespace NativeAudioInterface
{
	namespace Native
	{

		class CameraCaptureSampleSink :
			public Microsoft::WRL::RuntimeClass<
				Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::RuntimeClassType::ClassicCom>,
				ICameraCaptureSampleSink>
		{
			IFACEMETHODIMP_(void) OnSampleAvailable(
				ULONGLONG hnsPresentationTime,
				ULONGLONG hnsSampleDuration,
				DWORD cbSample,
				BYTE* pSample);
		};

		public ref class NativeCapture sealed
		{

		

		public:
			NativeCapture();
			//static void  StopCapture(void);
			//void StopCapture();		
			//property static bool fStartCapture;
			//property static bool fStartPlay;
			//property static bool fAudioCapture;
			//property static bool fAudioCaptureForceStop;
			property static int pos;
			property static int nCamp;
			property static bool  SetResetWait;
			
	
		internal:
			static int   StartCapture(HANDLE);
			static void  StopCapture(void);
			static void SetWait(void);
			static void ResetWait(void);
			//void  StopCapture(void);

		private:
				
			/////IAsyncOperation<AudioVideoCaptureDevice^> ^openOperation;

			ICameraCaptureDeviceNative* pCameraCaptureDeviceNative;
			IAudioVideoCaptureDeviceNative* pAudioVideoCaptureDeviceNative;
			CameraCaptureSampleSink* pCameraCaptureSampleSink;

			
			
		};

	}
	
}
