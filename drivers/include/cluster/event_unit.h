/*
 * Copyright (c) 2018, GreenWaves Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of GreenWaves Technologies, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __PI_HAL_EU_H__
#define __PI_HAL_EU_H__

#include "irq.h"
#include "memory_map.h"
#include "target.h"

#define CL_EU_BARRIER_BASE                                                                         \
	(CL_EU_BASE + 0x0400UL) /*!< RISC Core Event Unit HW Barrier Base Address */
#define CL_EU_SW_EVENTS_BASE                                                                       \
	(CL_EU_BASE + 0x0600UL) /*!< RISC Core Event Unit SW Events Base Address */
#define CL_EU_SOC_EVENTS_BASE                                                                      \
	(CL_EU_BASE + 0x0700UL) /*!< RISC Core Event Unit SOC Events Base Address */
#define CL_EU_EXT_EVENTS_BASE                                                                      \
	(CL_EU_BASE + 0x0780UL) /*!< RISC Core Event Unit External Events Base Address */

#define CORE_EU_CORE_DEMUX_BASE                                                                    \
	(CL_EU_DEMUX_BASE) /*!< RISC Core Event Unit Core Demux Base Address */
#define CL_EU_SEC_DEMUX_BASE                                                                       \
	(CL_EU_DEMUX_BASE + 0x0040UL) /*!< RISC Core Event Unit Security Demux Base Address */
#define CL_EU_LOOP_DEMUX_BASE                                                                      \
	(CL_EU_DEMUX_BASE + 0x0060UL) /*!< RISC Core Event Unit Loop Demux Base Address */
#define CL_EU_DISPATCH_DEMUX_BASE                                                                  \
	(CL_EU_DEMUX_BASE + 0x0080UL) /*!< RISC Core Event Unit Dispatch Demux Base Address */
#define CL_EU_MUTEX_DEMUX_BASE                                                                     \
	(CL_EU_DEMUX_BASE + 0x00C0UL) /*!< RISC Core Event Unit Mutex Demux Base Address */
#define CL_EU_SW_EVENTS_DEMUX_BASE                                                                 \
	(CL_EU_DEMUX_BASE + 0x0100UL) /*!< RISC Core Event Unit SW Event Demux Base Address */
#define CL_EU_BARRIER_DEMUX_BASE                                                                   \
	(CL_EU_DEMUX_BASE + 0x0200UL) /*!< RISC Core Event Unit HW Barrier Demux Base Address */

/* TODO: hack, hardcoded */
#define FC_BASE (0x1B000000UL) /*!< FC Base Address */
#define CORE_EU_SW_EVENTS_BASE                                                                     \
	(0x00200800UL + 0x0600UL) /*!< RISC Core Event Unit SW Events Base Address */
#define FC_EU_SW_EVENTS_BASE                                                                       \
	(FC_BASE + CORE_EU_SW_EVENTS_BASE) /*!< FC Event Unit SW Events Base Address */

typedef struct {
	volatile uint32_t MASK;		     /**< EU_DEMUX mask register, offset: 0x00 */
	volatile uint32_t MASK_AND;	     /**< EU_DEMUX mask and register, offset: 0x04 */
	volatile uint32_t MASK_OR;	     /**< EU_DEMUX mask or register, offset: 0x08 */
	volatile uint32_t MASK_IRQ;	     /**< EU_DEMUX mask irq register, offset: 0x0C */
	volatile uint32_t MASK_IRQ_AND;	     /**< EU_DEMUX mask irq and register, offset: 0x10 */
	volatile uint32_t MASK_IRQ_OR;	     /**< EU_DEMUX mask irq or register, offset: 0x14 */
	volatile uint32_t STATUS;	     /**< EU_DEMUX Status register, offset: 0x18 */
	volatile uint32_t BUFFER;	     /**< EU_DEMUX buffer register, offset: 0x1C */
	volatile uint32_t BUFFER_MASKED;     /**< EU_DEMUX buffer masked register, offset: 0x20 */
	volatile uint32_t BUFFER_IRQ_MASKED; /**< EU_DEMUX buffer irq masked register, offset: 0x24
					      */
	volatile uint32_t BUFFER_CLEAR;	     /**< EU_DEMUX buffer clear register, offset: 0x28 */
	volatile uint32_t SW_EVENTS_MASK; /**< EU_DEMUX software event mask register, offset: 0x2C
					   */
	volatile uint32_t SW_EVENTS_MASK_AND; /**< EU_DEMUX software event mask and register,
						 offset: 0x30 */
	volatile uint32_t SW_EVENTS_MASK_OR;  /**< EU_DEMUX software event mask or register, offset:
						 0x34 */
	volatile uint32_t EVENT_WAIT;	      /**< EU_DEMUX event wait register, offset: 0x38 */
	volatile uint32_t EVENT_WAIT_CLEAR; /**< EU_DEMUX event wait clear register, offset: 0x3C */
	volatile uint32_t MASK_SEC_IRQ;	    /**< EU_DEMUX mask sec irq register, offset: 0x40 */
} EU_CORE_DEMUX_Type;

