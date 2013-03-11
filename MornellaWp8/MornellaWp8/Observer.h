#pragma once

#include "Common.h"
#include "Common_new.h"
#include "Conf.h"

#define EVENT_NOT_INTERESTING (INT)-1

typedef struct _Listeners {
	UINT uTid;
	BOOL bProcessed;
	BOOL bDispatched;
} Listeners, *pListeners;

// Classe observer per il dispatch dei messaggi dalla coda IPC, e' singleton.
class Observer { 
private:
	static Observer *Instance;
	static volatile LONG lLock;

	HANDLE hQueueRead, hQueueWrite, hObserverMutex;
	INT m_iQueueCommand;
	BOOL bRefresh;
	list<Listeners> *pRegistered;
	MSGQUEUEOPTIONS qRead, qWrite;
	BYTE *pQueue;
	DWORD dwQueueBuf;
	DWORD dwMsgLen;
	IpcMsg *m_pIpcMsg;

private:
	/**
	 * Legge dalla coda in attesa in attesa di un messaggio, il timeout e' di un secondo. Se il messaggio
	 * viene letto dwMsgLen contiene la sua lunghezza completa, altrimenti e' 0. In caso di errore o di 
	 * messaggio non trovato, la funzione ritorna FALSE. Questa funzione effettua la lettura solo se
	 * bRefresh e' impostato a TRUE. Questo flag viene impostato dalla Dispatch() quando tutti i listener
	 * hanno markato il messaggio.
	 */
	BOOL ReadQueue();

	/**
	 * Invia un messaggio sulla coda grande 4 byte + sizeof(IpcMsg), il messaggio e' rappresentato da
	 * m_iQueueCommand che viene impostato dalla MarkMessage(). Sender e Receiver sono impostati ai valori
	 * contenuti nel messaggio letto. La funzione torna TRUE in caso di scrittura avvenuta con successo,
	 * FALSE altrimenti.
	 */
	BOOL WriteQueue();

	/**
	 * Imposta i flag bProcessed e bDispatched a FALSE per tutti i listener registrati. Viene chiamata
	 * prima di effettuare un nuovo ciclo di read della coda. Torna TRUE in caso di successo, FALSE
	 * altrimenti.
	 */
	BOOL ResetList();

	/**
	 * Se tutti i listener hanno markato il messaggio questa funzione scrive il comando nella coda,
	 * resetta la lista ed imposta bRefresh a TRUE. Altrimenti ritorna TRUE se la funzione va a buon
	 * fine, FALSE altrimenti.
	 */
	BOOL Dispatch();

	~Observer();

	/**
	* Apre la coda creata da Task(), crea il mutex e alloca il buffer pQueue utilizzato
	* per il processing dei messaggi. La dimensione di pQueue e' fissata ed e' definita dalla
	* dimensione massima stabilita da Task() all'atto della creazione della coda.
	*/
	Observer();

public:
	/**
	* Observer e' una classe singleton
	*/
	static Observer* self();

	/**
	 * Istanzia la lista pRegistered se non esiste ed aggiunge il thread il cui TID e' definito da
	 * uTid alla lista dei listeners. Torna TRUE in caso di successo, FALSE altrimenti.
	 */
	BOOL Register(UINT uTid);

	/**
	 * Rimuove un Listener dalla coda e la ridimensiona, torna FALSE solo se la lista non esiste.
	 */
	BOOL UnRegister(UINT uTid);

	/**
	 * Torna un puntatore al messaggio ricevuto se c'e', NULL altrimenti.
	 */
	const BYTE* GetMessage();

	/**
	 * Ogni listener DEVE chiamare questa funzione dopo la GetMessage(), uTid rappresenta il TID del
	 * listener chiamante, uCommand il comando da inviare alla coda e bProcessed lo stato del messaggio.
	 * Se bProcessed e' TRUE significa che il listener ha processato ed utilizzato il messaggio, quindi
	 * la classe valorizza m_iQueueCommand prendendolo da uCommand e invia il messaggio alla coda. Altrimenti
	 * uCommand viene ignorato. La WriteQueue() viene chiamata solo dopo che TUTTI i listener hanno
	 * markato il messaggio come processato/non-processato.
	 */
	BOOL MarkMessage(UINT uTid, UINT uCommand, BOOL bProcessed);
};