#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "app.h"

#include "stm32f1xx.h"

#include "reg.h"

#include "memfault/components.h"



static void uart_init(void);

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


void main(void)
{
    uart_init();
    setvbuf(stdin, NULL, _IONBF, 0);
    printf("Booting...\r\n");
    memfault_platform_boot();
    memfault_data_export_dump_chunks();

    test_coredump_storage(0, NULL);

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

    foo();

    __asm("push {r1-r12}\n");

    __asm("svc #69\n");

    // volatile uint32_t *bat_ptr = (uint32_t*)0xDEADBEEF;
    // *bat_ptr = 0x12345678;

    // while (1) {
    //   if (_gets(RxBuf, 10) > 0) {
    //     printf("Received: %s\r\n", RxBuf);
    //   }
    // }

skip_hf:
    while (1) {
        if (fgets(RxBuf, sizeof(RxBuf), stdin)) {
            // if (gets(RxBuf)) {
            printf("Received: %s\r\n", RxBuf);
        }
    }
    return 0;
}
