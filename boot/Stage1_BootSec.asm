bits 16
org 0

entry:
jmp short main                                  ; jmp over bpb
nop                                             ; bpb starts at byte 4 
%include "boot/include_common/FAT12BPB.inc"     ; Helper functions for loading data from a floppy

main:
  cli
  ; we use segments starting at 0x7c00
  mov     ax, 0x07C0
  mov     ds, ax
  mov     es, ax
  mov     fs, ax
  mov     gs, ax
  mov     ax, 0x0000
  mov     ss, ax
  mov     sp, 0xFFFF
  mov	  [dosDriveNumber], dl
  sti
	
LOAD_ROOT:
  ; compute size of root directory and store in "cx"
  xor     cx, cx
  xor     dx, dx
  mov     ax, 0x0020                            ; 32 byte directory entry
  mul     WORD [dosMaxRootDirEntries]           ; total size of directory
  div     WORD [dosBytesPerSector]              ; sectors used by directory
  xchg    ax, cx
          
  ; compute location of root directory and store in "ax"
  mov     al, BYTE [dosFatCount]                ; number of FATs
  mul     WORD [dosSectorsPerFat]               ; sectors used by FATs
  add     ax, WORD [dosReservedSectors]         ; adjust for bootsector
  mov     WORD [datasector], ax                 ; base of root directory
  add     WORD [datasector], cx                 
  mov     bx, 0x0200                            ; copy root dir above bootcode
  call    ReadSectors

  ; search root dir for image

  ; browse root directory for binary image
  mov     cx, WORD [dosMaxRootDirEntries]       ; load loop counter
  mov     di, 0x0200                            ; locate first root entry
  .LOOP:
    push    cx
    mov     cx, 0x000B                          ; eleven character name
    mov     si, ImageName                       ; image name to find
    push    di
    rep     cmpsb                               ; test for entry match
    pop     di
    je      LOAD_FAT
    pop     cx
    add     di, 0x0020                          ; queue next directory entry
    loop    .LOOP
    jmp     FAILURE

     ; Load FAT

LOAD_FAT:
  mov     si, msgCRLF                           
  call    Print                                 
  mov     dx, WORD [di + 0x001A]                
  mov     WORD [cluster], dx                    ; file's first cluster

  ; compute size of FAT and store in "cx"       

  xor     ax, ax                                
  mov     al, BYTE [dosFatCount]                ; number of FATs
  mul     WORD [dosSectorsPerFat]               ; sectors used by FATs
  mov     cx, ax                                
  ; compute location of FAT and store in "ax"   
  mov     ax, WORD [dosReservedSectors]         ; adjust for bootsector

  ; read FAT into memory (7C00:1200) (far away from the directory table)           

  mov     bx, WORD 0x0400                       ; copy FAT above bootcode
  call    ReadSectors                           

  ; read image file into memory (0050:0000)     

  mov     si, msgCRLF                           
  call    Print                                 
  mov     ax, 0x0100                            
  mov     es, ax                                ; destination for image
  mov     bx, 0x0000                            ; destination for image
  push    bx

  ; Load KRNLDR.LIG                               

LOAD_IMAGE:                                     
  mov     ax, WORD [cluster]                    ; cluster to read
  pop     bx                                    ; buffer to read into
  call    ClusterLBA                            ; convert cluster to LBA
  xor     cx, cx                                
  mov     cl, BYTE [dosSectorsPerCluster]       ; sectors to read

  call    ReadSectors                           
  push    bx                                    

  ; compute next cluster                        

  mov     ax, WORD [cluster]                    ; identify current cluster
  mov     cx, ax                                ; copy current cluster
  mov     dx, ax                                ; copy current cluster
  shr     dx, 0x0001                            ; divide by two
  add     cx, dx                                ; sum for (3/2)
  mov     bx, 0x0400                            ; location of FAT in memory
  add     bx, cx                                ; index into FAT
  mov     dx, WORD [bx]                         ; read two bytes from FAT
  test    ax, 0x0001
  jnz     .ODD_CLUSTER
  .EVEN_CLUSTER:
    and     dx, 0000111111111111b               ; take low twelve bits
    jmp     .DONE
  .ODD_CLUSTER:
    shr     dx, 0x0004  		        ; take high twelve bits
  .DONE:
    mov     WORD [cluster], dx                  ; store new cluster
    cmp     dx, 0x0FF0                          ; test for end of file
    jb      LOAD_IMAGE
          
DONE:
  mov	  si, msgGo
  call	  Print
  mov     si, msgCRLF
  call    Print
  mov     dl, [dosDriveNumber]
  jmp     0x0100:0x0000                         ; far jump to the stage 2 loaded at 0x1000 (this also sets cs)

FAILURE:
  mov     ah, 0x00
  int     0x16                                  ; await keypress
  int     0x19                                  ; warm boot computer
  cli
  hlt

%include "boot/include_common/Floppy16.inc"     ; Helper functions for loading data from a floppy
Print:
  lodsb
  or	al, al
  jz	.done
  mov	ah, 0eh
  int	10h
  jmp	Print
  .done:
    ret	

ImageName            db "KRNLDR  LIG"
msgCRLF              db 0x0D, 0x0A, 0x00
msgGo                db 0x0D, 0x0A, 0x00, "Let's go.", 0
times 510-$+$$       db 0
dw 0xAA55
times 1474560-($-$$) db 0x00 ; 1.44MiB empty space to put files in! yay!
