#ifndef __Hash_h__
#define __Hash_h__

#include "Sha1.h"
#include <Windows.h>

class Hash {
private: 
	SHA1Context shcxt;

public:
	Hash();
	~Hash();

	/**
	* Implementazione interna della ntohl per il byte flipping.
	*/
	UINT ntohl(UINT x);

	/**
	 * Inizializza l'engine di Sha1, primo step di un multi-pass hash
	 */
	void Sha1Init();

	/**
	 * Passa un buffer pInData lungo len che man mano riceve i byte
	 * su cui calcolare l'hash
	 */
	void Sha1Update(const unsigned char* pInData, unsigned int len);

	/**
	 * Torna in pSha (passato per riferimento) il risultato dell'hash
	 */
	void Sha1Final(unsigned int pSha[5]);

	/**
	 * Funzione single-pass per la generazione di un hash a 160-bit. 
	 * La funzione torna void, l'hash viene passato per riferimento
	 * ed inserito nel buffer pSha.
	 */
	void Sha1(const unsigned char* pInData, const unsigned int len, unsigned char pSha[20]);
};

#endif 