%include "boot/include_common/FAT12BPB.inc"     ; we need the BPB for the next bit
ReadSectors:
  .MAIN:
    mov     di, 0x0005                          ; five retries for error
  .SECTORLOOP:
    push    ax
    push    bx
    push    cx
    call    LBACHS                              ; convert starting sector to CHS
    mov     ah, 0x02                            ; BIOS read sector
    mov     al, 0x01                            ; read one sector
    mov     ch, BYTE [absoluteTrack]            ; track
    mov     cl, BYTE [absoluteSector]           ; sector
    mov     dh, BYTE [absoluteHead]             ; head
    mov     dl, BYTE [dosDriveNumber]           ; drive
    int     0x13                                ; invoke BIOS
    jnc     .SUCCESS                            ; test for read error
    xor     ax, ax                              ; BIOS reset disk
    int     0x13                                ; invoke BIOS
    dec     di                                  ; decrement error counter
    pop     cx
    pop     bx
    pop     ax
    jnz     .SECTORLOOP                         ; attempt to read again
    int     0x18
  .SUCCESS:
    pop     cx
    pop     bx
    pop     ax
    add     bx, WORD [dosBytesPerSector]        ; queue next buffer
    inc     ax                                  ; queue next sector
    loop    .MAIN                               ; read next sector
    ret

ClusterLBA:
  sub     ax, 0x0002                            ; zero base cluster number
  xor     cx, cx
  mov     cl, BYTE [dosSectorsPerCluster]       ; convert byte to word
  mul     cx
  add     ax, WORD [datasector]                 ; base data sector
  ret

LBACHS:
  xor     dx, dx                                ; prepare dx:ax for operation
  div     WORD [dosSectorsPerTrack]             ; calculate
  inc     dl                                    ; adjust for sector 0
  mov     BYTE [absoluteSector], dl
  xor     dx, dx                                ; prepare dx:ax for operation
  div     WORD [dosNumHeads]                    ; calculate
  mov     BYTE [absoluteHead], dl
  mov     BYTE [absoluteTrack], al
  ret
absoluteSector db 0x00
absoluteHead   db 0x00
absoluteTrack  db 0x00
datasector  dw 0x0000
cluster     dw 0x0000