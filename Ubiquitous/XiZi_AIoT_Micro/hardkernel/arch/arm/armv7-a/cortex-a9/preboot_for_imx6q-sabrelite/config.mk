export CROSS_COMPILE ?= arm-none-eabi-
export DEVICE = -march=armv7-a -mtune=cortex-a9 -mfpu=vfpv3-d16 -ftree-vectorize -ffast-math -mfloat-abi=softfp
# export CFLAGS := $(DEVICE) -std=c11 -Wall -O2 -g -gdwarf-2  -Wnull-dereference -Waddress -Warray-bounds -Wchar-subscripts -Wimplicit-int -Wimplicit-function-declaration -Wcomment -Wformat -Wmissing-braces -Wnonnull -Wparentheses -Wpointer-sign -Wreturn-type -Wsequence-point -Wstrict-aliasing -Wstrict-overflow=1 -Wswitch -Wtrigraphs -Wuninitialized -Wunknown-pragmas -Wunused-function -Wunused-label -Wunused-value -Wunused-variable -Wunused-function
export CFLAGS := $(DEVICE) -std=c11 -Wall -O2 -g -gdwarf-2 -Waddress -Warray-bounds -Wchar-subscripts -Wimplicit-int -Wimplicit-function-declaration -Wcomment -Wformat -Wmissing-braces -Wnonnull -Wparentheses -Wpointer-sign -Wreturn-type -Wsequence-point -Wstrict-aliasing -Wstrict-overflow=1 -Wswitch -Wtrigraphs -Wuninitialized -Wunknown-pragmas -Wunused-function -Wunused-label -Wunused-value -Wunused-variable -Wunused-function
export AFLAGS := -c $(DEVICE) -x assembler-with-cpp -D__ASSEMBLY__ -gdwarf-2
# export LFLAGS := $(DEVICE) -Wl,-Map=XiZi-imx6q-sabrelite.map,-cref,-u,_boot_start -T $(KERNEL_ROOT)/hardkernel/arch/arm/armv7-a/cortex-a9/preboot_for_imx6q-sabrelite/nxp_imx6q_sabrelite.lds
export LFLAGS := $(DEVICE) --specs=nosys.specs -Wl,-Map=XiZi-imx6q-sabrelite.map,-cref,-u,_boot_start -T $(KERNEL_ROOT)/hardkernel/arch/arm/armv7-a/cortex-a9/preboot_for_imx6q-sabrelite/nxp_imx6q_sabrelite.lds
export CXXFLAGS := 

ifeq ($(CONFIG_LIB_MUSLLIB), y)
export LFLAGS += -nostdlib -nostdinc -fno-builtin -nodefaultlibs
export LIBCC := -lgcc
export LINK_MUSLLIB := $(KERNEL_ROOT)/lib/musllib/libmusl.a
endif

export DEFINES := -DHAVE_CCONFIG_H -DCHIP_MX6DQ

export ARCH = arm
export ARCH_ARMV = armv7-a
