#ifndef _CSLR_CPU_H_
#define _CSLR_CPU_H_

#include "tistdtypes.h"

/* Minimal CPU register overlay stub for this project.
 * The uploaded soc.h requires CSL_CpuRegs only to form CSL_CPU_REGS.
 * PLL code in this PM project does not access CPU overlay fields.
 * If your CCS/CSL installation already has the official cslr_cpu.h,
 * prefer using the official file instead of this stub.
 */
typedef struct {
    volatile Uint16 RSVD0[1];
} CSL_CpuRegs;

#endif /* _CSLR_CPU_H_ */
