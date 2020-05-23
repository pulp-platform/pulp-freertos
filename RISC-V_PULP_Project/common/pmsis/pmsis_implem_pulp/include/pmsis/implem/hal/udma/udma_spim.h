/**
 * Defines/types and small low level driver for udma spim main functionnalities
 */

#ifndef __UDMA_SPIM_H__
#define __UDMA_SPIM_H__

#include "pmsis/targets/target.h"

#define SPIM(id) ((spi_t *) UDMA_SPIM(id))

static inline void spim_enqueue_channel(spi_t *spim, uint32_t addr,
        uint32_t size, uint32_t config, udma_channel_e channel)
{
	switch(channel)
	{
        case(RX_CHANNEL):
		udma_enqueue_channel(&(spim->rx), addr, size, config);
		break;
        case(TX_CHANNEL):
		udma_enqueue_channel(&(spim->tx), addr, size, config);
		break;
        case(COMMAND_CHANNEL):
		udma_enqueue_channel(&(spim->cmd), addr, size, config);
		break;
        default:
		break;
	}
}

#endif
