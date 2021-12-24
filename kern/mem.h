#ifndef _H_MEM_
#define _H_MEM_
#include <stddef.h>
#include "term.h"
#include "ckern.h"
typedef struct gdtentry_c
{
	// l1
	unsigned short limit_low;
	unsigned short base_low;

	unsigned char base_mid;
	unsigned char aflags;
	unsigned char limit_high : 4;
	unsigned char gflags : 4;
	unsigned char base_high;
} __attribute__((packed)) gdtentry_t;
typedef struct gdtdescriptor_t
{
	unsigned short gdt_size;
	unsigned long gdt_addr;
} __attribute__((packed)) gdtdescriptor_t;
#define GDT_AFLAG_ACCESSED_BIT 1
#define GDT_AFLAG_READ_WRITE_BIT 1 << 1
#define GDT_AFLAG_DIRECTION_BIT 1 << 2
#define GDT_AFLAG_EXECUTABLE_BIT 1 << 3
#define GDT_AFLAG_DESCRIPTOR_TYPE_BIT 1 << 4
#define GDT_AFLAG_PRIVILEGE_RING0 0 << 5
#define GDT_AFLAG_PRIVILEGE_RING1 1 << 5
#define GDT_AFLAG_PRIVILEGE_RING2 2 << 5
#define GDT_AFLAG_PRIVILEGE_RING3 3 << 5
#define GDT_AFLAG_PRESENT_BIT 1 << 7

#define GDT_GFLAG_SIZE_BIT 1 << 2
#define GDT_GFLAG_GRANULARITY_BIT 1 << 3
typedef struct pagetableentry_c
{
	union
	{
		struct
		{
			unsigned short present : 1;
			unsigned short rw : 1;
			unsigned short user_supervisor : 1;
			unsigned short write_through : 1;
			unsigned short cache_disabled : 1;
			unsigned short accessed : 1;
			unsigned short _unused : 1;
			unsigned short page_size : 1;
			unsigned short _a;
		} flags;
		unsigned long data;
	};
} __attribute__((packed)) pagetableentry_t;
// this also works for dirs
inline void set_ptable_addr(unsigned long* entry, unsigned long addr)
{
	if ((addr & 0xFFF) != 0)
	{
		writeLine("Page table/directory address truncated.");
		putintx(addr);
		puts(" to ");
		addr &= 0xFFFFF400;
		putintx(addr);
		putch('\n');
		panic(2);
	}
	*entry &= 0xFFF; // clear address, leave everything else alone
	*entry |= addr;  // 
}



extern void mem_init(size_t init_heap_size);
extern void* malloc(size_t len);
extern void* malloc_ivalign(size_t len);
extern void free(void* ptr);

#endif