#define EU_CORE_DEMUX ((EU_CORE_DEMUX_Type *)CL_EU_DEMUX_BASE)

typedef struct {
	volatile uint32_t STATE;  /**< EU_LOOP_DEMUX state register, offset: 0x00 */
	volatile uint32_t START;  /**< EU_LOOP_DEMUX start register, offset: 0x04 */
	volatile uint32_t END;	  /**< EU_LOOP_DEMUX end register, offset: 0x08 */
	volatile uint32_t INCR;	  /**< EU_LOOP_DEMUX increment register, offset: 0x0C */
	volatile uint32_t CHUNK;  /**< EU_LOOP_DEMUX chunk register, offset: 0x10 */
	volatile uint32_t EPOCH;  /**< EU_LOOP_DEMUX epoch register, offset: 0x14 */
	volatile uint32_t SINGLE; /**< EU_LOOP_DEMUX single register, offset: 0x18 */
} EU_LOOP_DEMUX_Type;

#define EU_LOOP_DEMUX_DONE_   0x0
#define EU_LOOP_DEMUX_LOCKED_ 0x1
#define EU_LOOP_DEMUX_SKIP_   0x2

#define EU_LOOP_DEMUX ((EU_LOOP_DEMUX_Type *)CL_EU_LOOP_DEMUX_BASE)

typedef struct {
	volatile uint32_t TRIGGER_SET[8]; /**< EU_SW_EVENTS_DEMUX trigger set register, offset: 0x00
					   */
	volatile uint32_t _reserved0[8];  /*!< Offset: 0x20 (R/W)  Empty Registers */
	volatile uint32_t TRIGGER_WAIT[8]; /**< EU_SW_EVENTS_DEMUX trigger wait register, offset:
					      0x40 */
	volatile uint32_t _reserved1[8];   /*!< Offset: 0x60 (R/W)  Empty Registers */
	volatile uint32_t TRIGGER_CLR[8];  /**< EU_SW_EVENTS_DEMUX trigger clear register, offset:
					      0x80 */
} EU_SW_EVENTS_DEMUX_Type;

#define CLUSTER_EU_SW_EVENTS ((EU_SW_EVENTS_DEMUX_Type *)CL_EU_SW_EVENTS_BASE)

typedef struct {
	volatile uint32_t FIFO_ACCESS; /**< EU_DISPATCH_DEMUX fifo access register, offset: 0x00 */
	volatile uint32_t TEAM_CONFIG; /**< EU_DISPATCH_DEMUX team config register, offset: 0x04 */
} EU_DISPATCH_DEMUX_Type;

#define EU_DISPATCH_DEMUX_ELEM_NUM 8

#define EU_DISPATCH_DEMUX ((EU_DISPATCH_DEMUX_Type *)CL_EU_DISPATCH_DEMUX_BASE)

typedef struct {
	volatile uint32_t MUTEX[1]; /**< EU_MUTEX_DEMUX mutex register, offset: 0x00 */
} EU_MUTEX_DEMUX_Type;

#define EU_MUTEX_DEMUX_ELEM_NUM 1

#define EU_MUTEX_DEMUX ((EU_MUTEX_DEMUX_Type *)CL_EU_MUTEX_DEMUX_BASE)

