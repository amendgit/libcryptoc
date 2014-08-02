#include "util.h"

#include "debug.h"

// uint32 to array of octets, Enddian dependness.
void u32_to_ao(uint32_t u32, byte_t ao[4])
{
    int32_t i;

    LOGPOS();

    for (i = 0; i < sizeof(uint32_t); i++) {
        ao[i] = (byte_t)u32 & 0xff;
        u32 >>= 8;
    }
}

// array of octets to uint32, Enddian dependness.
void ao_to_u32(byte_t ao[4], uint32_t *u32)
{
    int32_t i;

    LOGPOS();
    
    for (i = sizeof(uint32_t) - 1; i >= 0; i--) {
        (*u32) = ((*u32) << 8) | ao[i];
    }
}