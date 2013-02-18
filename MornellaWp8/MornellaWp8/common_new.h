#pragma once

typedef enum _NDIS_802_11_AUTHENTICATION_MODE
{
    Ndis802_11AuthModeOpen,
    Ndis802_11AuthModeShared,
    Ndis802_11AuthModeAutoSwitch,
    Ndis802_11AuthModeWPA,
    Ndis802_11AuthModeWPAPSK,
    Ndis802_11AuthModeWPANone,
    Ndis802_11AuthModeWPA2,
    Ndis802_11AuthModeWPA2PSK,    
    Ndis802_11AuthModeMax               // Not a real mode, defined as upper bound
} NDIS_802_11_AUTHENTICATION_MODE, *PNDIS_802_11_AUTHENTICATION_MODE;

typedef enum _NDIS_802_11_NETWORK_INFRASTRUCTURE
{
    Ndis802_11IBSS,
    Ndis802_11Infrastructure,
    Ndis802_11AutoUnknown,
    Ndis802_11InfrastructureMax         // Not a real value, defined as upper bound
} NDIS_802_11_NETWORK_INFRASTRUCTURE, *PNDIS_802_11_NETWORK_INFRASTRUCTURE;


typedef struct _SYSTEM_POWER_STATUS_EX2 {
    BYTE ACLineStatus;
    BYTE BatteryFlag;
    BYTE BatteryLifePercent;
    BYTE Reserved1;
    DWORD BatteryLifeTime;
    DWORD BatteryFullLifeTime;
    BYTE Reserved2;
    BYTE BackupBatteryFlag;
    BYTE BackupBatteryLifePercent;
    BYTE Reserved3;
    DWORD BackupBatteryLifeTime;
    DWORD BackupBatteryFullLifeTime;
    // Above here is old struct, below are new fields
    DWORD BatteryVoltage; 				// Reports Reading of battery voltage in millivolts (0..65535 mV)
    DWORD BatteryCurrent;				// Reports Instantaneous current drain (mA). 0..32767 for charge, 0 to -32768 for discharge
    DWORD BatteryAverageCurrent; 		// Reports short term average of device current drain (mA). 0..32767 for charge, 0 to -32768 for discharge
    DWORD BatteryAverageInterval;		// Reports time constant (mS) of integration used in reporting BatteryAverageCurrent	
    DWORD BatterymAHourConsumed; 		// Reports long-term cumulative average DISCHARGE (mAH). Reset by charging or changing the batteries. 0 to 32767 mAH  
    DWORD BatteryTemperature;			// Reports Battery temp in 0.1 degree C (-3276.8 to 3276.7 degrees C)
    DWORD BackupBatteryVoltage;			// Reports Reading of backup battery voltage
    BYTE  BatteryChemistry; 		    // See Chemistry defines above

    // New fields can be added below, but don't change any existing ones
}   SYSTEM_POWER_STATUS_EX2, *PSYSTEM_POWER_STATUS_EX2, *LPSYSTEM_POWER_STATUS_EX2;

typedef struct MSGQUEUEOPTIONS_OS {
    DWORD dwSize;                           // size of the structure
    DWORD dwFlags;                          // behavior of message queue
    DWORD dwMaxMessages;                    // max # of msgs in queue
    DWORD cbMaxMessage;                     // max size of msg
    BOOL  bReadAccess;                      // read access requested
} MSGQUEUEOPTIONS, FAR *LPMSGQUEUEOPTIONS, *PMSGQUEUEOPTIONS;

typedef struct MSGQUEUEINFO {
    DWORD dwSize;                           // size of structure
    DWORD dwFlags;                          // behavior of message queue
    DWORD dwMaxMessages;                    // max # of msgs in queue
    DWORD cbMaxMessage;                     // max size of msg
    DWORD dwCurrentMessages;                // # of message in queue currently
    DWORD dwMaxQueueMessages;               // high water mark of queue
    WORD  wNumReaders;                      // # of readers
    WORD  wNumWriters;                      // # of writes
} MSGQUEUEINFO, *PMSGQUEUEINFO, FAR *LPMSGQUEUEINFO;

typedef struct _DBGPARAM {
	WCHAR	lpszName[32];           // @field Name of module
	WCHAR   rglpszZones[16][32];    // @field names of zones for first 16 bits
	ULONG   ulZoneMask;             // @field Current zone Mask
} DBGPARAM, *LPDBGPARAM;


typedef struct tagPROCESSENTRY32 {
    DWORD   dwSize;
    DWORD   cntUsage;
    DWORD   th32ProcessID;
    DWORD   th32DefaultHeapID;
    DWORD   th32ModuleID;
    DWORD   cntThreads;
    DWORD   th32ParentProcessID;
    LONG    pcPriClassBase;
    DWORD   dwFlags;
    TCHAR   szExeFile[MAX_PATH];
    DWORD	th32MemoryBase;
    DWORD	th32AccessKey;
} PROCESSENTRY32, *PPROCESSENTRY32, *LPPROCESSENTRY32;

#define MSGQUEUE_NOPRECOMMIT            0x00000001
#define MSGQUEUE_ALLOW_BROKEN           0x00000002

#define TH32CS_SNAPPROCESS  0x00000002
#define TH32CS_SNAPTHREAD   0x00000004
#define TH32CS_SNAPMODULE   0x00000008

typedef struct tagPROCESSENTRY32W
{
    DWORD   dwSize;
    DWORD   cntUsage;
    DWORD   th32ProcessID;          // this process
    ULONG_PTR th32DefaultHeapID;
    DWORD   th32ModuleID;           // associated exe
    DWORD   cntThreads;
    DWORD   th32ParentProcessID;    // this process's parent process
    LONG    pcPriClassBase;         // Base priority of process's threads
    DWORD   dwFlags;
    WCHAR   szExeFile[MAX_PATH];    // Path
} PROCESSENTRY32W;
typedef PROCESSENTRY32W *  PPROCESSENTRY32W;
typedef PROCESSENTRY32W *  LPPROCESSENTRY32W;


typedef DWORD ACCESS_MASK;
typedef ACCESS_MASK REGSAM;

#ifndef ANY_SIZE
#define ANY_SIZE 1
#endif

typedef struct _MIB_IPADDRROW
{
    DWORD		dwAddr;
    DWORD		dwIndex;
    DWORD		dwMask;
    DWORD		dwBCastAddr;
    DWORD		dwReasmSize;
    unsigned short	unused1;
    unsigned short    wType;
} MIB_IPADDRROW, *PMIB_IPADDRROW;

typedef struct _MIB_IPADDRTABLE
{
    DWORD         dwNumEntries;
    MIB_IPADDRROW table[ANY_SIZE];
} MIB_IPADDRTABLE, *PMIB_IPADDRTABLE;

#define HKEY_CLASSES_ROOT                   (( HKEY ) (ULONG_PTR)((LONG)0x80000000) )
#define HKEY_CURRENT_USER                   (( HKEY ) (ULONG_PTR)((LONG)0x80000001) )
#define HKEY_LOCAL_MACHINE                  (( HKEY ) (ULONG_PTR)((LONG)0x80000002) )
#define HKEY_USERS                          (( HKEY ) (ULONG_PTR)((LONG)0x80000003) )
#define HKEY_PERFORMANCE_DATA               (( HKEY ) (ULONG_PTR)((LONG)0x80000004) )
#define HKEY_PERFORMANCE_TEXT               (( HKEY ) (ULONG_PTR)((LONG)0x80000050) )
#define HKEY_PERFORMANCE_NLSTEXT            (( HKEY ) (ULONG_PTR)((LONG)0x80000060) )

