#include "ffmt_t.h"

#include <string.h>
#include <stdio.h>

#include "mm.h"
#include "iodev_t.h"
#include "rc4_t.h"


ffmt_header_t *ffmt_header_new()
{
    ffmt_header_t *new_header;

    LOGPOS();

    new_header = (ffmt_header_t*)cpt_malloc(sizeof(ffmt_header_t));
    if (new_header == NULL) {
        LOGW("Error alloc memory for new_header");
        return NULL;
    }

    new_header->next = NULL;
    new_header->data = NULL;
    new_header->size = 0;
    new_header->type = CPT_HEADER_NONE;

    return new_header;
}

void ffmt_header_alloc(ffmt_header_t *header, uint32_t len, uint32_t align)
{
    LOGPOS();

    if (header->data != NULL) {
        free(header->data);
        header->data = NULL;
        header->size = 0;
    }

    header->size = (uint32_t)((len + align - 1) - (len + align - 1) % align);
    header->data = cpt_malloc(header->size);
    if (header->data == NULL) {
        LOGW("Error alloc memory for header->data");
        return ;
    }
}

void ffmt_header_destroy(ffmt_header_t *header)
{
    LOGPOS();

    if (header == NULL)
        return ;

    header->next = NULL;
    if (header->data != NULL);
        free(header->data);
    
    free(header);
}

void ffmt_headers_push(ffmt_t *ffmt, ffmt_header_t *header)
{
    ffmt_header_t *pre, *cur;
    ffmt_header_t dummy;

    LOGPOS();

    dummy.next = ffmt->headers;

    pre = &dummy;
    cur = ffmt->headers;

    while (cur != NULL && cur->type < header->type) {
        pre = cur;
        cur = cur->next;
    }

    header->next = pre->next;
    pre->next = header;

    ffmt->headers = dummy.next;
    return ;
} 

ffmt_header_t *ffmt_headers_top(ffmt_t *ffmt)
{
    LOGPOS();
    return ffmt->headers;
}

ffmt_header_t *ffmt_headers_find(ffmt_t *ffmt, uint32_t type)
{
    ffmt_header_t *header;

    LOGPOS();
    
    header = ffmt->headers;
    while (header != NULL && header->type < type) { 
        header = header->next; 
    }
    
    if (header != NULL && header->type == type) 
        return header;

    return NULL;
}

void ffmt_headers_pop(ffmt_t *ffmt)
{
    ffmt_header_t *del;

    LOGPOS();

    del = ffmt->headers;
    ffmt->headers = ffmt->headers->next;
    ffmt_header_destroy(del);
}

ffmt_t *ffmt_new()
{
    ffmt_t *new_ffmt;

    LOGPOS();

    new_ffmt = cpt_malloc(sizeof(ffmt_t));
    if (new_ffmt == NULL) {
        LOGW("Error alloc memory for new_ffmt");
        return NULL;
    }

    new_ffmt->headers = NULL;
    new_ffmt->hsize   = 0;

    return new_ffmt;
}

void ffmt_destroy(ffmt_t *ffmt)
{
    ffmt_header_t *del, *itr;

    LOGPOS();

    if (ffmt == NULL)
        return ;

    itr = ffmt->headers;
    while (itr != NULL) {
        del = itr;
        itr = itr->next;
        ffmt_header_destroy(del);
    }

    cpt_free(ffmt);

    return ;
}

errno_t ffmt_set_header(ffmt_t *ffmt, uint32_t type, bs_t *bs)
{
    ffmt_header_t *header;
    byte_t        *p;

    LOGPOS();

    header = ffmt_headers_find(ffmt, type);
    if (header == NULL) {
        header = ffmt_header_new();
        if (header == NULL) {
            goto cleanup;
        }

        header->type = type;
        ffmt_header_alloc(header, bs->len + sizeof(uint32_t), sizeof(uint32_t));
        if (header->data == NULL) {
            goto cleanup;
        }
        ffmt_headers_push(ffmt, header);
    }

    if (header->size < bs->len) {
        free(header->data);
        ffmt_header_alloc(header, bs->len + sizeof(uint32_t), sizeof(uint32_t));
        if (header->data == NULL) {
            goto cleanup;
        }
    }

    p = header->data;

    u32_to_ao(bs->len, p); 
    p += 4;
    
    memcpy(p, bs->data,  bs->len);

    return CPT_OK;

cleanup:
    if (header != NULL) {
        ffmt_header_destroy(header);
    }

    return CPT_ERROR;
}

