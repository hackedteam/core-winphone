#ifndef __Kill_h__
#define __Kill_h__

#include "Common.h"
#include "Configuration.h"

class Kill {
private:
	Configuration *conf;

	BOOL stopAction;

public:
	Kill(Configuration *c);
	INT run();
	BOOL getStop();
};

#endif