typedef struct {
	volatile uint32_t TRIGGER_MASK;	 /**< EU_BARRIER_DEMUX triger mask register, offset: 0x00 */
	volatile uint32_t STATUS;	 /**< EU_BARRIER_DEMUX status register, offset: 0x04 */
	volatile uint32_t STATUS_SUMMRY; /**< EU_BARRIER_DEMUX status summary register, offset: 0x08
					  */
	volatile uint32_t TARGET_MASK;	 /**< EU_BARRIER_DEMUX target mask register, offset: 0x0C */
	volatile uint32_t TRIGGER;	 /**< EU_BARRIER_DEMUX trigger register, offset: 0x10 */
	volatile uint32_t TRIGGER_SET;	 /**< EU_BARRIER_DEMUX trigger set register, offset: 0x14 */
	volatile uint32_t TRIGGER_WAIT; /**< EU_BARRIER_DEMUX trigger wait register, offset: 0x18 */
	volatile uint32_t TRIGGER_WAIT_CLEAR; /**< EU_BARRIER_DEMUX trigger clear register, offset:
						 0x1C */
} EU_BARRIER_DEMUX_Type;

#define EU_BARRIER_DEMUX(id)                                                                       \
	((EU_BARRIER_DEMUX_Type *)(CL_EU_BARRIER_DEMUX_BASE + (id * sizeof(EU_BARRIER_DEMUX_Type))))


typedef struct {
	volatile uint32_t CURRENT_EVENT; /**< EU_SOC_EVENTS current event register, offset: 0x00 */
} EU_SOC_EVENTS_Type;

#define EU_CURRENT_VALID_BIT_MASK  (0x80000000U)
#define EU_CURRENT_VALID_BIT_SHIFT (31U)
#define EU_CURRENT_VALID_BIT(x)                                                                    \
	(((uint32_t)(((uint32_t)(x)) << EU_CURRENT_VALID_BIT_SHIFT)) & EU_CURRENT_VALID_BIT_MASK)
#define EU_CURRENT_SOC_EVENT_MASK 0xFF

#define EU_SOC_EVENTS ((EU_SOC_EVENTS_Type *)CL_EU_SOC_EVENTS_BASE)


/*!
 * @addtogroup event_unit
 * @{
 */
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/** @name Event management
 *  The following function can be used for selecting which events can wake-up the core, put the core
 * the sleep, clear events and so on.
 */
/**@{*/

/** Clock-gated load.
 * On architectures that support it, this executes the special p.elw instruction which is used to
 put the core in idle mode until it receives an event. On other architectures, it executes a classic
 read which has the same effect but does not handle correctly interrupts while the core is sleeping.
 \param base   The base address of the event unit.
 \param offset The offset in the event unit where to load from. Depending on this offset, this will
 trigger different behaviors (barrier, wait, wait&clear, etc). \return       The loaded value, after
 the core has been waken-up. This value depends on the feature which is accessed.
*/
static inline uint32_t hal_evt_read32(uint32_t base, uint32_t offset)
{
	uint32_t value;
	value = __builtin_pulp_event_unit_read((int *)base, 0);
	// value = hal_read32((void*)((uint32_t)base+offset));
	return value;
}

/** Get event status.
    Return the value of the event status register. This register contains one bit per event, 1 means
   the event is set. Note that this register is actually used as a status register for both events
   and interrupts, so clearing one will clear the other. This register is a buffer, which means if
   an event is received while its bit is already at 1, the event is somehow lost as the bit stays
   at 1. Thus events should be used as Linux signals. \return       The content of the status
   register.
*/
static inline uint32_t hal_eu_evt_status()
{
	return EU_CORE_DEMUX->BUFFER;
}

/** Get active events.
    Same as eu_evt_status, but the content of the status register is ANDed with the content of the
   event mask register so that only relevant events (that can wake-up the core) are shown. It can
   for example be used to know which events must be handled after a core wake-up as the core has
   been waken-up due to one of them. \return       The content of the status register ANDed with the
   event mask register.
*/
static inline uint32_t hal_eu_evt_status_masked()
{
	return EU_CORE_DEMUX->BUFFER_MASKED;
}

/** Clear some bits of the event status register.
    This allows clearing events after they have been received so that they do not prevent the core
   from sleeping anymore. \param evtMask Bits to be cleared. 1 means the bit must be cleared, 0 it
   should remain the same.
*/
static inline void hal_eu_evt_clr(uint32_t evtMask)
{
	EU_CORE_DEMUX->BUFFER_CLEAR = evtMask;
}

