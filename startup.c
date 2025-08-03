#include <stdint.h>

extern uint32_t _estack;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _sdata_flash;

extern void main(void);
extern int puts(const char *str);

void reset_handler(void)
{
    // SEE: [Anders Sundman: Low, Lower, Lowest level Programming](https://youtu.be/-uZRiTgqQRU)

    // cody data section for flash to RAM
    uint32_t *src = &_sdata_flash;
    uint32_t *dest = &_sdata;
    while (dest < &_edata) { *dest++ = *src++; }

    // Zero out BSS
    dest = &_sbss;
    while (dest < &_ebss) { *dest++ = 0; }
    
	/* jump to C entry point */
	main();

    for (;;) {  }
}


void nmi_handler(void)
{
    puts("NMI Handler");

}

void hardfault_handler(void)
{
    // see: https://github.com/memfault/memfault-firmware-sdk/blob/master/components/panics/src/memfault_fault_handling_arm.c#L214
    puts("Hardfault Handler");
}

void memmanage_handler(void)
{
    puts("MemManage Handler");
}

void busfault_handler(void)
{
    puts("Bus Fault Handler");
}


void usagefault_handler(void)
{
    puts("Usage Fault Handler");
}

const uint32_t isr_vectors[] __attribute__((section(".isr_vector"))) = {
	(uint32_t)&_estack,
	(uint32_t) reset_handler,	/* code entry point */
    (uint32_t) nmi_handler,
    (uint32_t) hardfault_handler,
    (uint32_t) memmanage_handler,
    (uint32_t) busfault_handler,
    (uint32_t) usagefault_handler,
    0,
    0,
    0
};
