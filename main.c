#include "reg.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define USART_FLAG_TXE ((uint16_t)1 << 7)   // Ref: rm0041 manual page637
#define USART_FLAG_RXNE ((uint16_t)1 << 5)  // Ref: rm0041 manual page637
// See https://popovicu.com/posts/bare-metal-printf/ for UART driver
// on baremetal platform

#define USARTx USART1
#define USARTx_SR USART1_SR
#define USARTx_DR USART1_DR
#define USARTx_CR1 USART1_CR1

static void uart_init(void);

char RxBuf[12] = "DEFAULT";

int __io_putchar(int ch){
    while (!(*(USARTx_SR)&USART_FLAG_TXE)) {
    }
    *(USARTx_DR) = ch & 0xFF;
    return ch;
}

int __io_getchar(void){
    while (!(*USARTx_SR & USART_FLAG_RXNE)) {
    }
    char c =  *USARTx_DR & 0xFF;
    return c;
}

int _gets(char *buf, int maxlen) {
  int i = 0;
  while (i < maxlen - 1) {
    while (!(*USARTx_SR & USART_FLAG_RXNE)) {
    } // wait for data
    char c = *USARTx_DR & 0xFF;

    // Echo it back
    // while (!(*(USARTx_SR) & USART_FLAG_TXE));
    // *(USARTx_DR) = c & 0xFF;
    // Echo end

    if (c == '\n' || c == '\r')
      break; // end input on newline

    buf[i++] = c;
  }
  buf[i] = '\0';
  return i;
}

__attribute__((weak)) int _read(int file, char *ptr, int len)
{
  (void)file;
  int DataIdx;

  for (DataIdx = 0; DataIdx < len; DataIdx++)
  {
    char c = __io_getchar();
    *ptr++ = c;
  }

  return len;
}


void main(void) {
  uart_init();

  setvbuf(stdin, NULL, _IONBF, 0);
  puts("Hello World!");
  printf("Is printf working??\r\n");
  printf("Is printf working with arg: %s??\r\n", "Arg to printf");
  goto skip_hf;

  __asm__ (
    "mov    r1,  #0x11\n\t"
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

  volatile uint32_t *bat_ptr = (uint32_t*)0xDEADBEEF;
  *bat_ptr = 0x12345678;

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
}

static void uart_init(void) {
  *(RCC_APB2ENR) |= (uint32_t)(0x00000001 | 0x00000004);
  *(RCC_APB1ENR) |= (uint32_t)(0x00020000);

  /* USARTx Configuration */
  *(GPIOA_CRL) = 0x00004B00;
  *(GPIOA_CRH) = 0x44444444;

  *(USARTx_CR1) = 0x0000000C;
  *(USARTx_CR1) |= 0x2000;
}
