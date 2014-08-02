#ifndef CPT_UTIL_H
#define CPT_UTIL_H

#include "config.h"

void u32_to_ao(uint32_t u32, byte_t ao[4]);

void ao_to_u32(byte_t ao[4], uint32_t *u32);

#endif // CPT_UTIL_H