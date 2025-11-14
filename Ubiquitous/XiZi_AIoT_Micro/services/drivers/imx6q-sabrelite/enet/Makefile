ifeq ($(BOARD), imx6q-sabrelite)
toolchain ?= arm-none-eabi-
endif
ifeq ($(BOARD), zynq7000-zc702)
toolchain ?= arm-xilinx-eabi-
endif
cc = ${toolchain}gcc
ld = ${toolchain}g++
objdump = ${toolchain}objdump
user_ldflags = -N -Ttext 0

cflags = -std=c11 -march=armv7-a -mtune=cortex-a9 -nostdlib -nodefaultlibs -mfloat-abi=soft -fno-pic \
	-static -fno-builtin -fno-strict-aliasing -Wall -ggdb -Wno-unused -Werror -fno-omit-frame-pointer -fno-stack-protector -fno-pie 

c_useropts = -O0

INC_DIR = 	-I$(KERNEL_ROOT)/services/app \
		  	-I$(KERNEL_ROOT)/services/boards/$(BOARD) \
			-I$(KERNEL_ROOT)/services/lib/serial \
			-I$(KERNEL_ROOT)/services/drivers/$(BOARD)/include \
			-I$(KERNEL_ROOT)/services/drivers/$(BOARD)/enet \
			-I$(KERNEL_ROOT)/services/drivers/$(BOARD)/lib \
			-I$(KERNEL_ROOT)/services/lib/usyscall \
		  	-I$(KERNEL_ROOT)/services/lib/ipc \
		  	-I$(KERNEL_ROOT)/services/lib/memory 

all: enet_drv.o enet_test.o board_network.o enet_iomux_config.o
	@mv $^ $(KERNEL_ROOT)/services/app

%.o: %.c
	@echo "cc $^"
	@${cc} ${cflags} ${c_useropts} ${INC_DIR} -o $@ -c $^