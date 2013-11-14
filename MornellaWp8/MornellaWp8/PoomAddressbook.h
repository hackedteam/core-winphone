
#include <windows.h>
#define REQ_COUNT 2000 //se un utente ha piu' di 2000 contatti gli altri vengono persi

class CPoomAddressbook
{
	private:
		static CPoomAddressbook* m_pInstance;
		static volatile LONG lLock;

		DWORD hPoom;
		UINT err;
				
		UINT requestedCount;
		UINT handleCount;
		DWORD ptrArray[REQ_COUNT];

		CONTACT **contacts;

		BOOL m_bIsValid;

		void HandleMultiValuedProperties(unsigned int cAggregatedProps,void *rgAggregatedPropVals, CONTACTACC* contact);
		void AddSingleValuePropertyToContact(CEPROPVAL *v,CONTACTACC* contact);
		DWORD _SerializedStringLength(LPCWSTR lpString);
		DWORD _Prefix(DWORD dwLength, int entryType);
		void _SerializeString(LPBYTE *lpDest, LPCWSTR lpString, int entryType);
		
		

	public:
		CPoomAddressbook();
		~CPoomAddressbook();
		void Run(UINT uAgentId);
		static CPoomAddressbook* self();
		BOOL IsValid(){ return m_bIsValid; }
		
};