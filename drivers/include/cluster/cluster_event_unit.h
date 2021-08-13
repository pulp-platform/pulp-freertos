/*
 * Copyright (c) 2020, GreenWaves Technologies, Inc.
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

#ifndef __PMSIS_IMPLEM_HAL_CLUSTER_CLUSTER_EVENT_UNIT_H__
#define __PMSIS_IMPLEM_HAL_CLUSTER_CLUSTER_EVENT_UNIT_H__

#ifndef __ASSEMBLER__
#include <stdint.h>
#include "target.h"
#include "memory_map.h"
#endif

/*! Event_Unit Demux offset */
#define CLUSTER_DEMUX_EU_CORE_OFFSET	 (0x00000000)
#define CLUSTER_DEMUX_EU_LOOP_OFFSET	 (0x00000060)
#define CLUSTER_DEMUX_EU_DISPATCH_OFFSET (0x00000080)
#define CLUSTER_DEMUX_EU_MUTEX_OFFSET	 (0x000000C0)
#define CLUSTER_DEMUX_EU_SW_EVENT_OFFSET (0x00000100)
#define CLUSTER_DEMUX_EU_BARRIER_OFFSET	 (0x00000200)
#define CLUSTER_DEMUX_DMA_OFFSET	 (0x00000400)

/*! Event_Unit Core Demux */
#define CL_DEMUX_EU_CORE_EVENT_MASK	   (0x00)
#define CL_DEMUX_EU_CORE_EVENT_MASK_AND	   (0x04)
#define CL_DEMUX_EU_CORE_EVENT_MASK_OR	   (0x08)
#define CL_DEMUX_EU_CORE_IRQ_MASK	   (0x0C)
#define CL_DEMUX_EU_CORE_IRQ_MASK_AND	   (0x10)
#define CL_DEMUX_EU_CORE_IRQ_MASK_OR	   (0x14)
#define CL_DEMUX_EU_CORE_STATUS		   (0x18)
#define CL_DEMUX_EU_CORE_BUFFER		   (0x1C)
#define CL_DEMUX_EU_CORE_BUFFER_MASKED	   (0x20)
#define CL_DEMUX_EU_CORE_BUFFER_IRQ_MASKED (0x24)
#define CL_DEMUX_EU_CORE_BUFFER_CLEAR	   (0x28)
#define CL_DEMUX_EU_CORE_SW_EVT_MASK	   (0x2C)
#define CL_DEMUX_EU_CORE_SW_EVT_MASK_AND   (0x30)
#define CL_DEMUX_EU_CORE_SW_EVT_MASK_OR	   (0x34)
#define CL_DEMUX_EU_CORE_EVENT_WAIT	   (0x38)
#define CL_DEMUX_EU_CORE_EVENT_WAIT_CLEAR  (0x3C)
#define CL_DEMUX_EU_CORE_SEC_IRQ_MASK	   (0x40)
#define CL_DEMUX_EU_CORE_SEC_IRQ_MASK_AND  (0x44)
#define CL_DEMUX_EU_CORE_SEC_IRQ_MASK_OR   (0x48)

/*! Event_Unit Loop Demux */
#define CL_DEMUX_EU_LOOP_STATE	(0x00)
#define CL_DEMUX_EU_LOOP_START	(0x04)
#define CL_DEMUX_EU_LOOP_END	(0x08)
#define CL_DEMUX_EU_LOOP_INCR	(0x0C)
#define CL_DEMUX_EU_LOOP_CHUNK	(0x10)
#define CL_DEMUX_EU_LOOP_EPOCH	(0x14)
#define CL_DEMUX_EU_LOOP_SINGLE (0x18)

/*! Event_Unit Dispatch Demux */
#define CL_DEMUX_EU_DISPATCH_FIFO_ACCESS (0x00)
#define CL_DEMUX_EU_DISPATCH_TEAM_CONFIG (0x04)

/*! Event_Unit Mutex Demux */
#define CL_DEMUX_EU_MUTEX_MUTEX (0x00)

/*! Event_Unit SW_Events Demux */
#define CL_DEMUX_EU_SW_EVT_TRIGGER	      (0x00)
#define CL_DEMUX_EU_SW_EVT_TRIGGER_WAIT	      (0x40)
#define CL_DEMUX_EU_SW_EVT_TRIGGER_WAIT_CLEAR (0x80)

