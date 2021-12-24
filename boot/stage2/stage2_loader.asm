bits 16
org 0x1000
; bootloader jumps here after loading KERN.BIN to 0x0500
jmp                     0x0000:start                   ; jump over the functions and set cs to 0

%include "boot/include_common/FAT12Access16.inc"
; data and shit
; where the kernel is to be loaded to in protected mode
%define IMAGE_PMODE_BASE 0x100000
; where the kernel is to be loaded to in real mode
%define IMAGE_RMODE_BASE 0x5000
; kernel name (Must be 11 bytes)
ImageName     db "ST4     LIG"
; size of kernel image in bytes
ImageSize     dd 0
;---------------------------------------
; REAL MODE HELPER FUNCTIONS
; DO NOT USE THESE FROM PROTECTED MODE
; THEY WILL RUIN YOUR LIFE AND EVERYONE
; WILL LAUGH AT YOU.
;---------------------------------------
wait_output:                               ; wait for output buffer to be clear
  in               al, 0x64
  test             al, 01b
  jz               wait_output
  ret
wait_input:                                ; wait for input buffer to be clear
  in               al, 0x64
  test             al, 10b
  jnz              wait_input
  ret
;---------------------------------------
; REAL MODE ENTRY POINT
;---------------------------------------
start:
  cli
  mov              ax, cs
  mov              ds, ax
  mov              es, ax
  mov              ax, 0x00
  mov              ss, ax
  mov              sp, 0xFFFF
  mov		       [dosDriveNumber], dl

  cli
  lgdt             [gdt_desc]              ; load gdt descriptor into GDTR
  ; enable a20 line (to let us access 32-bit memory addrs)
  call             wait_input
  mov              al,0xAD
  out              0x64,al                          ; disable keyboard
  call             wait_input

  mov              al,0xD0
  out              0x64,al                          ; tell controller to read output port
  call             wait_output

  in               al,0x60
  push             eax                              ; get output port data and store it
  call             wait_input

  mov              al,0xD1
  out              0x64,al                          ; tell controller to write output port
  call             wait_input

  pop              eax
  or               al,2                             ; set bit 1 (enable a20)
  out              0x60,al                          ; write out data back to the output port

  call             wait_input
  mov              al,0xAE                          ; enable keyboard
  out              0x64,al

  call             wait_input
  sti
  call             LoadRoot ; load root directory to wherever it wants to go i guess lmao
  ; load KRNL.LIG
  mov              ebx, 0
  push             0
  mov              bp, IMAGE_RMODE_BASE
  mov              si, ImageName
  call             LoadFile
  mov              DWORD [ImageSize], ecx
  cmp              ax, 0
  je               EnterStage3
  cli
  hlt
  ; now we can switch to protected mode
EnterStage3:
  cli
  ; enter protected mode and jump to stage 3 bootstrap
  mov              eax, cr0                         ; bit 0 of cr0 is protected mode enable bit
  or               eax, 1
  mov              cr0, eax
  jmp              0x08:Stage3                      ; long jump sets cs for us
  cli
  hlt
  
data:
  msg db 'Booting...', 0x0D, 0x0A, 0x00
gdt:
;0x00 gdt_null:
  dd 0
  dd 0
;0x08 gdt_code:
  dw 0xFFFF
  dw 0x0000
  db 0
  db 10011010b
  db 11001111b
  db 0
;0x10 gdt_data:
  dw 0xFFFF                                ; limit low word
  dw 0x0000                                ; base low word
  db 0                                     ; base mid byte
  db 10010010b                             ; access flags
  db 11001111b                             ; granularity flags (+limit high nibble)
  db 0                                     ; base high byte
; 0x18 r3_code:
  dw 0xFFFF
  dw 0x0000
  db 0
  db 11111010b
  db 11001111b
  db 0
; 0x20 r3_data:
  dw 0xFFFF                                ; limit low word
  dw 0x0000                                ; base low word
  db 0                                     ; base mid byte
  db 11110010b                             ; access flags
  db 11001111b                             ; granularity flags (+limit high nibble)
  db 0                                     ; base high byte
gdt_end:
gdt_desc:
  dw gdt_end - gdt - 1
  dd gdt                                   ; gdt needs to be referenced in flat memory

bits 32
Stage3:
  ; segments
  mov	ax, 0x10
  mov	ds, ax
  mov	ss, ax
  mov	es, ax
  ; new stack
  mov	esp, 90000h
  mov ebp, esp
  ; relocate kernel image to 0x100000
  mov	eax, dword [ImageSize]
  movzx	ebx, word [dosBytesPerSector]
  mul	ebx
  mov	ebx, 4
  div	ebx
  cld
  mov esi, IMAGE_RMODE_BASE
  mov	edi, IMAGE_PMODE_BASE
  mov	ecx, eax
  rep	movsd
  xor eax, eax
  ; set up the lower meg page table
  mov eax, 0
  mov ebx, 0
  .fill_table:
    mov ecx, ebx
    or ecx, 3
    mov [id_page_table+eax*4], ecx
    add ebx, 4096
    inc eax
    cmp eax, 256
    je .end
    jmp .fill_table
  .end:
  ; set up kernel table (entire table is 0x100000 to 0x3FFFFF)
  mov eax, 0
  mov ebx, 0x100000
  .fill_kerntable:
    mov ecx, ebx
    or ecx, 3
    mov [0x4000+eax*4], ecx
    add ebx, 4096 ; next 4k
    inc eax
    cmp eax, 3072 ; 3072 pages for 3 full page tables
    je .end_kerntable
    jmp .fill_kerntable
  .end_kerntable:
  ; setup dir
  mov ax, id_page_table                    ; address of identity table
  or eax, 3                                ; flags (present, RW, kmode only) 
  mov [page_directory],eax                 ; put it in (that's what she said lmao)
  
  mov ax, 0x4000                           ; address of kernel table
  or eax, 7                                ; flags (present, RW, umode)
  mov [page_directory+767*4], eax          ; table entry 767 maps to virtual address 0xBFC00000
  
  mov ax, 0x5000                           ; address of kernel table
  or eax, 7                                ; flags (present, RW, umode)
  mov [page_directory+768*4], eax          ; table entry 768 maps to virtual address 0xC0000000
  
  mov ax, 0x6000                           ; address of kernel table
  or eax, 7                                ; flags (present, RW, umode)
  mov [page_directory+769*4], eax          ; table entry 769 maps to virtual address 0xC0400000
  ; enable paging
  mov eax, page_directory
  mov cr3, eax
  mov eax, cr0
  or eax, 0x80000001
  mov cr0, eax
  ; the following is spectacularly retarded, we should not have to offset anything in the linker
  ; set up kernel stack
  mov   esp, 0xC03FFFFF
  mov   ebp, esp
  mov   eax, [0xBFC00000 + 0x18]
  push  0x08
  push  eax                                ; pushing offset onto stack for evil far return trick
  retf                                     ; x86 instruction set abuse
  cli
  hlt
; page table stuff included down here
%include "boot/stage2/pdirtable.inc"
