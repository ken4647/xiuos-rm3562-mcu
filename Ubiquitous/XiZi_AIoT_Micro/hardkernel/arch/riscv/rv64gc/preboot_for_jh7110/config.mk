export CROSS_COMPILE ?= riscv64-unknown-elf-
export ARCH = riscv

export KBUILD_CFLAGS := -Wall -Wundef -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type -Wno-format-security -std=gnu89 -Wno-sign-compare -fno-asynchronous-unwind-tables -fno-delete-null-pointer-checks -fno-stack-protector   -Wno-main   -fomit-frame-pointer  -Wvla -Wno-pointer-sign   -Wno-array-bounds   -fno-strict-overflow -fno-stack-check -Werror=date-time    
export KBUILD_AFLAGS :=
export CHECKFLAGS += -D__riscv -D__riscv_xlen=64

export DEVICE := 
export CFLAGS := $(KBUILD_CFLAGS) $(KBUILD_AFLAGS) $(CHECKFLAGS) -std=c11 -mcmodel=medany
export LFLAGS := -T $(KERNEL_ROOT)/hardkernel/arch/riscv/rv64gc/preboot_for_jh7110/jh7110.lds
export CXXFLAGS := 

export DEFINES := 