/*! Event_Unit Barrier Demux */
#define CL_DEMUX_EU_HW_BARRIER_TRIGGER_MASK	  (0x00)
#define CL_DEMUX_EU_HW_BARRIER_STATUS		  (0x04)
#define CL_DEMUX_EU_HW_BARRIER_STATUS_SUMMARY	  (0x08)
#define CL_DEMUX_EU_HW_BARRIER_TARGET_MASK	  (0x0C)
#define CL_DEMUX_EU_HW_BARRIER_TRIGGER		  (0x10)
#define CL_DEMUX_EU_HW_BARRIER_TRIGGER_SELF	  (0x14)
#define CL_DEMUX_EU_HW_BARRIER_TRIGGER_WAIT	  (0x18)
#define CL_DEMUX_EU_HW_BARRIER_TRIGGER_WAIT_CLEAR (0x1C)

#define CL_DEMUX_EU_HW_BARRIER_SIZE (0x20)


#define CL_DEMUX_EU_CORE_ADDR	    (CL_EU_DEMUX_BASE + CLUSTER_DEMUX_EU_CORE_OFFSET)
#define CL_DEMUX_EU_LOOP_ADDR	    (CL_EU_DEMUX_BASE + CLUSTER_DEMUX_EU_LOOP_OFFSET)
#define CL_DEMUX_EU_DISPATCH_ADDR   (CL_EU_DEMUX_BASE + CLUSTER_DEMUX_EU_DISPATCH_OFFSET)
#define CL_DEMUX_EU_HW_MUTEX_ADDR   (CL_EU_DEMUX_BASE + CLUSTER_DEMUX_EU_HW_MUTEX_OFFSET)
#define CL_DEMUX_EU_SW_EVENT_ADDR   (CL_EU_DEMUX_BASE + CLUSTER_DEMUX_EU_SW_EVENT_OFFSET)
#define CL_DEMUX_EU_HW_BARRIER_ADDR (CL_EU_DEMUX_BASE + CLUSTER_DEMUX_EU_HW_BARRIER_OFFSET)
#define CL_DEMUX_EU_HW_SEMA_ADDR    (CL_EU_DEMUX_BASE + CLUSTER_DEMUX_EU_HW_SEMA_OFFSET)
#define CL_DEMUX_DMA_ADDR	    (CL_EU_DEMUX_BASE + CLUSTER_DEMUX_DMA_OFFSET)

#ifndef __ASSEMBLER__

/*! Event_Unit Core Demux */
typedef struct {
	volatile uint32_t event_mask;
	volatile uint32_t event_mask_and;
	volatile uint32_t event_mask_or;
	volatile uint32_t irq_mask;
	volatile uint32_t irq_mask_and;
	volatile uint32_t irq_mask_or;
	volatile uint32_t status;
	volatile uint32_t buffer;
	volatile uint32_t buffer_masked;
	volatile uint32_t buffer_irq_masked;
	volatile uint32_t buffer_clear;
	volatile uint32_t sw_evt_mask;
	volatile uint32_t sw_evt_mask_and;
	volatile uint32_t sw_evt_mask_or;
	volatile uint32_t event_wait;
	volatile uint32_t event_wait_clear;
	volatile uint32_t sec_irq_mask;
	volatile uint32_t sec_irq_mask_and;
	volatile uint32_t sec_irq_mask_or;
} __attribute__((packed)) cluster_eu_core_demux_t;

/*! Event_Unit Loop Demux */
typedef struct {
	volatile uint32_t state;
	volatile uint32_t start;
	volatile uint32_t end;
	volatile uint32_t incr;
	volatile uint32_t chunk;
	volatile uint32_t epoch;
	volatile uint32_t single;
} __attribute__((packed)) cluster_eu_loop_demux_t;

/*! Event_Unit Dispatch Demux */
typedef struct {
	volatile uint32_t fifo_access;
	volatile uint32_t team_config;
} __attribute__((packed)) cluster_eu_dispatch_demux_t;

/*! Event_Unit Mutex Demux */
typedef struct {
	volatile uint32_t mutex;
} __attribute__((packed)) cluster_eu_mutex_demux_t;

/*! Event_Unit SW_Events Demux */
typedef struct {
	volatile uint32_t trigger[8];
	volatile uint32_t reserved_0[8];
	volatile uint32_t trigger_wait[8];
	volatile uint32_t reserved_1[8];
	volatile uint32_t trigger_wait_clear[8];
} __attribute__((packed)) cluster_eu_sw_evt_demux_t;

