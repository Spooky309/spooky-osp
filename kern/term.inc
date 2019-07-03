%ifndef _TERM_INC_
%define _TERM_INC_
%define VIDMEM      0xB8000
%define ROWS        25
%define COLS        80
%define CHAR_ATTRIB 0x1F
global ClrScr32
ClrScr32:
  pusha
  cld
  mov           edi, VIDMEM
  mov           ecx, 2000
  mov           ah, CHAR_ATTRIB
  mov           al, 0x00
  rep           stosw
  popa
  ret
%endif
