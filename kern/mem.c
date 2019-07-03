#include "mem.h"
#include "ckern.h"
#include "term.h"
#define MAX_ALLOCS 1024

typedef struct allocation {
  void* addr;
  unsigned int size;
  int free;
} alloc_t;
alloc_t allocs[MAX_ALLOCS]; // keep track of up to 512 allocations
extern const unsigned int _heap_begin;
void* heap_ptr;
void initmem() {
  heap_ptr = (void*)(&_heap_begin);
  for (int i = 0; i < MAX_ALLOCS; i++) {
    allocs[i].addr = heap_ptr + (i * 4); // all allocs start off here
    allocs[i].size = 4;
    allocs[i].free = 1;
  }
}

// fixed size malloc for simple allocation and freeing
// if there are no allocs left, panic, i can't bring myself to pass someone a nullptr
void* malloc_32bits() {
  for (int i = 0; i < MAX_ALLOCS; i++) {
    if (allocs[i].free == 1) {
      allocs[i].free = 0;
      return allocs[i].addr;
    }
  }
  panic(1);
  // this shouldn't return
  return (void*)0x0;
}
void free_32bits(void* ptr) {
  for (int i = 0; i < MAX_ALLOCS; i++) {
    if (allocs[i].addr == ptr) {
      allocs[i].free = 1;
      return;
    }
  }
}

// don't use these
void* malloc(unsigned int size) {
  // at the moment the heap limit is about 256KiB
  if ((heap_ptr + size) > (void*)0x00150000) {
    panic(1);
  }
  void* p = heap_ptr;
  heap_ptr += size;
  return p;
}
void free(void* ptr) {
  return;
}
