
#include <windows.h>

class CPoomAddressbook
{
	private:
		static CPoomAddressbook* m_pInstance;
		static volatile LONG lLock;

		void HandleMultiValuedProperties(unsigned int cAggregatedProps,void *rgAggregatedPropVals, CONTACTACC* contact);
		void AddSingleValuePropertyToContact(CEPROPVAL *v,CONTACTACC* contact);
		DWORD _SerializedStringLength(LPCWSTR lpString);
		DWORD _Prefix(DWORD dwLength, int entryType);
		void _SerializeString(LPBYTE *lpDest, LPCWSTR lpString, int entryType);
		

	public:
		CPoomAddressbook();
		~CPoomAddressbook();
		static CPoomAddressbook* self();
		//void  GetContact(void);
};