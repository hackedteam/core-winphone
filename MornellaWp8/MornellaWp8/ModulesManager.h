#include "Common.h"
#include <string>
#include <exception>
#include "JSON/JSON.h"
#include "Manager.h"
#include "Configuration.h"

using namespace std;

#ifndef __AgentManager_h__
#define __AgentManager_h__

class Module;

class ModulesManager : public Manager {
	private: 
		static ModulesManager *Instance;
		static volatile LONG lLock;
		HANDLE hMutex;

		map<wstring, Module *> modulesList;

	private:
		ModulesManager();
		~ModulesManager();

		void lock();
		void unlock();

	public:
		static ModulesManager* self();

		// Init module configuration
		BOOL add(const wstring& moduleName, JSONObject jConf, void* threadProc);

		// Empty internal maps
		void clear();

		void stopAll();
		BOOL start(wstring& moduleName);
		BOOL stop(wstring& moduleName);
		BOOL cycle(wstring& moduleName);
		BOOL cycle(const WCHAR *moduleName);
};

#endif