# Hardfault Diagnostics on ARM

Functionality Support:
[  ]: Bare-Metal
[  ]: FreeRTOS


## Build & Run

```bash
make
make debug
make gdb
```


## References

### Fault Logging
- [CrashCatcher/../CrashCatcher\_armv6m.S (Github)](https://github.com/adamgreen/CrashCatcher/blob/master/Core/src/CrashCatcher_armv6m.S)
- [PX4-Autopilot/../hardfault\_log.c (Github)](https://github.com/PX4/PX4-Autopilot/blob/main/src/systemcmds/hardfault_log/hardfault_log.c)
- [Cortex-M3/M4 Hard Fault Handler](https://blog.frankvh.com/2011/12/07/cortex-m3-m4-hard-fault-handler/)
- [Developing a Generic Hard Fault handler for ARM Cortex-M3/Cortex-M4](https://feabhasblog.wpengine.com/2013/02/developing-a-generic-hard-fault-handler-for-arm-cortex-m3cortex-m4/)

### ARM References
- [Cortex-M3 Devices Generic User Guide](https://developer.arm.com/documentation/dui0552/a/introduction/about-the-cortex-m3-processor-and-core-peripherals/cortex-m3-core-peripherals?lang=en)
- [ARM Cortex-M for Beginners](https://community.arm.com/cfs-file/__key/telligent-evolution-components-attachments/01-2142-00-00-00-00-52-96/White-Paper-_2D00_-Cortex_2D00_M-for-Beginners-_2D00_-2016-_2800_final-v3_2900_.pdf)
- [Introduction to ARM - Gananand Kini (Slides)](https://www.opensecuritytraining.info/IntroARM_files/Introduction%20to%20ARM%20Systems-11-17-2012.pdf)
- [Introduction to ARM - Gananand Kini (Lectures)](https://www.youtube.com/playlist?list=PLUFkSN0XLZ-n91t_AX5zO007Giz1INwPd)
- [AArch64 Procedure Call Standard (AAPCS64): ABI, Calling Conventions & Machine Registers](https://medium.com/@tunacici7/aarch64-procedure-call-standard-aapcs64-abi-calling-conventions-machine-registers-a2c762540278)

### STM32F1xxxx
- [Reference Manual (PDF)](https://www.st.com/resource/en/reference_manual/rm0041-stm32f100xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [STM32CubeF1 HAL Driver MCU Component (Github)](https://github.com/STMicroelectronics/stm32f1xx-hal-driver/tree/master)
- [STMicroelectronics STM32 boards - QEMU Documentation](https://www.qemu.org/docs/master/system/arm/stm32.html)

### Debuggers
- [Demystifying Debuggers, Part 1: A Busy Intersection - Ryan Fleury](https://www.rfleury.com/p/demystifying-debuggers-part-1-a-busy)
- [How debuggers work: Part 1: Basics - Eli Bendersky](https://eli.thegreenplace.net/2011/01/23/how-debuggers-work-part-1)
