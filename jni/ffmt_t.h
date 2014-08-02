#ifndef CPT_FFMT_H
#define CPT_FFMT_H

#include "config.h"
#include "bs_t.h"
#include "iodev_t.h"

#define CPT_FFMT_VERSION 1

#define CPT_HEADER_NONE 0
#define CPT_HEADER_GUID 1
#define CPT_HEADER_VERSION 2
#define CPT_HEADER_FILENAME 32
#define CPT_HEADER_HMAC 33
#define CPT_HEADER_FILEKEY 34
#define CPT_HEADER_USERTYPE 64
#define CPT_HEADER_END 128

typedef struct ffmt_header_s ffmt_header_t;
struct ffmt_header_s {
    uint32_t       type;
    uint32_t       size;
    void          *data;
    ffmt_header_t *next;
};

ffmt_header_t *ffmt_header_new();

void ffmt_header_alloc(ffmt_header_t *header, uint32_t len, uint32_t align);

void ffmt_header_destroy(ffmt_header_t *header);

typedef struct ffmt_s ffmt_t;
struct  ffmt_s {
    ffmt_header_t *headers;
    size_t         hsize;
};

ffmt_t *ffmt_new();

void ffmt_headers_push(ffmt_t *ffmt, ffmt_header_t *header);

ffmt_header_t *ffmt_headers_top(ffmt_t *ffmt);

ffmt_header_t *ffmt_headers_find(ffmt_t *ffmt, uint32_t type);

void ffmt_headers_pop(ffmt_t *ffmt);

void ffmt_destroy(ffmt_t *ffmt);

typedef struct ffmt_version_s ffmt_version_t;
struct ffmt_version_s {
    byte_t file_ver_major; // FileMajor - Older versions cannot not read the format.
    byte_t file_ver_minor; // FileMinor - Older versions can read the format, but will not retain on save.
    byte_t ver_major;      // Major - New release, major functionality change.
    byte_t ver_minor;      // Minor - Changes, but no big deal.
    byte_t ver_minuscle;   // Minuscle - bugfix.
};

errno_t ffmt_set_header(ffmt_t *ffmt, uint32_t type, bs_t *bs);

errno_t ffmt_get_header(ffmt_t *ffmt, uint32_t type, bs_t *bs);

errno_t ffmt_set_version(ffmt_t *ffmt);

errno_t ffmt_set_filekey(ffmt_t *ffmt, bs_t *key);

errno_t ffmt_get_filekey(ffmt_t *ffmt, bs_t *key);

errno_t ffmt_set_filename(ffmt_t *ffmt, bs_t *fname);

// 写入文件头到文件。
errno_t ffmt_out_headers(ffmt_t *ffmt, iodev_t *iodev);

// 根据文件名加载文件头到内存。
errno_t ffmt_load_headers(ffmt_t *ffmt, bs_t *fname);

// 读取文件头到内存。
errno_t ffmt_in_headers(ffmt_t *ffmt, iodev_t *iodev);

// errno_t ffmt_decrypt_headers(ffmt *ffmt, bs_t *key);

// errno_t ffmt_encrypt_headers(ffmt *ffmt, bs_t *key);

errno_t ffmt_encrypt_file(ffmt_t *ffmt, bs_t *fnplain, bs_t *fncipher, bs_t *key);

errno_t ffmt_decrypt_file(ffmt_t *ffmt, bs_t *fncipher);

size_t  ffmt_headers_size_all(ffmt_t *ffmt);

#endif // CPT_FFMT_H