//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//
/*++
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

Module Name:  

service.h

Abstract:  
	Defines programming model for Windows CE Services
	
Notes: 


--*/
#if ! defined (__service_H__)
#define __service_H__		1

///#include <winioctl.h>

//
//	Return codes
//
#define SERVICE_SUCCESS				0

//
//	Service states
//
#define SERVICE_STATE_OFF           0
#define SERVICE_STATE_ON            1
#define SERVICE_STATE_STARTING_UP   2
#define SERVICE_STATE_SHUTTING_DOWN 3
#define SERVICE_STATE_UNLOADING     4
#define SERVICE_STATE_UNINITIALIZED 5
#define SERVICE_STATE_UNKNOWN       0xffffffff

//
// Service startup state (value passed on xxx_Init())
//
#define SERVICE_INIT_STARTED	0x00000000
// Service is a super-service, should not spin its own accept() threads.
#define SERVICE_INIT_STOPPED	0x00000001
// Service is being run in an isolated services.exe.  Interprocess communication 
// via IOCTLs or streaming interface is not supported in this configuration.
#define SERVICE_INIT_STANDALONE 0x00000002

// 
// Service may need to know whether it was called from device.exe or services.exe or elsewhere.
//
#define SERVICE_CALLER_PROCESS_SERVICES_EXE      1
#define SERVICE_CALLER_PROCESS_DEVICE_EXE        2
#define	SERVICE_CALLER_PROCESS_OTHER_EXE         100

#define SERVICE_SERVICES_EXE_PROCNAME            L"services.exe"
#define SERVICE_DEVICE_EXE_PROCNAME              L"device.exe"


typedef struct _ServiceEnumInfo {
	WCHAR   szPrefix[6];     
	WCHAR   *szDllName;
	HANDLE  hServiceHandle;
	DWORD   dwServiceState;   // one of SERVICE_STATE_XXX values above.
} ServiceEnumInfo;

// Called from service on initialization to determine where it's running from
#define SERVICE_FIND_CALLER(callerProc) { \
		WCHAR szCallerModName[MAX_PATH]; \
		if (GetModuleFileName((HINSTANCE)GetCurrentProcess(),szCallerModName,sizeof(szCallerModName)/sizeof(WCHAR)))  { \
			if (wcsstr(szCallerModName,SERVICE_DEVICE_EXE_PROCNAME))  { \
				(callerProc) = SERVICE_CALLER_PROCESS_DEVICE_EXE; \
			} \
			else if (wcsstr(szCallerModName,SERVICE_SERVICES_EXE_PROCNAME)) { \
				(callerProc) = SERVICE_CALLER_PROCESS_SERVICES_EXE; \
			} \
			else { \
				(callerProc) = SERVICE_CALLER_PROCESS_OTHER_EXE; \
			} \
		} \
		else \
			(callerProc) = SERVICE_CALLER_PROCESS_OTHER_EXE; \
	}

//
//	Service is interfaced via series of IOCTL calls that define service life cycle.
//	Actual implementation is service-specific.
//

//
//	Start the service that has been in inactive state. Return code: SERVICE_SUCCESS or error code.
//
#define IOCTL_SERVICE_START		CTL_CODE(FILE_DEVICE_SERVICE, 1, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
//	Stop service, but do not unload service's DLL
//
#define IOCTL_SERVICE_STOP		CTL_CODE(FILE_DEVICE_SERVICE, 2, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
//	Refresh service's state from registry or other configuration storage
//
#define IOCTL_SERVICE_REFRESH	CTL_CODE(FILE_DEVICE_SERVICE, 3, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
//	Have service configure its registry for auto-load
//
#define IOCTL_SERVICE_INSTALL	CTL_CODE(FILE_DEVICE_SERVICE, 4, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
//	Remove registry configuration
//
#define IOCTL_SERVICE_UNINSTALL	CTL_CODE(FILE_DEVICE_SERVICE, 5, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
//	Unload the service which should be stopped.
//
#define IOCTL_SERVICE_UNLOAD	CTL_CODE(FILE_DEVICE_SERVICE, 6, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
//	Supply a configuration or command string and code to the service.
//
#define IOCTL_SERVICE_CONTROL	CTL_CODE(FILE_DEVICE_SERVICE, 7, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
//	Return service status.
//
#define IOCTL_SERVICE_STATUS	CTL_CODE(FILE_DEVICE_SERVICE, 8, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
//	Set service's debug zone mask (parameter is sizeof(DWORD) and contains the mask)
//
#define IOCTL_SERVICE_DEBUG	CTL_CODE(FILE_DEVICE_SERVICE, 9, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
//	Toggle service's console on/off (string "on" on no parameters is ON, "off" means off)
//
#define IOCTL_SERVICE_CONSOLE	CTL_CODE(FILE_DEVICE_SERVICE, 10, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Notify service a service request has arrived.  Input contains connected socket.
//
#define IOCTL_SERVICE_REGISTER_SOCKADDR   CTL_CODE(FILE_DEVICE_SERVICE, 11, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Notify service a service request has arrived.  For now all sockets associated with service will be closed at once.
//
#define IOCTL_SERVICE_DEREGISTER_SOCKADDR   CTL_CODE(FILE_DEVICE_SERVICE, 12, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Notify service a another socket has bound to it.  Input contains an accepted socket.
//
#define IOCTL_SERVICE_CONNECTION   CTL_CODE(FILE_DEVICE_SERVICE, 13, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Service has finished initial control setup.  Ready to start.
//
#define IOCTL_SERVICE_STARTED      CTL_CODE(FILE_DEVICE_SERVICE, 14, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Service is called with IOCTL_SERVICE_CAN_DEINIT immediatly before xxx_Deinit is called during DeregisterService.  
// If xxx_IOControl returns TRUE and sets buffer in pBufOut to zero, service instance will remain loaded and 
// xxx_Deinit will not be called.
//
#define IOCTL_SERVICE_QUERY_CAN_DEINIT   CTL_CODE(FILE_DEVICE_SERVICE, 15, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// If iphlpapi is present, services receive this notification when table that 
// maps IP addresses changes.
// Input contains PIP_ADAPTER_ADDRESSES retrieved from a call to GetAdaptersAddresses().
// As soon as the service returns from this call, this buffer is not valid.  The buffer
// must not be pointed to and it MUST NOT BE COPIED (because internally it has pointers
// to other structures inside it).  If this data is required after the service returns,
// the service must make its own call to GetAdaptersAddresses().
#define IOCTL_SERVICE_NOTIFY_ADDR_CHANGE  CTL_CODE(FILE_DEVICE_SERVICE, 16, METHOD_BUFFERED, FILE_ANY_ACCESS)


