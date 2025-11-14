ifeq ($(BOARD), jh7110)
toolchain ?= riscv64-unknown-elf-
user_ldflags = -N -Ttext 0
CFLAGS = -Wall -Werror -O -fno-omit-frame-pointer -ggdb -gdwarf-2 -MD -mcmodel=medany -fno-common -nostdlib -fno-builtin-strncpy -fno-builtin-strncmp -fno-builtin-strlen -fno-builtin-memset -fno-builtin-memmove -fno-builtin-memcmp -fno-builtin-log -fno-builtin-bzero -fno-builtin-strchr -fno-builtin-exit -fno-builtin-malloc -fno-builtin-putc -fno-builtin-free -fno-builtin-memcpy -Wno-main -fno-builtin-printf -fno-builtin-fprintf -fno-builtin-vprintf -I. -fno-stack-protector -fno-pie -no-pie
cflags = $(CFLAGS) -Wno-unused -fno-strict-aliasing
endif

cc = ${toolchain}gcc
ld = ${toolchain}g++
objdump = ${toolchain}objdump

c_useropts = -O2

INC_DIR = 	-I$(KERNEL_ROOT)/services/fs/libfs \
			-I$(KERNEL_ROOT)/services/lib/ipc \
		  	-I$(KERNEL_ROOT)/services/lib/memory \
			-I$(KERNEL_ROOT)/services/lib/serial \
			-I$(KERNEL_ROOT)/services/lib/usyscall \
			-I$(KERNEL_ROOT)/services/boards/$(BOARD) \
			-I$(KERNEL_ROOT)/services/app

board: libserial.o arch_usyscall.o stub.o
	@mv $^ $(KERNEL_ROOT)/services/app

%.o: %.c
	@echo "cc $^"
	@${cc} ${cflags} ${c_useropts} ${INC_DIR} -o $@ -c $^

include $(KERNEL_ROOT)/compiler.mk