/*! Event_Unit Barrier Demux */
typedef struct {
	volatile uint32_t trigger_mask;
	volatile uint32_t status;
	volatile uint32_t status_summary;
	volatile uint32_t target_mask;
	volatile uint32_t trigger;
	volatile uint32_t trigger_self;
	volatile uint32_t trigger_wait;
	volatile uint32_t trigger_wait_clear;
} __attribute__((packed)) cluster_eu_barrier_demux_t;

/* TODO: redudant definitions */
#define cl_demux_eu_core(id) ((cluster_eu_core_demux_t *)CL_EU_DEMUX_BASE)
#define cl_demux_eu_loop(id)                                                                       \
	((cluster_eu_loop_demux_t *)(CL_EU_DEMUX_BASE + CLUSTER_DEMUX_EU_LOOP_OFFSET))
#define cl_demux_eu_dispatch(id)                                                                   \
	((cluster_eu_dispatch_demux_t *)(CL_EU_DEMUX_BASE + CLUSTER_DEMUX_EU_DISPATCH_OFFSET))
#define cl_demux_eu_mutex(id)                                                                      \
	((cluster_eu_mutex_demux_t *)(CL_EU_DEMUX_BASE + CLUSTER_DEMUX_EU_MUTEX_OFFSET))
#define cl_demux_eu_sw_evt(id)                                                                     \
	((cluster_eu_sw_evt_demux_t *)(CL_EU_DEMUX_BASE + CLUSTER_DEMUX_EU_SW_EVENT_OFFSET))
#define cl_demux_eu_barrier(id)                                                                    \
	((cluster_eu_barrier_demux_t *)(CL_EU_DEMUX_BASE + CLUSTER_DEMUX_EU_BARRIER_OFFSET +       \
					(id * CL_DEMUX_EU_HW_BARRIER_SIZE)))

#define CL_DEMUX_PERIPH_OFFSET	      (0x00004000)
#define CL_DEMUX_EU_CORE_OFFSET	      (CL_DEMUX_PERIPH_OFFSET + CLUSTER_DEMUX_EU_CORE_OFFSET)
#define CL_DEMUX_EU_LOOP_OFFSET	      (CL_DEMUX_PERIPH_OFFSET + CLUSTER_DEMUX_EU_LOOP_OFFSET)
#define CL_DEMUX_EU_DISPATCH_OFFSET   (CL_DEMUX_PERIPH_OFFSET + CLUSTER_DEMUX_EU_DISPATCH_OFFSET)
#define CL_DEMUX_EU_HW_MUTEX_OFFSET   (CL_DEMUX_PERIPH_OFFSET + CLUSTER_DEMUX_EU_HW_MUTEX_OFFSET)
#define CL_DEMUX_EU_SW_EVENT_OFFSET   (CL_DEMUX_PERIPH_OFFSET + CLUSTER_DEMUX_EU_SW_EVENT_OFFSET)
#define CL_DEMUX_EU_HW_BARRIER_OFFSET (CL_DEMUX_PERIPH_OFFSET + CLUSTER_DEMUX_EU_HW_BARRIER_OFFSET)

/* #define CL_GLOB_EU_CORE_ADDR(cid)       (ARCHI_CLUSTER_PERIPHERALS_GLOBAL_ADDR(cid) +
 * CL_GLOB_EU_CORE_OFFSET) */
#define CL_GLOB_EU_BARRIER_ADDR(cid)                                                               \
	(ARCHI_CLUSTER_PERIPHERALS_GLOBAL_ADDR(cid) + CL_GLOB_EU_BARRIER_OFFSET)
#define CL_GLOB_EU_SW_EVENT_ADDR(cid)                                                              \
	(ARCHI_CLUSTER_PERIPHERALS_GLOBAL_ADDR(cid) + CL_GLOB_EU_SW_EVENT_OFFSET)
#define CL_GLOB_EU_SOC_EVENT_ADDR(cid)                                                             \
	(ARCHI_CLUSTER_PERIPHERALS_GLOBAL_ADDR(cid) + CL_GLOB_EU_SOC_EVENT_OFFSET)
#define CL_GLOB_EU_DISPATCH_ADDR(cid)                                                              \
	(ARCHI_CLUSTER_PERIPHERALS_GLOBAL_ADDR(cid) + CL_GLOB_EU_DISPATCH_OFFSET)

#define cl_glob_eu_core(cid)	 ((cluster_eu_core_demux_t *)CL_GLOB_EU_CORE_ADDR(cid))
#define cl_glob_eu_dispatch(cid) ((cluster_eu_dispatch_demux_t *)CL_GLOB_EU_DISPATCH_ADDR(cid))
#define cl_glob_eu_sw_evt(cid)	 ((cluster_eu_sw_evt_demux_t *)CL_GLOB_EU_SW_EVENT_ADDR(cid))

