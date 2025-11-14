include $(KERNEL_ROOT)/services/net/net_server/lwip.mk
objs += sys_arch.o ethernetif.o


arch: ${objs}
	@echo "generate $^"
	@mv ${objs} $(KERNEL_ROOT)/services/net/net_server
	
%.o: %.c
	@echo "cc $^"
	@${cc} ${cflags} ${c_useropts} ${INC_DIR} -o $@ -c $^

