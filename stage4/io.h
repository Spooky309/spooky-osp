#ifndef _H_IO_
#define _H_IO_
#include <stdint.h>
// inline asm functions stolen from osdev.org
inline void io_wait(void) {
  // Linux seems to think this line is free, so I guess it's free.
  asm volatile ( "outb %%al, $0x80" : : "a"(0) );
}
inline void outb(uint16_t port, uint8_t val) {
  asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
  io_wait();
}
inline uint8_t inb(uint16_t port) {
  uint8_t ret;
  asm volatile ( "inb %1, %0"
                 : "=a"(ret)
                 : "Nd"(port) );
  io_wait();
  return ret;
}
inline uint16_t inw(uint16_t port)
{
	uint16_t ret;
	asm volatile ( "inw %1, %0"
				   : "=a"(ret)
				   : "Nd"(port) );
	io_wait();
	return ret;
}
inline void PIC_EOI(unsigned char irq) {
  if (irq >= 8)
    outb(0xA0, 0x20);
  outb(0x20, 0x20);
}
#endif
