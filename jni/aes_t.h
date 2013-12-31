#ifndef CPT_AES_H
#define CPT_AES_H

#include "config.h"
#include "bs_t.h"

#define AES_ROUND 10
#define AES_KEYBITS 128

/* Type AES. */
typedef struct aes_s aes_t;

struct aes_s 
{
	// The mode will be 
	//  1 for encrypt,
	//  2 for decrypt,
	//  0 means not initialize.
	uint32_t mode;

	// We need 128bits for each round key. 
	byte_t   round_key[(AES_ROUND + 1) * (AES_KEYBITS / 8)];
};

// Create the aes instance.
aes_t*   aes_new();

// Setup round key for encrypt.
errno_t aes_setup_enckey(aes_t* aes, bs_t* key);

// Encrypt the buffer.
errno_t aes_enc_buf(aes_t* aes, bs_t* plain, bs_t* cipher);

// Setup round key for decrypt.
errno_t aes_setup_deckey(aes_t* aes, bs_t* key);

// Decrypt the buffer.
errno_t aes_dec_buf(aes_t* aes, bs_t* cipher, bs_t* plain);

// Destory the aes object.
void     aes_destroy(aes_t* aes); 

#endif // CRYAES_H