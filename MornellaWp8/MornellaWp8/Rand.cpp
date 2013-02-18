/* Coded by Alberto Pelliccione
alberto@bitchx.it

1. Create an object:
- Rand r; // Initializations will be done internally
OR
- Rand r(some_value1, some_value2);
some_value1 and some_value2 are random unsigned _short_ integers,
if you don't have a random value, don't use this method, the
first one will take care of every initialization.

2. Get your random number
- int a = r.rand24(); // Will return a 24bit pseudo-random integer
- int a = r.rand32(); // Will return a 32bit pseudo-random integer
- double a = r.random(2.0); // Will return a double within the range -2.0/2.0
- double a = r.random(0.5); // Will return a double within the range -0.5/0.5
- double a = r.random(1.0); // Will return a double within the range -1.0/1.0
- double a = r.frandom(1.0); // Will return a double within the range -1.0/1.0 (faster)
- unsigned long a = r.rand_mod(6); // Will return an unsigned long between 0 and 6 - 1

3. Have fun :)
*/

#include "Rand.h"
#include <memory.h>
#include <windows.h>
#include "FunctionFunc.h"

Rand::Rand() :
state_size(97), p(179), pm1(179 - 1), q(179 - 10),
init_c(362436), cd(7654321), cm(16777213), two_to_24(0x01000000),
init_done(0)
{
	
	FILETIME ft;
	SYSTEMTIME st;
	
	UINT seed;
	
	GetSystemTime(&st);
	
	SystemTimeToFileTime(&st, &ft);
	
	seed = ft.dwLowDateTime ^ _GetTickCount();

	init_rand((USHORT)(seed & 0xFFFF0000 >> 16), (USHORT)(seed & 0x0000FFFF));
	
}

Rand::Rand(unsigned short a, unsigned short b) :
state_size(97), p(179), pm1(179 - 1), q(179 - 10),
init_c(362436), cd(7654321), cm(16777213), two_to_24(0x01000000),
init_done(0), sa(a), sb(b)
{
	init_rand(sa, sb);
}

unsigned short Rand::col(short anyint, unsigned short size)
{
	short int i = anyint;

	if (i < 0)
		i = -(i >> 2);

	while (i >= size)
		i = i >> 2;

	return i;
}



void Rand::init_rand(unsigned short int seed_a, unsigned short int seed_b)
{
	unsigned long int s, bit;
	unsigned short int ii, jj, kk, mm;
	unsigned short int ll;
	short int sd, ind;

	sd = col(seed_a, pm1 * pm1);
	ii = 1 + sd / pm1;
	jj = 1 + sd % pm1;

	sd = col(seed_b, pm1 * q);
	kk = 1 + sd / pm1;
	ll = sd % q;

	if (ii == 1 && jj == 1 && kk == 1)
		ii = 2;

	for (ind = 0; (unsigned short int)ind < state_size; ind++){
		s = 0;
		bit = 1;

		do{
			mm = (((ii * jj) % p) * kk) % p;

			ii = jj;
			jj = kk;
			kk = mm;

			ll = (53 * ll + 1) % q;

			if ((ll * mm) & 0x0020)
				s += bit;

			bit <<= 1;
		}while (bit < two_to_24);

		u[ind] = s;
	}

	index_i = state_size - 1;
	index_j = state_size / 3;
	c = init_c;

	init_done = 1;
}

// Return uniformly distributed pseudo random numbers in the range
// 0..2^(24)-1 inclusive. There are 2^24 possible return values.
unsigned long Rand::rand24()
{
	unsigned long int temp;

	if (!init_done)
		init_rand(sa, sb);

	c = (c < cd ? c + (cm - cd) : c - cd);

	temp = (u[index_i] -= u[index_j]);

	if (!index_i--)
		index_i = state_size - 1;

	if (!index_j--)
		index_j = state_size - 1;

	return (temp - c) & (two_to_24 - 1);
}

// Return uniformly distributed pseudo random number in the range
// 0..2^(32)-1 inclusive. There are 2^32 possible return values.
unsigned long Rand::rand32()
{
	unsigned short int i, val;

	if (!bno) {
		for (i = 0; i < 12; i += 2) {
			val = (unsigned short int)rand24();
			memcpy(buf + i, &val, 2);
		}
		bno = 12;
	}
	bno -= 4;

	return *(unsigned long *) (buf + 8 - bno);
}

