#ifndef _RISCV_PLIC_H
#define _RISCV_PLIC_H

#include "memlayout.h"
#include "ptrace.h"
#include <stdint.h>

#define PLIC_BASE    PLIC_VIRTMEM_BASE

#define PLIC_PRIORITY(hwirq) (PLIC_BASE + (hwirq) * 4)
#define PLIC_PENDING(hwirq) (PLIC_BASE + 0x1000 + ((hwirq) / 32) * 4)
#define PLIC_MENABLE(hart) (PLIC_BASE + 0x2000 + (hart) * 0x80)
#define PLIC_MTHRESHOLD(hart) (PLIC_BASE + 0x200000 + (hart) * 0x1000)
#define PLIC_MCLAIM(hart) (PLIC_BASE + 0x200004 + (hart) * 0x1000)


int plic_init(void);
void plic_enable_irq(int cpu, int hwirq, int enable);
void plic_handle_irq(struct pt_regs *regs);
void plic_init_hart(uint32_t cpu_id);
uint32_t plic_read_irq_ack(void);
void plic_write_end_of_irq(uint32_t x);
void intr_on(void);
void intr_off(void);

#endif /* _RISCV_PLIC_H */
