#ifndef CPT_RC4_H
#define CPT_RC4_H

#include "config.h"
#include "bs_t.h"

/* Type RC4 */
typedef struct rc4_s rc4_t;
struct rc4_s
{
	byte_t  s[256];
	uint8_t i,j;
};

rc4_t*  rc4_new(bs_t* key);

errno_t rc4_xor(rc4_t* r, bs_t *src, bs_t *dst);

void    rc4_reset(rc4_t* r);

void    rc4_destroy(rc4_t* r);

#endif 