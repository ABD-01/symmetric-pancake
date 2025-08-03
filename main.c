#include <stdint.h>
#include "reg.h"

#define USART_FLAG_TXE	((uint16_t) 1 << 7)
#define USART_FLAG_RXNE ((uint16_t) 1 << 5)

#define USARTx      USART1
#define USARTx_SR   USART1_SR
#define USARTx_DR   USART1_DR
#define USARTx_CR1  USART1_CR1


char RxBuf[20] = "DEFAULT";

int puts(const char *str)
{
	while (*str) {
		while (!(*(USARTx_SR) & USART_FLAG_TXE));
		*(USARTx_DR) = *str++ & 0xFF;
	}
	return 0;
}


int _getc(void)
{

	return 0;
}


int _gets(char *buf, int maxlen)
{
	int i = 0;
	while (i < maxlen - 1) {
		while (!(*USARTx_SR & USART_FLAG_RXNE));  // wait for data
		char c = *USARTx_DR & 0xFF;

        // Echo it back
		while (!(*(USARTx_SR) & USART_FLAG_TXE));
		*(USARTx_DR) = c & 0xFF;
        // Echo end

		if (c == '\n' || c == '\r') break;      // end input on newline

		buf[i++] = c;
	}
	buf[i] = '\0';
	return i;
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

    // volatile uint32_t *bat_ptr = (uint32_t*)0xDEADBEEF;
    // *bat_ptr = 0x12345678;


	while (1)
    {
        if (_gets(RxBuf, 10) > 0)
        {
            puts("Received: ");
            puts(RxBuf);
            puts("\r\n");
        }
        
    }
}
