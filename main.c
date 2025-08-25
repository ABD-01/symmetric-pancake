#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "stm32f1xx.h"

#include "app.h"
#include "usart.h"
#include "shell.h"
#if defined(USING_MEMFAULT)
#include "memfault/components.h"
#endif

char RxBuf[12] = "DEFAULT";

int __io_putchar(int ch)
{
    while (!(*(USARTx_SR)&USART_FLAG_TXE)) {}
    *(USARTx_DR) = ch & 0xFF;
    return ch;
}

int __io_getchar(void)
{
    while (!(*USARTx_SR & USART_FLAG_RXNE)) {}
    char c = *USARTx_DR & 0xFF;
    return c;
}

#if defined(USING_MEMFAULT)
int test_coredump_storage(int argc, char *argv[])
{

    // Note: Coredump saving runs from an ISR prior to reboot so should
    // be safe to call with interrupts disabled.
    __disable_irq();
    memfault_coredump_storage_debug_test_begin();
    __enable_irq();

    memfault_coredump_storage_debug_test_finish();
    return 0;
}
#endif



void main(void)
{
    initial_setup();
    printf("Booting...\r\n");
#if defined(USING_MEMFAULT)
    memfault_platform_boot();
    memfault_data_export_dump_chunks();

    test_coredump_storage(0, NULL);
#else
    check_last_hardfault();
#endif

    __asm("mov    r1,  #0x11\n\t"
          "mov    r2,  #0x22\n\t"
          "mov    r3,  #0x33\n\t"
          "mov    r4,  #0x44\n\t"
          "mov    r5,  #0x55\n\t"
          "mov    r6,  #0x66\n\t"
          "mov    r7,  #0x77\n\t"
          "mov    r8,  #0x88\n\t"
          "mov    r9,  #0x99\n\t"
          "mov    r10, #0xAA\n\t"
          "mov    r11, #0xBB\n\t"
          "mov    r12, #0xCC\n\t"

    );

    __asm("push {r1-r12}\n");

    __asm("svc #69\n");

    prompt();

    return 0;
}

void initial_setup(void)
{
    platform_init();
    setvbuf(stdin, NULL, _IONBF, 0);
    SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk;
    // SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
    __enable_irq();

    // Initialize systick 1ms
    SysTick_Config(SystemCoreClock / 1000);
    NVIC_SetPriority(SysTick_IRQn, 0x0);
    NVIC_EnableIRQ(SysTick_IRQn);
    
}

void platform_init() {
  uart_init();
  set_read_char(__io_getchar);
  set_write_char(__io_putchar);
}

void SysTick_Handler(void)
{
    static uint32_t ticks = 0;
    ticks++;
}