/**
 * \brief Clock-gated load.
 *
 * On architectures that support it, this executes the special p.elw instruction
 * which is used to put the core in idle mode until it receives an event.
 * On other architectures, it executes a classic read which has the same effect
 * but does not handle correctly interrupts while the core is sleeping.
 *
 * \param base           Base address of the event unit.
 * \param offset         Offset in the event unit where to load from.
 *                       Depending on this offset, this will trigger different
 *                       behaviors (barrier, wait, wait&clear, etc).
 *
 * \return               The loaded value, after the core has been waken-up.
 *                       This value depends on the feature which is accessed.
 */
// static inline uint32_t hal_eu_read32(uint32_t base, uint32_t offset)
#define hal_eu_read32(base, offset)                                                                \
	({                                                                                         \
		uint32_t value = 0;                                                                \
		asm volatile("" : : : "memory");                                                   \
		value = __builtin_pulp_event_unit_read((void *)base, offset);                      \
		asm volatile("" : : : "memory");                                                   \
		value;                                                                             \
	})

/* value = __builtin_pulp_event_unit_read_fenced((void *) base, offset); */

/**
 * \brief EU_Core.
 *
 * Function to acces cluster event unit.
 *
 * Set event mask .
 * Clear event mask.
 * Set IRQ mask.
 * Clear IRQ mask.
 * Wait for event, used for synchro.
 * Wait for event and clear, used for synchro.
 */
static inline void hal_cl_eu_evt_mask_set(uint32_t mask)
{
	// cl_demux_eu_core(0)->event_mask_or = mask;
	uint32_t base = (uint32_t)cl_demux_eu_core(0);
	hal_write32((volatile void *)(base + CL_DEMUX_EU_CORE_EVENT_MASK_OR), mask);
}

static inline void hal_cl_eu_evt_mask_clear(uint32_t mask)
{
	// cl_demux_eu_core(0)->event_mask_and = mask;
	uint32_t base = (uint32_t)cl_demux_eu_core(0);
	hal_write32((volatile void *)(base + CL_DEMUX_EU_CORE_EVENT_MASK_AND), mask);
}

static inline void hal_cl_eu_irq_mask_set(uint32_t mask)
{
	// cl_demux_eu_core(0)->irq_mask_or = mask;
	uint32_t base = (uint32_t)cl_demux_eu_core(0);
	hal_write32((volatile void *)(base + CL_DEMUX_EU_CORE_IRQ_MASK_OR), mask);
}

static inline void hal_cl_eu_irq_mask_clear(uint32_t mask)
{
	// cl_demux_eu_core(0)->irq_mask_and = mask;
	uint32_t base = (uint32_t)cl_demux_eu_core(0);
	hal_write32((volatile void *)(base + CL_DEMUX_EU_CORE_IRQ_MASK_AND), mask);
}

/**
 * \brief Events and IRQs status.
 *
 * This register contains one bit per event, 1 means the event is set.
 * This register is a buffer, which means if an event is received while its bit
 * is already at 1, the event is somehow lost as the bit stays at 1.
 * Thus events should be used as Linux signals.
 *
 * \return               Events and IRQs received.
 *
 * \note This register is actually used as a status register for both events and
 *       interrupts, so clearing one will clear the other.
 */
static inline uint32_t hal_cl_eu_evt_irq_status(void)
{
	uint32_t base = (uint32_t)cl_demux_eu_core(0);
	return hal_eu_read32(base, CL_DEMUX_EU_CORE_BUFFER);
}

/**
 * \brief Event status.
 *
 * Same as hal_cl_eu_evt_irq_status(), with the content ANDed with current event
 * mask, so that only relevant events (that can wake-up the core) are shown.
 * It can for example be used to know which events must be handled after a core
 * wake-up as the core has been waken-up due to one of them.
 *
 * \return               Events received.
 */
static inline uint32_t hal_cl_eu_evt_status(void)
{
	uint32_t base = (uint32_t)cl_demux_eu_core(0);
	return hal_eu_read32(base, CL_DEMUX_EU_CORE_BUFFER_MASKED);
}

/**
 * \brief IRQs status.
 *
 * Same as hal_cl_eu_evt_irq_status(), with the content ANDed with current event
 * IRQ mask, so that only relevant events (that can wake-up the core) are shown.
 * It can for example be used to know which events must be handled after a core
 * wake-up as the core has been waken-up due to one of them.
 *
 * \return               IRQs received.
 */
