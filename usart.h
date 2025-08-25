#ifndef __REG_H_
#define __REG_H_

#define __REG_TYPE	volatile uint32_t
#define __REG		__REG_TYPE *

/* RCC Memory Map */
#define RCC		((__REG_TYPE) 0x40021000)
#define RCC_APB2ENR	((__REG) (RCC + 0x18))
#define RCC_APB1ENR	((__REG) (RCC + 0x1C))

/* GPIO Memory Map */
#define GPIOA		((__REG_TYPE) 0x40010800)
#define GPIOA_CRL	((__REG) (GPIOA + 0x00))
#define GPIOA_CRH	((__REG) (GPIOA + 0x04))

/* USART2 Memory Map */
#define USART2		((__REG_TYPE) 0x40004400)
#define USART2_SR	((__REG) (USART2 + 0x00))
#define USART2_DR	((__REG) (USART2 + 0x04))
#define USART2_CR1	((__REG) (USART2 + 0x0C))

/* USART1 Memory Map */
#define USART1		((__REG_TYPE) 0x40013800)
#define USART1_SR	((__REG) (USART1 + 0x00))
#define USART1_DR	((__REG) (USART1 + 0x04))
#define USART1_CR1	((__REG) (USART1 + 0x0C))


#define USART_FLAG_TXE  ((uint16_t)1 << 7) // Ref: rm0041 manual page637
#define USART_FLAG_RXNE ((uint16_t)1 << 5) // Ref: rm0041 manual page637
// See https://popovicu.com/posts/bare-metal-printf/ for UART driver
// on baremetal platform

#define USARTx     USART1
#define USARTx_SR  USART1_SR
#define USARTx_DR  USART1_DR
#define USARTx_CR1 USART1_CR1

static void uart_init(void)
{
    *(RCC_APB2ENR) |= (uint32_t)(0x00000001 | 0x00000004);
    *(RCC_APB1ENR) |= (uint32_t)(0x00020000);

    /* USARTx Configuration */
    *(GPIOA_CRL) = 0x00004B00;
    *(GPIOA_CRH) = 0x44444444;

    *(USARTx_CR1) = 0x0000000C;
    *(USARTx_CR1) |= 0x2000;
}


#endif
