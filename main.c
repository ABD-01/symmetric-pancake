#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "app.h"
#include "reg.h"




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

int _gets(char *buf, int maxlen)
{
    int i = 0;
    while (i < maxlen - 1) {
        while (!(*USARTx_SR & USART_FLAG_RXNE)) {} // wait for data
        char c = *USARTx_DR & 0xFF;

        // Echo it back
        // while (!(*(USARTx_SR) & USART_FLAG_TXE));
        // *(USARTx_DR) = c & 0xFF;
        // Echo end

        if (c == '\n' || c == '\r') break; // end input on newline

        buf[i++] = c;
    }
    buf[i] = '\0';
    return i;
}

void main(void)
{
    uart_init();
    setvbuf(stdin, NULL, _IONBF, 0);
    printf("Booting...\r\n");

    check_last_hardfault();

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
