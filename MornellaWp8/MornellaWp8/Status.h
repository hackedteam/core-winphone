#include "Common.h"
#include <string>
#include <exception>
#include "JSON/JSON.h"

using namespace std;

#ifndef __Status_h__
#define __Status_h__

#define MAX_WAIT_TIME 1000 // Tempo massimo che attendiamo perche' un agente si fermi

class Status {
	private: UINT uCrisis;
	private: static Status *Instance;
	private: static volatile LONG lLock;

	/**
	* Costruttore di default
	*/
	private: Status();
	public: ~Status();

	/**
	* Status e' una classe singleton
	*/
	public: static Status* self();

	/**
	* Imposta a TRUE il flag di crisi.
	*/
	public: void StartCrisis(UINT uType);

	/**
	* Imposta a CRISIS_NONE il flag di crisi.
	*/
	public: void StopCrisis();

	/**
	 * Torna lo stato di crisi.
	 */
	public: UINT Crisis();
};

#endif
