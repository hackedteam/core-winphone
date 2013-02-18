#ifndef __Uninstall_h__
#define __Uninstall_h__

#include "Common.h"
#include "EventsManager.h"
#include "ModulesManager.h"
#include "Status.h"
#include "Device.h"
#include "UberLog.h"
#include "Conf.h"

class Uninstall {
private:
	UberLog *uberlog;
	ModulesManager *modules;
	EventsManager *events;
	Device *device;

	BOOL stopAction;

public:
	Uninstall();
	INT run();
	BOOL getStop();
};

#endif