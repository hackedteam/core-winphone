#ifndef __Start_h__
#define __Start_h__

#include "Common.h"
#include "ModulesManager.h"
#include "Configuration.h"

class Start {
private:
	ModulesManager *modules;
	Configuration *conf;
	BOOL stopAction;

public:
	Start(Configuration *c);
	INT run();
	BOOL getStop();
};

#endif