errno_t ffmt_get_header(ffmt_t *ffmt, uint32_t type, bs_t *bs)
{
    ffmt_header_t *header;
    byte_t        *p;
    uint32_t       len;

    LOGPOS();

    header = ffmt_headers_find(ffmt, type);
    if (header == NULL) {
        LOGW("Error header find failed.");
        return CPT_ERROR;
    }
    
    p = header->data;

    ao_to_u32(p, &len);
    p += 4;
    
    bs_resize(bs, len);
    memcpy(bs->data, p, len);
    bs->len = len;

    return CPT_OK;
}

errno_t  ffmt_set_version(ffmt_t *ffmt)
{
    ffmt_version_t *version;
    ffmt_header_t  *header;

    LOGPOS();

    header = ffmt_headers_find(ffmt, CPT_HEADER_VERSION);

    if (header == NULL) {
        header = ffmt_header_new();
        if (header == NULL) {
            goto cleanup;
        }
        ffmt_headers_push(ffmt, header);

        header->type = CPT_HEADER_VERSION;
        ffmt_header_alloc(header, sizeof(ffmt_version_t), sizeof(uint32_t));
        if (header->data == NULL) {
            goto cleanup;
        }
        ffmt_headers_push(ffmt, header);
    }

    version = header->data;

    version->file_ver_major = 1;
    version->file_ver_minor = 1;
    version->ver_major = 1;
    version->ver_minor = 1;
    version->ver_minuscle = 1;

    return CPT_OK;

cleanup:
    if (header != NULL) {
        ffmt_header_destroy(header);
    }

    return CPT_ERROR;
}

// TODO: fix memory bug.
errno_t ffmt_set_filekey(ffmt_t *ffmt, bs_t *key)
{
    ffmt_header_t *header;
    byte_t        *p;

    LOGPOS();

    header = ffmt_headers_find(ffmt, CPT_HEADER_FILEKEY);
    if (header == NULL) {
        header = ffmt_header_new();
        if (header == NULL) {
            goto cleanup;
        }

        header->type = CPT_HEADER_FILEKEY;
        ffmt_header_alloc(header, key->len + sizeof(uint32_t), sizeof(uint32_t));
        if (header->data == NULL) {
            goto cleanup;
        }
        ffmt_headers_push(ffmt, header);
    }

    if (header->size < key->len) {
        free(header->data);
        ffmt_header_alloc(header, key->len + sizeof(uint32_t), sizeof(uint32_t));
        if (header->data == NULL) {
            goto cleanup;
        }
    }

    p = header->data;

    u32_to_ao(key->len, p); 
    p += 4;
    
    memcpy(p, key->data, key->len);

    return CPT_OK;

cleanup:
    if (header != NULL) {
        ffmt_header_destroy(header);
    }

    return CPT_ERROR;
}

errno_t ffmt_get_filekey(ffmt_t *ffmt, bs_t *key)
{
    ffmt_header_t *header;
    byte_t        *p;
    uint32_t       len;

    LOGPOS();

    header = ffmt_headers_find(ffmt, CPT_HEADER_FILEKEY);
    if (header == NULL) {
        LOGW("Error filekey find failed.");
        return CPT_ERROR;
    }
    
    p = header->data;

    ao_to_u32(p, &len);
    p += 4;
    
    bs_resize(key, len);
    memcpy(key->data, p, len);
    key->len = len;
}

