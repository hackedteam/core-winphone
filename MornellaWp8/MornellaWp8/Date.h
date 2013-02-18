#ifndef __Date_h__
#define __Date_h__

#include "Common.h"
#include <wchar.h>

class Date {
private:
	wstring date;

public:
	Date(const wstring& d);
	Date();

	unsigned __int64 stringToMsFromMidnight();
	unsigned __int64 stringToAbsoluteMs();
	unsigned int getCurMsFromMidnight();
	unsigned __int64 getCurAbsoluteMs();
	unsigned __int64 stringDateToMs(); // Format: "YYYY-MM-DD HH:MM:SS"
	unsigned int getMsDay(); // ms in one day
	void setDate(const wstring& d);
};

#endif