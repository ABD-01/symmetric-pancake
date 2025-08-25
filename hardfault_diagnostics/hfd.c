#include "stm32f1xx.h"
#include "utils.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "hfd.h"

static void dump_stack(uint32_t const *, uint32_t const *);

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

void HardFault_Handler_Main(struct regs_t const *regs, uint32_t lr)
{
    // TODO: handle edge cases
    // if rges is NULL
    // if regs->sp is invalid
    // if the printf causes another hardfault, the cpu will lock up, See Section 12.7, pg 399

    __disable_irq();

    uint32_t *coredump_ptr = (uint32_t *)&_coredumpBuffer;
    *coredump_ptr          = 1;
    coredump_ptr++;
    memcpy(coredump_ptr, regs, sizeof(struct regs_t));

    HFD_LOG("[HardFault]\r\n");

    HFD_LOG("\r\n---Registers---\r\n");

    HFD_LOG("        r0: %08lX        r1: %08lX        r2: %08lX\r\n", regs->r0, regs->r1, regs->r2);
    HFD_LOG("        r3: %08lX        r4: %08lX        r5: %08lX\r\n", regs->r3, regs->r4, regs->r5);
    HFD_LOG("        r6: %08lX        r7: %08lX        r8: %08lX\r\n", regs->r6, regs->r7, regs->r8);
    HFD_LOG("        r9: %08lX       r10: %08lX       r11: %08lX\r\n", regs->r9, regs->r10, regs->r11);
    HFD_LOG("       r12: %08lX        sp: %08lX        lr: %08lX\r\n", regs->r12, regs->sp, regs->lr);
    HFD_LOG("        pc: %08lX      xPSR: %08lX\r\n", regs->pc, regs->xPSR);

    HFD_LOG("       psp: %08lX       msp: %08lX   primask: %08lX\r\n", __get_PSP(), __get_MSP(), __get_PRIMASK());
    HFD_LOG("   control: %08lX   basepri: %08lX faultmask: %08lX\r\n", __get_CONTROL(), __get_BASEPRI(), __get_FAULTMASK());

    HFD_LOG("\r\n---Special Registers---\r\n");
    HFD_LOG("EXC_RETURN: %08lX\r\n", lr);
    HFD_LOG("      ICSR: %08lX (" INT32_BINARY_PATTERN ")\r\n", SCB->ICSR, PRINT_INT32_BIN(SCB->ICSR));
    HFD_LOG("      HFSR: %08lX (" INT32_BINARY_PATTERN ") %s\r\n", SCB->HFSR, PRINT_INT32_BIN(SCB->HFSR), (SCB->HFSR & SCB_HFSR_FORCED_Msk) ? "// HFSR.FORCED: 1" : "");

    // See The Definitive Guide to ARM® CORTEX®-M3 and CORTEX®-M4 Processors, Chapter 12, Page 387
    // If HardFault is forced, then check CFSR for the cause
    // CFSR is made of three sub-registers: MMFSR (8 bits), BFSR (8 bits), UFSR (16 bits)
    uint32_t cfsr = SCB->CFSR;
    HFD_LOG("      CFSR: %08lX (" INT32_BINARY_PATTERN ")\r\n", cfsr, PRINT_INT32_BIN(cfsr));
    if ((cfsr & 0xFF) && (cfsr & SCB_CFSR_MMARVALID_Msk)) {
        // if MMARVALID is set, then MMFAR is valid. Ref: Table 12.3 in the above book
        HFD_LOG("     MMFAR: %08lX\r\n", SCB->MMFAR);
    }
    if ((cfsr & 0xFF00) && (cfsr & SCB_CFSR_BFARVALID_Msk)) { HFD_LOG("      BFAR: %08lX\r\n", SCB->BFAR); }

#if HFD_REASON_VERBOSE == 1
    HFD_LOG("\r\n---Fault Reason Details---\r\n");
    if (cfsr & 0xFF) {
        HFD_LOG("MemManage Fault: ");
        if (cfsr & SCB_CFSR_IACCVIOL_Msk) { HFD_LOG("IACCVIOL (Instruction Access Violation)"); }
        if (cfsr & SCB_CFSR_DACCVIOL_Msk) { HFD_LOG("DACCVIOL (Data Access Violation)"); }
        if (cfsr & SCB_CFSR_MUNSTKERR_Msk) { HFD_LOG("MUNSTKERR (Unstacking error)"); }
        if (cfsr & SCB_CFSR_MSTKERR_Msk) { HFD_LOG("MSTKERR (Stacking error)"); }

        // available only in cortex M4
        // if (cfsr & SCB_CFSR_MLSPERR_Msk) { HFD_LOG("MLSPERR (Floating-point lazy stacking error)"); }
        HFD_LOG("\r\n");
        if (cfsr & SCB_CFSR_MMARVALID_Msk) { HFD_LOG("MemManage Fault occurred at address: 0x%08lX\r\n", SCB->MMFAR); }
    }
    if (cfsr & 0xFF00) {
        HFD_LOG("Bus Fault: ");
        if (cfsr & SCB_CFSR_IBUSERR_Msk) { HFD_LOG("IBUSERR (Instruction Bus Error)"); }
        if (cfsr & SCB_CFSR_PRECISERR_Msk) { HFD_LOG("PRECISERR (Precise Data Bus Error)"); }
        if (cfsr & SCB_CFSR_IMPRECISERR_Msk) { HFD_LOG("IMPRECISERR (Imprecise Data Bus Error)"); }
        if (cfsr & SCB_CFSR_UNSTKERR_Msk) { HFD_LOG("UNSTKERR (Unstacking error)"); }
        if (cfsr & SCB_CFSR_STKERR_Msk) { HFD_LOG("STKERR (Stacking error)"); }
        // available only in cortex M4
        // if (cfsr & SCB_CFSR_LSPERR_Msk) { HFD_LOG("LSPERR (Floating-point lazy stacking error)"); }
        HFD_LOG("\r\n");
        if (cfsr & SCB_CFSR_BFARVALID_Msk) { HFD_LOG("Bus Fault occurred at address: 0x%08lX\r\n", SCB->BFAR); }
    }
    if (cfsr & 0xFFFF0000) {
        HFD_LOG("Usage Fault: ");
        if (cfsr & SCB_CFSR_UNDEFINSTR_Msk) { HFD_LOG("UNDEFINSTR (Undefined Instruction)"); }
        if (cfsr & SCB_CFSR_INVSTATE_Msk) { HFD_LOG("INVSTATE (Switch to Invalid State)"); }
        if (cfsr & SCB_CFSR_INVPC_Msk) {
            HFD_LOG("INVPC (Attempts to do an exception with a bad value in the EXC_RETURN number)");
        }
        if (cfsr & SCB_CFSR_NOCP_Msk) { HFD_LOG("NOCP (Attempts to execute a coprocessor instruction)"); }
        if (cfsr & SCB_CFSR_UNALIGNED_Msk) { HFD_LOG("UNALIGNED (Unaligned Access)"); }
        if (cfsr & SCB_CFSR_DIVBYZERO_Msk) { HFD_LOG("DIVBYZERO (Divide by Zero)"); }
        HFD_LOG("\r\n");
    }

#endif /* HFD_REASON_VERBOSE */

    HFD_LOG("\r\n---Stack Dump---\r\n");
    dump_stack((uint32_t *)regs->sp, (uint32_t *)&_estack);

    HFD_LOG("\r\n---System Reset---\r\n");

    NVIC_SystemReset();

    // Unreachable
    while (1) {}
}

