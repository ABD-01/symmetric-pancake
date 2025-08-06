#include <stdint.h>

extern uint32_t _estack;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _sdata_flash;

extern void main(void);
extern int puts(const char *str);

void Reset_Handler(void) {
  // SEE: [Anders Sundman: Low, Lower, Lowest level
  // Programming](https://youtu.be/-uZRiTgqQRU)

  // cody data section for flash to RAM
  uint32_t *src = &_sdata_flash;
  uint32_t *dest = &_sdata;
  while (dest < &_edata) { *dest++ = *src++; }

  // Zero out BSS
  dest = &_sbss;
  while (dest < &_ebss) { *dest++ = 0; }

  /* jump to C entry point */
  main();

  for (;;) {
  }
}

void NMI_Handler(void) { puts("NMI Handler"); }

void HardFault_Handler(void) {
  // see:
  // https://github.com/memfault/memfault-firmware-sdk/blob/master/components/panics/src/memfault_fault_handling_arm.c#L214
  puts("HardFault Handler");
}

void MemManage_Handler(void) { puts("MemManage Handler"); }

void BusFault_Handler(void) { puts("BusFault Handler"); }

void UsageFault_Handler(void) { puts("Usage Fault Handler"); }

void SVC_Handler(void) {}
void DebugMon_Handler(void) {}

const uint32_t isr_vectors[] __attribute__((section(".isr_vector"))) = {
    (uint32_t)&_estack,
    (uint32_t)Reset_Handler, /* code entry point */
    (uint32_t)NMI_Handler,
    (uint32_t)HardFault_Handler,
    (uint32_t)MemManage_Handler,
    (uint32_t)BusFault_Handler,
    (uint32_t)UsageFault_Handler,
    0,
    0,
    0,
    0,
    (uint32_t)SVC_Handler,
    (uint32_t)DebugMon_Handler,
    0};
