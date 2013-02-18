#include "Manager.h"

Manager::Manager() : hMutex(NULL) {
	/*hMutex = CreateMutex(NULL, FALSE, NULL);

	if (hMutex == NULL)
		throw new exception();*/
}

Manager::~Manager() {
	/*if (hMutex != NULL)
		CloseHandle(hMutex);*/
}