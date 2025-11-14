export CROSS_COMPILE ?= arm-xilinx-eabi-
export DEVICE = 
export CFLAGS := $(DEVICE) -Wall -O0 -g -std=c11 
export AFLAGS := -r
export LFLAGS := $(DEVICE) -Wl,-T -Wl,$(KERNEL_ROOT)/hardkernel/arch/arm/armv7-a/cortex-a9/preboot_for_$(BOARD)/$(BOARD).lds -Wl,--start-group,-lgcc,-lc,--end-group
export CXXFLAGS := 

export ARCH = arm
export ARCH_ARMV = armv7-a