// TODO: fix memory bug.
errno_t ffmt_set_filename(ffmt_t *ffmt, bs_t *fname)
{
    ffmt_header_t *header;
    byte_t        *p;

    LOGPOS();

    header = ffmt_headers_find(ffmt, CPT_HEADER_FILENAME);
    if (header == NULL) {
        header = ffmt_header_new();
        if (header == NULL) {
            goto cleanup;
        }

        header->type = CPT_HEADER_FILENAME;
        ffmt_header_alloc(header, fname->len + sizeof(uint32_t), sizeof(uint32_t));
        if (header->data == NULL) {
            goto cleanup;
        }
        ffmt_headers_push(ffmt, header);
    }

    if (header->size < fname->len) {
        free(header->data);
        ffmt_header_alloc(header, fname->len + sizeof(uint32_t), sizeof(uint32_t));
        if (header->data == NULL) {
            goto cleanup;
        }
    }

    p = header->data;

    u32_to_ao(fname->len, p);
    p += 4;

    memcpy(p, fname->data, fname->len);

    return CPT_OK;

cleanup:
    if (header != NULL) 
        ffmt_header_destroy(header);

    return CPT_ERROR;
}

errno_t ffmt_get_filename(ffmt_t *ffmt, bs_t *fname)
{
    ffmt_header_t *header;
    byte_t        *p;
    uint32_t       len;

    LOGPOS();

    header = ffmt_headers_find(ffmt, CPT_HEADER_FILENAME);
    if (header == NULL) {
        LOGW("Error find filename failed.");
        return CPT_ERROR;
    }
    
    p = header->data;

    ao_to_u32(p, &len);
    p += 4;
    
    bs_resize(fname, len+1);
    memcpy(fname->data, p, len);
    fname->data[len] = '\0';

    fname->len = len;

    return CPT_OK;
}

errno_t  ffmt_set_end(ffmt_t *ffmt) {
    ffmt_header_t *header;

    LOGPOS();

    header = ffmt_headers_find(ffmt, CPT_HEADER_END);
    if (header == NULL) {
        header = ffmt_header_new();
        if (header == NULL) {
            return CPT_ERROR;
        }

        header->type = CPT_HEADER_END;
        header->size = 0;
        ffmt_headers_push(ffmt, header);
    }

    return CPT_OK;
}

errno_t ffmt_out_headers(ffmt_t *ffmt, iodev_t *iodev)
{
    ffmt_header_t *header;

    LOGPOS();

    header = ffmt->headers;
    while (header != NULL) {
        iodev_out_u32(iodev, header->type); 
        LOGI("otype: %d", header->type);

        iodev_out_u32(iodev, header->size);
        LOGI("osize: %d", header->size);
        
        if (header->size != 0) {
            iodev_out(iodev, header->data, header->size);
        }
        
        header = header->next;
    }

    return CPT_OK;
}

errno_t ffmt_load_headers(ffmt_t *ffmt, bs_t *fname)
{
    errno_t err = CPT_OK;
    iodev_t *in;

    LOGPOS();

    in = iodev_iopen(fname);
    if (in == NULL) {
        return CPT_ERROR;
    }

    err = ffmt_in_headers(ffmt, in);

    if (in != NULL) {
        iodev_close(in);
    }
    return err;
}

errno_t ffmt_in_headers(ffmt_t *ffmt, iodev_t *iodev)
{
    uint32_t       type, size;
    ffmt_header_t *header;
    errno_t err;

    LOGPOS();

    while (CPT_TRUE) {
        header = ffmt_header_new();
        if (header == NULL) {
            LOGW("Error alloc memory for header");
            return CPT_ERROR;
        } 

        iodev_in_u32(iodev, &type); 
        LOGI("itype:%d", type);

        iodev_in_u32(iodev, &size); 
        LOGI("isize:%d", size); 

        header->type = type;
        header->size = size;

        if (size != 0) {
            ffmt_header_alloc(header, size, sizeof(uint32_t));
            iodev_in(iodev, header->data, size);
        }

        ffmt_headers_push(ffmt, header);

        if (type == CPT_HEADER_END) {
            err = CPT_OK;
            break;
        }

        header = NULL;
    }

    return err;
}

size_t ffmt_headers_size_all(ffmt_t *ffmt)
{
    size_t all = 0;
    ffmt_header_t *header;

    LOGPOS();

    header = ffmt->headers;
    while (header != NULL) {
        all += header->size + sizeof(uint32_t) + sizeof(uint32_t);
        header = header->next;
    }

    return all;
}

const uint32_t kBufSize = 512 * 1024;

