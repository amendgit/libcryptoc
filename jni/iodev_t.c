#include "iodev_t.h"

#include <fcntl.h>
#include "debug.h"
#include "mm.h"

void   iodev_destroy(iodev_t *iodev)
{
    LOGPOS();

    if (iodev == NULL) 
        return ;

    bs_destroy(iodev->fname);
    cpt_free(iodev);
}

iodev_t *iodev_open(bs_t *fname, int flags)
{
    iodev_t *new_iodev;
    
    LOGPOS();

    #ifdef O_BINARY
    flags |= O_BINARY;
    #endif

    new_iodev = (iodev_t*)cpt_malloc(sizeof(iodev_t));
    if (new_iodev == NULL) {
        LOGW("Error alloc memory for new_iodev!");
        return NULL;
    }

    new_iodev->fd = open(fname->data, flags, 0666);
    if (new_iodev->fd == -1) {
        LOGW("Error open file %s!", fname->data);
        goto cleanup;
    }

    new_iodev->fname = bs_new();
    if (new_iodev->fname == NULL) {
        LOGW("Error alloc memory for new_iodev->fname!");
        goto cleanup;
    }

    bs_copy(new_iodev->fname, fname);

    new_iodev->fsize = 0;

    return new_iodev;

cleanup:
    iodev_destroy(new_iodev);
    return NULL;
}

iodev_t *iodev_oopen(bs_t *fname)
{
    LOGPOS();
    return iodev_creat(fname, CPT_TRUE);
}

iodev_t *iodev_iopen(bs_t *fname)
{
    LOGPOS();
    return iodev_open(fname, O_RDONLY);
}

iodev_t *iodev_creat(bs_t *fname, uint32_t create_always)
{
    iodev_t *new_iodev;

    LOGPOS();

    new_iodev = (iodev_t*)cpt_malloc(sizeof(iodev_t));
    if (new_iodev == NULL) {
        LOGW("Error alloc memory for new_iodev!");
        return NULL; // No need to cleanup.
    }
    
    if (create_always) {
        new_iodev->fd = creat(fname->data, 0666);
    } else {
        new_iodev->fd = open(fname->data, O_CREAT|O_EXCL|O_WRONLY, 0666);
    }

    if (new_iodev->fd == -1) { 
        LOGW("Error create file %s!", fname->data);
        goto cleanup;
    } 

    new_iodev->fname = bs_new(); 
    if (new_iodev->fname == NULL) {
        LOGW("Error alloc memory for new_iodev->fname!");
        goto cleanup;
    }

    bs_copy(new_iodev->fname, fname); 

    new_iodev->fsize = 0;

    return new_iodev;

cleanup:
    iodev_destroy(new_iodev);
    return NULL;
}

errno_t iodev_close(iodev_t *iodev)
{
    errno_t err = CPT_OK;

    LOGPOS();

    if (iodev->fd == -1) {
        LOGI("Close file %s which is closed.", iodev->fname->data);
        err = CPT_OK;
    }

    if (close(iodev->fd) != 0) {
        LOGW("Error close file %s", iodev->fname->data);
        return CPT_ERROR;
    }

    iodev->fd = -1;

    iodev_destroy(iodev);

    return err;
}

uint64_t iodev_filelen(iodev_t *iodev)
{
    off_t cur, len;

    LOGPOS();

    cur = iodev_seek(iodev, 0, SEEK_CUR);
    len = iodev_seek(iodev, 0, SEEK_END);
    
    iodev_seek(iodev, cur, SEEK_SET);
    
    iodev->fsize = len;
    
    return len;
}

off_t iodev_seek(iodev_t *iodev, off_t offset, int move)
{
    LOGPOS();
    return lseek(iodev->fd, offset, move);
}

ssize_t  iodev_out(iodev_t *iodev, const void *data, size_t size)
{
    LOGPOS();
    return write(iodev->fd, data, size);
}

ssize_t iodev_out_u32(iodev_t *iodev, const uint32_t u32)
{
    byte_t ao[4];

    LOGPOS();

    u32_to_ao(u32, ao);
    return iodev_out(iodev, ao, 4);
}

ssize_t iodev_in(iodev_t *iodev, void *data, size_t size)
{
    LOGPOS();
    return read(iodev->fd, data, size);
}

ssize_t iodev_in_u32(iodev_t *iodev, uint32_t *u32)
{
    byte_t ao[4];
    size_t cb;

    LOGPOS();

    cb = iodev_in(iodev, ao, 4);
    ao_to_u32(ao, u32);

    return cb;
}