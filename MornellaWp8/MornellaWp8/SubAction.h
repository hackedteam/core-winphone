#ifndef __SubAction_h__
#define __SubAction_h__

#include "Configuration.h"

class SubAction {
private:
	Configuration *conf; // Deleted by the destructor
	BOOL stopAction;

private:
	void clear();

public:
	SubAction(Configuration *c);
	~SubAction();

	const wstring& getActionType();
	INT run();
	BOOL getStop();
};

#endif