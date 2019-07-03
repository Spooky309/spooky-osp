%include "boot/include_common/Floppy16.inc"

%define ROOT_OFFSET 0x2e00
%define FAT_SEG 0x2c0
%define ROOT_SEG 0x2e0

LoadRoot:
  pusha
    push                  es
    xor                   cx, cx
    xor                   dx, dx
    mov                   ax, 32
    mul                   WORD [dosMaxRootDirEntries]
    div                   WORD [dosBytesPerSector]
    xchg                  ax, cx
    mov                   al, BYTE [dosFatCount]
    mul                   WORD [dosSectorsPerFat]
    add                   ax, WORD [dosReservedSectors]
    mov                   WORD [datasector], ax
    add                   WORD [datasector], cx
    push                  WORD ROOT_SEG
    pop                   es
    mov                   bx, 0x0
    call                  ReadSectors
    pop                   es
  popa
  ret
  
LoadFAT:
  pusha
    push                  es
    xor                   ax, ax
    mov                   al, BYTE [dosFatCount]
    mul                   WORD [dosSectorsPerFat]
    mov                   cx, ax
    mov                   ax, WORD [dosReservedSectors]
    push                  WORD FAT_SEG
    pop                   es
    xor                   bx, bx
    call                  ReadSectors
    pop                   es
  popa
  ret
  
FindFile:
  push                    cx
  push                    dx
  push                    bx
  mov                     bx, si
  mov                     cx, WORD [dosMaxRootDirEntries]
  mov                     di, ROOT_OFFSET
  cld
  .LOOP:
    push                  cx
    mov                   cx, 11
    mov                   si, bx
    push                  di
    rep                   cmpsb
    pop                   di
    je                    .Found
    pop                   cx
    add                   di, 32
    loop                  .LOOP
  .NotFound:
    pop                   bx
    pop                   dx
    pop                   cx
    pop                   ds
    mov                   ax, -1 ; FnF
    ret
  .Found:
    pop                   ax
    pop                   bx
    pop                   dx
    pop                   cx
    ret
    
LoadFile:
  xor                     ecx, ecx
  push                    ecx
  .FIND_FILE:
    push                  bx
    push                  bp
    call                  FindFile
    cmp                   ax, -1
    jne                   .LOAD_IMAGE_PRE
    pop                   bp
    pop                   bx
    pop                   ecx
    mov                   ax, -1
    ret
  .LOAD_IMAGE_PRE:
    sub                   edi, ROOT_OFFSET
    sub                   eax, ROOT_OFFSET
    push                  WORD ROOT_SEG
    pop                   es
    mov                   dx, WORD [es:di + 0x001A]
    mov                   WORD [cluster], dx
    pop                   bx
    pop                   es
    push                  bx
    push                  es
    call                  LoadFAT
  .LOAD_IMAGE:
    mov                   ax, WORD [cluster]
    pop                   es
    pop                   bx
    call                  ClusterLBA
    xor                   cx, cx
    mov                   cl, BYTE [dosSectorsPerCluster]
    call                  ReadSectors
    pop                   ecx
    inc                   ecx
    push                  ecx
    push                  bx
    push                  es
    mov                   ax, FAT_SEG
    mov                   es, ax
    xor                   bx, bx
    mov                   ax, WORD [cluster]
    mov                   cx, ax
    mov                   dx, ax
    shr                   dx, 0x0001
    add                   cx, dx
    mov                   bx, 0
    add                   bx, cx
    mov                   dx, WORD [es:bx]
    test                  ax, 0x0001
    jnz                   .ODD_CLUSTER
    .EVEN_CLUSTER:
      and                 dx, 0000111111111111b
      jmp                 .DONE
    .ODD_CLUSTER:
      shr                 dx, 0x0004
    .DONE:
      mov                 WORD [cluster], dx
      cmp                 dx, 0x0FF0
      jb                  .LOAD_IMAGE
  DONE:
    pop                   es
    pop                   bx
    pop                   ecx
    xor                   ax, ax
    ret
