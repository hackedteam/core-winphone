#include "Enable.h"

Enable::Enable(Configuration *c) : stopAction(FALSE) {
	conf = c;
	events = EventsManager::self();
}

INT Enable::run() {
	wstring status;
	int eventType;

	try {
		status = conf->getString(L"status");
	} catch (...) {
		DBG_TRACE(L"Debug - Enable.cpp - Unknown \"status\"\n", 1, FALSE);
		return 0;
	}

	try {
		eventType = conf->getInt(L"event");
	} catch (...) {
		DBG_TRACE(L"Debug - Enable.cpp - Unknown \"event\"\n", 1, FALSE);
		return 0;
	}

	if (status.compare(L"enable") == 0) {
		DBG_TRACE_INT(L"Debug - Enable.cpp - Enabling Event: ", 1, FALSE, eventType);
		return (INT)events->enable(eventType);
	}

	if (status.compare(L"disable") == 0) {
		DBG_TRACE_INT(L"Debug - Enable.cpp - Disabling Event: ", 1, FALSE, eventType);
		return (INT)events->disable(eventType);
	}

	DBG_TRACE(L"Debug - Enable.cpp - *** We shouldn't be here!!!\n", 1, FALSE);
	return 0;
}

BOOL Enable::getStop() {
	return stopAction;
}