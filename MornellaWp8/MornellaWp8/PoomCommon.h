#pragma once
/*
#include <windows.h>
#include <list>
#include <new>
#include <map>

using namespace std;
*/
#define	POOM_V1_0_PROTO	0x01000000
#define	POOM_V2_0_PROTO 0x01000001

#define FLAG_REMINDER			0x00000001
#define FLAG_COMPLETE			0x00000002
#define FLAG_TEAMTASK			0x00000004
#define FLAG_RECUR				0x00000008
#define FLAG_RECUR_NoEndDate	0x00000010
#define FLAG_MEETING			0x00000020
#define FLAG_ALLDAY				0x00000040
#define FLAG_ISTASK				0x00000080

enum ObjectTaskTypes{
	POOM_TYPE_MASK	= 0x00FFFFFF,
	/*
	POOM_STRING_SUBJECT		=	0x01000000, 
	POOM_STRING_CATEGORIES	=	0x02000000,
	POOM_STRING_BODY		=	0x04000000,
	POOM_STRING_RECIPIENTS	=	0x08000000,
	POOM_STRING_LOCATION	=	0x10000000,

	POOM_OBJECT_RECUR		=	0x80000000
	*/
	POOM_STRING_SUBJECT		=	0x01, 
	POOM_STRING_CATEGORIES	=	0x02,
	POOM_STRING_BODY		=	0x04,
	POOM_STRING_RECIPIENTS	=	0x08,
	POOM_STRING_LOCATION	=	0x10,

	POOM_OBJECT_RECUR		=	0x80

};

typedef struct _TaskRecur{
	LONG lRecurrenceType;
	LONG lInterval;
	LONG lMonthOfYear;
	LONG lDayOfMonth;
	LONG lDayOfWeekMask;
	LONG lInstance;
	LONG lOccurrences;
	FILETIME	ftPatternStartDate;
	FILETIME	ftPatternEndDate;
} RecurStruct, *pRecurStruct;





#define SETLONG(x)	lTmp = x();	\
	CopyMemory(pPtr, &lTmp, sizeof(LONG));	pPtr += sizeof(LONG); \

#define GETBSTR(get, set)								\
	do{													\
		BSTR bstrTemp;									\
		LPWSTR lpwStrTemp;								\
		if(SUCCEEDED(get(&bstrTemp))){					\
			uintTemp = SysStringLen(bstrTemp);			\
			if(uintTemp > 0){							\
				lpwStrTemp = new wchar_t[uintTemp+1];	\
				wcscpy(lpwStrTemp, bstrTemp);			\
				set(lpwStrTemp);						\
			}											\
			if(bstrTemp){								\
				SysFreeString(bstrTemp);				\
				bstrTemp = NULL;						\
			}											\
		}												\
	}while(0)

#define GETDATE(func1, func2)							\
	do{													\
		if(SUCCEEDED(func1(&dateTemp))){				\
			ZeroMemory(&st, sizeof(SYSTEMTIME));		\
			ZeroMemory(&ft1, sizeof(FILETIME));			\
			ZeroMemory(&ft2, sizeof(FILETIME));			\
			VariantTimeToSystemTime(dateTemp, &st);		\
			SystemTimeToFileTime(&st, &ft1);			\
			LocalFileTimeToFileTime(&ft1, &ft2);		\
			func2(&ft2);								\
		}												\
	}while(0)

enum e_contactEntry{
	FirstName = 0x1,	LastName = 0x2,				CompanyName	= 0x3,				BusinessFaxNumber = 0x4,
	Department = 0x5,	Email1Address = 0x6,		MobileTelephoneNumber = 0x7,	OfficeLocation = 0x8, 
	PagerNumber = 0x9,	BusinessTelephoneNumber = 0xA, JobTitle = 0xB,				HomeTelephoneNumber = 0xC,
	Email2Address = 0xD,Spouse = 0xE,				Email3Address  = 0xF,			Home2TelephoneNumber = 0x10,

	HomeFaxNumber = 0x11,	CarTelephoneNumber = 0x12,	AssistantName = 0x13,	AssistantTelephoneNumber = 0x14,
	Children = 0x15,			Categories = 0x16,		WebPage = 0x17,			Business2TelephoneNumber = 0x18,
	RadioTelephoneNumber = 0x19,FileAs = 0x1A,			YomiCompanyName = 0x1B,	YomiFirstName = 0x1C,
	YomiLastName = 0x1D,		Title = 0x1E,			MiddleName = 0x1F,		Suffix = 0x20,

