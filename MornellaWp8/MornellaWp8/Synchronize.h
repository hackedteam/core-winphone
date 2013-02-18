#ifndef __Synchronize_h__
#define __Synchronize_h__

#include "Common.h"
#include "Configuration.h"
#include "Transfer.h"
#include "Status.h"
#include "Device.h"

class Synchronize : public Transfer {
private:
	Configuration *conf;
	Status *status;
	ModulesManager *modules;
	Device *device;

	BOOL stopAction;

private:
	INT syncWiFi();
	INT syncApn();
	INT syncGprs();

public:
	Synchronize(Configuration *c);
	INT run();
	BOOL getStop();
};

#endif