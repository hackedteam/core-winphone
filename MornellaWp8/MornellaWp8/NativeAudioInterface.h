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
			void StopCapture();		
			property static bool fStartPlay;
			property static bool fAudioCapture;
			property static bool fAudioCaptureForceStop;
			property static int pos;
			property static int nCamp;
			
	
		internal:
			int  StartCapture(HANDLE);

		private:
				
			/////IAsyncOperation<AudioVideoCaptureDevice^> ^openOperation;

			ICameraCaptureDeviceNative* pCameraCaptureDeviceNative;
			IAudioVideoCaptureDeviceNative* pAudioVideoCaptureDeviceNative;
			CameraCaptureSampleSink* pCameraCaptureSampleSink;
			
			
		};

	}
	
}
