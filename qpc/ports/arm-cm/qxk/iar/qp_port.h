//============================================================================
// QP/C Real-Time Embedded Framework (RTEF)
//
//                   Q u a n t u m  L e a P s
//                   ------------------------
//                   Modern Embedded Software
//
// Copyright (C) 2005 Quantum Leaps, LLC <state-machine.com>.
//
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-QL-commercial
//
// This software is dual-licensed under the terms of the open source GNU
// General Public License version 3 (or any later version), or alternatively,
// under the terms of one of the closed source Quantum Leaps commercial
// licenses.
//
// The terms of the open source GNU General Public License version 3
// can be found at: <www.gnu.org/licenses/gpl-3.0>
//
// The terms of the closed source Quantum Leaps commercial licenses
// can be found at: <www.state-machine.com/licensing>
//
// Redistributions in source code must retain this top-level comment block.
// Plagiarizing this software to sidestep the license obligations is illegal.
//
// Contact information:
// <www.state-machine.com>
// <info@state-machine.com>
//============================================================================
//! @date Last updated on: 2023-12-02
//! @version Last updated for: @ref qpc_7_3_1
//!
//! @file
//! @brief QP/C port to ARM Cortex-M, dual-mode QXK kernel, IAR-ARM

#ifndef QP_PORT_H_
#define QP_PORT_H_

#include <stdint.h>  // Exact-width types. WG14/N843 C99 Standard
#include <stdbool.h> // Boolean type.      WG14/N843 C99 Standard
#include <intrinsics.h> // IAR intrinsic functions

#ifdef QP_CONFIG
#include "qp_config.h" // external QP configuration
#endif

// no-return function specifier (C11 Standard)
#define Q_NORETURN   _Noreturn void

// QF configuration for QXK -- data members of the QActive class...

// QActive event-queue type used for AOs and eXtended threads.
#define QACTIVE_EQUEUE_TYPE     QEQueue

// QActive OS-Object type used for the private stack pointer for eXtended threads.
// (The private stack pointer is NULL for basic-threads).
#define QACTIVE_OS_OBJ_TYPE     void*

// QActive "thread" type used to store the MPU settings in the AO
#define QACTIVE_THREAD_TYPE     void const *

// QF interrupt disable/enable and log2()...
#if (__ARM_ARCH == 6) // ARMv6-M?

    // CMSIS threshold for "QF-aware" interrupts, see NOTE2 and NOTE4
    #define QF_AWARE_ISR_CMSIS_PRI 0

    // hand-optimized LOG2 in assembly for Cortex-M0/M0+/M1(v6-M, v6S-M)
    #define QF_LOG2(n_) QF_qlog2((uint32_t)(n_))

#else // ARMv7-M or higher

    // BASEPRI threshold for "QF-aware" interrupts, see NOTE3
    #define QF_BASEPRI          0x3F

    // CMSIS threshold for "QF-aware" interrupts, see NOTE4
    #define QF_AWARE_ISR_CMSIS_PRI (QF_BASEPRI >> (8 - __NVIC_PRIO_BITS))

    // ARMv7-M or higher provide the CLZ instruction for fast LOG2
    #define QF_LOG2(n_) ((uint_fast8_t)(32U - __CLZ((unsigned long)(n_))))

#endif

// interrupt disabling policy, see NOTE2 and NOTE3
#define QF_INT_DISABLE()        (QF_int_disable_())
#define QF_INT_ENABLE()         (QF_int_enable_())

// QF critical section, see NOTE1, NOTE2, and NOTE3
#define QF_CRIT_STAT
#define QF_CRIT_ENTRY()         (QF_crit_entry_())
#define QF_CRIT_EXIT()          (QF_crit_exit_())

#define QF_CRIT_EXIT_NOP()      __ISB()

#if (__ARM_ARCH == 6) // ARMv6-M?
    // hand-optimized quick LOG2 in assembly
    uint_fast8_t QF_qlog2(uint32_t x);
#endif // ARMv7-M or higher

// Memory isolation ----------------------------------------------------------
#ifdef QF_MEM_ISOLATE

    // Memory isolation requires the context-switch
    #define QF_ON_CONTEXT_SW   1U

    // Memory System setting
    #define QF_MEM_SYS() QF_onMemSys()

    // Memory Application setting
    #define QF_MEM_APP() QF_onMemApp()

    // callback functions for memory settings (provided by applications)
    void QF_onMemSys(void);
    void QF_onMemApp(void);

#endif // def QF_MEM_ISOLATE

// determination if the code executes in the ISR context
#define QXK_ISR_CONTEXT_()     (__get_IPSR() != 0U)

// trigger the PendSV exception to perform the context switch
#define QXK_CONTEXT_SWITCH_()  \
    *Q_UINT2PTR_CAST(uint32_t, 0xE000ED04U) = (1U << 28U)

