#include "stm32f1xx.h"
#include <stdio.h>

__attribute__((naked)) void SVC_Handler(void) {
    __asm(
        ".global    SVC_Handler_Main    \n\t"
        "tst    lr, #4                  \n\t"
        "ite    EQ                      \n\t"
        "mrseq  r0, MSP                 \n\t"
        "mrsne  r0, PSP                 \n\t"
        "b      SVC_Handler_Main        \n\t"
    );
}

void SVC_Handler_Main(uint32_t * sp)
{
    /*
    * Stack contains:
    * r0, r1, r2, r3, r12, r14, the return address and xPSR
    * First argument (r0) is svc_args[0]
    */
    uint32_t svc_num;
    svc_num = ((char*)sp[ 6 ])[ -2 ];
    
    printf("Received syscall: %lu\r\n", svc_num);
}