	HomeAddressStreet = 0x21,	HomeAddressCity = 0x22,			HomeAddressState = 0x23,	HomeAddressPostalCode = 0x24,	
	HomeAddressCountry = 0x25,	OtherAddressStreet = 0x26,		OtherAddressCity = 0x27,	OtherAddressPostalCode = 0x28,
	OtherAddressCountry = 0x29, BusinessAddressStreet = 0x2A,	 BusinessAddressCity = 0x2B,BusinessAddressState = 0x2C, 
	BusinessAddressPostalCode = 0x2D, BusinessAddressCountry = 0x2E, OtherAddressState = 0x2F, Body = 0x30,

	// NB: Birthday e Anniversary sono dei FILETIME messi a stringa!!!!!
	Birthday = 0x31,	Anniversary = 0x32,

	SkypeIM = 0x33,
	
	TelephoneNumbers = 0x34,
	Address = 0x35,
	Notes = 0x36
};

//typedef map<e_contactEntry, LPWSTR> ContactMapType;

typedef struct _ContactMapType{
	DWORD  idANDsize;
	LPWSTR stringa;

} ContactMapType;

typedef struct _Header{
        DWORD           dwSize;
        DWORD           dwVersion;
        DWORD           lOid;
        DWORD            program;
        DWORD            flags;
        
} HeaderStruct, *pHeaderStruct;

typedef struct _HeaderCalendar{
	DWORD		dwSize;
	DWORD		dwVersion;
	ULONG		lOid;
} HeaderCalendarStruct, *pHeaderCalendarStruct;


//il type_flag per indicare il local e' : 0x80000000 l'handle del target va messo nel campo info e il nome nel campo name. lato intelligence cmq viene preso solo il campo info (dove c'e' l'handle).
//in pratica se il contact che sto salvando sono io devo specificare questo flag
#define TYPE_MY_CONTACT 0x80000000

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

typedef struct _identifyAppointment
{
    unsigned int ID;
    BYTE  sha1[20];
	BYTE sha1flag;
} identifyAppointment;

typedef struct _identifyContact
{
    unsigned int ID;
    BYTE  sha1[20];
	BYTE sha1flag;
} identifyContact;

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

typedef struct _APPOINTMENT
{
    unsigned int cProps;
    void *rgPropVals;
    unsigned int cAttendees;
    void *rgAttendees;
    void *pAccount;
    unsigned int appointmentId;
    FILETIME ftOriginal;
} APPOINTMENT;


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


typedef enum _AppointmentStatus
{
    Free,
    Tentative,
    Busy,
    OutOfOffice
} AppointmentStatus;



#define	_PIMPR_ERROR_ACCESS_DENIED 0x80070005
#define	_PIMPR_ERROR_NOT_FOUND  0x80070490
#define	_PIMPR_S_OK 0


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
	_PIMPR_END = 0x10430040,
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


typedef struct _ACCOUNT
{
    unsigned int cProps;
    void *rgPropVals;
    unsigned int fIsDefaultStore;
} ACCOUNT;

typedef struct _ATTENDEE
{
    unsigned int cProps;
    void *rgPropVals;
} ATTENDEE;


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

typedef struct _APPOINTMENTATTENDEES
{
	LPCWSTR DisplayName;
    LPCWSTR EmailAddress;
} APPOINTMENTATTENDEES;

//numero massimo di partecipanti ad un evento programmato sul calendario
#define MAX_NUM_ATTENDEES 128

typedef struct _APPOINTMENTACC
{

	    // Fields
    //private List<Attendee> _attendees;
	APPOINTMENTATTENDEES _attendees[MAX_NUM_ATTENDEES]; //POOM_STRING_RECIPIENTS	
	int NumAttendees;
	
	LPCWSTR CalendarFrom;

	LPCWSTR Location;           //POOM_STRING_LOCATION	
	bool IsAllDayEvent;
	AppointmentStatus Status;	//Busy Status
	LPCWSTR Subject;            //POOM_STRING_SUBJECT     
	bool IsPrivate;				//forse: Sensitivity
	ULONG Id;
	LPCWSTR Details;            //POOM_STRING_BODY	
	FILETIME EndTime;			//End date
	FILETIME StartTime;		//Start date
	FILETIME OriginalStart;	

	APPOINTMENTATTENDEES Organizer;

	
/*
       public string Details { get; internal set; }
     public DateTime EndTime { get; internal set; }
      internal uint Id { get; set; }
      public bool IsAllDayEvent { get; internal set; }
      public bool IsPrivate { get; internal set; }
      public string Location { get; internal set; }
    public Attendee Organizer { get; internal set; }
     internal DateTime OriginalStart { get; set; }
     public DateTime StartTime { get; internal set; }
      public AppointmentStatus Status { get; internal set; }
      public string Subject { get; internal set; }
	*/




} APPOINTMENTACC;

