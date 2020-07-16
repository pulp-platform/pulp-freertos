#ifndef __PI_HAL_UDMA_CORE_H__
#define __PI_HAL_UDMA_CORE_H__

/**
 * Small low level driver for udma core main functionnalities
 */

#include "pmsis/targets/target.h"
#include "pmsis/implem/hal/udma/udma_ctrl.h"

#define UDMA_MAX_SIZE_LOG2    (17)
#define UDMA_MAX_SIZE         (1 << UDMA_MAX_SIZE_LOG2)


static inline void udma_init_device(uint32_t device_id)
{
	/* disable clock gating for device with device_id */
	udma_ctrl_cg_disable(device_id);
}

/*
 * Common uDMA channel enqueue
 */
static inline void udma_enqueue_channel(udma_channel_t *chan, uint32_t addr,
        uint32_t size, uint32_t config)
{
	hal_write32(&chan->saddr, addr);
	hal_write32(&chan->size, size);
	hal_write32(&chan->cfg, config);
}

static inline void udma_channel_clear(udma_channel_t *chan)
{
	/* TODO: adjust macro */
        hal_write32(&(chan->cfg), UDMA_CORE_RX_CFG_CLR(1));
}

static inline void udma_deinit_device(uint32_t device_id)
{
	/* enable clock gating for device with device_id */
	udma_ctrl_cg_enable(device_id);
}

#endif  /* __PI_HAL_UDMA_CORE_H__ */
