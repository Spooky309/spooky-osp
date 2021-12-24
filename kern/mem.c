#include "mem.h"
#include "term.h"
#include "ckern.h"
// the heap is just a bunch of memory with a linked list of alloc blocks that have headers and stuff
// i think it's pretty neat

typedef struct _mem_header_c
{
    struct _mem_header_c* next;
    struct _mem_header_c* prev;
    size_t alloc_len;
} _mem_header_t;

void* heap;
size_t heap_size;
void* _heap_begin;
gdtentry_t* pGdt;
gdtdescriptor_t* pGdtDesc;
int mem_self_test(void)
{
  int* a = malloc(sizeof(int));
  int* b = malloc(sizeof(int));
  if (a == NULL)
  {
    writeLine("a was null");
    return 0;
  }
  if (b == NULL)
  {
    writeLine("b was null");
    return 0;
  }
  *a = 100;
  *b = 300;
  int* c = malloc(sizeof(int));
  if (c == NULL)
  {
    writeLine("c was null");
    return 0;
  }
  *c = (*a) * (*b);
  if (*c != 30000)
  {
    puts("c wasn't as expected, wanted 30000, got ");
    putint(*c);
    putch('\n');
    return 0;
  }
  free(b);
  int* d = malloc(sizeof(int));
  if (*d != 300)
  {
    puts("d ptr was not same as b, which is what we expected. it was ");
    putint(*d);
    putch('\n');
    return 0;
  }
  return 1;
}

void fill_gdt(gdtentry_t* gdt)
{
  // clear first gdt entry
  for (unsigned long i = 0; i < sizeof(gdtentry_t); i++)
  {
    *(((char*)gdt) + i) = 0;
  }
  // code segment (ring 0)
  gdt[1].limit_low = 0xFFFF;
  gdt[1].base_low = 0;
  gdt[1].base_mid = 0;
  gdt[1].aflags = (GDT_AFLAG_PRESENT_BIT | GDT_AFLAG_PRIVILEGE_RING0 | GDT_AFLAG_EXECUTABLE_BIT | GDT_AFLAG_READ_WRITE_BIT | GDT_AFLAG_DESCRIPTOR_TYPE_BIT);
  gdt[1].gflags = (GDT_GFLAG_GRANULARITY_BIT | GDT_GFLAG_SIZE_BIT);
  gdt[1].limit_high = 0b1111;
  gdt[1].base_high = 0;
  // data segment (ring 0)
  gdt[2].limit_low = 0xFFFF;
  gdt[2].base_low = 0;
  gdt[2].base_mid = 0;
  gdt[2].aflags = (GDT_AFLAG_PRESENT_BIT | GDT_AFLAG_PRIVILEGE_RING0 | GDT_AFLAG_READ_WRITE_BIT | GDT_AFLAG_DESCRIPTOR_TYPE_BIT);
  gdt[2].gflags = (GDT_GFLAG_GRANULARITY_BIT | GDT_GFLAG_SIZE_BIT);
  gdt[2].limit_high = 0b1111;
  gdt[2].base_high = 0;
  // code segment (ring 3)
  gdt[3].limit_low = 0xFFFF;
  gdt[3].base_low = 0;
  gdt[3].base_mid = 0;
  gdt[3].aflags = (GDT_AFLAG_PRESENT_BIT | GDT_AFLAG_PRIVILEGE_RING3 | GDT_AFLAG_EXECUTABLE_BIT | GDT_AFLAG_READ_WRITE_BIT | GDT_AFLAG_DESCRIPTOR_TYPE_BIT);
  gdt[3].gflags = (GDT_GFLAG_GRANULARITY_BIT | GDT_GFLAG_SIZE_BIT);
  gdt[3].limit_high = 0b1111;
  gdt[3].base_high = 0;
  // data segment (ring 3)
  gdt[4].limit_low = 0xFFFF;
  gdt[4].base_low = 0;
  gdt[4].base_mid = 0;
  gdt[4].aflags = (GDT_AFLAG_PRESENT_BIT | GDT_AFLAG_PRIVILEGE_RING3 | GDT_AFLAG_READ_WRITE_BIT | GDT_AFLAG_DESCRIPTOR_TYPE_BIT);
  gdt[4].gflags = (GDT_GFLAG_GRANULARITY_BIT | GDT_GFLAG_SIZE_BIT);
  gdt[4].limit_high = 0b1111;
  gdt[4].base_high = 0;
  // TODO: TSS


  pGdtDesc->gdt_addr = (unsigned long)gdt;
  pGdtDesc->gdt_size = (sizeof(gdtentry_t) * 6) - 1;
}

