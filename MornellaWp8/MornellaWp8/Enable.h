#ifndef __Enable_h__
#define __Enable_h__

#include "Common.h"
#include "EventsManager.h"
#include "Configuration.h"

class Enable {
private:
	EventsManager *events;
	Configuration *conf;
	BOOL stopAction;

public:
	Enable(Configuration *c);
	INT run();
	BOOL getStop();
};

#endif