#include "common_new.h"
#include <Windows.h>
///#include <winioctl.h>
///#include <NtDDNdis.h>
///#include <nuiouser.h>
#include "Common.h"
///#include <eapol.h>
///#include <wzcsapi.h>
#include <stdio.h>

#if _MSC_VER > 1000
#pragma once
#endif

struct BSSIDInfo
{
	BYTE BSSID[6];	//mac
	WCHAR SSID[32];
	int RSSI;
	int Channel;
	int Infastructure;
	int Auth;
};

class CWifi
{
public:
	CWifi();
	~CWifi();

	BOOL IsWiFiPresent();
	BOOL RefreshBSSIDs(wstring &pAdapter);
	BOOL GetBSSIDs(wstring &pAdapter, struct BSSIDInfo *pDest, DWORD dwBufSizeBytes, DWORD &dwReturnedItems);	
	BOOL SetAdHocConnection(wstring &pAdapter, LPSTR pSsid);
	BOOL SetConnectionMode(wstring &pAdapter, NDIS_802_11_NETWORK_INFRASTRUCTURE iMode, NDIS_802_11_AUTHENTICATION_MODE aMode);
	BOOL SetSsid(wstring &pAdapter, LPCSTR pSsid);
	BOOL GetBSSID(wstring &pAdapter, LPWSTR pSsid, struct BSSIDInfo *pDest);
	BOOL SetEncryptionKey(wstring &pAdapter, BYTE* pKey, UINT uKeyLen);
	BOOL WZCCheckAssociation(wstring &pAdapterName);
	BOOL WZCConfigureNetwork(wstring &pAdapter, struct BSSIDInfo *pBssid, BYTE* pKey, UINT uKeyLen);
	BOOL WZCConnectToPreferred(wstring &pAdapterName); // Nome senza GUID
	BOOL WZCConnectToOpen(wstring &pAdapterName); // Nome senza GUID
	BOOL GetWiFiAdapterName(wstring &pDest); // Torna il nome della scheda WiFi senza GUID
	BOOL Disassociate(wstring &pAdapter);

private:
	BOOL WZCGetAdapterName(wstring &pDest); // Torna il nome in pDest senza GUID
	BOOL GetAdapters(wstring &pDest); // Torna la lista degli adattatori WiFi separati da virgola
	BOOL OpenDriver();
	void CloseDriver();

	CRITICAL_SECTION m_Lock;
	HANDLE m_hNDUIO;
};