void mem_init(size_t init_heap_size)
{
    heap_size = init_heap_size;
    writeLine("Early: Init Memory");
    putint((size_t)_heap_begin);
    putch('\n');
    heap = (void*)0xC0400000;
    for (size_t i = 0; i < init_heap_size; i++)
    {
      ((char*)heap)[i] = 0x00;
    }
    ((_mem_header_t*)heap)->alloc_len = sizeof(_mem_header_t);
    ((_mem_header_t*)heap)->next = NULL;
    ((_mem_header_t*)heap)->prev = NULL;
    if (mem_self_test() == 0)
    {
      writeLine("Allocation self-test FAILED");
      panic(2);
    }
    writeLine("Allocation self-test GOOD");

    // create a new GDT and load it (replace old GDT)
    pGdt = malloc((sizeof(gdtentry_t) * 6) + sizeof(gdtdescriptor_t));
    pGdtDesc = (gdtdescriptor_t*)(pGdt + 6);
    fill_gdt(pGdt);
    asm volatile ( "push %eax" );
    asm volatile ( "movl (%0), %%eax" : : "m"(pGdtDesc) );
    asm volatile ( "lgdt (%eax)" );
    asm volatile ( "pop %eax" );
    // long jump to reset code segment and data segments
    asm volatile ( "ljmp $0x08, $dosegs" );
    asm volatile ( "dosegs:" );
    asm volatile (
      "mov $0x10, %ax\n"
      "mov %ax, %ds\n"
      "mov %ax, %es\n"
      "mov %ax, %fs\n"
      "mov %ax, %gs\n"
      "mov %ax, %ss\n"
    );
    writeLine("New GDT Installed");
}
void* malloc_intl(size_t len, int ivalign)
{
    size_t totalallocs = sizeof(_mem_header_t) + len; // total size of the allocated block including the header
    void* cur = heap;
    while (cur != NULL)
    {
        _mem_header_t* curh = (_mem_header_t*)cur;
        // if there are no more allocs left on the heap
        if (curh->next == NULL)
        {
            // potential location of the new block is this alloc plus its total size
            _mem_header_t* potalloc = (_mem_header_t*)(cur+curh->alloc_len);
            // 4kib alignment stuff
            if (ivalign != 0)
            {
              unsigned long p = (unsigned long)potalloc;
              unsigned long r = p % 4096;
              unsigned long round = p + 4096 - r;
              round -= sizeof(_mem_header_t); // take off header so block starts at 4k align
              // if the subtract ran us into the previous alloc add 4k
              if (round < ((unsigned long)curh + curh->alloc_len)) round += 4096;
              potalloc = (_mem_header_t*)round;
            }
            // if a pointer to the end of the alloc block is larger than the pointer to the end of the heap then we're fucked
            if (((void*)potalloc) + totalallocs < heap+heap_size)
            {
                // we can
                potalloc->prev = (_mem_header_t*)cur;
                potalloc->next = NULL;
                potalloc->alloc_len = totalallocs;
                curh->next = potalloc;
                return ((void*)potalloc)+sizeof(_mem_header_t);
            }
            else
            {
                // nope
                writeLine("No room at the inn...\n");
                return 0;
            }
        }
        else
        {
            _mem_header_t* curh = (_mem_header_t*)cur;
            _mem_header_t* nexth = curh->next;
            _mem_header_t* potalloc = (_mem_header_t*)(cur+curh->alloc_len);
            // if we have space between the end of current block and the next one, we can slot in between them
            if (totalallocs <= (size_t)((void*)nexth - (cur+curh->alloc_len)))
            {
                potalloc->alloc_len = totalallocs;
                potalloc->prev = curh;
                potalloc->next = nexth;
                curh->next = potalloc;
                nexth->prev = potalloc;
                return ((void*)potalloc)+sizeof(_mem_header_t);
            }
        }
        cur = (void*)(curh->next);
    }
    writeLine("Can't get you an alloc. Sorry.");
    return NULL;
}
void* malloc(size_t len)
{
  return malloc(len, 0);
}
void* malloc_ivalign(size_t len)
{
  return malloc(len, 1);
}
void free(void* ptr)
{
    _mem_header_t* header = ptr - sizeof(_mem_header_t);
    if (header->next != NULL)
    {
        header->next->prev = header->prev;
    }
    header->prev->next = header->next; // remove myself from the chain, if this is the end we will be null anyway
}