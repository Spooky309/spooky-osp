#include "syscall.h"
#include "io.h"
#include "term.h"
__attribute__((naked)) void SYSCALL(void)
{
	// save ecx and edx values (they are used by sysexit)
	unsigned int ecx;
	unsigned int edx;
	asm volatile ("movl %%ecx,%0" : "=m"(ecx));
	asm volatile ("movl %%edx,%0" : "=m"(edx));
	writeLine("A syscall was performed.");
	
	// restore ecx and edx
	asm volatile ("movl %0,%%ecx" : : "m"(ecx));
	asm volatile ("movl %0,%%edx" : : "m"(edx));
	asm volatile ( "sysexit" );
}

void InitSyscall(void)
{
	wrmsr(0x174, 0x08);
	unsigned long esp;
	unsigned long eip = (unsigned long)&SYSCALL;
	asm volatile ( "movl %%esp,%0" : "=r"(esp));
	wrmsr(0x175, (uint64_t)esp);
	wrmsr(0x176, (uint64_t)eip);
}
