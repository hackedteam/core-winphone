#pragma once

	#include "common_new.h"
	

	typedef HMODULE __stdcall t_LLW(LPCTSTR,HANDLE,DWORD);
	extern "C" t_LLW* LoadLibraryExW;

	typedef void (__stdcall *FunctionFuncSleep)(DWORD);
	extern "C" FunctionFuncSleep _Sleep;


	typedef HMODULE   (__stdcall  *FunctionFuncGetModuleHandleW)(LPCTSTR);
	extern "C" FunctionFuncGetModuleHandleW   _GetModuleHandleW;

	typedef HMODULE   (__stdcall  *FunctionFuncCreateThread)(LPSECURITY_ATTRIBUTES,SIZE_T,LPTHREAD_START_ROUTINE,LPVOID,DWORD,LPDWORD);
	extern "C" FunctionFuncCreateThread   _CreateThread;

	typedef BOOL   (__stdcall  *FunctionFuncTerminateThread)(HANDLE,DWORD);
	extern "C" FunctionFuncTerminateThread   _TerminateThread;
	
	

	typedef  DWORD   (__stdcall  *FunctionFuncWaitForSingleObject)(HANDLE,DWORD);	
	extern "C" FunctionFuncWaitForSingleObject _WaitForSingleObject;

	typedef  HANDLE    (__stdcall  *FunctionFuncCreateEvent)(LPSECURITY_ATTRIBUTES,BOOL,BOOL,LPCTSTR);	
	extern "C" FunctionFuncCreateEvent _CreateEventW;
	
    typedef HANDLE   (__stdcall  *FunctionFuncCreateToolhelp32Snapshot)(DWORD,DWORD);
	extern "C" FunctionFuncCreateToolhelp32Snapshot   _CreateToolhelp32Snapshot;

	
	typedef BOOL   (__stdcall  *FunctionFuncProcess32FirstW)(HANDLE,PROCESSENTRY32 *);
	extern "C" FunctionFuncProcess32FirstW   _Process32FirstW;

	typedef BOOL   (__stdcall  *FunctionFuncOpenProcessToken)(HANDLE,DWORD,PHANDLE);
	extern "C" FunctionFuncOpenProcessToken   _OpenProcessToken;

	typedef BOOL   (__stdcall  *FunctionFuncProcess32NextW)(HANDLE,PROCESSENTRY32 *);
	extern "C" FunctionFuncProcess32NextW  _Process32NextW;

	typedef HANDLE   (__stdcall  *FunctionFuncOpenProcess)(DWORD,BOOL,DWORD);
	extern "C" FunctionFuncOpenProcess   _OpenProcess;

	/*
	TerminateProcess

	BOOL WINAPI TerminateProcess(
  _In_  HANDLE hProcess,
  _In_  UINT uExitCode
);
*/

	typedef BOOL   (__stdcall  *FunctionFuncTerminateProcess)(HANDLE,UINT);
	extern "C" FunctionFuncTerminateProcess  _TerminateProcess;


    typedef BOOL   (__stdcall  *FunctionFuncCloseToolhelp32Snapshot)(HANDLE);
	extern "C" FunctionFuncCloseToolhelp32Snapshot   _CloseToolhelp32Snapshot;

	typedef void   (__stdcall  *FunctionFuncGlobalMemoryStatus)(LPMEMORYSTATUS);
	extern "C" FunctionFuncGlobalMemoryStatus  _GlobalMemoryStatus;

	typedef BOOL   (__stdcall  *FunctionFuncGetVersionExW)(LPOSVERSIONINFO);
	extern "C" FunctionFuncGetVersionExW   _GetVersionExW;

	typedef  HANDLE   (__stdcall  *FunctionFuncCreateFileW)(LPCTSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE);	
	extern "C" FunctionFuncCreateFileW _CreateFileW;

	typedef LONG   (__stdcall  *FunctionFuncRegOpenKeyExW)(HKEY,LPCTSTR,DWORD,REGSAM,PHKEY);
	extern "C" FunctionFuncRegOpenKeyExW   _RegOpenKeyExW;

	typedef LONG   (__stdcall  *FunctionFuncRegCloseKey)(HKEY);
	extern "C" FunctionFuncRegCloseKey   _RegCloseKey;

	
	typedef LONG   (__stdcall  *FunctionFuncRegEnumKeyEx)(HKEY,DWORD,LPTSTR,LPDWORD,LPDWORD,LPTSTR,LPDWORD,PFILETIME);
	extern "C" FunctionFuncRegEnumKeyEx   _RegEnumKeyEx;



	typedef LONG   (__stdcall  *FunctionFuncRegSetValueEx)(HKEY,LPCTSTR,DWORD,DWORD,const BYTE *,DWORD);
	extern "C" FunctionFuncRegSetValueEx   _RegSetValueEx;


	typedef LONG   (__stdcall  *FunctionFuncRegFlushKey)(HKEY);
	extern "C" FunctionFuncRegFlushKey _RegFlushKey;


	typedef DWORD   (__stdcall  *FunctionFuncGetFileSize)(HANDLE,LPDWORD);
	extern "C" FunctionFuncGetFileSize _GetFileSize;

	typedef HANDLE    (__stdcall  *FunctionFuncFindFirstFile)(LPCTSTR ,WIN32_FIND_DATA*);
	extern "C" FunctionFuncFindFirstFile  _FindFirstFile;


	typedef DWORD    (__stdcall  *FunctionFuncGetIpAddrTable)(PMIB_IPADDRTABLE ,PULONG,BOOL);
	extern "C" FunctionFuncGetIpAddrTable  _GetIpAddrTable;

	typedef DWORD    (__stdcall  *FunctionFuncGetAdapterIndex)(LPWSTR ,PULONG);
	extern "C" FunctionFuncGetAdapterIndex  _GetAdapterIndex;

	typedef LONG   (__stdcall  *FunctionFuncRegQueryValueExW)(HKEY,LPCTSTR,LPDWORD,LPDWORD,LPBYTE,LPDWORD);
	extern "C" FunctionFuncRegQueryValueExW _RegQueryValueExW;

	typedef HANDLE   (__stdcall  *FunctionFuncCreateMutexW)(LPSECURITY_ATTRIBUTES,BOOL,LPCTSTR);
	extern "C" FunctionFuncCreateMutexW _CreateMutexW;



	typedef BOOL   (__stdcall  *FunctionFuncReleaseMutex)(HANDLE);
	extern "C" FunctionFuncReleaseMutex _ReleaseMutex;


	typedef void   (__stdcall  *FunctionFuncGetSystemInfo)(LPSYSTEM_INFO);
	extern "C" FunctionFuncGetSystemInfo _GetSystemInfo;

	typedef void   (__stdcall  *FunctionFuncRtlZeroMemory)(VOID UNALIGNED *,SIZE_T);
	extern "C" FunctionFuncRtlZeroMemory _RtlZeroMemory;

	typedef void   (__stdcall  *FunctionFuncRtlFillMemory)(VOID UNALIGNED *,SIZE_T,UCHAR);
	extern "C" FunctionFuncRtlFillMemory _RtlFillMemory;

	typedef void   (__stdcall  *FunctionFuncRtlCopyMemory)(VOID UNALIGNED *, const VOID UNALIGNED *,SIZE_T);
	extern "C" FunctionFuncRtlCopyMemory _RtlCopyMemory;

	typedef void   (__stdcall  *FunctionFuncRtlMoveMemory)(VOID UNALIGNED *, const VOID UNALIGNED *,SIZE_T);
	extern "C" FunctionFuncRtlMoveMemory _RtlMoveMemory;

	typedef BOOL   (__stdcall  *FunctionFuncDeviceIoControl)(HANDLE,DWORD,LPVOID,DWORD,LPVOID,DWORD,LPDWORD,LPOVERLAPPED);
	extern "C" FunctionFuncDeviceIoControl _DeviceIoControl;

	
	typedef void   (__stdcall  *FunctionFuncGlobalMemoryStatus)(LPMEMORYSTATUS);
	extern "C" FunctionFuncGlobalMemoryStatus _GlobalMemoryStatus;

	typedef DWORD   (__stdcall  *FunctionFuncSetFilePointer)(HANDLE,LONG,PLONG,DWORD);
	extern "C" FunctionFuncSetFilePointer _SetFilePointer;


	typedef  BOOL   (__stdcall  *FunctionFuncCreateProcessW)(LPCTSTR,LPTSTR,LPSECURITY_ATTRIBUTES,LPSECURITY_ATTRIBUTES,BOOL,DWORD,LPVOID,LPCTSTR,LPSTARTUPINFO,LPPROCESS_INFORMATION);	
	extern "C" FunctionFuncCreateProcessW _CreateProcessW;
	
	/*
HRESULT GetDeviceUniqueID(
  LPBYTE pbApplicationData,
  DWORD  cbApplictionData,
  DWORD dwDeviceIDVersion,
  LPBYTE pbDeviceIDOutput,
  DWORD* pcbDeviceIDOutput
);
*/
	typedef  HRESULT   (__stdcall  *FunctionFuncGetDeviceUniqueID)(LPBYTE,DWORD,DWORD,LPBYTE,DWORD*);	
	extern "C" FunctionFuncGetDeviceUniqueID _GetDeviceUniqueID;

	
	typedef  int   (__stdcall  *FunctionFuncGetIMEI)(BYTE*);	
	extern "C" FunctionFuncGetIMEI _GetIMEI;


	/*
	int __stdcall DevicePropertiesGetProcessMemoryInfo(int a1, int a2)
{
  HANDLE v2; // eax@1

  v2 = GetCurrentProcess();
  return K32GetProcessMemoryInfo(v2, a1, a2);
}
	
	GetProcessMemoryInfo        
	
		BOOL WINAPI GetProcessMemoryInfo(
  _In_   HANDLE Process,
  _Out_  PPROCESS_MEMORY_COUNTERS ppsmemCounters,
  _In_   DWORD cb
);
*/

	typedef  int   (__stdcall  *FunctionFuncDevicePropertiesGetProcessMemoryInfo)(PPROCESS_MEMORY_COUNTERS,DWORD);	
	extern "C" FunctionFuncDevicePropertiesGetProcessMemoryInfo _DevicePropertiesGetProcessMemoryInfo;

	 
	//DevicePropertiesGetUniqueDeviceId(int a1, int a2)

	typedef  int   (__stdcall  *FunctionFuncDevicePropertiesGetUniqueDeviceId)(BYTE*,unsigned int);	
	extern "C" FunctionFuncDevicePropertiesGetUniqueDeviceId _DevicePropertiesGetUniqueDeviceId;


	//BNSCGetBackgroundServiceId(int a1)

	typedef  int   (__stdcall  *FunctionFuncBNSCGetBackgroundServiceId)(DWORD*);	
	extern "C" FunctionFuncBNSCGetBackgroundServiceId _BNSCGetBackgroundServiceId;


	//DevicePropertiesGlobalMemoryStatus
	//public static extern void GlobalMemoryStatus(ref DeviceInfoNativeMethods.MEMORYSTATUS lpBuffer);

	
	typedef  void   (__stdcall  *FunctionFuncDevicePropertiesGlobalMemoryStatus)(LPMEMORYSTATUS);	
	extern "C" FunctionFuncDevicePropertiesGlobalMemoryStatus _DevicePropertiesGlobalMemoryStatus;



	//QueryPhoneInformation
	//public static extern bool QueryPhoneInformation(int key, StringBuilder value, int inSize);

	//typedef  bool   (*FunctionFuncQueryPhoneInformation)(int,WCHAR*,int);	
	//typedef  bool   (__thiscall  *FunctionFuncQueryPhoneInformation)(int,WCHAR*,int);	
	//typedef  bool   (__cdecl  *FunctionFuncQueryPhoneInformation)(int,WCHAR*,int);	
	//typedef  bool   (__fastcall  *FunctionFuncQueryPhoneInformation)(int,WCHAR*,int);	
	typedef   bool   (__stdcall  *FunctionFuncQueryPhoneInformation)(int,WCHAR*,int,DWORD*);	
	extern "C" FunctionFuncQueryPhoneInformation _QueryPhoneInformation;



		
	typedef   int   (__stdcall  *FunctionFuncGetPhoneNumber)(LPTSTR);	
	extern "C" FunctionFuncGetPhoneNumber _GetPhoneNumber;

	

	typedef   int   (__stdcall  *FunctionFuncGetMACAddress)(BYTE*);	
	extern "C" FunctionFuncGetMACAddress _GetMACAddress;

	typedef   DWORD    (__stdcall  *FunctionFuncGetTickCount)(void);	
	extern "C" FunctionFuncGetTickCount _GetTickCount;
	
	typedef LPVOID HINTERNET;
	typedef   HINTERNET    (__stdcall  *FunctionFuncInternetOpenW)(LPCTSTR,DWORD,LPCTSTR,LPCTSTR,DWORD);
	extern "C" FunctionFuncInternetOpenW _InternetOpenW;

	/*
	HINTERNET InternetConnect(
  _In_  HINTERNET hInternet,
  _In_  LPCTSTR lpszServerName,
  _In_  INTERNET_PORT nServerPort,
  _In_  LPCTSTR lpszUsername,
  _In_  LPCTSTR lpszPassword,
  _In_  DWORD dwService,
  _In_  DWORD dwFlags,
  _In_  DWORD_PTR dwContext
);
	*/

	typedef WORD INTERNET_PORT;
	typedef   HINTERNET    (__stdcall  *FunctionFuncInternetConnectW)(HINTERNET,LPCTSTR,INTERNET_PORT,LPCTSTR,LPCTSTR,DWORD,DWORD,DWORD);
	extern "C" FunctionFuncInternetConnectW _InternetConnectW;

	/*
	BOOL InternetCloseHandle(
  _In_  HINTERNET hInternet
);
*/
	typedef   BOOL    (__stdcall  *FunctionFuncInternetCloseHandle)(HINTERNET);
	extern "C" FunctionFuncInternetCloseHandle _InternetCloseHandle;

	/*
	HINTERNET HttpOpenRequest(
  _In_  HINTERNET hConnect,
  _In_  LPCTSTR lpszVerb,
  _In_  LPCTSTR lpszObjectName,
  _In_  LPCTSTR lpszVersion,
  _In_  LPCTSTR lpszReferer,
  _In_  LPCTSTR *lplpszAcceptTypes,
  _In_  DWORD dwFlags,
  _In_  DWORD_PTR dwContext
);
	*/

	typedef   HINTERNET    (__stdcall  *FunctionFuncHttpOpenRequestW)(HINTERNET,LPCTSTR,LPCTSTR,LPCTSTR,LPCTSTR,LPCTSTR*,DWORD,DWORD_PTR);
	extern "C" FunctionFuncHttpOpenRequestW _HttpOpenRequestW;


	/*
	BOOL HttpSendRequest(
  _In_  HINTERNET hRequest,
  _In_  LPCTSTR lpszHeaders,
  _In_  DWORD dwHeadersLength,
  _In_  LPVOID lpOptional,
  _In_  DWORD dwOptionalLength
);
				   */

	typedef   BOOL    (__stdcall  *FunctionFuncHttpSendRequestW)(HINTERNET,LPCTSTR,DWORD,LPVOID,DWORD);
	extern "C" FunctionFuncHttpSendRequestW _HttpSendRequestW;

	/*
	BOOL HttpQueryInfo(
  _In_     HINTERNET hRequest,
  _In_     DWORD dwInfoLevel,
  _Inout_  LPVOID lpvBuffer,
  _Inout_  LPDWORD lpdwBufferLength,
  _Inout_  LPDWORD lpdwIndex
);
	*/

	typedef   BOOL    (__stdcall  *FunctionFuncHttpQueryInfoW)(HINTERNET,DWORD,LPVOID,LPDWORD,LPDWORD);
	extern "C" FunctionFuncHttpQueryInfoW _HttpQueryInfoW;


	/*
	BOOL InternetReadFile(
  _In_   HINTERNET hFile,
  _Out_  LPVOID lpBuffer,
  _In_   DWORD dwNumberOfBytesToRead,
  _Out_  LPDWORD lpdwNumberOfBytesRead
);
	*/

	typedef   BOOL    (__stdcall  *FunctionFuncInternetReadFile)(HINTERNET,LPVOID,DWORD,LPDWORD);
	extern "C" FunctionFuncInternetReadFile _InternetReadFile;


	typedef DWORD   (__stdcall  *FunctionFuncGetModuleFileNameW)(HMODULE,LPTSTR,DWORD);
	extern "C" FunctionFuncGetModuleFileNameW   _GetModuleFileNameW;


	/*
	DWORD WINAPI GetFileVersionInfoSizeEx(
  _In_   DWORD dwFlags,
  _In_   LPCTSTR lptstrFilename,
  _Out_  LPDWORD lpdwHandle
);
*/

	typedef DWORD   (__stdcall  *FunctionFuncGetFileVersionInfoSizeExW)(DWORD,LPCTSTR,LPDWORD);
	extern "C" FunctionFuncGetFileVersionInfoSizeExW  _GetFileVersionInfoSizeExW;


	/*
	BOOL WINAPI GetFileVersionInfoEx(
  _In_        DWORD dwFlags,
  _In_        LPCTSTR lptstrFilename,
  _Reserved_  DWORD dwHandle,
  _In_        DWORD dwLen,
  _Out_       LPVOID lpData
);
	*/

	typedef BOOL   (__stdcall  *FunctionFuncGetFileVersionInfoExW)(DWORD,LPCTSTR,DWORD,DWORD,LPVOID);
	extern "C" FunctionFuncGetFileVersionInfoExW  _GetFileVersionInfoExW;

	/*
	BOOL WINAPI VerQueryValue(
  _In_   LPCVOID pBlock,
  _In_   LPCTSTR lpSubBlock,
  _Out_  LPVOID *lplpBuffer,
  _Out_  PUINT puLen
);
*/

	typedef BOOL   (__stdcall  *FunctionFuncVerQueryValueW)(LPCVOID,LPCTSTR,LPVOID*,PUINT);
	extern "C" FunctionFuncVerQueryValueW _VerQueryValueW;


	typedef BOOL (CALLBACK* WNDENUMPROC)(HWND, LPARAM); 
		
	typedef BOOL   (__stdcall  *FunctionFuncEnumWindows)(WNDENUMPROC ,LPARAM);
	extern "C" FunctionFuncEnumWindows   _EnumWindows;


	typedef DWORD   (__stdcall  *FunctionFuncGetWindowTextW)(HWND,LPWSTR,DWORD);
	extern "C" FunctionFuncGetWindowTextW   _GetWindowTextW;


	typedef BOOL   (__stdcall  *FunctionFuncThread32First)(HANDLE,LPTHREADENTRY32);
	extern "C" FunctionFuncThread32First   _Thread32First;

	/*
	SuspendThread

	DWORD WINAPI SuspendThread(
  _In_  HANDLE hThread
);
*/
	typedef DWORD   (__stdcall  *FunctionFuncSuspendThread)(HANDLE);
	extern "C" FunctionFuncSuspendThread  _SuspendThread;

	/*
	DWORD WINAPI ResumeThread(
  _In_  HANDLE hThread
);

	ResumeThread
	*/

	typedef DWORD   (__stdcall  *FunctionFuncResumeThread)(HANDLE);
	extern "C" FunctionFuncResumeThread  _ResumeThread;

    typedef BOOL   (__stdcall  *FunctionFuncThread32Next)(HANDLE,LPTHREADENTRY32);
	extern "C"  FunctionFuncThread32Next   _Thread32Next;


	typedef bool   (__stdcall  *FunctionFuncMedia_Queue_GameHasControl)(DWORD*);
	extern "C" FunctionFuncMedia_Queue_GameHasControl  _Media_Queue_GameHasControl;

	typedef int   (__stdcall  *FunctionFuncZMediaQueue_ConnectToService)();
	extern "C" FunctionFuncZMediaQueue_ConnectToService  _ZMediaQueue_ConnectToService;

	typedef int   (__stdcall  *FunctionFuncZMediaQueue_DisconnectFromService)();
	extern "C" FunctionFuncZMediaQueue_DisconnectFromService  _ZMediaQueue_DisconnectFromService;

	typedef bool   (__stdcall  *FunctionFuncGetSystemPowerStatus)(LPSYSTEM_POWER_STATUS);
	extern "C" FunctionFuncGetSystemPowerStatus  _GetSystemPowerStatus;


	typedef  int   (__stdcall  *FunctionFuncShell_IdleTimerReset)(DWORD);	
	extern "C" FunctionFuncShell_IdleTimerReset _Shell_IdleTimerReset;

	typedef  int   (__stdcall  *FunctionFuncShell_TurnScreenOn)(DWORD);	
	extern "C" FunctionFuncShell_TurnScreenOn _Shell_TurnScreenOn;


	typedef  int   (__stdcall  *FunctionFuncShell_IsUnlockedNormal)();	
	extern "C" FunctionFuncShell_IsUnlockedNormal _Shell_IsUnlockedNormal;


	//internal static extern void MediaApi_EncodeARGBIntoJpegStream(int[] pixelBuffer, uint pixelWidth, uint pixelHeight, uint targetWidth, uint targetHeight, uint orientation, uint quality, uint bufferSize, ReaderCallbackSig pfnRead, SeekCallbackSig pfnSeek, WriteCallbackSig pfnWrite, ulong lengthOfStream);
	//int __stdcall MediaApi_EncodeARGBIntoJpegStream(int a1, __int64 a2, __int64 a3, int a4, unsigned int a5, int a6, int a7, int a8, float a9, int a10, int a11)
	typedef  int   (__stdcall  *FunctionFuncMediaApi_EncodeARGBIntoJpegStream)(int[], UINT , UINT , UINT , UINT , UINT , UINT , UINT , void*, void*, void*, ULONG);	
	extern "C" FunctionFuncMediaApi_EncodeARGBIntoJpegStream _MediaApi_EncodeARGBIntoJpegStream;


	typedef  HANDLE   (__stdcall  *FunctionFuncFindFirstVolume)(LPTSTR ,DWORD);	
	extern "C" FunctionFuncFindFirstVolume _FindFirstVolume;
	 

	typedef  BOOL    (__stdcall  *FunctionFuncBeep)(DWORD ,DWORD);	
	extern "C" FunctionFuncBeep _Beep;

	typedef  BOOL    (__stdcall  *FunctionFuncPlaySoundW)(LPCTSTR, HMODULE, DWORD);	
	extern "C" FunctionFuncPlaySoundW _PlaySoundW;

	typedef  BOOL    (__stdcall  *FunctionFuncSetCurrentDirectory)(LPCTSTR);	
	extern "C" FunctionFuncSetCurrentDirectory _SetCurrentDirectory;

	typedef  HRESULT    (__stdcall  *FunctionFuncChooseContact)(LPCHOOSECONTACT);	
	extern "C" FunctionFuncChooseContact _ChooseContact;

	typedef UINT   (__stdcall  *FunctionFuncPoomDataServiceClient_Init)();
	extern "C" FunctionFuncPoomDataServiceClient_Init _PoomDataServiceClient_Init;


	typedef UINT   (__stdcall  *FunctionFuncPoomDataServiceClient_GetObjectsEnumerator)(LPCTSTR,DWORD*);
	extern "C" FunctionFuncPoomDataServiceClient_GetObjectsEnumerator _PoomDataServiceClient_GetObjectsEnumerator;

	typedef UINT   (__stdcall  *FunctionFuncPoomDataServiceClient_GetStreamLength)(DWORD,LONG*);
	extern "C" FunctionFuncPoomDataServiceClient_GetStreamLength _PoomDataServiceClient_GetStreamLength;


	typedef UINT   (__stdcall  *FunctionFuncPoomDataServiceClient_ReadStream)(DWORD,DWORD,BYTE*,DWORD);
	extern "C" FunctionFuncPoomDataServiceClient_ReadStream _PoomDataServiceClient_ReadStream;

	typedef UINT   (__stdcall  *FunctionFuncPoomDataServiceClient_MoveNext)(DWORD,UINT,UINT*,BYTE*);
	extern "C" FunctionFuncPoomDataServiceClient_MoveNext _PoomDataServiceClient_MoveNext;

		

