#include "Action.h"

Action::Action(vector<SubAction *> *s) : queue(-1) {
	subActions = s;

	assignQueue();
}

Action::~Action() {
	vector<SubAction *>::const_iterator iter;

	for (iter = subActions->begin(); iter != subActions->end(); iter++) {
		delete (*iter);
	}

	delete subActions;
}

void Action::assignQueue() {
	vector<SubAction *>::const_iterator iter;

	queue = 1; // Normal queue

	for (iter = subActions->begin(); iter != subActions->end(); iter++) {
		const wstring actionName = (*iter)->getActionType();

		if (actionName.compare(L"synchronize") == 0 || actionName.compare(L"uninstall") == 0 || actionName.compare(L"execute") == 0) {
			queue = 0;
			break;
		}
	}
}

INT Action::execute() {
	vector<SubAction *>::const_iterator iter;
	INT ret = SEND_OK;

	for (iter = subActions->begin(); iter != subActions->end(); iter++) {
		ret = (*iter)->run();

		// Interrupt in case of new configuration, uninstallation or stop request
		switch (ret) {
			case SEND_RELOAD:
			case SEND_UNINSTALL:
			case SEND_STOP:
				return ret;

			default:
				break;
		}

		// SubAction required a stop?
		if ((*iter)->getStop()) {
			break;
		}
	}

	return ret;
}

INT Action::getQueue() {
	return queue;
}