// QXK ISR entry and exit
#define QXK_ISR_ENTRY() ((void)0)

#ifdef QF_MEM_ISOLATE
    #define QXK_ISR_EXIT()  do {                                  \
        QF_INT_DISABLE();                                         \
        QF_MEM_SYS();                                             \
        if (QXK_sched_() != 0U) {                                 \
            *Q_UINT2PTR_CAST(uint32_t, 0xE000ED04U) = (1U << 28U);\
        }                                                         \
        QF_MEM_APP();                                             \
        QF_INT_ENABLE();                                          \
        QXK_ARM_ERRATUM_838869();                                 \
    } while (false)
#else
    #define QXK_ISR_EXIT()  do {                                  \
        QF_INT_DISABLE();                                         \
        if (QXK_sched_() != 0U) {                                 \
            *Q_UINT2PTR_CAST(uint32_t, 0xE000ED04U) = (1U << 28U);\
        }                                                         \
        QF_INT_ENABLE();                                          \
        QXK_ARM_ERRATUM_838869();                                 \
    } while (false)
#endif

#if (__ARM_ARCH == 6) // ARMv6-M?
    #define QXK_ARM_ERRATUM_838869() ((void)0)
#else // ARMv7-M or higher
    // The following macro implements the recommended workaround for the
    // ARM Erratum 838869. Specifically, for Cortex-M3/M4/M7 the DSB
    // (memory barrier) instruction needs to be added before exiting an ISR.
    #define QXK_ARM_ERRATUM_838869() __DSB()

#endif // ARMv6-M

// initialization of the QXK kernel
#define QXK_INIT() QXK_init()
void QXK_init(void);
void QXK_thread_ret(void);

#ifdef __ARM_FP         //--------- if VFP available...
// When the FPU is configured, clear the FPCA bit in the CONTROL register
// to prevent wasting the stack space for the FPU context.
#define QXK_START()     __set_CONTROL(0U)
#endif

// include files -------------------------------------------------------------
#include "qequeue.h"   // QXK kernel uses the native QP event queue
#include "qmpool.h"    // QXK kernel uses the native QP memory pool
#include "qp.h"        // QP framework
#include "qxk.h"       // QXK kernel

// prototypes
void QF_int_disable_(void);
void QF_int_enable_(void);
void QF_crit_entry_(void);
void QF_crit_exit_(void);

extern int32_t volatile QF_int_lock_nest_;

//============================================================================
// NOTE1:
// The critical section policy does not use the "saving and restoring"
// interrupt status policy (macro QF_CRIT_STAT is defined to nothing).
// However, this the QF critical sections might still be able to nest,
// depending on the implementation of the QF_crit_entry_()/QF_crit_exit_()
// functions. They are defined as "weak" in the qv_port.c module,
// so the application can provide a different implementation.
// Please see the definitions of the interrupt and critical-section
// funcctions in the qv_port.c module for details.
//
// NOTE2:
// On Cortex-M0/M0+/M1 (architecture ARMv6-M, ARMv6S-M), the interrupt
// disabling policy uses the PRIMASK register to disable interrupts globally.
// The QF_AWARE_ISR_CMSIS_PRI level is zero, meaning that all interrupts
// are "kernel-aware".
//
// NOTE3:
// On ARMv7-M or higher, the interrupt disable/enable policy uses the BASEPRI
// register (which is not implemented in ARMv6-M) to disable interrupts only
// with priority lower than the threshold specified by the QF_BASEPRI macro.
// The interrupts with priorities above QF_BASEPRI (i.e., with numerical
// priority values lower than QF_BASEPRI) are NOT disabled in this method.
// These free-running interrupts have very low ("zero") latency, but they
// are NOT allowed to call any QF services, because QF is unaware of them
// ("kernel-unaware" interrupts). Consequently, only interrupts with
// numerical values of priorities equal to or higher than QF_BASEPRI
// ("kernel-aware" interrupts ), can call QF services.
//
// NOTE4:
// The QF_AWARE_ISR_CMSIS_PRI macro is useful as an offset for enumerating
// the "QF-aware" interrupt priorities in the applications, whereas the
// numerical values of the "QF-aware" interrupts must be greater or equal to
// QF_AWARE_ISR_CMSIS_PRI. The values based on QF_AWARE_ISR_CMSIS_PRI can be
// passed directly to the CMSIS function NVIC_SetPriority(), which shifts
// them by (8 - __NVIC_PRIO_BITS) into the correct bit position, while
// __NVIC_PRIO_BITS is the CMSIS macro defining the number of implemented
// priority bits in the NVIC. Please note that the macro QF_AWARE_ISR_CMSIS_PRI
// is intended only for applications and is not used inside the QF port, which
// remains generic and not dependent on the number of implemented priority bits
// implemented in the NVIC.

#endif // QP_PORT_H_

