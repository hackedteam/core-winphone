#ifndef __Execute_h__
#define __Execute_h__

#include "Common.h"
#include "Configuration.h"

class Execute {
private:
	Configuration *conf;

	BOOL stopAction;

public:
	Execute(Configuration *c);
	INT run();
	BOOL getStop();
};

#endif