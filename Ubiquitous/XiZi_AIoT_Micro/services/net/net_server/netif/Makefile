include $(KERNEL_ROOT)/services/net/net_server/lwip.mk

objs += bridgeif.o \
        ethernet.o \
		zepif.o

netif:${objs}
	@echo "generate $^"
	@mv $^ $(KERNEL_ROOT)/services/net/net_server

%.o: %.c
	@echo "cc $^"
	@${cc} ${cflags} ${c_useropts} ${INC_DIR} -o $@ -c $^
	
