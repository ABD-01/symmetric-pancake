ENTRY(Reset_Handler)

_estack = ORIGIN(RAM) + LENGTH(RAM);

_Min_Heap_Size = 0x200; /* required amount of heap */
_Min_Stack_Size = 0x400; /* required amount of stack */

MEMORY
{
	FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 128K
    RAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 0x1F00
    DumpRAM (xrw) : ORIGIN = 0x20001F00, LENGTH = 0x100
}

SECTIONS
{
	.text :
	{
		KEEP(*(.isr_vector))
		*(.text)
		*(.text*)
		*(.glue_7)
		*(.glue_7t)
		*(.eh_frame)

        KEEP (*(.init))
        KEEP (*(.fini))

        . = ALIGN(4);
        _etext = .;
	} >FLASH

    /* constant data goes into FLASH */
    .rodata :
    {
        . = ALIGN(4);
        __CMD_TABLE_START__ = .;
        KEEP(*(.cmd_list*))
        __AUTO_TABLE_START__ = .;
        KEEP(*(.auto_list*))
        KEEP(*(.cmd_end*))
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } >FLASH

    .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >FLASH
    .ARM (READONLY) : {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } >FLASH
    /* See: https://alexkalmuk.medium.com/how-stack-trace-on-arm-works-5634b35ddca1
     * also: https://github.com/torvalds/linux/blob/6f0d349d922ba44e4348a17a78ea51b7135965b1/arch/arm/kernel/vmlinux.lds.h
     */

    .preinit_array     :
    {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    } >FLASH
    .init_array :
    {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array*))
        PROVIDE_HIDDEN (__init_array_end = .);
    } >FLASH
    .fini_array :
    {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array*))
        PROVIDE_HIDDEN (__fini_array_end = .);
    } >FLASH

    /* used by the startup to initialize data */
    _sidata = LOADADDR(.data);

    /* initialized data sections goes into RAM */
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)

        . = ALIGN(4);
        _edata = .;
    } >RAM AT >FLASH

    . = ALIGN(4);
    .bss :
    {
        _sbss = .;
        __bss_start__ = _sbss;
        *(.bss)
        *(.bss*)
        _ebss = .;
        __bss_end__ = _ebss;
    } >RAM

    ._user_heap_stack :
    {
      . = ALIGN(8);
      PROVIDE ( end = . );
      PROVIDE ( _end = . );
      . = . + _Min_Heap_Size;
      . = . + _Min_Stack_Size;
      . = ALIGN(8);
    } >RAM

    /* Memfault integration */
    .noinit (NOLOAD): { KEEP(*(*.noinit.mflt*)) } >RAM

    .note.gnu.build-id :
    {
        __start_gnu_build_id_start = .;
        KEEP(*(.note.gnu.build-id))
    } >FLASH

    .CoreDump (NOLOAD) : {
        _coredumpBuffer = .;
    } >DumpRAM

    /* Remove information from the standard libraries */
    /DISCARD/ :
    {
      libc.a ( * )
      libm.a ( * )
      libgcc.a ( * )
    }

    .ARM.attributes 0 : { *(.ARM.attributes) }
}