errno_t ffmt_encrypt_file(ffmt_t *ffmt, bs_t *fnplain, bs_t *fncipher, bs_t *key)
{
    iodev_t *iplain, *ocipher;
    errno_t  err;
    bs_t    *buf;
    rc4_t   *rc4;
    // sha1_t  *sha1;

    LOGPOS();

    buf = bs_new();
    err = bs_resize(buf, kBufSize); // At least 256KB buf.
    if (err != CPT_OK) {
        goto cleanup;
    }
    buf->len = kBufSize;

    rc4 = rc4_new(key);
    if (rc4 == NULL) {
        err = CPT_ERROR;
        goto cleanup;
    }

    iplain = iodev_iopen(fnplain);
    if (iplain == NULL) {
        err = CPT_ERROR;
        goto cleanup;
    }

    ocipher = iodev_oopen(fncipher);
    if (ocipher == NULL) {
        err = CPT_ERROR;
        goto cleanup;
    }
    // Original filename, to rename back when decrypt.
    ffmt_set_filename(ffmt, fnplain);
    // Filekey for encryt or decrypt file.
    ffmt_set_filekey(ffmt, key);
    // End of the headers.
    ffmt_set_end(ffmt); 

    LOGI("head size all %d", ffmt_headers_size_all(ffmt));
    iodev_seek(ocipher, ffmt_headers_size_all(ffmt), SEEK_SET);
    
    buf->len = iodev_in(iplain, buf->data, kBufSize);
    // sha1_init(sha1);
    while (buf->len != 0) {
        // sha1_update(sha1, buf);
        rc4_xstream(rc4, buf, buf);
        iodev_out(ocipher, buf->data, buf->len);
        buf->len = iodev_in(iplain, buf->data, kBufSize);
    }
    // sha1_final(sha1, hamc);
    // ffmt_set_hmac(ffmt, hamc);

    iodev_seek(ocipher, 0, SEEK_SET);
    ffmt_out_headers(ffmt, ocipher);

    err = CPT_OK;

cleanup:
    if (iplain != NULL) 
        iodev_close(iplain);

    if (ocipher != NULL) 
        iodev_close(ocipher);

    if (buf != NULL)
        bs_destroy(buf);

    if (rc4 != NULL)
        rc4_destroy(rc4);

    return err;

}

errno_t ffmt_decrypt_file(ffmt_t *ffmt, bs_t *fncipher)
{
    errno_t  err;
    iodev_t *icipher, *oplain;
    rc4_t   *rc4;
    bs_t    *buf, *key;
    bs_t    *fnplain;

    LOGPOS();

    buf = bs_new();
    err = bs_resize(buf, kBufSize); // At least 256KB buf.
    if (err != CPT_OK) {
        goto cleanup;
    }
    buf->len = kBufSize;

    icipher = iodev_iopen(fncipher);
    if (icipher == NULL) {
        err = CPT_ERROR;
        goto cleanup;
    }

    ffmt_in_headers(ffmt, icipher);

    key = bs_new();
    if (key == NULL) {
        err = CPT_ERROR;
        goto cleanup;
    }

    ffmt_get_filekey(ffmt, key);

    rc4 = rc4_new(key);
    if (rc4 == NULL) {
        err = CPT_ERROR;
        goto cleanup;
    }

    fnplain = bs_new();
    if (fnplain == NULL) {
        err = CPT_ERROR;
        goto cleanup;
    }
    ffmt_get_filename(ffmt, fnplain);

    oplain = iodev_oopen(fnplain);
    if (oplain == NULL) {
        err = CPT_ERROR;
        goto cleanup;
    }

    LOGI("head size all %d", ffmt_headers_size_all(ffmt));

    iodev_seek(icipher, ffmt_headers_size_all(ffmt), SEEK_SET);
    
    buf->len = iodev_in(icipher, buf->data, kBufSize); 

    LOGI("in data len: %d", buf->len);

    while (buf->len != 0) { 
        rc4_xstream(rc4, buf, buf); 
        iodev_out(oplain, buf->data, buf->len);
        buf->len = iodev_in(icipher, buf->data, kBufSize);
    }

cleanup:
    if (oplain != NULL) 
        iodev_close(oplain);

    if (icipher != NULL) 
        iodev_close(icipher);

    if (buf != NULL)
        bs_destroy(buf);

    if (rc4 != NULL)
        rc4_destroy(rc4);

    return err;
}