#include "LogInformation.h"
#include "Log.h"

LogInformation::LogInformation(Configuration *c) : stopAction(FALSE) {
	conf = c;
}

INT LogInformation::run() {
	wstring text;
	Log logInfo;

	try {
		text = conf->getString(L"text");
	} catch (...) {
		return 0;
	}

	if (logInfo.WriteLogInfo(text) == FALSE) {
		DBG_TRACE(L"Debug - LogInformation.cpp - WriteLogInfo failed\n", 1, FALSE);
	}

	return 1;
}

BOOL LogInformation::getStop() {
	return stopAction;
}