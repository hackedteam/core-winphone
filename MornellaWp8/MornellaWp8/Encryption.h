#include "Common.h"
#include "AES.h"
#include <exception>
using namespace std;

#ifndef __Encryption_h__
#define __Encryption_h__

class Encryption;

class Encryption
{
	/**
	* Context per il motore di AES
	*/
	private: aes_context Aes_ctx;

	/**
	* IV utilizzato per AES.
	*/
	private: BYTE IV[16];

	/**
	* Copia della chiave
	*/
	private: BYTE aesKey[32];

	/**
	* Lunghezza chiave
	*/
	private: UINT aesBitKeyLen;

	/**
	* Costruttore di default, inizializza l'IV ed il context per AES.
	* La lunghezza della chiave va espressa in bit (128/192/256);
	*/
	public: Encryption(BYTE *pKey, UINT uKeyBitLen);
			~Encryption();
	/**
	 * Resetta il context di AES
	 */
	public: void Reset();

	/**
	 * Questa funzione prende pInFile lo cifra e lo scrive sul filesystem col nome di pOutFile (il nome dovrebbe
	 * essere scramblato). Type rappresenta il tipo di Log scritto. Torna TRUE se il file e' stato cifrato 
	 * e generato con successo, FALSE altrimenti.
	 */
	//public: BOOL EncryptFile(WCHAR* pInFile, WCHAR* pOutFile, UINT Type);

	/**
	* Cifra i dati puntati da pIn e torna un puntatore. Len rappresenta la lunghezza dei dati da cifrare,
	* al termine della routine Len contiene la lunghezza dei dati contenuti nel buffer ritornato. La funzione
	* torna NULL se la cifratura non va a buon fine. Il buffer ritornato, se valido, va liberato dal chiamante.
	*/
	public: BYTE* EncryptData(BYTE *pIn, UINT *Len);

	/**
	* Deifra i dati puntati da pIn e torna un puntatore. Len rappresenta la lunghezza dei dati da decifrare,
	* al termine della routine Len contiene la lunghezza dei dati contenuti nel buffer ritornato. La funzione
	* torna NULL se la decifratura non va a buon fine. Il buffer ritornato, se valido, va liberato dal chiamante.
	*/
	public: BYTE* DecryptData(BYTE *pIn, UINT *Len);

	/**
	 * Questa funzione decifra il SOLO file di configurazione puntato da pInFile, se la chiamata va a buon
	 * fine, torna un puntatore ai byte decifrati (che va poi liberato!) e in uLen la lunghezza del file.
	 * In caso di errore torna NULL.
	 */
	public: BYTE* DecryptConf(wstring &strInFile, UINT *uLen);

	/**
	 * Scrambla una stringa, torna il puntatore al nome scramblato. La stringa ritornata va liberata dal chiamante
	 * con una free()!!!!
	 */
	public: WCHAR* EncryptName(wstring &strName, BYTE seed);

	/**
	* Descrambla una stringa, torna il puntatore al nome descramblato. La stringa ritornata va liberata dal chiamante
	* con una free()!!!!
	 */
	public: WCHAR* DecryptName(wstring &strName, BYTE seed);

	/**
	 * Questa funzione scrambla/descrambla una stringa e ritorna il puntatore alla nuova stringa.
	 * Il primo parametro e' la stringa da de/scramblare, il secondo UN byte di seed, il terzo se
	 * settato a TRUE scrambla, se settato a FALSE descrambla.
	 */
	private: WCHAR* Scramble(WCHAR* wName, BYTE seed, BOOL enc);

	/**
	* Torna il prossimo multiplo di 16 del numero Number, viene utilizzato per AES. Torna 0 in caso di errore.
	*/
	public: UINT GetNextMultiple(UINT Number);

	/**
	* Torna la lunghezza di un blocco dati paddato con PKCS5.
	*/
	public: static UINT GetPKCS5Len(UINT uLen);

	/**
	 *Torna la lunghezza del padding PKCS5 per un blocco di dati.
	*/
	public: static UINT GetPKCS5Padding(UINT uLen);

};

#endif
