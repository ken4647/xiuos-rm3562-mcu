
SRC_FILES := vector.S trampoline.S $(BOARD)/trap_common.c error_debug.c hard_spinlock.S

ifeq ($(BOARD), imx6q-sabrelite)
SRC_DIR := gicv2
endif

ifeq ($(BOARD), zynq7000-zc702)
# SRC_DIR := gicv2
SRC_DIR := gicv3
SRC_FILES += $(BOARD)/xil_assert.c
endif


include $(KERNEL_ROOT)/compiler.mk