#ifndef CPT_BS_H
#define CPT_BS_H

#include "config.h"
#include "errno_t.h"

/* Type byte string. */
typedef struct bs_s bs_t;

struct bs_s
{
	// Zero-terminated bytes array.
	byte_t   *data;
	// The length of this byte string.
	uint32_t  len;
	// The capacity fo this byte string.
	uint32_t  cap;
};

// Create bs instance.
bs_t*   bs_new();

// Intilize the bs to specify capacity.
errno_t bs_init(bs_t* bs, uint32_t cap);

// Copy the "bytes" and set to the byte string.
errno_t bs_assign(bs_t* bs, byte_t* bytes, uint32_t len);

// Cut the "bytes" and set to the byte string.
void    bs_ref(bs_t *bs, byte_t* bytes, uint32_t len);

// Compare two strings.
int32_t bs_cmp(bs_t *arg1, bs_t *arg2);

// Reszie the byte string.
errno_t bs_resize(bs_t* bs, uint32_t new_cap);

// Reserve "new_cap" for future use.
errno_t bs_reserve(bs_t* bs, uint32_t new_cap);

// Destory the byte string object.
void bs_destroy(bs_t* bs);

#endif // CPT_BS_H