static inline uint32_t hal_cl_eu_irq_status(void)
{
	uint32_t base = (uint32_t)cl_demux_eu_core(0);
	return hal_eu_read32(base, CL_DEMUX_EU_CORE_BUFFER_IRQ_MASKED);
}

/**
 * \brief Clear event status.
 *
 * Clears the corresponding buffered bit in eu_core_buffer to 0.
 * If the corresponding event line is active in the same cycle where the clear
 * operation gets effective, the bit still does get cleared to 0.
 */
static inline void hal_cl_eu_evt_clear(uint32_t mask)
{
	uint32_t base = (uint32_t)cl_demux_eu_core(0);
	hal_write32((volatile void *)(base + CL_DEMUX_EU_CORE_BUFFER_CLEAR), mask);
}

/**
 * \brief Wait and sleep.
 *
 * Reading this register will stop the clock of the core until at least one
 * event with the corresponding mask bit set to 1 occurs. The read content of
 * this register is identical to that of eu_core_buffer_masked.
 *
 * \return               Events received.
 */
static inline uint32_t hal_cl_eu_evt_wait(void)
{
	uint32_t base = (uint32_t)cl_demux_eu_core(0);
	return hal_eu_read32(base, CL_DEMUX_EU_CORE_EVENT_WAIT);
}

/**
 * \brief Wait and sleep.
 *
 * Reading from this register has the same effect as reading from eu_core_event_wait.
 * In addition, the bits of eu_core_buffer that are set to 1 in eu_core_mask
 * will be cleared to 0 after the read.
 *
 * \return               Events received.
 */
static inline uint32_t hal_cl_eu_evt_wait_and_clear(void)
{
	uint32_t base = (uint32_t)cl_demux_eu_core(0);
	return hal_eu_read32(base, CL_DEMUX_EU_CORE_EVENT_WAIT_CLEAR);
}

static inline void hal_cl_eu_evt_mask_wait(uint32_t event_mask)
{
	hal_cl_eu_evt_mask_set(event_mask);
	uint32_t result = 0;
	do {
		result = hal_cl_eu_evt_wait();
	} while ((result & event_mask) == 0);
	hal_cl_eu_evt_mask_clear(event_mask);
}

static inline void hal_cl_eu_evt_mask_wait_clear(uint32_t event_mask)
{
	hal_cl_eu_evt_mask_set(event_mask);
	uint32_t result = 0;
	do {
		result = hal_cl_eu_evt_wait();
	} while ((result & event_mask) == 0);
	hal_cl_eu_evt_clear(event_mask);
	hal_cl_eu_evt_mask_clear(event_mask);
}


/**
 * \brief EU_Dispatch.
 *
 * Functions to dispatch messages/info to cores.
 * Used by cluster core master to send cluster task info to slaves.
 *
 * Push a message to slave cores.
 * Pop a message, by slave cores.
 * Configure cores that will receive messages.
 */
static inline void hal_cl_eu_dispatch_fifo_push(uint32_t message)
{
	// cl_demux_eu_dispatch(0)->fifo_access = elem;
	uint32_t base = (uint32_t)cl_demux_eu_dispatch(0);
	hal_write32((volatile void *)(base + CL_DEMUX_EU_DISPATCH_FIFO_ACCESS), message);
}

static inline uint32_t hal_cl_eu_dispatch_fifo_pop(void)
{
	// cl_demux_eu_dispatch(0)->fifo_access = elem;
	uint32_t base = (uint32_t)cl_demux_eu_dispatch(0);
	return hal_read32((volatile void *)(base + CL_DEMUX_EU_DISPATCH_FIFO_ACCESS));
}

static inline void hal_cl_eu_dispatch_team_config(uint32_t team_mask)
{
	// cl_demux_eu_dispatch(0)->team_config = team_mask;
	uint32_t base = (uint32_t)cl_demux_eu_dispatch(0);
	hal_write32((volatile void *)(base + CL_DEMUX_EU_DISPATCH_TEAM_CONFIG), team_mask);
}


/**
 * \brief EU_HW_Mutex.
 *
 * Functions to lock/unlock cluster HW mutex.
 *
 * Initialize mutex.
 * Lock mutex.
 * Unlock mutex.
 */
