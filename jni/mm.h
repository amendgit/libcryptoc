// mm.h
// Memory manager. For debug.

#ifndef CPT_MM_H
#define CPT_MM_H

#include "debug.h"
#include <malloc.h>

#ifdef DEBUG

#define cpt_malloc malloc 
#define cpt_free   free
// void *cpt_malloc(size_t numbytes);
// void  cpt_free(void *p);

#else // DEBUG

#define cpt_malloc malloc 
#define cpt_free   free

#endif // DEBUG

#endif // CPT_MM_H