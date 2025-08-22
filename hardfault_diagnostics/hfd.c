#include "stm32f1xx.h"
#include "utils.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "hfd.h"

static void dump_stack(uint32_t *, uint32_t *);

extern uint32_t volatile _coredumpBuffer;
extern uint32_t _estack;

struct regs_t __attribute__((section(".CoreDump"))) coreDump = {0};

__attribute__((naked)) void HardFault_Handler(void)
{
    __asm("TST    LR, #4      \n"
          "ITE    EQ          \n"
          "MRSEQ  R0, MSP     \n"
          "MRSNE  R0, PSP     \n"
          "// TODO: see is second parameter is needed\n"
          "MOV    R1, R0      \n"
          "STMDB  R0!, {r1, r4-r11}\n"
          "MOV    SP, R0      \n"
          "MOV    R1, LR      \n"
          "B      HardFault_Handler_Main\n");
}

void HardFault_Handler_Main(struct regs_t *regs, uint32_t lr)
{
    // TODO: handle edge cases
    // if rges is NULL
    // if regs->sp is invalid
    // if the printf causes another hardfault, the cpu will lock up

    __disable_irq();

    uint32_t *coredump_ptr = (uint32_t *)&_coredumpBuffer;
    *coredump_ptr          = 1;
    coredump_ptr++;
    memcpy(coredump_ptr, regs, sizeof(struct regs_t));

    printf("[HardFault]\r\n");

    printf("\r\n---Registers---\r\n");

    printf("        r0: %08lX        r1: %08lX        r2: %08lX\r\n", regs->r0, regs->r1, regs->r2);
    printf("        r3: %08lX        r4: %08lX        r5: %08lX\r\n", regs->r3, regs->r4, regs->r5);
    printf("        r6: %08lX        r7: %08lX        r8: %08lX\r\n", regs->r6, regs->r7, regs->r8);
    printf("        r9: %08lX       r10: %08lX       r11: %08lX\r\n", regs->r9, regs->r10, regs->r11);
    printf("       r12: %08lX        sp: %08lX        lr: %08lX\r\n", regs->r12, regs->sp, regs->lr);
    printf("        pc: %08lX      xPSR: %08lX\r\n", regs->pc, regs->xPSR);

    printf("       psp: %08lX       msp: %08lX   primask: %08lX\r\n", __get_PSP(), __get_MSP(), __get_PRIMASK());
    printf("   control: %08lX   basepri: %08lX faultmask: %08lX\r\n", __get_CONTROL(), __get_BASEPRI(), __get_FAULTMASK());

    printf("\r\n---Special Registers---\r\n");
    printf("EXC_RETURN: %08lX\r\n", lr);
    printf("      ICSR: %08lX (" INT32_BINARY_PATTERN ")\r\n", SCB->ICSR, PRINT_INT32_BIN(SCB->ICSR));
    printf("      HFSR: %08lX (" INT32_BINARY_PATTERN ") %s\r\n", SCB->HFSR, PRINT_INT32_BIN(SCB->HFSR), (SCB->HFSR & SCB_HFSR_FORCED_Msk) ? "// HFSR.FORCED: 1" : "");

    // See The Definitive Guide to ARM® CORTEX®-M3 and CORTEX®-M4 Processors, Chapter 12, Page 387
    // If HardFault is forced, then check CFSR for the cause
    // CFSR is made of three sub-registers: MMFSR (8 bits), BFSR (8 bits), UFSR (16 bits)
    printf("      CFSR: %08lX (" INT32_BINARY_PATTERN ")\r\n", SCB->CFSR, PRINT_INT32_BIN(SCB->CFSR));
    if ((SCB->CFSR & 0xFF) && (SCB->CFSR & SCB_CFSR_MMARVALID_Msk)) {
        // if MMARVALID is set, then MMFAR is valid. Ref: Table 12.3 in the above book
        printf("     MMFAR: %08lX\r\n", SCB->MMFAR);
    }
    if ((SCB->CFSR & 0xFF00) && (SCB->CFSR & SCB_CFSR_BFARVALID_Msk)) { printf("      BFAR: %08lX\r\n", SCB->BFAR); }

    printf("\r\n---Stack Dump---\r\n");
    dump_stack((uint32_t *)regs->sp, (uint32_t *)&_estack);

    printf("\r\n---System Reset---\r\n");

    NVIC_SystemReset();

    // Unreachable
    while (1) {}
}


static void dump_stack(uint32_t *sp, uint32_t *estack)
{
    estack = (uint32_t *)((uint32_t)estack & ~0xF);
    uint32_t *addr = (uint32_t *)((uint32_t)sp & ~0xF);

    // Edge Case: if sp is unaligned
    if (addr < sp) {
        printf("0x%08lX: ", addr);
        for (int i = 0; i < 4; i++) {
            if (addr < sp) {
                printf("???????? ");
            } else {
                printf("%08lX ", *addr);
            }
            addr++;
        }
        printf("\r\n");
    }

    while (addr < estack) {
        printf("0x%08lX: ", addr);
        for (int i = 0; i < 4; i++) {
            printf("%08lX ", *addr);
            addr++;
        }
        printf("\r\n");
    }
}

void check_last_hardfault(void)
{
    uint32_t *coredump_ptr = (uint32_t *)&_coredumpBuffer;
    if (*coredump_ptr != 0) {
        printf("HardFault occurred before...\r\n");
        *coredump_ptr = 0;
        coredump_ptr++;
        struct regs_t *regs = (struct regs_t *)coredump_ptr;
        printf("- Stack frame and Registers:\r\n");
        printf(" SP:   %08lX\r\n", regs->sp);
        printf(" R0:   %08lX\r\n", regs->r0);
        printf(" R1:   %08lX\r\n", regs->r1);
        printf(" R2:   %08lX\r\n", regs->r2);
        printf(" R3:   %08lX\r\n", regs->r3);
        printf(" R4:   %08lX\r\n", regs->r4);
        printf(" R5:   %08lX\r\n", regs->r5);
        printf(" R6:   %08lX\r\n", regs->r6);
        printf(" R7:   %08lX\r\n", regs->r7);
        printf(" R8:   %08lX\r\n", regs->r8);
        printf(" R9:   %08lX\r\n", regs->r9);
        printf(" R10:  %08lX\r\n", regs->r10);
        printf(" R11:  %08lX\r\n", regs->r11);
        printf(" R12:  %08lX\r\n", regs->r12);
        printf(" LR:   %08lX\r\n", regs->lr);
        printf(" PC:   %08lX\r\n", regs->pc);
        printf(" xPSR: %08lX\r\n", regs->xPSR);
    }
}