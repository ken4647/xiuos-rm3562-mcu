include $(KERNEL_ROOT)/services/net/net_server/lwip.mk

objs += autoip.o \
		dhcp.o \
		etharp.o \
		icmp.o \
		igmp.o \
		ip4_addr.o \
		ip4_frag.o \
		ip4.o 

ip4: ${objs}
	@echo "generate $^"
	@mv $^ $(KERNEL_ROOT)/services/net/net_server
	
%.o: %.c
	@echo "cc $^"
	@${cc} ${cflags} ${c_useropts} ${INC_DIR} -o $@ -c $^
