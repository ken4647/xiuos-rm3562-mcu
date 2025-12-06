SRC_DIR := user_api
SRC_FILES := transform.c 

ifeq ($(CONFIG_SEPARATE_COMPILE),y)
	SRC_FILES += userspace.c
endif

include $(KERNEL_ROOT)/compiler.mk

