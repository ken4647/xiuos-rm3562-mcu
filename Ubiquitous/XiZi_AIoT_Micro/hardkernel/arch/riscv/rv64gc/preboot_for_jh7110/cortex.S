/*!
 * @file cortex.s
 * @brief This file contains cortexA55 functions
 *
 */
/*************************************************
File name: cortex.S
Description: This file contains cortexA9 functions
Others: 
History:
1. Date: 2024-05-08
Author: AIIT XUOS Lab
Modification:
1. No modifications
*************************************************/
    .section ".text","ax"

    .global cpu_get_current

  # int cpu_get_current(void)@
  # get current CPU ID
    .func cpu_get_current
cpu_get_current:
    li a0, 0
    ret
    .endfunc

    .global psci_call
psci_call:
        ret


# ------------------------------------------------------------
# End of cortexA55.s
# ------------------------------------------------------------
    .end