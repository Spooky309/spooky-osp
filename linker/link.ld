SECTIONS
{
  /* this will be the offset starting from .text used by the kernel */
  /* if paging is impl'd then this won't be needed anymore because kernel page will be properly set */
	. = 0xC0001000;
	
  .text BLOCK(4K) : ALIGN(4K)
	{
		*(.text)
	}
  .rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}
  .data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}
  .bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}
	. = ALIGN(4K);
	_heap_begin = ABSOLUTE(.);
}
ENTRY(start)
