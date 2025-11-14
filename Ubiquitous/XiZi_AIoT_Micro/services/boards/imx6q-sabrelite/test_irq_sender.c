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
#include "libserial.h"
#include "usyscall.h"

enum {
    SW_INTERRUPT_3 = 3,
};

enum {
    ARM_PERIPHERAL_BASE = 0x00A00000,
    MX6Q_GICD_BASE_OFFSET = 0x1000,
    MX6Q_GICC_BASE_OFFSET = 0x100,

    ARM_PERIPHERAL_VIRT_BASE = 0x50000000,
};

enum _gicd_sgi_filter {
    //! Forward the interrupt to the CPU interfaces specified in the @a target_list parameter.
    kGicSgiFilter_UseTargetList = 0,

    //! Forward the interrupt to all CPU interfaces except that of the processor that requested
    //! the interrupt.
    kGicSgiFilter_AllOtherCPUs = 1,

    //! Forward the interrupt only to the CPU interface of the processor that requested the
    //! interrupt.
    kGicSgiFilter_OnlyThisCPU = 2
};

struct _gicd_registers {
    uint32_t CTLR; //!< Distributor Control Register.
    uint32_t TYPER; //!< Interrupt Controller Type Register.
    uint32_t IIDR; //!< Distributor Implementer Identification Register.
    uint32_t _reserved0[29];
    uint32_t IGROUPRn[8]; //!< Interrupt Group Registers.
    uint32_t _reserved1[24];
    uint32_t ISENABLERn[32]; //!< Interrupt Set-Enable Registers.
    uint32_t ICENABLERn[32]; //!< Interrupt Clear-Enable Registers.
    uint32_t ISPENDRn[32]; //!< Interrupt Set-Pending Registers.
    uint32_t ICPENDRn[32]; //!< Interrupt Clear-Pending Registers.
    uint32_t ICDABRn[32]; //!< Active Bit Registers.
    uint32_t _reserved2[32];
    uint8_t IPRIORITYRn[255 * sizeof(uint32_t)]; //!< Interrupt Priority Registers. (Byte accessible)
    uint32_t _reserved3;
    uint8_t ITARGETSRn[255 * sizeof(uint32_t)]; //!< Interrupt Processor Targets Registers. (Byte accessible)
    uint32_t _reserved4;
    uint32_t ICFGRn[64]; //!< Interrupt Configuration Registers.
    uint32_t _reserved5[128];
    uint32_t SGIR; //!< Software Generated Interrupt Register
};
typedef volatile struct _gicd_registers gicd_t;

enum _gicd_sgir_fields {
    kBP_GICD_SGIR_TargetListFilter = 24,
    kBM_GICD_SGIR_TargetListFilter = (0x3 << kBP_GICD_SGIR_TargetListFilter),

    kBP_GICD_SGIR_CPUTargetList = 16,
    kBM_GICD_SGIR_CPUTargetList = (0xff << kBP_GICD_SGIR_CPUTargetList),

    kBP_GICD_SGIR_NSATT = 15,
    kBM_GICD_SGIR_NSATT = (1 << kBP_GICD_SGIR_NSATT),

    kBP_GICD_SGIR_SGIINTID = 0,
    kBM_GICD_SGIR_SGIINTID = 0xf
};

void gic_send_sgi(uint32_t irqID, uint32_t target_list, uint32_t filter_list)
{
    gicd_t* gicd = (gicd_t*)(ARM_PERIPHERAL_VIRT_BASE + MX6Q_GICD_BASE_OFFSET);
    gicd->SGIR = (filter_list << kBP_GICD_SGIR_TargetListFilter) //
        | (target_list << kBP_GICD_SGIR_CPUTargetList) //
        | (irqID & 0xf);
}

int main()
{
    static char prog_name[] = "TEST_IRQ_SEND";
    printf("%s: Mapping GIC\n", prog_name);
    mmap(ARM_PERIPHERAL_VIRT_BASE, ARM_PERIPHERAL_BASE, 0x2000, true);

    // int send_time = 1000;
    int send_time = 1;
    printf("%s: Sending soft interrupt for %d times\n", prog_name, send_time);
    for (int i = 0; i < send_time; i++) {
        gic_send_sgi(SW_INTERRUPT_3, 0xF, kGicSgiFilter_UseTargetList);
        printf("%s: Soft interrupt send 1 time\n", prog_name);
    }
    printf("%s: Soft interrupt send done\n", prog_name);
    exit(0);
}