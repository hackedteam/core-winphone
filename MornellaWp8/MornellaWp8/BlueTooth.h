#pragma once
#include <Winsock2.h>

typedef ULONGLONG BT_ADDR;

class BlueTooth {
public:
	virtual BOOL InquiryService(WCHAR *address) = 0;
	virtual BT_ADDR FindServer(SOCKET bt) = 0;
	virtual BOOL ActivateBT() = 0;
	virtual BOOL DeActivateBT() = 0;
	virtual BOOL SetInstanceName(PWCHAR pwName) = 0;
	virtual void SetGuid(GUID guid) = 0;
};

