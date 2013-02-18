#ifndef __LogInformation_h__
#define __LogInformation_h__

#include "Common.h"
#include "Configuration.h"

class LogInformation {
private:
	Configuration *conf;
	BOOL stopAction;

public:
	LogInformation(Configuration *c);
	INT run();
	BOOL getStop();
};

#endif