/** Modify the event mask.
    The event mask tells the event unit which events can wake-up the core. When the core executes
   the p.elw instruction it goes to sleep unless there is one bit which is set in both the event
   mask and the event status register. If the event is received while the core is already sleeping
    and the corresponding bit is set in the event mask, the core is waken-up and cannot go back to
   sleep until either the event mask or the event status register is modified (usually the bit is
   cleared using eu_evt_clr) \param evtMask Value to write to the event mask. There is 1 bit per
   event, 1 means it can wake-up the core, 0 it cannot
*/
static inline void hal_eu_evt_mask(uint32_t evtMask)
{
	EU_CORE_DEMUX->MASK = evtMask;
}

/** Set part of the event mask to 1.
    This is similar to eu_evt_mask but for each bit at 1 in the new value, the corresponding bit is
   set in the event mask and the others remain the same. This is useful when the mask must be
   updated before waiting for a specific event without modifying the other events (this saves a few
   instructions) \param evtMask Bit mask used to update the event mask. There is 1 bit per event, 1
   means the corresponding bit is set in the event mask.
*/
static inline void hal_eu_evt_mask_set(uint32_t evtMask)
{
	EU_CORE_DEMUX->MASK_OR = evtMask;
}

/** Set part of the event mask to 0.
    This is the opposite of eu_evt_maskSet. For each bit at 1 in the new value, the corresponding
   bit is set to 0 in the event mask and the others remain the same. \param evtMask Bit mask used to
   update the event mask. There is 1 bit per event, 1 means the corresponding bit is cleared in the
   event mask.
*/
static inline void hal_eu_evt_mask_clr(uint32_t evtMask)
{
	EU_CORE_DEMUX->MASK_AND = evtMask;
}

/** Put the core to sleep mode until it receives an event
    The core goes to sleep until there is one bit at 1 in both the event status register and the
   event mask register. If it is already the case, when this function is called, the core does not
   go to sleep and this function returns immediately.
*/
static inline uint32_t hal_eu_evt_wait()
{
	return hal_evt_read32((uint32_t)&EU_CORE_DEMUX->EVENT_WAIT, 0);
}

/** Put the core to sleep mode until it receives an event and clears the active events
    This is similar to eu_evt_wait but when the core is waken-up, the active events are cleared in
   the status register. An event is active if its bit is at 1 in both the event status register and
   the event mask register.
*/
static inline uint32_t hal_eu_evt_wait_and_clr()
{
	return hal_evt_read32((uint32_t)&EU_CORE_DEMUX->EVENT_WAIT_CLEAR, 0);
}

/** Modify the event mask, put the core to sleep mode until it receives an event, clears the active
   events and restore the mask. This is similar to eu_evt_waitAndClr but the events whose bit is set
   to 1 in the given mask, are activated in the event mask before going to sleep, and are cleared
    when the core is waken-up.
    Not that this function is using 3 instructions instead of 1 for most of other primitives, as it
   needs 2 instruction to set and clear the mask. \param evtMask Event mask used for activating and
   deactivating events. Each bit to 1 means the corresponding event is activated before sleeping and
   deactivated after.
*/
static inline uint32_t hal_eu_evt_mask_wait_and_clr(uint32_t evt_mask)
{
	hal_eu_evt_mask_set(evt_mask);
	uint32_t result = hal_eu_evt_wait_and_clr();
	hal_eu_evt_mask_clr(evt_mask);
	return result;
}

/** Modify the event mask, put the core to sleep mode until it receives an event, clears the active
   events and restore the mask. This is similar to eu_evt_waitAndClr but the events whose bit is set
   to 1 in the given mask, are activated in the event mask before going to sleep, and are cleared
    when the core is waken-up.
    Not that this function is using 3 instructions instead of 1 for most of other primitives, as it
   needs 2 instruction to set and clear the mask. \param evtMask Event mask used for activating and
   deactivating events. Each bit to 1 means the corresponding event is activated before sleeping and
   deactivated after.
*/
static inline uint32_t hal_eu_evt_mask_wait(uint32_t evt_mask)
{
	hal_eu_evt_mask_set(evt_mask);
	uint32_t result = hal_eu_evt_wait();
	hal_eu_evt_mask_clr(evt_mask);
	return result;
}


