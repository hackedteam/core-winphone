#include "Start.h"

Start::Start(Configuration *c) : stopAction(FALSE) {
	conf = c;
	modules = ModulesManager::self();
}

INT Start::run() {
	wstring module, status;
	
	try {
		status = conf->getString(L"status");
	} catch (...) {
		DBG_TRACE(L"Debug - Start.cpp - Unknown \"status\"\n", 1, FALSE);
		return 0;
	}

	try {
		module = conf->getString(L"module");
	} catch (...) {
		DBG_TRACE(L"Debug - Start.cpp - Unknown \"module\"\n", 1, FALSE);
		return 0;
	}

	if (status.compare(L"start") == 0) {
		DBG_TRACE_3(L"Debug - Start.cpp - Starting Module: ", module.c_str(), L"\n", 1, FALSE);
		return (INT)modules->start(module);
	}

	if (status.compare(L"stop") == 0) {
		DBG_TRACE_3(L"Debug - Start.cpp - Stopping Module: ", module.c_str(), L"\n", 1, FALSE);
		return (INT)modules->stop(module);
	}

	DBG_TRACE(L"Debug - Start.cpp - *** We shouldn't be here!!!\n", 1, FALSE);
	return 0;
}

BOOL Start::getStop() {
	return stopAction;
}