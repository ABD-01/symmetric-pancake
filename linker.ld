ENTRY(Reset_Handler)

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

        _etext = .;
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
        __bss_start__ = _sbss;
        *(.bss)
        *(.bss*)
        _ebss = .;
        __bss_end__ = _ebss;
    } >RAM

  .ARM (READONLY) :
  {
    . = ALIGN(4);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
    . = ALIGN(4);
  } >FLASH
  /* See: https://alexkalmuk.medium.com/how-stack-trace-on-arm-works-5634b35ddca1
   * also: https://github.com/torvalds/linux/blob/6f0d349d922ba44e4348a17a78ea51b7135965b1/arch/arm/kernel/vmlinux.lds.h
   */

  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >RAM
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

_Min_Heap_Size = 0x200; /* required amount of heap */
_Min_Stack_Size = 0x400; /* required amount of stack */
