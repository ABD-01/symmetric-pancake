TOOLCHAIN_PREFIX	?=	arm-none-eabi-

CC		=	$(TOOLCHAIN_PREFIX)gcc
CXX		=	$(TOOLCHAIN_PREFIX)g++
AS		=	$(TOOLCHAIN_PREFIX)as
LD		=	$(TOOLCHAIN_PREFIX)ld
OBJCOPY	=	$(TOOLCHAIN_PREFIX)objcopy
OBJDUMP	=	$(TOOLCHAIN_PREFIX)objdump

LDFLAGS	=	-Wl,--print-memory-usage
CFLAGS	=	-mcpu=cortex-m3 -mthumb -mfloat-abi=soft -nostartfiles
CFLAGS	+=	-O0
CFLAGS	+=	-g3 -ggdb
CFLAGS	+=	-Wall -Wextra -Wpedantic
CFLAGS	+=	--specs=nano.specs
#CFLAGS	+=	-lc -lnosys
CXFLAGS	=	$(CFLAGS)  -fno-rtti -fno-exceptions

# ------ objects ---------
OBJS	?=
OBJS	+=	obj/main.o \
			obj/startup.o \
			obj/syscalls.o



.PHONY: all qemu clean

all: kernel.elf kernel.bin

kernel.bin: kernel.elf
	$(OBJCOPY) -Obinary $< $@

kernel.elf: linker.ld $(OBJS)
	# $(LD) $(LDFLAGS) -T $< -o $@ $(OBJS) -Map=kernel.map
	$(CC) $(CFLAGS) $(LDFLAGS) -Wl,-T $< -o $@ $(OBJS) -Wl,-Map=kernel.map
	$(OBJDUMP) -D -S $@ > kernel.list

obj/%.o: %.S
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

obj/%.o: %.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

obj/%.o: %.cpp
	mkdir -p $(@D)
	$(CXX) $(CFLAGS) -c $< -o $@

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
	rm -rvf *.elf *.bin *.list obj/

