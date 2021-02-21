#ifndef _H_INTERRUPT_
#define _H_INTERRUPT_
#include "term.h"
#include "io.h"
#include "ckern.h"

// todo: fix mixed tabs and spaces, i'm in too much of a bad mood to do it right now
// why is this all in the header anyway? much like your sister, it beats the hell outta me.

struct IDT_entry{
	unsigned short int offset_lowerbits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short int offset_higherbits;
};
struct IDT_entry IDT[286];

void idivzero_handler(void) {
  writeLine("WARNING: DIVISION BY ZERO");
}
// map all unrecoverable/unimplemented ints here
void ifatal_handler(void) {
  panic(0);
}
void irq0_handler(void) {
  sysclock++;
  PIC_EOI(0);
}
 
void irq1_handler(void) {
  kbd_input(inb(0x60));
  PIC_EOI(1);
}
 
void irq2_handler(void) {
  PIC_EOI(2);
}
 
void irq3_handler(void) {
  PIC_EOI(3);
}
 
void irq4_handler(void) {
  PIC_EOI(4);
}
 
void irq5_handler(void) {
  PIC_EOI(5);
}
 
void irq6_handler(void) {
  PIC_EOI(6);
}
 
void irq7_handler(void) {
  PIC_EOI(7);
}
 
void irq8_handler(void) {
  PIC_EOI(8);    
}
 
void irq9_handler(void) {
  PIC_EOI(9);
}
 
void irq10_handler(void) {
  PIC_EOI(10);
}
 
void irq11_handler(void) {
  PIC_EOI(11);
}
 
void irq12_handler(void) {
  PIC_EOI(12);
}
 
void irq13_handler(void) {
  PIC_EOI(13);
}
 
void irq14_handler(void) {
  PIC_EOI(14);
}
 
void irq15_handler(void) {
  PIC_EOI(15);
}

