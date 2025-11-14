include $(KERNEL_ROOT)/services/net/net_server/lwip.mk
objs += altcp_alloc.o \
		altcp_tcp.o \
		altcp.o \
		def.o \
		dns.o \
		inet_chksum.o \
		init.o \
		ip.o \
		mem.o \
		memp.o \
		netif.o \
		pbuf.o \
		raw.o \
		stats.o \
		sys.o \
		tcp_in.o \
		tcp_out.o \
		tcp.o \
		timeouts.o \
		udp.o

core: ${objs} COMPILER
	@echo "generate $^"
	@mv ${objs} $(KERNEL_ROOT)/services/net/net_server
	
%.o: %.c
	@echo "cc $^"
	@${cc} ${cflags} ${c_useropts} ${INC_DIR} -o $@ -c $^


SRC_DIR += ipv4
include $(KERNEL_ROOT)/compiler.mk
