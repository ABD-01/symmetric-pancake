TOOLCHAIN_PREFIX	?=	arm-none-eabi-

CC		=	$(TOOLCHAIN_PREFIX)gcc
AS		=	$(TOOLCHAIN_PREFIX)as
LD		=	$(TOOLCHAIN_PREFIX)ld
OBJCOPY	=	$(TOOLCHAIN_PREFIX)objcopy
OBJDUMP	=	$(TOOLCHAIN_PREFIX)objdump

LDFLAGS	=	-T linker.ld
LDFLAGS	+=	--print-memory-usage
CFLAGS	=	-mcpu=cortex-m3 -mthumb -nostartfiles -nostdlib -O0
CFLAGS	+=	-O0
CFLAGS	+=	-g -ggdb

.PHONY: all qemu clean

all: kernel.elf kernel.bin

kernel.bin: kernel.elf
	$(OBJCOPY) -Obinary $< $@

kernel.elf: start.o main.o
	$(LD) $(LDFLAGS) -o $@ $^ -Map=kernel.map
	$(OBJDUMP) -D -S $@ > kernel.lst

start.o: startup.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

debug: kernel.bin
	@echo Please use \"make gdb\" in another terminal to attach gdb.
	@echo Use \"cltr+a,x\" to quit qemu debug
	qemu-system-arm -machine stm32vldiscovery -nographic -kernel $< -s -S

run: kernel.bin
	@echo "Use Ctrl+A X to exit Qemu"
	qemu-system-arm -machine stm32vldiscovery -nographic -kernel $<


gdb: kernel.elf
	# wget https://raw.githubusercontent.com/cyrus-and/gdb-dashboard/refs/heads/master/.gdbinit ~/.gdbinit
	# gdb-multiarch $< -ex 'target remote localhost:1234'
	gdb-multiarch $< -ex 'target remote 10.0.2.2:1234'

clean:
	rm -f *.o *.elf *.bin *.lst