static inline uint32_t hal_eu_fc_evt_mask_wait_and_clr(uint32_t evt_mask)
{
	hal_eu_evt_mask_set(evt_mask);
	uint32_t result;
	do {
		result = hal_eu_evt_wait_and_clr();
	} while (result == 0);
	hal_eu_evt_mask_clr(evt_mask);
	return result;
}

//!@}


/** @name Interrupt management
 *  Functions for setting-up which IRQ can trigger an interrupt handler, clear interrupts and so on
 */
/**@{*/

static inline uint32_t hal_eu_evt_status_irq_masked()
{
	return EU_CORE_DEMUX->BUFFER_IRQ_MASKED;
}

static inline void hal_eu_irq_mask(uint32_t irq_mask)
{
	EU_CORE_DEMUX->MASK_IRQ = irq_mask;
}

static inline void hal_eu_irq_sec_mask(uint32_t irq_mask)
{
	EU_CORE_DEMUX->MASK_SEC_IRQ = irq_mask;
}

static inline void hal_eu_irq_mask_set(uint32_t irq_mask)
{
	EU_CORE_DEMUX->MASK_IRQ_OR = irq_mask;
}

static inline void hal_eu_irq_mask_clr(uint32_t irq_mask)
{
	EU_CORE_DEMUX->MASK_IRQ_AND = irq_mask;
}

/*
static inline void hal_eu_irq_sec_mask_set(uint32_t irq_mask)
{
    EU_SEC_DEMUX->MASK_OR = irq_mask;
}

static inline void hal_eu_irq_sec_mask_clr(uint32_t irq_mask)
{
    EU_SEC_DEMUX->MASK_AND = irq_mask;
}
*/

static inline uint32_t hal_eu_irq_status()
{
	return EU_CORE_DEMUX->BUFFER_IRQ_MASKED;
}

//!@}


/** @name SW event trigger.
 *  Functions for triggering SW events which can be used to wake-up cluster cores from SW.
 */
/**@{*/

#ifdef CONFIG_CLUSTER
static inline void hal_eu_cluster_evt_trig_set(int event_num, uint32_t value)
{
	CLUSTER_EU_SW_EVENTS->TRIGGER_SET[event_num] = value;
}
#endif


static inline void hal_eu_fc_evt_trig_set(int event_num, uint32_t value)
{
	/* pend software interrupt in fc */
	irq_pend(1 << event_num);
}

/*
static inline void hal_eu_fc_evt_demux_trig_set(int event_num, uint32_t value)
{
    hal_write32(&EU_SW_EVENTS_DEMUX->TRIGGER_SET[event_num],value);
}
*/

//!@}


/** @name Hardware barriers.
 *  Functions for managing the hardware barriers which can be used to implement a rendez-vous
 */
/**@{*/

// Convert barrier ID into barrier address, that can be used more efficiently by
// subsequent operations (usually leads to only 1 lw or sw)

// All-in-one barrier: Trigger barrier notif, wait for barrier and clear the event
// This also return the pending active events.
// Barrier event must be active for this to work
static inline uint32_t hal_eu_barrier_trigger_wait_clear()
{
	return hal_evt_read32((uint32_t)&EU_BARRIER_DEMUX(0)->TRIGGER_WAIT_CLEAR, 0);
}

static inline void hal_eu_barrier_setup(uint32_t core_mask)
{
	EU_BARRIER_DEMUX(0)->TRIGGER_MASK = core_mask;
	EU_BARRIER_DEMUX(0)->TARGET_MASK = core_mask;
}

static inline void hal_eu_barrier_trigger(uint32_t mask)
{
	EU_BARRIER_DEMUX(0)->TRIGGER = mask;
}

static inline uint32_t hal_eu_barrier_team_get(void)
{
	return EU_BARRIER_DEMUX(0)->TRIGGER_MASK;
}


//!@}

/** @name SoC events FIFO.
 *  Functions for managing SoC events coming from outside the cluster and pushed by the HW to the
 * SoC event FIFO.
 */
/**@{*/

static inline uint32_t hal_eu_soc_events_pop()
{
	return hal_read32(&EU_SOC_EVENTS->CURRENT_EVENT);
}

