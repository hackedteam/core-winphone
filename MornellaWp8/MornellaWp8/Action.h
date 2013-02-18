#ifndef __Action_h__
#define __Action_h__

#include "Transfer.h"
#include "Configuration.h"
#include "SubAction.h"

class Action {
private:
	INT queue;
	Configuration *conf;
	vector<SubAction *> *subActions;

private:
	void assignQueue();

public:
	Action(vector<SubAction *> *s);
	~Action();

	INT execute();
	INT getQueue();
};

#endif