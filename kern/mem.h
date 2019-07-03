#ifndef _H_MEM_
#define _H_MEM_
#include <stddef.h>
void initmem();
void* malloc_32bits();
void free_32bits(void* ptr);
// uncomment these when they are implemented
//void* malloc(unsigned int size);
//void free(void* ptr);
#endif