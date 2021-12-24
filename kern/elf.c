#include "elf.h"
#include "mem.h"

// loads sections of ELF where they want to be and gives them a page
PROGMAINFUN PutELF(void* elfBegin, void** sectBegin, unsigned long* sectSz)
{
	unsigned char* src = (unsigned char*)elfBegin;
	elfheader_t* eHeader = (elfheader_t*)elfBegin;
	if (eHeader->magic != ELF_MAGIC)
	{
		writeLine("ERROR: ELF_MAGIC BORKED");
		puts("Expected ");
		putintx(ELF_MAGIC);
		puts(" got ");
		putintx(eHeader->magic);
		putch('\n');
		return 0;
	}
	// get bounds of all sections
	unsigned long lowMem = 0xFFFFFFFF;
	unsigned long highMem = 0;
	for (int i = 0; i < eHeader->numprogheaderents; i++)
	{
		elfpheader_t* ePHeader = (elfpheader_t*)(src + eHeader->progheaderoff + (eHeader->progheaderentsize * i));
		if (ePHeader->type != 1) continue; // 1 = LOAD
		if (ePHeader->vaddr < 0x100000)
		{
			writeLine("ERROR: ELF Program runs into lower meg!");
			puts("vaddr = ");
			putintx(ePHeader->vaddr);
			putch('\n');
			return 0;
		}
		if (ePHeader->vaddr + ePHeader->memsz > 0xC0000000)
		{
			writeLine("ERROR: ELF Program runs into kmem!");
			puts("vaddr + memsz = ");
			putintx(ePHeader->vaddr + ePHeader->memsz);
			putch('\n');
			return 0;
		}
		if (ePHeader->vaddr < lowMem) lowMem = ePHeader->vaddr;
		if (ePHeader->vaddr + ePHeader->memsz > highMem) highMem = ePHeader->vaddr + ePHeader->memsz;
	}
	if ((lowMem & 0xFFF) != 0)
	{
		writeLine("ERROR: ELF Program Base is not 4KiB Aligned!");
		puts("lowMem = ");
		putintx(lowMem);
		putch('\n');
		return 0;
	}
	unsigned long loadSz = highMem - lowMem;
	unsigned char* buf = (unsigned char*)malloc(loadSz + 1);
	*sectBegin = (void*)buf;
	*sectSz = loadSz;
	for (int i = 0; i < eHeader->numprogheaderents; i++)
	{
		elfpheader_t* ePHeader = (elfpheader_t*)(src + eHeader->progheaderoff + (eHeader->progheaderentsize * i));
		if (ePHeader->type != 1) continue; // 1 = LOAD
		// copy into buffer using vaddr-lowMem as offset
		unsigned char* tbuf = (buf) + ((ePHeader->vaddr - lowMem)); // vaddr - lowmem is the offset from the start of the buffer
		for (unsigned long b = 0; b < ePHeader->memsz; b++)
		{
			*(tbuf + b) = *(src + ePHeader->offset + b); // copy from source to dest
		}
	}
	// THIS PTR CANNOT BE USED STRAIGHT AWAY
	// BUF MUST BE MAPPED 
	return (PROGMAINFUN)eHeader->entry;
}