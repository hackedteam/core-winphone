#pragma once

#include <list>

struct _CONNMGR_CONNECTION_DETAILED_STATUS;

class GPRSConnection
{
private:
	std::list<_CONNMGR_CONNECTION_DETAILED_STATUS*> _conns;

	HANDLE _hConnMgr;
	HANDLE _hConnection;
	DWORD dwCacheVal;
	WCHAR wString[MAX_PATH];
	HKEY hKey;
	BOOL bForced;

	bool _connect();
	bool _disconnect();
	bool isGprsConnected();

public:
	GPRSConnection();
	virtual ~GPRSConnection();

	bool Connect() { return _connect(); }
	bool Disconnect() { return _disconnect(); }
};
