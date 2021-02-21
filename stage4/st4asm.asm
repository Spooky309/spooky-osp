bits 32
section .text
%include "stage4/term.inc"
global start
extern puts
extern st4main
start:
  cli                                      ; interrupts are re-enabled by idt init function in idt.inc
  call                  ClrScr32           ; term.inc, this function is used by C code too
  call                  st4main            ; kmain should never return, if it does, we're retarded, so make sure we let us know
hang:
  cli
  hlt
  jmp hang