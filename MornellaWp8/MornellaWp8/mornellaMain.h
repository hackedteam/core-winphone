

extern "C" int mornellaStart(void);

COPYFILE2_EXTENDED_PARAMETERS copyParams = {
	sizeof(copyParams), COPY_FILE_FAIL_IF_EXISTS, NULL, NULL, NULL
};

// Core Procedure
extern "C" DWORD WINAPI CoreProc(LPVOID lpv);

// Use CE's built-in debugging framework.
#ifdef _DEBUG

DBGPARAM dpCurSettings = {
	TEXT("bthclient"), {
		TEXT("Error"),TEXT("Init"),TEXT("Net Client"),TEXT("Interface"),
			TEXT("API"),TEXT(""),TEXT(""),TEXT(""),
			TEXT(""),TEXT(""),TEXT(""),TEXT(""),
			TEXT(""),TEXT(""),TEXT(""),TEXT("") },
			0x0007  // Turn on Error, Init, and Client DEBUGZONE's by default
};

#define ZONE_ERROR  DEBUGZONE(0)
#define ZONE_INIT   DEBUGZONE(1)
#define ZONE_CLIENT DEBUGZONE(2)
#define ZONE_INTRF  DEBUGZONE(3)
#define ZONE_API    DEBUGZONE(4)
#define ZONE_NET    DEBUGZONE(5)
#endif
