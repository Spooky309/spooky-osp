#include "term.h"
#include "interrupt.h"
#include "ckern.h"
#include "disk.h"
#include "mem.h"
#include "elf.h"
#include "command.h"
#include "syscall.h"

void runcmd(void)
{
	void* buf;
	unsigned long fileSz = LoadFile("TPROG   ELF", &buf);
	if (fileSz != 0)
	{
    void* sectBegin;
    unsigned long sectSz;
		PROGMAINFUN ent = PutELF(buf, &sectBegin, &sectSz);
		if (ent != 0)
		{
      // TODO: Set up a page that maps the elf to its vaddr

			ent(); // call into main
		}
	}
  free(buf);
}

int kmain(void) {
  mem_init(0x400000); // heap size 4MiB
  idt_init();
  initTerm();
  InitDisk();
  InitSyscall();
  writeLine("TODO: ???");
  char* fileContent = 0;
  
  if (LoadFile("TEST    TXT", (void**)&fileContent) != 0)
  {
	  puts("Loaded TEST.TXT to ");
	  putintx(fileContent);
	  writeLine(". I'm going to print its contents:\n");
	  writeLine(fileContent);
  }
  else
  {
	  writeLine("TEST.TXT wasn't found therefore I cannot test.");
  }
  if (fileContent != 0) free(fileContent);
  
  register_command("runtest", "run TPROG.ELF", runcmd);
  cmdPrompt();
  while(1) {
    asm("hlt"); // stop until we receive some interrupt
  }
  return 0;
}

void panic(unsigned int errcode) {
  // i didn't really think this one through so i can't actually get an error code from the ISR.
  // pls fix
  puts("\nFatal Error ");
  putintx(errcode);
  puts(": ");
  switch (errcode) {
  case 0:
	writeLine("Mysterious Internal CPU Exception");
	break;
  case 1:
	writeLine("Ran out of memory on kernel heap.");
	break;
  case 2:
	writeLine("See above.");
	break;
  case 65536:
	writeLine("I said no anime.");
	break;
  case 65535:
	writeLine("Myston Inarahj CPUOvComenmFATAL EXCEPTION SYSTEM HALTED");
        break;
  default:
        writeLine("allan please add details");
        break;
  }
  if (errcode != 65535) writeLine("panic.");
  // halt and catch fire
  while (1)
  {
    asm volatile ("cli");
    asm volatile ("hlt");
  }
}
