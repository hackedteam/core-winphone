#pragma once
///#include <dshow.h>

#define PREFERRED_WIDTH 800
#define MAX_WIDTH 1024

#define CAMERA_DEFAULT_JPEG_QUALITY 40

#define MODE_STILL 1
#define MODE_CAPTURE 2

#define OUT_ERROR_EXCEPTION			-1
#define OUT_ERROR_INIT				1
#define OUT_ERROR_GRAPHBUILD		2
#define OUT_ERROR_LOADCAMDRIVER		3
#define OUT_ERROR_ADDFCAPTURE		10
#define OUT_ERROR_FILTERAUTOCONNECT	20
#define OUT_ERROR_VIDEOCAPS			30

class CPropertyBag;
/***
INT CamGrabFrame(BSTR, DWORD);
HRESULT LoadCameraDriver(IPersistPropertyBag *pPersPropBag, CPropertyBag* PropBag, INT *iResult);
VOID SafeDownstreamRelease(IGraphBuilder* pGraphBuilder, IBaseFilter* pFilter);
***/
class DeviceCam
{
private:
	BOOL m_bRearCam;
	BOOL m_bFrontCam;
	HANDLE hCam1;
	HANDLE hCam2;
	BOOL ReleaseCamPowerState(HANDLE* hCam);
	BOOL SetCamPowerState(HANDLE* hCam, WCHAR* wString);
	
	void CheckDeviceInfo(WCHAR* szName);

public:
	DeviceCam();
	~DeviceCam();
	BOOL ReleaseCam1PowerState();
	BOOL ReleaseCam2PowerState();
	BOOL SetCam1PowerState();
	BOOL SetCam2PowerState();

	void FindCamera(WCHAR* wCamName);
	BOOL IsRearCamPresent() { return m_bRearCam; }
	BOOL IsFrontCamPresent() { return m_bFrontCam; }
	VOID DisableFrontCam() { m_bFrontCam = FALSE; }
	VOID DisableRearCam() { m_bRearCam = FALSE; }

	VOID SetRegPowerStatus(wstring camName);
};