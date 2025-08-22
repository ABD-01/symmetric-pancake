TOOLCHAIN_PREFIX	?=	arm-none-eabi-

CC		=	$(TOOLCHAIN_PREFIX)gcc
CXX		=	$(TOOLCHAIN_PREFIX)g++
AS		=	$(TOOLCHAIN_PREFIX)as
LD		=	$(TOOLCHAIN_PREFIX)ld
OBJCOPY	=	$(TOOLCHAIN_PREFIX)objcopy
OBJDUMP	=	$(TOOLCHAIN_PREFIX)objdump

LDFLAGS	=	-Wl,--print-memory-usage
CFLAGS	=	-mcpu=cortex-m3 -mthumb -mfloat-abi=soft
#CFLAGS	+=	-nographic -nostdlib
CFLAGS	+=	-O0
CFLAGS	+=	-g3 -ggdb
CFLAGS	+=	-Wall -Wextra -Wpedantic
CFLAGS	+=	--specs=nano.specs
#CFLAGS	+=	-lc -lnosys
CXFLAGS	=	$(CFLAGS)  -fno-rtti -fno-exceptions

# ------ includes --------
INCS	= -I. \
		  -Icmsis \
		  -Idevice

# ------ defines --------
DEFINES	= -DSTM32F100xB

CFLAGS	+=	$(DEFINES) $(INCS)

# ------ objects ---------
OBJS	?=
OBJS	+=	obj/device/startup_stm32f100xb.o \
			obj/device/system_stm32f1xx.o \
			obj/device/syscalls.o \
			obj/device/sysmem.o \
			obj/main.o \
			obj/temp.o \
			obj/hf_handler.o \
			obj/app.o

			#obj/startup.o \

# Linker
LD_SCRIPT	=	linker.ld


.PHONY: all qemu clean

all: kernel.elf kernel.bin

kernel.bin: kernel.elf
	$(OBJCOPY) -Obinary $< $@

kernel.elf: $(LD_SCRIPT) $(OBJS)
	# $(LD) $(LDFLAGS) -T $< -o $@ $(OBJS) -Map=kernel.map
	$(CC) $(CFLAGS) $(LDFLAGS) -Wl,-T $< -o $@ $(OBJS) -Wl,-Map=kernel.map
	$(OBJDUMP) -D -S $@ > kernel.list


obj/%.o: %.s
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

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

