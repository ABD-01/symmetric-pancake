#include <stdint.h>
#include "reg.h"

#define USART_FLAG_TXE	((uint16_t) 1 << 7)

#define USARTx      USART1
#define USARTx_SR   USART1_SR
#define USARTx_DR   USART1_DR
#define USARTx_CR1  USART1_CR1

int puts(const char *str)
{
	while (*str) {
		while (!(*(USARTx_SR) & USART_FLAG_TXE));
		*(USARTx_DR) = *str++ & 0xFF;
	}
	return 0;
}

void main(void)
{
	*(RCC_APB2ENR) |= (uint32_t) (0x00000001 | 0x00000004);
	*(RCC_APB1ENR) |= (uint32_t) (0x00020000);

	/* USARTx Configuration */
	*(GPIOA_CRL) = 0x00004B00;
	*(GPIOA_CRH) = 0x44444444;

	*(USARTx_CR1) = 0x0000000C;
	*(USARTx_CR1) |= 0x2000;

	puts("Hello World!\n");

	while (1);
}
