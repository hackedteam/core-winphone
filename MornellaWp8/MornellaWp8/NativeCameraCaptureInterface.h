#pragma once

#include "pch.h"

namespace NativeCameraCaptureInterface
{
	namespace Native
	{

		class CameraCapturePreviewSink :
			public Microsoft::WRL::RuntimeClass<
				Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::RuntimeClassType::ClassicCom>,
				ICameraCapturePreviewSink>
		{
			IFACEMETHODIMP_(void) OnFrameAvailable(
				DXGI_FORMAT format,
				UINT width,
				UINT height,
				BYTE* pixels);
		};

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
			void StartCapture();
			void StopCapture();

		private:
			
			ICameraCaptureDeviceNative* pCameraCaptureDeviceNative;
			IAudioVideoCaptureDeviceNative* pVideoCaptureDeviceNative;
			CameraCapturePreviewSink* pCameraCapturePreviewSink;
			CameraCaptureSampleSink* pCameraCaptureSampleSink;
		};

	}

}
