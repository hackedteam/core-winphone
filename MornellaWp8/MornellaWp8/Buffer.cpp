/*
	Buffer Management Class v0.1
	Date: 27/Jan/2011
	Coded by: Alberto "Quequero" Pelliccione
	E-mail: quequero@hackingteam.it
*/

#include "Buffer.h"

Buffer::Buffer() : p(0), p_size(0), p_pos(0), p_mark(0) {

}

Buffer::Buffer(unsigned int u_size) : p(0), p_size(0), p_pos(0), p_mark(0) {
	if (p || u_size == 0)
		return;

	p = new(std::nothrow) unsigned char[u_size];

	if (p)
		p_size = u_size;

	zero();
}

Buffer::Buffer(unsigned char *b, unsigned int u_size) : p(0), p_size(0), p_pos(0), p_mark(0) {
	if (p || b == 0 || u_size == 0)
		return;

	p = new(std::nothrow) unsigned char[u_size];

	if (p)
		p_size = u_size;

	for (unsigned int i = 0; i < u_size; i++)
		p[i] = b[i];
}

Buffer::~Buffer() {
	if (!p)
		return;

	zero();

	delete[] p;
}

bool Buffer::setSize(unsigned int u_size) {
	if (p)
		delete[] p;

	p = new(std::nothrow) unsigned char[u_size];

	if (p)
		return true;

	return false;
}

bool Buffer::setPos(unsigned int u_pos) {
	if (u_pos > p_size)
		return false;

	p_pos = u_pos;
	return true;
}

unsigned int Buffer::getSize() {
	return p_size;
}

unsigned int Buffer::getPos() {
	return p_pos;
}

bool Buffer::ok() {
	if (p_size)
		return true;

	return false;
}

void Buffer::free() {
	if (p_size == 0)
		return;

	zero();
	delete[] p;

	p_size = p_pos = p_mark = 0;
	p = 0;
}

void Buffer::zero() {
	if (p_size == 0)
		return;

	for (unsigned i = 0; i < p_size; i++)
		p[i] = 0;
}

bool Buffer::append(unsigned char *s, unsigned int u_len) {
	if (p_pos + u_len <= p_size) {
		memcpy(p + p_pos, s, u_len);
		p_pos += u_len;
		return true;
	}

	unsigned int i, j;

	unsigned char *t = new(std::nothrow) unsigned char[p_pos + u_len];

	if (t == 0)
		return false;

	for (i = 0; i < p_pos + u_len; i++)
		t[i] = 0;

	// new size
	p_size = p_pos + u_len;

	for (i = 0; i < p_pos; i++)
		t[i] = p[i];

	// append
	for (i = p_pos, j = 0; j < u_len; i++, j++)
		t[i] = s[j];

	delete[] p;

	p = t;
	p_pos += u_len;

	return true;
}

bool Buffer::repeat(unsigned char c, unsigned int times) {
	unsigned int i, j;

	if (p_pos + times <= p_size) {
		for (i = p_pos; i < p_pos + times; i++)
			p[i] = c;

		p_pos += times;
		return true;
	}

	unsigned char *t = new(std::nothrow) unsigned char[p_pos + times];

	if (t == 0)
		return false;

	for (i = 0; i < p_pos + times; i++)
		t[i] = 0;

	// new size
	p_size = p_pos + times;

	for (i = 0; i < p_pos; i++)
		t[i] = p[i];

	// append
	for (i = p_pos, j = 0; j < times; i++, j++)
		t[i] = c;

	delete[] p;

	p = t;
	p_pos += times;

	return true;
}

bool Buffer::mark(unsigned int m) {
	if (m > p_pos)
		return false;

	p_mark = m;
	return true;
}

void Buffer::mark() {
	p_mark = p_pos;
}

unsigned int Buffer::getMark() {
	return p_mark;
}

void Buffer::clear() {
	p_mark = 0;
}

void Buffer::end() {
	p_pos = p_size;
}

void Buffer::begin() {
	p_pos = 0;
}

const unsigned char* Buffer::getBuf() {
	return p;
}

const unsigned char* Buffer::getCurBuf() {
	return p + p_pos;
}

unsigned int Buffer::getInt() {
	if (p_pos > p_size - sizeof(int))
		return 0;

	unsigned int val;

	memcpy(&val, &p[p_pos], sizeof(unsigned int));

	p_pos += sizeof(int);

	return val;
}

bool Buffer::read(unsigned char *b, unsigned int u_len) {
	if (p_pos + u_len > p_size)
		return false;

	for (unsigned int i = 0; i < u_len; i++)
		b[i] = p[p_pos + i];

	p_pos += u_len;

	return true;
}

const unsigned char* Buffer::read(unsigned int i) {
	if (i > p_size)
		return 0;

	return p + i;
}

const unsigned char* Buffer::getMarkBuf() {
	return p + p_mark;
}

void Buffer::reset() {
	p_pos = 0;
}