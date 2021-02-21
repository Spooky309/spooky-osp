bits 32
section .text
%include "kern/term.inc"                   ; basic terminal functionality for pre-C printing
%include "kern/idt.inc"                    ; C side of IDT stuff is in interrupt.h.
global start
extern puts
extern kmain
start:
  cli                                      ; interrupts are re-enabled by idt init function in idt.inc
  call                  ClrScr32           ; term.inc, this function is used by C code too
  call                  kmain              ; kmain should never return, if it does, we're retarded, so make sure we let us know
  ; we don't care about saving stack frame, since we are about to die.
  mov                   ebp, esp
  push                  qmsg
  call                  puts
hang:
  cli
  hlt
  jmp hang
  
section .rodata
qmsg: db "kmain returned. shouldn't happen.", 0x0