static void dump_stack(uint32_t const *sp, uint32_t const *estack)
{
    estack         = (uint32_t *)((uint32_t)estack & ~0xF);
    uint32_t *addr = (uint32_t *)((uint32_t)sp & ~0xF);

    // Edge Case: if sp is unaligned
    if (addr < sp) {
        HFD_LOG("0x%08lX: ", addr);
        for (int i = 0; i < 4; i++) {
            if (addr < sp) {
                HFD_LOG("???????? ");
            } else {
                HFD_LOG("%08lX ", *addr);
            }
            addr++;
        }
        HFD_LOG("\r\n");
    }

    while (addr < estack) {
        HFD_LOG("0x%08lX: ", addr);
        for (int i = 0; i < 4; i++) {
            HFD_LOG("%08lX ", *addr);
            addr++;
        }
        HFD_LOG("\r\n");
    }
}

void check_last_hardfault(void)
{
    uint32_t *coredump_ptr = (uint32_t *)&_coredumpBuffer;
    if (*coredump_ptr != 0) {
        HFD_LOG("HardFault occurred before...\r\n");
        *coredump_ptr = 0;
        coredump_ptr++;
        struct regs_t *regs = (struct regs_t *)coredump_ptr;
        HFD_LOG("- Stack frame and Registers:\r\n");
        HFD_LOG(" SP:   %08lX\r\n", regs->sp);
        HFD_LOG(" R0:   %08lX\r\n", regs->r0);
        HFD_LOG(" R1:   %08lX\r\n", regs->r1);
        HFD_LOG(" R2:   %08lX\r\n", regs->r2);
        HFD_LOG(" R3:   %08lX\r\n", regs->r3);
        HFD_LOG(" R4:   %08lX\r\n", regs->r4);
        HFD_LOG(" R5:   %08lX\r\n", regs->r5);
        HFD_LOG(" R6:   %08lX\r\n", regs->r6);
        HFD_LOG(" R7:   %08lX\r\n", regs->r7);
        HFD_LOG(" R8:   %08lX\r\n", regs->r8);
        HFD_LOG(" R9:   %08lX\r\n", regs->r9);
        HFD_LOG(" R10:  %08lX\r\n", regs->r10);
        HFD_LOG(" R11:  %08lX\r\n", regs->r11);
        HFD_LOG(" R12:  %08lX\r\n", regs->r12);
        HFD_LOG(" LR:   %08lX\r\n", regs->lr);
        HFD_LOG(" PC:   %08lX\r\n", regs->pc);
        HFD_LOG(" xPSR: %08lX\r\n", regs->xPSR);
    }
}