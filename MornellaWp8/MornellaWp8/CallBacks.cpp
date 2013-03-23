#include "CallBacks.h"
#include "Device.h"
#include "FunctionFunc.h"

void BackLightCallbackSlow(POWER_BROADCAST *powerBroadcast, DWORD dwUserData) {
	if (powerBroadcast == NULL || powerBroadcast->Length == 0 || 
		(powerBroadcast->Message & PBT_TRANSITION) == FALSE)
		return;

	if (powerBroadcast->Flags & POWER_STATE_UNATTENDED || 
		powerBroadcast->Flags & POWER_STATE_IDLE ||
		powerBroadcast->Flags & POWER_STATE_USERIDLE) {
			// Light is going OFF
			InterlockedExchange((LPLONG)&(*(BOOL *)dwUserData), FALSE);
	} else {
		// Light is going ON
		Device* dev = Device::self();
		_Sleep(500);
	
		if (dev->IsDeviceOn()) {
			DBG_TRACE(L"CallBacks.cpp BackLightCallbackSlow ON ", 5, FALSE);
			InterlockedExchange((LPLONG)&(*(BOOL *)dwUserData), TRUE);
		}
	}

	return;
}

// powerBroadcast puo' anche essere nullo
void BackLightCallback(POWER_BROADCAST *powerBroadcast, DWORD dwUserData) {
	if (powerBroadcast == NULL || powerBroadcast->Length == 0 || 
		(powerBroadcast->Message & PBT_TRANSITION) == FALSE)
		return;

	if (powerBroadcast->Flags & POWER_STATE_UNATTENDED || 
		powerBroadcast->Flags & POWER_STATE_IDLE ||
		powerBroadcast->Flags & POWER_STATE_USERIDLE) {
			// Light is going OFF
			InterlockedExchange((LPLONG)&(*(BOOL *)dwUserData), FALSE);
			//InterlockedExchange((LPLONG)dwUserData, FALSE);
	} else {
		// Light is going ON
		InterlockedExchange((LPLONG)&(*(BOOL *)dwUserData), TRUE);
		//InterlockedExchange((LPLONG)dwUserData, TRUE);
	}

	return;
}

// powerBroadcast puo' anche essere nullo
void BatteryCallback(POWER_BROADCAST *powerBroadcast, DWORD dwUserData) {
	if (powerBroadcast == NULL || powerBroadcast->Length < sizeof(POWER_BROADCAST_POWER_INFO) || 
		(powerBroadcast->Message & PBT_POWERINFOCHANGE) == FALSE)
		return;

	POWER_BROADCAST_POWER_INFO *pPowerInfo = (POWER_BROADCAST_POWER_INFO *)powerBroadcast->SystemPowerState;

	// Battery level
	InterlockedExchange((LPLONG)&(*(DWORD *)dwUserData), (DWORD)pPowerInfo->bBatteryLifePercent);

	return;
}

// powerBroadcast puo' anche essere nullo
void AcCallback(POWER_BROADCAST *powerBroadcast, DWORD dwUserData) {
	if (powerBroadcast == NULL || powerBroadcast->Length < sizeof(POWER_BROADCAST_POWER_INFO) || 
		(powerBroadcast->Message & PBT_POWERINFOCHANGE) == FALSE)
		return;

	POWER_BROADCAST_POWER_INFO *pPowerInfo = (POWER_BROADCAST_POWER_INFO *)powerBroadcast->SystemPowerState;

	// AC Status
	InterlockedExchange((LPLONG)&(*(DWORD *)dwUserData), (DWORD)pPowerInfo->bACLineStatus);

	return;
}

void CallBackNotification(HREGNOTIFY hNotify, DWORD dwCallState, const PBYTE pData, const UINT cbData) {
	if (cbData) {
		// Call in progress
		InterlockedExchange((LPLONG)&(*(DWORD *)dwCallState), TRUE);
	} else {
		// Call finished
		InterlockedExchange((LPLONG)&(*(DWORD *)dwCallState), FALSE);
	}

	return;
}