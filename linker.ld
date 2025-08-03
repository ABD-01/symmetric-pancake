ENTRY(reset_handler)

MEMORY
{
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 8K
	FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 128K
}

SECTIONS
{
	.text :
	{
		KEEP(*(.isr_vector))
		*(.text)
	} >FLASH

    .rodata :
    {
        *(.rodata)
        *(.rodata*)
    } >FLASH

    .data :
    {
        _sdata_flash = LOADADDR(.data);
        _sdata = .;
        *(.data)
        *(.data*)
        _edata = .;
    } >RAM AT >FLASH

    .bss :
    {
        _sbss = .;
        *(.bss)
        *(.bss*)
        _ebss = .;
    } >RAM
}

_estack = ORIGIN(RAM) + LENGTH(RAM);
