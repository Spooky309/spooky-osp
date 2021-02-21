SECTIONS
{
	. = 0xBFC01000;
	
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
	_end_ = ABSOLUTE(.);
}
ENTRY(start)
