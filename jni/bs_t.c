#include <stdlib.h>
#include "bs_t.h"
#include "debug.h"

// Create bs instance.
bs_t* bs_new()
{
	LOGPOS();

	bs_t* new_bs = (bs_t*)malloc(sizeof(bs_t));
	if (new_bs == NULL) {
		LOGE("Error: Memory allocate error.");
		return NULL;
	}

	new_bs->data = NULL;
	new_bs->len = 0;
	new_bs->cap = 0;

	return new_bs;
}

// Intilize the bs to specify capacity.
errno_t bs_init(bs_t* bs, uint32_t cap)
{
	LOGPOS();

	if (cap > CPT_LIMITMEM) {
		return CPT_ERROR;
	}

	bs->data = (byte_t*)malloc(cap * sizeof(byte_t));
	if (bs->data == NULL) {
		LOGW("Error: Memory allocate error.");
		return CPT_ERROR;
	}

	bs->len = 0;
	bs->cap = cap;

	return CPT_OK;
}

// Copy the "bytes" and set to the byte string.
errno_t bs_assign(bs_t* bs, byte_t* bytes, uint32_t len)
{
	errno_t err;

	LOGPOS();

	if (len > CPT_LIMITMEM) {
		return CPT_ERROR;
	}
	
	if (bs->len < len) {
		err = bs_resize(bs, len+1);
		if (err != CPT_OK)
			return err;
	}

	strncpy(bs->data, bytes, len);
	bs->len = len;

	return CPT_OK;
}

// Cut the "bytes" and set to the byte string.
void bs_ref(bs_t *bs, byte_t* bytes, uint32_t len)
{
	LOGPOS();

	bs->data = bytes;
	bs->len = len;
}

// Compare two strings.
int32_t bs_cmp(bs_t *arg1, bs_t *arg2)
{
	int i = 0;
	byte_t *p1, *p2;

	LOGPOS();

	if (arg1->len != arg2->len)
		return arg1->len - arg2->len;

	i  = arg1->len-2;
	p1 = arg1->data;
	p2 = arg2->data;

	while (i > 0 && *p1 == *p2) {
		i--; p1++; p2++;
	}

	return *p1 - *p2;
}

// Reszie the byte string.
errno_t bs_resize(bs_t* bs, uint32_t new_size)
{
	byte_t *new_data;

	LOGPOS();

	if (new_size > CPT_LIMITMEM) {
		return CPT_ERROR;
	}
	
	new_data = (byte_t*)malloc((new_size+1) * sizeof(byte_t));
	if (new_data == NULL) {
		return CPT_ERROR;
	}

	bs->len = CPT_MIN(new_size, bs->cap);
	bs->cap = new_size+1;

	strncpy(new_data, bs->data, bs->len);
	free(bs->data);
	bs->data = new_data;

	return CPT_OK;
}

// Reserve "new_cap" for future use.
errno_t bs_reserve(bs_t* bs, uint32_t new_cap)
{
	byte_t *new_data;

	LOGPOS();

	if (new_cap > CPT_LIMITMEM) {
		return CPT_ERROR;
	}

	if (new_cap > bs->cap) {
		new_data = (byte_t*)malloc(new_cap * sizeof(byte_t));
		if (new_data == NULL)
			return CPT_ERROR;
	
		strncpy(new_data, bs->data, bs->len);
		free(bs->data);
		bs->data = new_data;
	}

	return CPT_OK;
}

// Destory the byte string object.
void bs_destroy(bs_t* bs)
{
	LOGPOS();

	if (bs != NULL) {
		if (bs->data != NULL) {
			free(bs->data);
		}
		free(bs);
	}
}