static inline void hal_cl_eu_mutex_init(uint32_t mutex_id)
{
	uint32_t base = (uint32_t)cl_demux_eu_mutex(mutex_id);
	hal_compiler_barrier();
	hal_write32((volatile void *)(base + CL_DEMUX_EU_MUTEX_MUTEX), 0);
	hal_compiler_barrier();
}

static inline void hal_cl_eu_mutex_lock(uint32_t mutex_id)
{
	uint32_t base = (uint32_t)cl_demux_eu_mutex(mutex_id);
	hal_eu_read32(base, CL_DEMUX_EU_MUTEX_MUTEX);
}

static inline void hal_cl_eu_mutex_unlock(uint32_t mutex_id)
{
	uint32_t base = (uint32_t)cl_demux_eu_mutex(mutex_id);
	hal_compiler_barrier();
	hal_write32((volatile void *)(base + CL_DEMUX_EU_MUTEX_MUTEX), 0);
	hal_compiler_barrier();
}


/**
 * \brief EU_SW_Trig.
 *
 * Functions to use to trigger a SW IRQ from FC to cluster.
 *
 * Trigger a SW IRQ to cores of a given cluster.
 */
static inline void hal_cl_eu_glob_sw_trig(uint32_t cluster_id, uint32_t sw_event,
					  uint32_t core_mask)
{
	uint32_t base = (uint32_t)cl_glob_eu_sw_evt(cluster_id);
	uint32_t offset = (CL_DEMUX_EU_SW_EVT_TRIGGER + (sw_event << 2));
	hal_write32((volatile void *)(base + offset), core_mask);
}


/**
 * \brief EU_HW_Barrier.
 *
 * Functions to use to config/access cluster barrier.
 *
 * Setup a barrier for a mask of cores.
 * Get current status of barrier.
 * Trigger barrier with a mask.
 * Trigger barrier with core_id of core calling func.
 * Trigger barrier with core_id of core calling func and sleep.
 * Trigger barrier with core_id of core calling func and sleep and clear buffer.
 */
static inline void hal_cl_eu_barrier_setup(uint32_t barrier_id, uint32_t core_mask)
{
	uint32_t base = (uint32_t)cl_demux_eu_barrier(barrier_id);
	hal_write32((volatile void *)(base + CL_DEMUX_EU_HW_BARRIER_TRIGGER_MASK), core_mask);
	hal_write32((volatile void *)(base + CL_DEMUX_EU_HW_BARRIER_TARGET_MASK), core_mask);
}

static inline uint32_t hal_cl_eu_barrier_team_get(uint32_t barrier_id)
{
	uint32_t base = (uint32_t)cl_demux_eu_barrier(barrier_id);
	uint32_t team_mask =
		hal_read32((volatile void *)(base + CL_DEMUX_EU_HW_BARRIER_TRIGGER_MASK));
	return team_mask;
}

static inline uint32_t hal_cl_eu_barrier_status(uint32_t barrier_id)
{
	uint32_t base = (uint32_t)cl_demux_eu_barrier(barrier_id);
	return hal_eu_read32(base, CL_DEMUX_EU_HW_BARRIER_STATUS);
}

static inline void hal_cl_eu_barrier_trigger(uint32_t barrier_id, uint32_t core_mask)
{
	uint32_t base = (uint32_t)cl_demux_eu_barrier(barrier_id);
	hal_write32((volatile void *)(base + CL_DEMUX_EU_HW_BARRIER_TRIGGER), core_mask);
}

static inline uint32_t hal_cl_eu_barrier_trigger_self(uint32_t barrier_id)
{
	uint32_t base = (uint32_t)cl_demux_eu_barrier(barrier_id);
	return hal_eu_read32(base, CL_DEMUX_EU_HW_BARRIER_TRIGGER_SELF);
}

static inline uint32_t hal_cl_eu_barrier_trigger_wait(uint32_t barrier_id)
{
	uint32_t base = (uint32_t)cl_demux_eu_barrier(barrier_id);
	return hal_eu_read32(base, CL_DEMUX_EU_HW_BARRIER_TRIGGER_WAIT);
}

static inline uint32_t hal_cl_eu_barrier_trigger_wait_clear(uint32_t barrier_id)
{
	uint32_t base = (uint32_t)cl_demux_eu_barrier(barrier_id);
	return hal_eu_read32(base, CL_DEMUX_EU_HW_BARRIER_TRIGGER_WAIT_CLEAR);
}

#endif /* __ASSEMBLER__ */

#endif /* __PMSIS_IMPLEM_HAL_CLUSTER_CLUSTER_EVENT_UNIT_H__ */
