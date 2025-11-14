ifeq ($(BOARD), imx6q-sabrelite)
toolchain ?= arm-none-eabi-
user_ldflags = -N -Ttext 0
cflags = -std=c11 -march=armv7-a -mtune=cortex-a9 -nostdlib -nodefaultlibs -mfloat-abi=soft -fno-pic -static -fno-builtin -fno-strict-aliasing -Wall -ggdb -Wno-unused -Werror -fno-omit-frame-pointer -fno-stack-protector -fno-pie
endif
ifeq ($(BOARD), zynq7000-zc702)
toolchain ?= arm-xilinx-eabi-
user_ldflags = --start-group,-lgcc,-lc,--end-group
cflags = -std=c11 -march=armv7-a -mtune=cortex-a9 -nostdlib -nodefaultlibs -mfloat-abi=soft -fno-pic -static -fno-builtin -fno-strict-aliasing -Wall -ggdb -Wno-unused -Werror -fno-omit-frame-pointer -fno-stack-protector -fno-pie
#cflags = -Wall -g -std=c11 
endif
cc = ${toolchain}gcc
ld = ${toolchain}g++
objdump = ${toolchain}objdump

c_useropts = -O0


INC_DIR = 	-I$(KERNEL_ROOT)/services/drivers/usb/components \
		-I$(KERNEL_ROOT)/services/drivers/usb/components/class \
		-I$(KERNEL_ROOT)/services/drivers/usb/components/class/cdc \
		-I$(KERNEL_ROOT)/services/drivers/usb/components/class/hub \
		-I$(KERNEL_ROOT)/services/drivers/usb/components/class/wireless \
		-I$(KERNEL_ROOT)/services/drivers/usb/components/common \
		-I$(KERNEL_ROOT)/services/drivers/usb/components/core \
		-I$(KERNEL_ROOT)/services/drivers/usb/components/osal \
		-I$(KERNEL_ROOT)/services/drivers/usb/components/port \
		-I$(KERNEL_ROOT)/services/drivers/usb/components/port/xhci \
		-I$(KERNEL_ROOT)/services//semaphore \
		-I$(KERNEL_ROOT)/services/lib/ipc \
		-I$(KERNEL_ROOT)/services/lib/memory \
		-I$(KERNEL_ROOT)/services/lib/serial \
		-I$(KERNEL_ROOT)/services/lib/usyscall \
		-I$(KERNEL_ROOT)/services/boards/$(BOARD) \
		-I$(KERNEL_ROOT)/services/app


# include $(KERNEL_ROOT)/compiler.mk