//
// Services.exe supports a set of callbacks that can only be called from a running service (not API).
// pBufIn data structure contains a pointer to ServiceCallbackFunctions data structure
// IOCTL_SERVICE_CALLBACKS will sent to service during its initial load, and only if there
// are supported callbacks for service's mode of operation.
//
#define IOCTL_SERVICE_CALLBACK_FUNCTIONS  CTL_CODE(FILE_DEVICE_SERVICE, 17, METHOD_BUFFERED, FILE_ANY_ACCESS)


//
// Service returns a DWORD containing a set of flags that specify what options it supports.
// The meaning of the flags is service specific.
//
#define IOCTL_SERVICE_SUPPORTED_OPTIONS   CTL_CODE(FILE_DEVICE_SERVICE, 18, METHOD_BUFFERED, FILE_ANY_ACCESS)


//
// Service is called from services.exe cmd line with arguments passed into it.
// (i.e. "services command HTP0: Arg1 Arg2 Arg3...")
// Args will be passed to service as ServicesExeCommandLineParams struct as the pBufIn paramater.
// In pBufOut paramater services may put a WCHAR* string, which will be displayed to command line user.
//
#define IOCTL_SERVICE_COMMAND_LINE_PARAMS CTL_CODE(FILE_DEVICE_SERVICE, 19, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// If a cmd line user of services.exe queries it for help on a particular service
// (i.e. "services help HTP0:") then this IOCTL will be passed to the service.  If
// the service supports command line help, it should return a WCHAR* to be displayed
// to the user in pBufOut.

#define IOCTL_SERVICE_COMMAND_LINE_HELP CTL_CODE(FILE_DEVICE_SERVICE, 20, METHOD_BUFFERED, FILE_ANY_ACCESS)


// A service will be called with IOCTL_SERVICE_CALLBACKS and a pointer to this struct
// on startup potentially.  Note that only functions that are supported for a service's
// mode of operation will be set.  It is possible that function pointers will be NULL.

typedef void (WINAPI *PFN_SERVICE_SHUTDOWN)(void);


typedef struct _ServicesExeCallbackFunctions {
	// Only set when service is running in context SERVICE_INIT_STANDALONE, i.e.
	// in its own copy of services.exe.  Service calls this function to request itself
	// to be unloaded and the services.exe process to exit.  For typical scenario
	// (no SERVICE_INIT_STANDALONE), use DeregisterService() API to cause service to be unloaded.
	PFN_SERVICE_SHUTDOWN       pfnServiceShutdown;
} ServicesExeCallbackFunctions;


#define SERVICES_EXE_MAX_CMD_LINE_OPTIONS        16

typedef struct _ServicesExeCommandLineParams {
	DWORD dwArgs;
	const WCHAR *ppwszArgs[SERVICES_EXE_MAX_CMD_LINE_OPTIONS];
} ServicesExeCommandLineParams;


#ifdef __cplusplus
extern "C" {
#endif

HANDLE ActivateService(LPCWSTR lpszDevKey, DWORD dwClientInfo);
HANDLE RegisterService(LPCWSTR lpszType, DWORD dwIndex, LPCWSTR lpszLib, DWORD dwInfo);
BOOL DeregisterService(HANDLE hDevice);
// void CloseAllServiceHandles(HANDLE proc);  // not supported
HANDLE CreateServiceHandle(LPCWSTR lpNew, DWORD dwAccess, DWORD dwShareMode, HANDLE hProc);
BOOL GetServiceByIndex(DWORD dwIndex, LPWIN32_FIND_DATA lpFindFileData);
BOOL ServiceIoControl(HANDLE hService, DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, 
                          LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);

#if defined (_WINSOCKAPI_) || defined (_WINSOCK2API_)
BOOL ServiceAddPort(HANDLE hService, SOCKADDR *pSockAddr, int cbSockAddr, int iProtocol, WCHAR *szRegWritePath);
BOOL ServiceClosePort(HANDLE hService, SOCKADDR *pSockAddr, int cbSockAddr, int iProtocol, BOOL fRemoveFromRegistry);
#endif

BOOL ServiceUnbindPorts(HANDLE hService);
BOOL EnumServices(PBYTE pBuffer, DWORD *pdwServiceEntries, DWORD *pdwBufferLen);
HANDLE GetServiceHandle(LPWSTR szPrefix, LPWSTR szDllName, DWORD *pdwDllBuf);


#ifdef __cplusplus
}
#endif


#if defined(WINCEOEM) && defined(UNDER_CE)
#ifdef WINCEMACRO
#include <mservice.h>
#endif
#endif


#endif	/* __service_H__ */