static inline uint32_t hal_fc_eu_soc_events_pop()
{
	hal_compiler_barrier();
	return hal_read32(&EU_SOC_EVENTS->CURRENT_EVENT);
}

static inline int32_t hal_eu_soc_events_is_valid(uint32_t word)
{
	hal_compiler_barrier();
	return ((word & EU_CURRENT_VALID_BIT_MASK) >> EU_CURRENT_VALID_BIT_SHIFT);
}

static inline uint32_t hal_eu_soc_events_get_event(uint32_t word)
{
	hal_compiler_barrier();
	return word & EU_CURRENT_SOC_EVENT_MASK;
}

//!@}


/** @name Hardware mutexes.
 *  Functions for managing, locking and unlocking hardware mutexes
 */
/**@{*/
static inline void hal_eu_mutex_lock(uint32_t mutex_id)
{
	hal_evt_read32((uint32_t)&EU_MUTEX_DEMUX->MUTEX[mutex_id], 0);
}

static inline void hal_eu_mutex_unlock(uint32_t mutex_id)
{
	__asm__ __volatile__("" : : : "memory");
	EU_MUTEX_DEMUX->MUTEX[mutex_id] = 0;
	__asm__ __volatile__("" : : : "memory");
}

static inline void hal_eu_mutex_init(uint32_t mutex_id)
{
	__asm__ __volatile__("" : : : "memory");
	EU_MUTEX_DEMUX->MUTEX[mutex_id] = 0;
	__asm__ __volatile__("" : : : "memory");
}

//!@}


/** @name Hardware dispatcher.
 *  Functions for managing the hardware dispatcher, which can be used to dispatch entry points to
 * sleeping slave cores.
 */
/**@{*/

static inline uint32_t hal_eu_dispatch_pop()
{
	return hal_evt_read32((uint32_t)&EU_DISPATCH_DEMUX->FIFO_ACCESS, 0);
}

static inline void hal_eu_dispatch_push(uint32_t value)
{
	hal_write32(&EU_DISPATCH_DEMUX->FIFO_ACCESS, value);
}

static inline void hal_eu_dispatch_team_config(uint32_t value)
{
	hal_write32(&EU_DISPATCH_DEMUX->TEAM_CONFIG, value);
}

#if 0
/* This register is write-only. */
static inline uint32_t hal_eu_dispatch_read_team_config()
{
    return EU_DISPATCH_DEMUX->TEAM_CONFIG;
}
#endif

//!@}


/** @name Hardware loops.
 *  Functions for managing the hardware loops, which can be used to dispatch a range of indexes to a
 * core set.
 */
/**@{*/

static inline uint32_t hal_eu_loop_addr(uint32_t id)
{
	return CL_EU_LOOP_DEMUX_BASE;
}

static inline uint32_t hal_eu_loop_get_chunk()
{
	return EU_LOOP_DEMUX->CHUNK;
}

static inline uint32_t hal_eu_loop_get_start()
{
	return EU_LOOP_DEMUX->START;
}

static inline uint32_t hal_eu_loop_get_state()
{
	return EU_LOOP_DEMUX->STATE;
}

static inline void hal_eu_loop_set_start(uint32_t value)
{
	EU_LOOP_DEMUX->START = value;
}

static inline void hal_eu_loop_set_end(uint32_t value)
{
	EU_LOOP_DEMUX->END = value;
}

static inline void hal_eu_loop_set_incr(uint32_t value)
{
	EU_LOOP_DEMUX->INCR = value;
}

static inline void hal_eu_loop_set_chunk(uint32_t value)
{
	EU_LOOP_DEMUX->CHUNK = value;
}

static inline void hal_eu_loop_init_epoch(uint32_t core_set)
{
	EU_LOOP_DEMUX->EPOCH = core_set;
}

static inline uint32_t hal_eu_loop_get_single()
{
	return EU_LOOP_DEMUX->SINGLE;
}

//!@}

//!@}

static inline uint32_t hal_eu_evt_get_cluster_base(uint32_t core_id)
{
	return ARCHI_CLUSTER_PERIPHERALS_ADDR + ARCHI_EU_OFFSET + (core_id << 6);
}

#endif /* __PI_HAL_EU_H__ */
