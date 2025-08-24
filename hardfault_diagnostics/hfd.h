#ifndef HFD_H
#define HFD_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __has_include
    #if __has_include("hfd_user_confg.h")
        #include "hfd_user_confg.h"
    #endif
#endif

#ifndef HFD_LOG
    #define HFD_LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#endif

struct __attribute__((packed)) regs_t {
    uint32_t sp;

    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;

    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t xPSR;
};

void check_last_hardfault(void);
void HardFault_Handler_Main(struct regs_t *, uint32_t);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* HFD_H */