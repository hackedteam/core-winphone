#pragma once

#include "Common.h"
#include <cmath>
#include "ril.h"
#include "Device.h"

/**
 *	<<singleton>>
 *	Cell
 *		Retrieve Phone position via RIL/GSM
 **/
class Cell
{
	public:
		static Cell *self(DWORD dwTimeout);
		BOOL	RadioReady();

		BOOL	getCELL(RILCELLTOWERINFO *);

		void	Stop();
		BOOL	Start();

	protected:
		Cell();	// .ctor

		static Cell *_instance;	// Global self
		
		/**
		* Inizializza l'oggetto, torna FALSE se l'inizializzazione fallisce.
		* dwTimeout e' in millisecondi e rappresenta il tempo massimo di attesa 
		* per ottenere una posizione valida.
		**/
		BOOL	Initialize(DWORD dwTimeout);
		BOOL	RefreshData();	// Try to refresh data

	private: ~Cell();	// .dtor

	private:
		DWORD	_dwTimeout;			// constructor parameters

		HANDLE	_hMutex;			// mutex
		INT		_iReference;		// Reference counter

		HRIL	_hRil;

		RILCELLTOWERINFO _rilCellTowerInfo;
		
		DWORD	_dwLastCell;		// time of last CELL
		HRESULT _cellRequestID;		// cellRequestID
		HANDLE	_hCellEvent;		// hCellEvent

		BOOL	bInitialized;

		Device *deviceObj;

		static	BOOL _bRadioReady;	// Radio is READY
		static volatile LONG lLock;	// Il nostro mutex

	private:
		static void _RIL_Callback(DWORD dwCode, HRESULT hrCmdID, const void* lpData, DWORD cbData, DWORD dwParam);
		static void _RIL_Notify(DWORD dwCode, const void* lpData, DWORD cbData, DWORD dwParam);

		void RIL_Callback(DWORD dwCode, HRESULT hrCmdID, const void* lpData, DWORD cbData);
		void RIL_Notify(DWORD dwCode, const void* lpData, DWORD cbData);
};