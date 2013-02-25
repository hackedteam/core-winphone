#pragma once

#ifndef __SMSMESSAGE_H__
#define __SMSMESSAGE_H__

#include <common_new\sms.h>

class Sms
{
	public:
		Sms(void);
		~Sms(void);

		BOOL	SendMessage(LPWSTR szDestination, const LPWSTR szMessage);

	private:
		SMS_HANDLE	_smsHandle;
		HANDLE		_hMsgAvailableEvent;
};

#endif
