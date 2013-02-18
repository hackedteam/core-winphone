#include "Kill.h"

/***
extern "C" {
	BOOL WINAPI SetKMode(BOOL fMode);
	DWORD WINAPI SetProcPermissions(DWORD);
	LPVOID WINAPI MapPtrToProcess (LPVOID lpv, HANDLE hProc);
	LPVOID WINAPI MapPtrUnsecure(LPVOID lpv, HANDLE hProc);
};
***/

Kill::Kill(Configuration *c) : stopAction(FALSE) {
	conf = c;
}

INT Kill::run() {
	/***
	BOOL kill;

	kill = conf->getBool(L"permanent");

	SetKMode(TRUE);
	SetProcPermissions(0xFFFFFFFF);

	if (kill) {
			
	}

	// Freeze the device
	unsigned int* ptr = (unsigned int *)0x80000000;

	for (;;) {
		*ptr = 0;
		ptr++;
	}
	***/
	return 1;
}

BOOL Kill::getStop() {
	return stopAction;
}