void idt_init(void) {
  extern int load_idt();
  extern int idivzero();
  extern int ifatal();
  extern int irq0();
  extern int irq1();
  extern int irq2();
  extern int irq3();
  extern int irq4();
  extern int irq5();
  extern int irq6();
  extern int irq7();
  extern int irq8();
  extern int irq9();
  extern int irq10();
  extern int irq11();
  extern int irq12();
  extern int irq13();
  extern int irq14();
  extern int irq15();
  unsigned long idivzero_address;
  unsigned long ifatal_address;
  unsigned long irq0_address;
  unsigned long irq1_address;
  unsigned long irq2_address;
  unsigned long irq3_address;          
  unsigned long irq4_address; 
  unsigned long irq5_address;
  unsigned long irq6_address;
  unsigned long irq7_address;
  unsigned long irq8_address;
  unsigned long irq9_address;          
  unsigned long irq10_address;
  unsigned long irq11_address;
  unsigned long irq12_address;
  unsigned long irq13_address;
  unsigned long irq14_address;          
  unsigned long irq15_address;         
  unsigned long idt_address;
  unsigned long idt_ptr[2];
 
  // IBM made a design flaw where the PIC maps IRQs to the same lines as reserved internal Intel ones
  // So we remap the IRQs outside of that range.
  outb(0x20, 0x11);
  outb(0xA0, 0x11);
  outb(0x21, 0x20);
  outb(0xA1, 0x28);
  outb(0x21, 0x04);
  outb(0xA1, 0x02);
  outb(0x21, 0x01);
  outb(0xA1, 0x01);
  outb(0x21, 0x0);
  outb(0xA1, 0x0);
  
  idivzero_address = (unsigned long)idivzero;
  IDT[0].offset_lowerbits = idivzero_address & 0xFFFF;
  IDT[0].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[0].zero = 0;
	IDT[0].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT[0].offset_higherbits = (idivzero_address & 0xffff0000) >> 16;
  
  ifatal_address = (unsigned long)ifatal;
  IDT[8].offset_lowerbits = ifatal_address & 0xFFFF;
  IDT[8].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[8].zero = 0;
	IDT[8].type_attr = 0x8E; /* INTERRUPT_GATE */
	IDT[8].offset_higherbits = (ifatal_address & 0xffff0000) >> 16;
  
	irq0_address = (unsigned long)irq0; 
	IDT[32].offset_lowerbits = irq0_address & 0xffff;
	IDT[32].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[32].zero = 0;
	IDT[32].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[32].offset_higherbits = (irq0_address & 0xffff0000) >> 16;
 
	irq1_address = (unsigned long)irq1; 
	IDT[33].offset_lowerbits = irq1_address & 0xffff;
	IDT[33].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[33].zero = 0;
	IDT[33].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[33].offset_higherbits = (irq1_address & 0xffff0000) >> 16;
 
	irq2_address = (unsigned long)irq2; 
	IDT[34].offset_lowerbits = irq2_address & 0xffff;
	IDT[34].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[34].zero = 0;
	IDT[34].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[34].offset_higherbits = (irq2_address & 0xffff0000) >> 16;
 
	irq3_address = (unsigned long)irq3; 
	IDT[35].offset_lowerbits = irq3_address & 0xffff;
	IDT[35].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[35].zero = 0;
	IDT[35].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[35].offset_higherbits = (irq3_address & 0xffff0000) >> 16;
 
	irq4_address = (unsigned long)irq4; 
	IDT[36].offset_lowerbits = irq4_address & 0xffff;
	IDT[36].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[36].zero = 0;
	IDT[36].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[36].offset_higherbits = (irq4_address & 0xffff0000) >> 16;
 
	irq5_address = (unsigned long)irq5; 
	IDT[37].offset_lowerbits = irq5_address & 0xffff;
	IDT[37].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[37].zero = 0;
	IDT[37].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[37].offset_higherbits = (irq5_address & 0xffff0000) >> 16;
 
	irq6_address = (unsigned long)irq6; 
	IDT[38].offset_lowerbits = irq6_address & 0xffff;
	IDT[38].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[38].zero = 0;
	IDT[38].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[38].offset_higherbits = (irq6_address & 0xffff0000) >> 16;
 
	irq7_address = (unsigned long)irq7; 
	IDT[39].offset_lowerbits = irq7_address & 0xffff;
	IDT[39].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[39].zero = 0;
	IDT[39].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[39].offset_higherbits = (irq7_address & 0xffff0000) >> 16;
 
	irq8_address = (unsigned long)irq8; 
	IDT[40].offset_lowerbits = irq8_address & 0xffff;
	IDT[40].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[40].zero = 0;
	IDT[40].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[40].offset_higherbits = (irq8_address & 0xffff0000) >> 16;
 
	irq9_address = (unsigned long)irq9; 
	IDT[41].offset_lowerbits = irq9_address & 0xffff;
	IDT[41].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[41].zero = 0;
	IDT[41].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[41].offset_higherbits = (irq9_address & 0xffff0000) >> 16;
 
	irq10_address = (unsigned long)irq10; 
	IDT[42].offset_lowerbits = irq10_address & 0xffff;
	IDT[42].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[42].zero = 0;
	IDT[42].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[42].offset_higherbits = (irq10_address & 0xffff0000) >> 16;
 
	irq11_address = (unsigned long)irq11; 
	IDT[43].offset_lowerbits = irq11_address & 0xffff;
	IDT[43].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[43].zero = 0;
	IDT[43].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[43].offset_higherbits = (irq11_address & 0xffff0000) >> 16;
 
	irq12_address = (unsigned long)irq12; 
	IDT[44].offset_lowerbits = irq12_address & 0xffff;
	IDT[44].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[44].zero = 0;
	IDT[44].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[44].offset_higherbits = (irq12_address & 0xffff0000) >> 16;
 
	irq13_address = (unsigned long)irq13; 
	IDT[45].offset_lowerbits = irq13_address & 0xffff;
	IDT[45].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[45].zero = 0;
	IDT[45].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[45].offset_higherbits = (irq13_address & 0xffff0000) >> 16;
 
	irq14_address = (unsigned long)irq14; 
	IDT[46].offset_lowerbits = irq14_address & 0xffff;
	IDT[46].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[46].zero = 0;
	IDT[46].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[46].offset_higherbits = (irq14_address & 0xffff0000) >> 16;
 
        irq15_address = (unsigned long)irq15; 
	IDT[47].offset_lowerbits = irq15_address & 0xffff;
	IDT[47].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
	IDT[47].zero = 0;
	IDT[47].type_attr = 0x8e; /* INTERRUPT_GATE */
	IDT[47].offset_higherbits = (irq15_address & 0xffff0000) >> 16;
 
	/* fill the IDT descriptor */
	idt_address = (unsigned long)IDT ;
	idt_ptr[0] = (sizeof (struct IDT_entry) * 286) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;

	load_idt(idt_ptr);
}
#endif
