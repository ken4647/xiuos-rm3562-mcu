/*
 * This driver implements a version of the RISC-V PLIC with the actual layout
 * specified in chapter 8 of the SiFive U5 Coreplex Series Manual:
 *
 *     https://static.dev.sifive.com/U54-MC-RVCoreIP.pdf
 *
 */
#include "asm/csr.h" 
#include "printf.h"
#include "plic.h"
#include "asm/mmio.h"
#include "ptrace.h"


extern unsigned long boot_cpu_hartid;

#define MAX_CPUS 4
#define MAX_PLIC_IRQS 136

#define CPU_TO_HART(cpu) ((2 * cpu) + 2)


//TODO: to debug
void plic_set_priority(int hwirq, int pro)
{
#if 0
    unsigned int reg = PLIC_PRIORITY(hwirq);
    writel(pro, reg);
#endif
}

//TODO: to debug
void plic_enable_irq(int cpu, int hwirq, int enable)
{
#if 0
    unsigned int hwirq_mask = 1 << (hwirq % 32);
    int hart = CPU_TO_HART(cpu);
    unsigned int reg = PLIC_MENABLE(hart) + 4 * (hwirq / 32);

//    printk("plic_enable_irq hwirq=%d\n", hwirq);

    if (enable) {
        writel(readl(reg) | hwirq_mask, reg);
    }
    else {
        writel(readl(reg) & ~hwirq_mask, reg);
    }
#endif
}

//TODO: to debug
//Refer to linux/drivers/irqchip/irq-sifive-plic.c
int plic_init(void)
{
#if 0
    int i;
    int hwirq;

//    printk("plic_init boot_cpu_hartid=%lu\n", boot_cpu_hartid);

    for (i = 0; i < MAX_CPUS; i++) {
        writel(0, PLIC_MTHRESHOLD(CPU_TO_HART(i)));

        for (hwirq = 1; hwirq <= MAX_PLIC_IRQS; hwirq++) {
            plic_enable_irq(i, hwirq, 0);
            plic_set_priority(hwirq, 1);
        }
    }
    csr_set(CSR_IE, IE_EIE);
#endif
    return 0;
}

void plic_handle_irq(struct pt_regs *regs)
{
#if 0
    int hwirq;
    int hart = CPU_TO_HART(0);
    unsigned int claim_reg = PLIC_MCLAIM(hart);
    csr_clear(CSR_IE, IE_EIE);
    //TODO
    csr_set(CSR_IE, IE_EIE);
#endif
}

void plic_init_hart(uint32_t cpu_id)
{
    ;
}

uint32_t plic_read_irq_ack(void)
{
    return 0;
}

void plic_write_end_of_irq(uint32_t x)
{
    ;
}

void intr_on(void)
{
    ;
}

void intr_off(void)
{
    ;
}
