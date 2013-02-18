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
