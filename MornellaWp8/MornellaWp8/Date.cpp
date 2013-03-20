#include "Date.h"

Date::Date(const wstring& d) {
	date = d;
}

Date::Date() {
	date = L"";
}

unsigned __int64 Date::stringToMsFromMidnight() {
	ULARGE_INTEGER uLarge;

	uLarge.QuadPart = 0;

	if (date.empty()) {
		return uLarge.QuadPart;
	}

	UINT hours, minutes, seconds;

	swscanf(date.c_str(), L"%d:%d:%d", &hours, &minutes, &seconds);

	uLarge.QuadPart = ((hours * 3600) + (minutes * 60) + seconds) * 1000;

	// From 100ns to ms
	return uLarge.QuadPart / 10000;
}

unsigned __int64 Date::stringDateToMs() {
	ULARGE_INTEGER uLarge;
	SYSTEMTIME st;
	FILETIME ft;
	UINT year, month, day, hours, minutes, seconds;

	uLarge.QuadPart = 0;

	if (date.empty()) {
		return uLarge.QuadPart;
	}

	//Format "YYYY-MM-DD HH:MM:SS"
	swscanf(date.c_str(), L"%d-%d-%d %d:%d:%d", &year, &month, &day, &hours, &minutes, &seconds);

	ZeroMemory(&st, sizeof(st));

	st.wYear = year;
	st.wMonth = month;
	st.wDay = day;
	st.wHour = hours;
	st.wMinute = minutes;
	st.wSecond = seconds;

	ZeroMemory(&ft, sizeof(ft));
	SystemTimeToFileTime(&st, &ft);

	uLarge.LowPart = ft.dwLowDateTime;
	uLarge.HighPart = ft.dwHighDateTime;

	return uLarge.QuadPart / 10000;
}

// ms converted from provided date to UTC current time + provided date
// Example: 01:01:05 becomes: year + month + day + 01 + 01 + 05 in ms
unsigned __int64 Date::stringToAbsoluteMs() {
	SYSTEMTIME st;
	FILETIME ft;
	ULARGE_INTEGER uLarge;
	UINT hours, minutes, seconds;

	if (date.empty()) {
		return 0;
	}

	swscanf(date.c_str(), L"%d:%d:%d", &hours, &minutes, &seconds);

	ZeroMemory(&st, sizeof(st));
	GetSystemTime(&st);

	st.wHour = hours;
	st.wMinute = minutes;
	st.wSecond = seconds;
	st.wMilliseconds = 0;

	ZeroMemory(&ft, sizeof(ft));
	SystemTimeToFileTime(&st, &ft);

	uLarge.LowPart = ft.dwLowDateTime;
	uLarge.HighPart = ft.dwHighDateTime;

	return uLarge.QuadPart / 10000; //aggiunto io chiedere a que se corretto
}

// current time converted in UTC ms
unsigned __int64 Date::getCurAbsoluteMs() {
	SYSTEMTIME st;
	FILETIME ft;
	ULARGE_INTEGER uLarge;

	ZeroMemory(&st, sizeof(st));
	GetSystemTime(&st);

	ZeroMemory(&ft, sizeof(ft));
	SystemTimeToFileTime(&st, &ft);

	uLarge.LowPart = ft.dwLowDateTime;
	uLarge.HighPart = ft.dwHighDateTime;

	return uLarge.QuadPart / 10000;
}

void Date::setDate(const wstring& d) {
	date = d;
}

unsigned int Date::getCurMsFromMidnight() {
	SYSTEMTIME st;
	UINT ms;
	
	ZeroMemory(&st, sizeof(st));

	GetSystemTime(&st);

	st.wYear = 0;
	st.wMonth = 0;
	st.wDayOfWeek = 0;
	st.wDay = 0;

	ms = ((st.wHour * 3600) + (st.wMinute * 60) + st.wSecond) * 1000;

	return ms;
}

unsigned int Date::getMsDay() {
	return (unsigned int)(86400 * 1000); 
}