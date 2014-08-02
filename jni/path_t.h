#ifndef CPT_PATH_H
#define CPT_PATH_H

typedef struct path_s path_t;
struct path_s {
    byte_t dir[MAX_DIR];
    byte_t name[MAX_NAME];
    byte_t ext[MAX_EXT];
    
};

#endif