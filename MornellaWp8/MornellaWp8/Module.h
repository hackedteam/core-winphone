#include "Common.h"
#include <string>
#include <exception>
#include "JSON/JSON.h"
#include "Configuration.h"

using namespace std;

#ifndef __Module_h__
#define __Module_h__

class Module {
	private:
		HANDLE hHandle, eventHandle;
		DWORD threadID;
		UINT status;
		void* threadProc;
		Configuration *conf;
		volatile LONG stopModule, cycleModule;

	private: 
		void requestStop();
		HANDLE getHandle();

	public:
		Module(void* proc, Configuration *c);
		~Module();

		BOOL run();
		void stop();
		UINT getStatus();
		void setStatus(UINT newStatus);
		BOOL shouldStop();		// Called from the module
		BOOL shouldCycle();		// Ibidem
		void requestCycle();	// Called from ModulesManager
		HANDLE getEvent();
		Configuration* getConf(); // Called from the module
};

#endif