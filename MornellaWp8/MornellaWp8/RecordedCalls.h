#ifndef __RecordedCalls_h__
#define __RecordedCalls_h__

using namespace std;

#define LOCAL_CALL_DEF_BUFFER_LEN (100*1024)
#define LOCAL_CALL_DEF_COMPRESS_FACTOR 8
#define LOCAL_CALL_COUNT_BUFF 2
#define LOCAL_CALL_SAMPLE_SIZE 2
#define PHONE_NUMBER_LEN 32
#define CALL_SAMPLE_RATE 8000L

typedef struct _VoiceAdditionalData {
#define CALL_LOG_VERSION (UINT)2008121901
	UINT uVersion;
	UINT uChannel;
	UINT uProgramType;
	UINT uSampleRate;
	UINT uIngoing;
	FILETIME start;
	FILETIME stop;
	UINT uCallerIdLen;
	UINT uCalleeIdLen;
} VoiceAdditionalData, *pVoiceAdditionalData;

extern DWORD WINAPI RecordedCalls(LPVOID lpParam);

#endif

