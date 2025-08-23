TOOLCHAIN_PREFIX	?=	arm-none-eabi-

CC		=	$(TOOLCHAIN_PREFIX)gcc
CXX		=	$(TOOLCHAIN_PREFIX)g++
AS		=	$(TOOLCHAIN_PREFIX)as
LD		=	$(TOOLCHAIN_PREFIX)ld
OBJCOPY	=	$(TOOLCHAIN_PREFIX)objcopy
OBJDUMP	=	$(TOOLCHAIN_PREFIX)objdump
BINSIZE	=	$(TOOLCHAIN_PREFIX)size

LDFLAGS	=	-Wl,--print-memory-usage -Wl,--build-id=sha1
CFLAGS	=	-mcpu=cortex-m3 -mthumb -mfloat-abi=soft
#CFLAGS	+=	-nographic -nostdlib
CFLAGS	+=	-O0
CFLAGS	+=	-g3 -ggdb
CFLAGS	+=	-Wall -Wextra -Wpedantic
CFLAGS	+=	--specs=nano.specs
#CFLAGS	+=	-lc -lnosys
CXFLAGS	=	$(CFLAGS)  -fno-rtti -fno-exceptions

# ------ memfault --------
MEMFAULT_PORT_ROOT := third_party/memfault
MEMFAULT_SDK_ROOT := $(MEMFAULT_PORT_ROOT)/memfault-firmware-sdk

MEMFAULT_COMPONENTS := core util panics metrics
include $(MEMFAULT_SDK_ROOT)/makefiles/MemfaultWorker.mk

# ------ esh --------
ESH_ROOT = third_party/shell
ESH_SRCS  = $(ESH_ROOT)/shell/shell.c  \
			$(ESH_ROOT)/lib/a2i/a2i.c \
			$(ESH_ROOT)/lib/memlog/memlog.c \
			$(ESH_ROOT)/lib/printf/printf.c \
			$(ESH_ROOT)/lib/utils/utils.c  
ESH_INCLUDES = -I$(ESH_ROOT) \
			-I$(ESH_ROOT)/lib/a2i \
			-I$(ESH_ROOT)/lib/memlog \
			-I$(ESH_ROOT)/lib/printf \
			-I$(ESH_ROOT)/lib/regs \
			-I$(ESH_ROOT)/lib/string \
			-I$(ESH_ROOT)/shell


# ------ includes --------
INCS	= -I. \
		  -Icmsis \
		  -Idevice \
		  -I$(MEMFAULT_COMPONENTS_INC_FOLDERS) \
  		  -I$(MEMFAULT_SDK_ROOT)/ports/include \
  		  -I$(MEMFAULT_PORT_ROOT) \
		  $(ESH_INCLUDES)

# ------ defines --------
DEFINES	= -DSTM32F100xB

CFLAGS	+=	$(DEFINES) $(INCS)

# ------ objects ---------
SRCS	?=
SRCS	+=	device/startup_stm32f100xb.s \
			device/system_stm32f1xx.c \
			device/syscalls.c \
			device/sysmem.c \
			main.c \
			temp_svc_handler.c \
			app.cpp
SRCS	+=	$(MEMFAULT_COMPONENTS_SRCS) \
  			$(MEMFAULT_PORT_ROOT)/memfault_platform_port.c \
			${MEMFAULT_SDK_ROOT}/ports/panics/src/memfault_platform_ram_backed_coredump.c
SRCS	+=  $(ESH_SRCS)



SRCS_C    := $(filter %.c,$(SRCS))
SRCS_CPP  := $(filter %.cpp,$(SRCS))
SRCS_S    := $(filter %.s,$(SRCS))
SRCS_CAPS := $(filter %.S,$(SRCS))

OBJS  = $(patsubst %.c,obj/%.o,$(SRCS_C))
OBJS += $(patsubst %.cpp,obj/%.o,$(SRCS_CPP))
OBJS += $(patsubst %.s,obj/%.o,$(SRCS_S))
OBJS += $(patsubst %.S,obj/%.o,$(SRCS_CAPS))

# Linker
LD_SCRIPT	=	linker.ld


.PHONY: all qemu clean

all: kernel.elf kernel.bin

kernel.bin: kernel.elf
	$(OBJCOPY) -Obinary $< $@

kernel.elf: $(LD_SCRIPT) $(OBJS)
	@$(BINSIZE) -B -d -t $(OBJS)
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
	rm -rvf *.elf *.bin *.list *.map obj/

