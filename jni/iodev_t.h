#ifndef CPT_IODEV_H
#define CPT_IODEV_H

#include "config.h"
#include "bs_t.h"
#include "errno_t.h"

typedef struct iodev_s iodev_t;
struct iodev_s {
    int       fd;
    bs_t     *fname;
    uint64_t  fsize;
};

iodev_t *iodev_open(bs_t *fname, int flags);

iodev_t *iodev_oopen(bs_t *fname);

iodev_t *iodev_iopen(bs_t *fname);

iodev_t *iodev_creat(bs_t *fname, uint32_t create_always);

errno_t  iodev_close(iodev_t *iodev);

// Recaculate the file length and reset the fsize.
uint64_t iodev_filelen(iodev_t *iodev);

off_t    iodev_seek(iodev_t *iodev, off_t offset, int move);

ssize_t  iodev_out(iodev_t *iodev, const void *data, size_t size);

ssize_t  iodev_out_u32(iodev_t *iodev, const uint32_t u32);

ssize_t  iodev_in(iodev_t *iodev, void *data, size_t size);

#endif // CPT_IODEV_H