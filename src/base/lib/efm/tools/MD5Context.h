#ifndef EFEU_MD5Context_h
#define EFEU_MD5Context_h

#include <EFEU/config.h>
#include <EFEU/stdint.h>

typedef struct {
	uint32_t buf[4];
	uint32_t bits[2];
	unsigned char in[64];
} MD5Context;

void MD5Context_init (MD5Context *ctx);
void MD5Context_update (MD5Context *ctx, unsigned char const *buf, size_t len);
void MD5Context_final (unsigned char digest[16], MD5Context *ctx);

#endif /* EFEU/MD5Context.h */
