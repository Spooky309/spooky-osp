#include "term.h"
#include "interrupt.h"
#include "ckern.h"
#include "mem.h"

int kmain(void) {
  mem_init(4096); // heap size 4KiB
  idt_init();
  initTerm();
  writeLine("Malloc-ing pointer, init to 100, times dereference by 1000.\nResults should be 100, 100000");
  
  writeLine("TODO: proper memory management, disk reading.");
  cmdPrompt();
  while(1) {
    asm("hlt"); // stop until we receive some interrupt
  }
  return 0;
}

void panic(unsigned int errcode) {
  //clearScreen();
  // i didn't really think this one through so i can't actually get an error code from the ISR.
  // pls fix
  // everything panics right now, because systems aren't detailed enough to handle exceptions gracefully
  writeLine("\nI've got problems. Sorry.");
  puts("Code ");
  putint(errcode);
  putch('\n');
  switch (errcode) {
  case 0:
	writeLine("Mysterious Internal CPU Exception");
	break;
  case 1:
	writeLine("Ran out of memory on kernel heap.");
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
  if (errcode != 65535) writeLine("\nKernel got ligma... halting. Reboot at your leisure.");
  // halt and catch fire
  while (1)
  {
    asm volatile ("cli");
    asm volatile ("hlt");
  }
}