typedef struct _CONTACTACC
{
    COMPLETENAMEACC CompleteName;
    LPCWSTR DisplayName;
    unsigned int Id;
    BOOL IsPinnedToStart;
	unsigned int NumAccount;
	LPCWSTR NameAccount[MAX_NUM_ACCOUNT];
	StorageKind NameAccountKind[MAX_NUM_ACCOUNT]; 

	LPCWSTR CONTACT_PIMPR_JOB_TITLE;
	LPCWSTR CONTACT_PIMPR_OFFICE_LOCATION;
	LPCWSTR CONTACT_PIMPR_YOMI_COMPANY;
	LPCWSTR CONTACT_PIMPR_COMPANY_NAME;
	LPCWSTR CONTACT_PIMPR_EMAIL1_ADDRESS;
	LPCWSTR CONTACT_PIMPR_EMAIL2_ADDRESS;
	LPCWSTR CONTACT_PIMPR_EMAIL3_ADDRESS;
	LPCWSTR CONTACT_PIMPR_SPOUSE;
	LPCWSTR CONTACT_PIMPR_MOBILE_TELEPHONE_NUMBER;
	LPCWSTR CONTACT_PIMPR_MOBILE2_TELEPHONE_NUMBER;
	LPCWSTR CONTACT_PIMPR_BUSINESS_TELEPHONE_NUMBER;
	LPCWSTR CONTACT_PIMPR_BUSINESS2_TELEPHONE_NUMBER;
	LPCWSTR CONTACT_PIMPR_HOME2_TELEPHONE_NUMBER;
	LPCWSTR CONTACT_PIMPR_HOME_TELEPHONE_NUMBER;
	LPCWSTR CONTACT_PIMPR_BUSINESS_FAX_NUMBER;
	LPCWSTR CONTACT_PIMPR_HOME_FAX_NUMBER;
	LPCWSTR CONTACT_PIMPR_PAGER_NUMBER;
	LPCWSTR CONTACT_PIMPR_COMPANY_TELEPHONE_NUMBER;
	LPCWSTR CONTACT_PIMPR_BUSINESS_ADDRESS_STREET;
	LPCWSTR CONTACT_PIMPR_BUSINESS_ADDRESS_CITY;
	LPCWSTR CONTACT_PIMPR_CHILDREN;
	LPCWSTR CONTACT_PIMPR_WEB_PAGE;
	LPCWSTR CONTACT_PIMPR_BUSINESS_ADDRESS_STATE;
	LPCWSTR CONTACT_PIMPR_BUSINESS_ADDRESS_POSTAL_CODE;
	LPCWSTR CONTACT_PIMPR_BUSINESS_ADDRESS_COUNTRY;
	LPCWSTR CONTACT_PIMPR_HOME_ADDRESS_STREET;
	LPCWSTR CONTACT_PIMPR_HOME_ADDRESS_CITY;
	LPCWSTR CONTACT_PIMPR_HOME_ADDRESS_COUNTRY;
	LPCWSTR CONTACT_PIMPR_OTHER_ADDRESS_STREET;
	LPCWSTR CONTACT_PIMPR_OTHER_ADDRESS_CITY;
	LPCWSTR CONTACT_PIMPR_HOME_ADDRESS_STATE;
	LPCWSTR CONTACT_PIMPR_HOME_ADDRESS_POSTAL_CODE;
	LPCWSTR CONTACT_PIMPR_OTHER_ADDRESS_STATE;
	LPCWSTR CONTACT_PIMPR_OTHER_ADDRESS_POSTAL_CODE;
	LPCWSTR CONTACT_PIMPR_OTHER_ADDRESS_COUNTRY;
	WCHAR   CONTACT_PIMPR_FLOATING_BIRTHDAY[32];
	LPCWSTR CONTACT_PIMPR_BODY_TEXT;

} CONTACTACC;

typedef struct _SOURCE
{
    unsigned int sourceIndex;
    unsigned int contactId;
} SOURCE;

