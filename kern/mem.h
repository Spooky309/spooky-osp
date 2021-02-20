#ifndef _H_MEM_
#define _H_MEM_
#include <stddef.h>

extern void mem_init(size_t init_heap_size);
extern void* malloc(size_t len);
extern void free(void* ptr);

#endif