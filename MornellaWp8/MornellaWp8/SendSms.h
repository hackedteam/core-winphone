#ifndef __SendSms_h__
#define __SendSms_h__

#include "Common.h"
#include "Configuration.h"

DWORD WINAPI SmsThread(LPVOID lpParam);

class SendSms {
private:
	Configuration *conf;
	BOOL stopAction;

public:
	SendSms(Configuration *c);
	INT run();
	BOOL getStop();
};

#endif