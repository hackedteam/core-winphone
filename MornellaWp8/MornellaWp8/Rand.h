#ifndef __RAND_H
#define __RAND_H

#include <Winbase.h>
#include <stdlib.h>

#define MAX_32BIT_NUM 4294967295 // 2^32-1
#define MAX_24BIT_NUM 16777215   // 2^24-1

static unsigned char buf[13];    // buffer for conversion from 24 to 32 bit	
static unsigned short bno = 0;   // number of output values in buffer

class Rand{
public:
	Rand();
	Rand(unsigned short int a, unsigned short int b);

	// Generate 24bit pseudo-random number
	unsigned long rand24();

	// Generate 32bit pseudo-random number
	unsigned long rand32();

private:
	const unsigned int state_size;
	const unsigned short int p, pm1, q;
	const unsigned long int init_c, cd, cm, two_to_24;
	unsigned short int index_i, index_j, sa, sb;
	unsigned long int u[97], c;
	int init_done;

	unsigned short col(short, unsigned short);
	void init_rand(unsigned short, unsigned short);
};

#endif
