#ifndef __Buffer_h__
#define __Buffer_h__

#include <new>

using namespace std;

class Buffer {
private:
	unsigned char *p;
	unsigned int p_size, p_pos, p_mark;

public:
	Buffer();
	Buffer(unsigned int u_size);
	Buffer(unsigned char *b, unsigned int u_size);
	~Buffer();

	bool setSize(unsigned int u_size);
	bool setPos(unsigned int u_pos);
	unsigned int getSize();
	unsigned int getPos();
	bool ok();
	void free();
	void zero();
	bool append(unsigned char *s, unsigned int u_len);
	bool repeat(unsigned char c, unsigned int times);
	bool mark(unsigned int m);
	void mark();
	unsigned int getMark();
	void clear();
	void end();
	void begin();
	const unsigned char* getBuf();
	const unsigned char* getCurBuf();
	const unsigned char* getMarkBuf();
	unsigned int getInt();
	bool read(unsigned char *b, unsigned int u_len);
	const unsigned char* read(unsigned int i);
	void reset();
};

#endif