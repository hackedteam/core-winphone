#include "Hash.h"

Hash::Hash() {

}

Hash::~Hash() {

}

UINT Hash::ntohl(UINT x) {
	UCHAR *s = (UCHAR *)&x;

	return (UINT)(s[0] << 24 | s[1] << 16 | s[2] << 8 | s[3]);
}

void Hash::Sha1Init() {
	SHA1Reset(&shcxt);
}

void Hash::Sha1Update(const unsigned char* pInData, unsigned int len) {
	SHA1Input(&shcxt, pInData, len);
}

void Hash::Sha1Final(unsigned int pSha[5]) {
	SHA1Result(&shcxt);

	memcpy(pSha, shcxt.Message_Digest, 20);
}

void Hash::Sha1(const unsigned char* pInData, const unsigned int len, unsigned char pSha[20]) {
	SHA1Context context;

	SHA1Reset(&context);
	SHA1Input(&context, pInData, len);
	SHA1Result(&context);

	for (int i = 0; i < 5; i++)
		context.Message_Digest[i] = ntohl(context.Message_Digest[i]);

	memcpy(pSha, context.Message_Digest, 20);
}