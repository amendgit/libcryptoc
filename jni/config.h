#ifndef CPT_CONFIG_H
#define CPT_CONFIG_H

#include <stddef.h>
#include <sys/types.h>

// 8bit binary type.
typedef unsigned char  byte_t;

// 8bit character type.
typedef char           char_t;

// 32bit integer types.
typedef signed int     int32_t;
typedef unsigned int   uint32_t;

// float type (32bit IEEE754).
typedef float          real_t;

// double type (64bit IEEE754).
typedef double         double_t;

// charactor-code type (16bit).
typedef  uint16_t      cid_t;
typedef  uint16_t      unicode_t;

// boolean type (0 : false, !0 : true).
typedef int32_t        bool_t;

#define CPT_TRUE 1
#define CPT_FALSE 0

#define CPT_MIN(x, y) (x < y ? x : y)
#define CPT_MAX(x, y) (x > y ? x : y)

// The limit memory fot malloc each time. In case of Integer Overflows attack.
#define CPT_LIMITMEM (1024 * 1024)

#endif // CPT_CONFIG_H