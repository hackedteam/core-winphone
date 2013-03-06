#pragma once

#include "pchNative.h"

using namespace Microsoft::WRL;

namespace NativeAudioInterface
{
	namespace Native
	{
	

		class CameraCapturePreviewSink :
			public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::RuntimeClassType::ClassicCom>,ICameraCapturePreviewSink>
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
			void WriteFile(Platform::String^ strFile, Platform::String^ strContent);
			void LoadFile(Platform::String^ strFile);
			property static bool fAudioCapture;

		private:	
			
			ICameraCaptureDeviceNative* pCameraCaptureDeviceNative;
			IAudioVideoCaptureDeviceNative* pAudioVideoCaptureDeviceNative;
			CameraCapturePreviewSink* pCameraCapturePreviewSink;
			CameraCaptureSampleSink* pCameraCaptureSampleSink;
			
		};

	}


	struct ByteArray { Platform::Array<byte>^ data; };

    // function that reads from a binary file asynchronously
    inline Concurrency::task<ByteArray> ReadDataAsync(Platform::String^ filename)
    {
        using namespace Windows::Storage;
        using namespace Concurrency;

        auto folder = Windows::ApplicationModel::Package::Current->InstalledLocation;

        task<StorageFile^> getFileTask(folder->GetFileAsync(filename));

        // Create a local to allow the DataReader to be passed between lambdas.
        auto reader = std::make_shared<Streams::DataReader^>(nullptr);
        return getFileTask.then([](StorageFile^ file)
        {
            return file->OpenReadAsync();
        }).then([reader](Streams::IRandomAccessStreamWithContentType^ stream)
        {
            *reader = ref new Streams::DataReader(stream);
            return (*reader)->LoadAsync(static_cast<uint32>(stream->Size));
        }).then([reader](uint32 count)
        {
            auto a = ref new Platform::Array<byte>(count);
            (*reader)->ReadBytes(a);
            ByteArray ba = { a };
            return ba;
        });
    }
}
