include $(KERNEL_ROOT)/services/drivers/usb/components/usb.mk

objs += usb_osal.o

all: ${objs} 
	@echo "generate $^"
	@mv $^ $(KERNEL_ROOT)/services/drivers/usb/components

%.o: %.c
	@echo "cc $^"
	@${cc} ${cflags} ${c_useropts} ${INC_DIR} -o $@ -c $^