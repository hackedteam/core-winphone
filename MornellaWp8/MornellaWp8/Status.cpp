#include "Status.h"
#include "Conf.h"
#include "FunctionFunc.h"

/**
* La nostra unica reference a Status
*/
Status* Status::Instance = NULL;
volatile LONG Status::lLock = 0;

Status* Status::self() {
	while (InterlockedExchange((LPLONG)&lLock, 1) != 0)
		_Sleep(1);

	if (Instance == NULL)
		Instance = new(std::nothrow) Status();

	InterlockedExchange((LPLONG)&lLock, 0);

	return Instance;
}

Status::Status() : uCrisis(FALSE) {
}

Status::~Status() {

}

void Status::StartCrisis(UINT uType) {
	uCrisis = uType;
}

void Status::StopCrisis() {
	uCrisis = CRISIS_NONE;
}

UINT Status::Crisis() {
	return uCrisis;
}
