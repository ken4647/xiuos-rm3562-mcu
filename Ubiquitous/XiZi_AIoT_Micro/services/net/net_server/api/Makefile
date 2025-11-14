
include $(KERNEL_ROOT)/services/net/net_server/lwip.mk
objs += api_lib.o \
		api_msg.o \
		err.o \
		if_api.o \
		netbuf.o \
		netdb.o \
		netifapi.o \
		sockets.o \
		tcpip.o 


api: ${objs}
	@echo "generate $^"
	@mv $^ $(KERNEL_ROOT)/services/net/net_server

%.o: %.c
	@echo "cc $^"
	@${cc} ${cflags} ${c_useropts} ${INC_DIR} -o $@ -c $^

