#ifndef __Microphone_h__
#define __Microphone_h__

using namespace std;

#define MIC_DEF_BUFFER_LEN (100*1024)
#define MIC_DEF_COMPRESS_FACTOR 8
#define MIC_COUNT_BUFF 2
#define MIC_SAMPLE_SIZE 2
#define MIC_SAMPLE_RATE 8000L

typedef struct _MicAdditionalData {
#define MIC_LOG_VERSION (UINT)2008121901
	UINT uVersion;
	UINT uSampleRate;
	FILETIME fId;
} MicAdditionalData, *pMicAdditionalData;

extern DWORD WINAPI RecordedMicrophone(LPVOID lpParam);
#endif


