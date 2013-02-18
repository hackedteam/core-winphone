#include "Common.h"
#include <string>
#include <exception>
#include "JSON/JSON.h"
#include "Configuration.h"
#include "ActionsManager.h"

using namespace std;

#ifndef __Event_h__
#define __Event_h__

class Event {
private:
	HANDLE threadHandle, eventHandle;
	DWORD threadID;
	UINT status;
	INT startAction, endAction, repeatAction;
	void* threadProc;

	Configuration *conf;
	ActionsManager *actions;

	volatile LONG stopModule;

private: 
	HANDLE getHandle();

public:
	Event(void* proc, Configuration *c);
	~Event();

	BOOL run();
	void stop();		// Called from the EventManager
	void requestStop(); // Called only by an Event
	UINT getStatus();
	void setStatus(UINT newStatus);
	BOOL shouldStop();
	Configuration* getConf();
	BOOL isEnabled();
	const wstring getName();
	void triggerStart();
	void triggerEnd();
	void triggerRepeat();
	HANDLE getEvent();
};

#endif