/*
 * Copyright (c) 2020 AIIT XUOS Lab
 * XiUOS is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *        http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */
/**
 * @file trap.c
 * @brief trap interface of hardkernel
 * @version 1.0
 * @author AIIT XUOS Lab
 * @date 2023.05.06
 */

/*************************************************
File name: trap.c
Description: trap interface of hardkernel
Others:
History:
Author: AIIT XUOS Lab
Modification:
1. first version
*************************************************/
#include <stdint.h>

#include "assert.h"
#include "core.h"
#include "multicores.h"
#include "syscall.h"
#include "task.h"

#include "mmu.h"

#include "asm/csr.h"
#include "ptrace.h"
#include "plic.h"


extern void dabort_handler(struct trapframe* r);
extern void iabort_handler(struct trapframe* r);

void kernel_abort_handler(struct trapframe* tf)
{
    uint64_t ec = tf->cause;

    switch (ec) {
    case 0:
    case 1:
    case 2:
    case 12:
        iabort_handler(tf);
        break;
    case 4:
    case 5:
    case 6:
    case 7:
    case 13:
    case 15:
        dabort_handler(tf);
        break;
    default: {
        ERROR("tf->cause: %016lx\n", tf->cause);
        ERROR("Current Task: %s.\n", cur_cpu()->task->name);
        panic("Unimplemented Error Occured.\n");
    }
    }
    panic("Return from abort handler.\n");
}

void kernel_intr_handler(struct trapframe* tf)
{
    panic("Intr at kernel mode should never happen by design.\n");
}

extern void context_switch(struct context*, struct context*);
void syscall_arch_handler(struct trapframe* tf)
{
    uint64_t ec = tf->cause;

    switch (ec) {
    case EXC_SYSCALL:
        software_irq_dispatch(tf);
        break;

    default: {
        ERROR("USYSCALL: unexpected\n");
        ERROR("tf->cause: %016lx\n", tf->cause);

        extern void dump_tf(struct trapframe * tf);
        dump_tf(tf);

        // kill error task
        xizi_enter_kernel();
        assert(cur_cpu()->task != NULL);
        ERROR("Error Task: %s\n", cur_cpu()->task->name);
        sys_exit(cur_cpu()->task);
        context_switch(cur_cpu()->task->thread_context.context, &cur_cpu()->scheduler);
        panic("dabort end should never be reashed.\n");
    }
    }
}



extern void handle_exception(void);

void trap_init(void)
{
    csr_write(stvec, handle_exception);
    csr_write(sie, 0);
    __asm__ volatile("csrw sscratch, zero" : : : "memory");
#if 0
    printk("trap_init test\n");
    __asm__ volatile("ebreak");
    printk("trap_init test ok\n");
#endif
}

void trap_set_exception_vector(uint64_t new_tbl_base)
{
    csr_write(stvec, new_tbl_base);
}

static void do_trap_error(struct pt_regs *regs, const char *str)
{
    printk("Oops: %s\n", str);
    printk("sstatus: 0x%016lx, sbadaddr: 0x%016lx, scause: 0x%016lx\n",
           regs->status, regs->badaddr, regs->cause);
    panic("Fatal exception\n");
}

#define DO_ERROR_INFO(name) \
static int name(struct pt_regs *regs, const char *str) \
{ \
    do_trap_error(regs, str); \
    return 0; \
}

DO_ERROR_INFO(do_trap_unknown);
DO_ERROR_INFO(do_trap_insn_misaligned);
DO_ERROR_INFO(do_trap_insn_fault);
DO_ERROR_INFO(do_trap_insn_illegal);
DO_ERROR_INFO(do_trap_load_misaligned);
DO_ERROR_INFO(do_trap_load_fault);
DO_ERROR_INFO(do_trap_store_misaligned);
DO_ERROR_INFO(do_trap_store_fault);
DO_ERROR_INFO(do_trap_ecall_u);
DO_ERROR_INFO(do_trap_ecall_s);
DO_ERROR_INFO(do_trap_break);
DO_ERROR_INFO(do_page_fault);

struct fault_info {
    int (*fn)(struct pt_regs *regs, const char *name);
    const char *name;
};

static struct fault_info fault_inf[] = {
    {do_trap_insn_misaligned, "Instruction address misaligned"},
    {do_trap_insn_fault, "Instruction access fault"},
    {do_trap_insn_illegal, "Illegal instruction"},
    {do_trap_break, "Breakpoint"},
    {do_trap_load_misaligned, "Load address misaligned"},
    {do_trap_load_fault, "Load access fault"},
    {do_trap_store_misaligned, "Store/AMO address misaligned"},
    {do_trap_store_fault, "Store/AMO access fault"},
    {do_trap_ecall_u, "Environment call from U-mode"},
    {do_trap_ecall_s, "Environment call from S-mode"},
    {do_trap_unknown, "unknown 10"},
    {do_trap_unknown, "unknown 11"},
    {do_page_fault, "Instruction page fault"},
    {do_page_fault, "Load page fault"},
    {do_trap_unknown, "unknown 14"},
    {do_page_fault, "Store/AMO page fault"},
};


struct fault_info * ec_to_fault_info(unsigned long scause)
{
    struct fault_info *inf;
    if (scause >= (sizeof(fault_inf)/sizeof(fault_inf[0]))) {
        printk("The cause is out of range Exception Code, scause=0x%lx\n", scause);
        panic("Fatal exception\n");
    }
    inf = &fault_inf[scause];
    return inf;
}

void handle_irq(struct pt_regs *regs, unsigned long scause)
{
    switch (scause & ~CAUSE_IRQ_FLAG) {
        case IRQ_S_TIMER:
            //handle_timer_irq();
            break;
        case IRQ_S_EXT:
            plic_handle_irq(regs);
            break;
        case IRQ_S_SOFT:
            // TODO
            break;
        default:
            panic("unexpected interrupt cause\n");
    }
}

void do_exception(struct pt_regs *regs, unsigned long scause)
{
    const struct fault_info *inf;

    printk("%s, scause: 0x%lx\n", __func__, scause);

    if (scause & CAUSE_IRQ_FLAG) {
        intr_irq_dispatch((struct trapframe *)regs);
    }
    else {
        inf = ec_to_fault_info(scause);
        if (!inf->fn(regs, inf->name)) {
            return;
        }
    }
}


#define INIT_THREAD_INFO			\
{						\
	.flags		= 0,			\
	.preempt_count	= 1,	\
}
struct thread_info init_thread_info = INIT_THREAD_INFO;
