#include <stdint.h>
#include <stdio.h>

#include "stm32f1xx.h"

#define PRINT_UINT32_BIN(x)                                                                                            \
    do {                                                                                                               \
        for (int i = 31; i >= 0; i--) {                                                                                \
            putchar(((x) >> i) & 1 ? '1' : '0');                                                                       \
            if (i % 4 == 0 && i != 0) putchar(' ');                                                                    \
        }                                                                                                              \
        putchar('\n');                                                                                                 \
    } while (0)

extern uint32_t _estack;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

extern void main(void);
extern int  puts(const char *str);

__attribute__((naked)) void Reset_Handler(void)
{
    // SEE: [Anders Sundman: Low, Lower, Lowest level
    // Programming](https://youtu.be/-uZRiTgqQRU)

    __ASM("ldr r0, =_sdata        \n"
          "ldr r1, =_edata        \n"
          "ldr r2, =_sidata       \n"
          "movs r3, #0            \n"
          "b LoopCopyDataInit     \n"

          "CopyDataInit:            \n"
          "ldr r4, [r2, r3]       \n"
          "str r4, [r0, r3]       \n"
          "adds r3, r3, #4        \n"

          "LoopCopyDataInit:        \n"
          "adds r4, r0, r3        \n"
          "cmp r4, r1             \n"
          "bcc CopyDataInit       \n"

          "ldr r2, =_sbss         \n"
          "ldr r4, =_ebss         \n"
          "movs r3, #0            \n"
          "b LoopFillZerobss      \n"

          "FillZerobss:             \n"
          "str  r3, [r2]          \n"
          "adds r2, r2, #4        \n"

          "LoopFillZerobss:         \n"
          "cmp r2, r4             \n"
          "bcc FillZerobss        \n");

    /* jump to C entry point */

    printf("Reset Reason: %lu\r\n", RCC->CSR);

    main();
    // Reset_Handler1();

    for (;;) {}
}

void NMI_Handler(void) { puts("NMI Handler"); }

void HardFault_Handler(void)
{
    // see:
    // https://github.com/memfault/memfault-firmware-sdk/blob/master/components/panics/src/memfault_fault_handling_arm.c#L214
    puts("HardFault Handler");

    volatile uint32_t *DHCSR = (volatile uint32_t *)0xE000EDF0;
    printf("DHCSR: ");
    PRINT_UINT32_BIN((uint32_t)DHCSR);

    volatile uint32_t *ICSR = (volatile uint32_t *)0xE000ED04;
    printf("\r\nICSR: ");
    PRINT_UINT32_BIN((uint32_t)ICSR);

    for (;;) {}
}

void MemManage_Handler(void)
{
    puts("MemManage Handler");
    for (;;) {}
}

void BusFault_Handler(void)
{
    puts("BusFault Handler");
    for (;;) {}
}

void PendSV_Handler(void)
{
    puts("PendSV_Handler");
    for (;;) {}
}

void SysTick_Handler(void)
{
    puts("SysTick_Handler");
    for (;;) {}
}

void WWDG_IRQHandler(void)
{
    puts("WWDG_IRQHandler");
    for (;;) {}
}

void PVD_IRQHandler(void)
{
    puts("PVD_IRQHandler");
    for (;;) {}
}

void UsageFault_Handler(void) { puts("Usage Fault Handler"); }

__attribute__((naked)) void SVC_Handler(void)
{
    __asm(".global    SVC_Handler_Main    \n\t"
          "tst    lr, #4                  \n\t"
          "ite    EQ                      \n\t"
          "mrseq  r0, MSP                 \n\t"
          "mrsne  r0, PSP                 \n\t"
          "b      SVC_Handler_Main        \n\t");
}

void SVC_Handler_Main(uint32_t *sp)
{
    /*
     * Stack contains:
     * r0, r1, r2, r3, r12, r14, the return address and xPSR
     * First argument (r0) is svc_args[0]
     */
    uint32_t svc_num;
    svc_num = ((char *)sp[6])[-2];

    printf("Received syscall: %lu\r\n", svc_num);

    NVIC_SystemReset();
}

void DebugMon_Handler(void)
{
    puts("DebugMon_Handler");
    for (;;) {}
}

const uint32_t isr_vectors[] __attribute__((section(".isr_vector"))) = {(uint32_t)&_estack,
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
                                                                        0,
                                                                        (uint32_t)PendSV_Handler,
                                                                        (uint32_t)SysTick_Handler,
                                                                        (uint32_t)WWDG_IRQHandler,
                                                                        (uint32_t)PVD_IRQHandler,
                                                                        0};
