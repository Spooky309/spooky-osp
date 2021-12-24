%ifndef _PGTBL_
%define _PGTBL_
times 4096-($-$$) db 0x00 ; 4KiB alignment
id_page_table:
	; 1024 empty 4 byte entries, this will be filled by code (or a macro)
	; this SHOULD occupy exactly 4KiB in the image...
	times 4096 db 0
page_directory:
	db 11000000b
	db 0
	dw id_page_table
	; first entry is for identity mapped lower meg
	; rest of entries with the following attribs:
	; NOT present; READ/WRITE; USERMODE; WRITE-THROUGH caching; NO address
	times 1023 dd 00110000000000000000000000000000b
%endif
