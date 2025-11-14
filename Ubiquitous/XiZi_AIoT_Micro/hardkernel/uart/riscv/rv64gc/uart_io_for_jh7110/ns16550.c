/*
 * COM1 NS16550 support
 * originally from linux source (arch/powerpc/boot/ns16550.c)
 * modified to use CONFIG_SYS_ISA_MEM and new defines
 */

#include "ns16550.h"
#include "mmio_access.h"

struct ns16550 g_ns16550_com_port = {0};
struct ns16550_plat g_ns16550_plat = {0};
unsigned long g_ns16550_uart_base = {0};

#define CONFIG_SYS_NS16550_UART_BASE	0x10000000
#define CONFIG_BAUDRATE					115200
#define CONFIG_SYS_NS16550_CLK			24000000
#define CONFIG_SYS_NS16550_UART_BASE_VIRT	 MMIO_P2V_WO(CONFIG_SYS_NS16550_UART_BASE)


#define UART_LCRVAL UART_LCR_8N1		/* 8 data, 1 stop, no parity */
#define UART_MCRVAL (UART_MCR_DTR | \
		     UART_MCR_RTS)		/* RTS/DTR */

#ifndef CONFIG_SYS_NS16550_IER
#define CONFIG_SYS_NS16550_IER  0x00
#endif /* CONFIG_SYS_NS16550_IER */

#define CONFIG_SYS_NS16550_PORT_MAPPED

#define readb(addr) \
	({ unsigned char __v = (*(volatile unsigned char *)(addr)); __v; })

#define writeb(b, addr) (void)((*(volatile unsigned char *)(addr)) = (b))


static inline void serial_out_shift(void *addr, int shift, int value)
{
	writeb(value, addr);
}

static inline int serial_in_shift(void *addr, int shift)
{
	return readb(addr);
}


static void ns16550_writeb(struct ns16550 *port, int offset, int value)
{
	struct ns16550_plat *plat = port->plat;
	unsigned char *addr;

	offset *= 1 << plat->reg_shift;
	addr = (unsigned char *)plat->base + offset + plat->reg_offset;

	serial_out_shift(addr, plat->reg_shift, value);
}

static int ns16550_readb(struct ns16550 *port, int offset)
{
	struct ns16550_plat *plat = port->plat;
	unsigned char *addr;

	offset *= 1 << plat->reg_shift;
	addr = (unsigned char *)plat->base + offset + plat->reg_offset;

	return serial_in_shift(addr, plat->reg_shift);
}

static uint32_t ns16550_getfcr(struct ns16550 *port)
{
	struct ns16550_plat *plat = port->plat;

	return plat->fcr;
}

/* We can clean these up once everything is moved to driver model */
#define serial_out(value, addr)	\
	ns16550_writeb(com_port, \
		(unsigned char *)addr - (unsigned char *)com_port, value)
#define serial_in(addr) \
	ns16550_readb(com_port, \
		(unsigned char *)addr - (unsigned char *)com_port)

/* Divide positive dividend by positive divisor and round to closest integer. */
#define DIV_ROUND_CLOSEST(x, divisor) \
	(((x) + ((divisor) / 2)) / (divisor))

int ns16550_calc_divisor(struct ns16550 *port, int clock, int baudrate)
{
	const unsigned int mode_x_div = 16;

	return DIV_ROUND_CLOSEST(clock, mode_x_div * baudrate);
}

static void ns16550_setbrg(struct ns16550 *com_port, int baud_divisor)
{
	/* to keep serial format, read lcr before writing BKSE */
	int lcr_val = serial_in(&com_port->lcr) & ~UART_LCR_BKSE;

	serial_out(UART_LCR_BKSE | lcr_val, &com_port->lcr);
	serial_out(baud_divisor & 0xff, &com_port->dll);
	serial_out((baud_divisor >> 8) & 0xff, &com_port->dlm);
	serial_out(lcr_val, &com_port->lcr);
}

void ns16550_init(struct ns16550 *com_port, int baud_divisor)
{
	while (!(serial_in(&com_port->lsr) & UART_LSR_TEMT))
		;
	serial_out(CONFIG_SYS_NS16550_IER, &com_port->ier);
	serial_out(UART_MCRVAL, &com_port->mcr);
	serial_out(ns16550_getfcr(com_port), &com_port->fcr);
	/* initialize serial config to 8N1 before writing baudrate */
	serial_out(UART_LCRVAL, &com_port->lcr);
	if (baud_divisor != -1)
		ns16550_setbrg(com_port, baud_divisor);
}

void ns16550_putc(struct ns16550 *com_port, char c)
{
	while ((serial_in(&com_port->lsr) & UART_LSR_THRE) == 0)
		;
	serial_out(c, &com_port->thr);
}


char ns16550_getc(struct ns16550 *com_port)
{
	while ((serial_in(&com_port->lsr) & UART_LSR_DR) == 0)
		;
	return serial_in(&com_port->rbr);
}

int ns16550_tstc(struct ns16550 *com_port)
{
	return (serial_in(&com_port->lsr) & UART_LSR_DR) != 0;
}

static void ns16550_plat_init(void)
{
	struct ns16550_plat *plat = &g_ns16550_plat;

	/* refer jh7110 u-boot/arch/riscv/dts/jh7110.dtsi */
	plat->base = g_ns16550_uart_base;
	plat->reg_offset = 0;
	plat->reg_shift = 2;
	plat->reg_width = 4;
	plat->clock = CONFIG_SYS_NS16550_CLK;
	plat->fcr = UART_FCR_DEFVAL;
}

static void ns16550_serial_init(void)
{
	struct ns16550_plat *plat = &g_ns16550_plat;
	struct ns16550 *com_port = &g_ns16550_com_port;

	ns16550_plat_init();
	com_port->plat = plat;
	ns16550_init(com_port, -1);
}

static void ns16550_serial_setbrg(int baudrate)
{
	struct ns16550 *const com_port = &g_ns16550_com_port;
	struct ns16550_plat *plat = com_port->plat;
	int clock_divisor;

	clock_divisor = ns16550_calc_divisor(com_port, plat->clock, baudrate);
	ns16550_setbrg(com_port, clock_divisor);
}

void _debug_uart_init(void)
{
	int baudrate = CONFIG_BAUDRATE;

	g_ns16550_uart_base = CONFIG_SYS_NS16550_UART_BASE_VIRT;
	ns16550_serial_init();
	ns16550_serial_setbrg(baudrate);
//	_debug_uart_printascii("_debug_uart_init success.\n");
}

void _debug_uart_init_early(void)
{
	int baudrate = CONFIG_BAUDRATE;

	g_ns16550_uart_base = CONFIG_SYS_NS16550_UART_BASE;
	ns16550_serial_init();
	ns16550_serial_setbrg(baudrate);
//	_debug_uart_printascii("_debug_uart_init_early success.\n");
}

void _debug_uart_putc(int ch)
{
    struct ns16550* com_port = &g_ns16550_com_port;
    if (ch == '\n')
        ns16550_putc(com_port, '\r');
    ns16550_putc(com_port, ch);
}

int _debug_uart_getc(void)
{
    struct ns16550* com_port = &g_ns16550_com_port;
    return ns16550_getc(com_port);
}

static void _printch(int ch)
{
	if (ch == '\n')
		_debug_uart_putc('\r');
	_debug_uart_putc(ch);
}

void _debug_uart_printascii(const char *str)
{
	while (*str)
		_printch(*str++);
}
