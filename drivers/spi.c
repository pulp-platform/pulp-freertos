/*
 * Copyright 2020 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include "pmsis_types.h"
#include "pmsis_task.h"

/* TODO: remove this hack */
#include "implementation_specific_defines.h"

#include "udma.h"
#include "fll.h"
#include "events.h"
#include "properties.h"
#include "spi_periph.h"
#include "spi.h"
#include "udma_spim.h"
#include "udma_ctrl.h"

#ifdef DEBUG
#define DEBUG_PRINTF printf
#define DBG_PRINTF   printf
#else
#define DEBUG_PRINTF(...) ((void)0)
#define DBG_PRINTF(...)	  ((void)0)
#endif /* DEBUG */

/* TODO: remove this glue */

#define UDMA_CORE_TX_CFG_EN(val)                                               \
	(((uint32_t)(((uint32_t)(val)) << UDMA_CORE_TX_CFG_EN_SHIFT)) &        \
	 UDMA_CORE_TX_CFG_EN_MASK)
#define UDMA_CORE_TX_CFG_DATASIZE(val)                                         \
	(((uint32_t)(((uint32_t)(val)) << UDMA_CORE_TX_CFG_DATASIZE_SHIFT)) &  \
	 UDMA_CORE_TX_CFG_DATASIZE_MASK)

#define SPIM_CS_DATA_GET_DRV_DATA(cs_data) (cs_data->drv_data)

struct spim_driver_data *__g_spim_drv_data[UDMA_NB_SPIM] = {0};

struct spim_drv_fifo {
	pi_task_t *fifo_head;
	pi_task_t *fifo_tail;
};

/* Structure holding infos for each chip selects (itf, cs, polarity etc...) */
struct spim_cs_data {
	struct spim_cs_data *next;
	struct spim_driver_data *drv_data;
	uint32_t cfg;
	uint32_t udma_cmd[8];
	uint32_t max_baudrate;
	uint32_t polarity;
	uint32_t phase;
	uint8_t cs;
	uint8_t wordsize;
	uint8_t big_endian;
};

/* Structure holding info for each interfaces
 * most notably the fifo of enqueued transfers and meta to know whether
 * interface is free or not */
struct spim_driver_data {
	struct spim_drv_fifo *drv_fifo;
	struct spim_cs_data *cs_list;
	pi_task_t *repeat_transfer;
	pi_task_t *end_of_transfer;
	uint32_t nb_open;
	uint8_t device_id;
};

struct spim_transfer {
	pi_spi_flags_e flags;
	void *data;
	uint32_t len;
	uint32_t cfg_cmd;
	uint32_t byte_align;
	uint32_t is_send;
};

void __spim_execute_callback(void *arg);
void spim_eot_handler(void *arg);
void spim_tx_handler(void *arg);
void spim_rx_handler(void *arg);
int __pi_spi_open(struct spim_cs_data **cs_data, struct pi_spi_conf *conf);
int __pi_spi_close(struct spim_cs_data *cs_data);

static int32_t __pi_spim_drv_fifo_enqueue(struct spim_cs_data *data,
					  struct spim_transfer *transfer,
					  pi_task_t *end_task);
static inline pi_task_t *__pi_spim_drv_fifo_pop(struct spim_driver_data *data);
static inline void __pi_spim_exec_transfer(pi_task_t *task);

void __pi_spi_send_async(struct spim_cs_data *cs_data, void *data, size_t len,
			 pi_spi_flags_e flags, pi_task_t *task);
void __pi_spi_receive_async(struct spim_cs_data *cs_data, void *data,
			    size_t len, pi_spi_flags_e flags, pi_task_t *task);
void __pi_spi_receive_async_with_ucode(struct spim_cs_data *cs_data, void *data,
				       size_t len, pi_spi_flags_e flags,
				       int ucode_size, void *ucode,
				       pi_task_t *task);
void __pi_spi_send_async_with_ucode(struct spim_cs_data *cs_data, void *data,
				    size_t len, pi_spi_flags_e flags,
				    int ucode_size, void *ucode,
				    pi_task_t *task);
void __pi_spi_xfer_async(struct spim_cs_data *cs_data, void *tx_data,
			 void *rx_data, size_t len, pi_spi_flags_e flags,
			 pi_task_t *task);

static inline uint32_t __pi_spi_get_config(struct spim_cs_data *cs_data)
{
	return cs_data->cfg;
}

static inline int32_t __pi_spim_drv_fifo_enqueue(struct spim_cs_data *cs_data,
						 struct spim_transfer *transfer,
						 pi_task_t *end_task)
{
	uint32_t irq = __disable_irq();
	struct spim_driver_data *drv_data = cs_data->drv_data;
	/* Callback args. */
	end_task->data[0] = (uintptr_t)cs_data;
	end_task->data[1] = (uintptr_t)transfer->data;
	end_task->data[2] = (uintptr_t)transfer->len;
	end_task->data[3] = (uintptr_t)transfer->flags;
	end_task->data[4] = (uintptr_t)end_task;
	end_task->data[5] = (uintptr_t)transfer->is_send;
	end_task->next = NULL;
	/* Enqueue transfer in drv fifo. */
	if (drv_data->drv_fifo->fifo_head == NULL) {
		/* Empty fifo. */
		drv_data->drv_fifo->fifo_head = end_task;
		drv_data->drv_fifo->fifo_tail = drv_data->drv_fifo->fifo_head;
	} else {
		/* Enqueue to tail. */
		drv_data->drv_fifo->fifo_tail->next = end_task;
		drv_data->drv_fifo->fifo_tail =
			drv_data->drv_fifo->fifo_tail->next;
	}
	__restore_irq(irq);
	return 0;
}

static inline pi_task_t *__pi_spim_drv_fifo_pop(struct spim_driver_data *data)
{

	pi_task_t *task_return = NULL;
	uint32_t irq = __disable_irq();
	if (data->drv_fifo->fifo_head != NULL) {
		task_return = data->drv_fifo->fifo_head;
		data->drv_fifo->fifo_head = data->drv_fifo->fifo_head->next;
		if (data->drv_fifo->fifo_head == NULL) {
			data->drv_fifo->fifo_tail = NULL;
		}
	}
	__restore_irq(irq);
	return task_return;
}

static inline struct spim_cs_data *
__pi_spim_get_cs_data(struct spim_driver_data *drv_data, int cs)
{
	struct spim_cs_data *cs_cur = drv_data->cs_list;
	while (cs_cur != NULL && cs_cur->cs != cs) {
		cs_cur = cs_cur->next;
	}
	return cs_cur;
}

static inline void __pi_spim_cs_data_del(struct spim_driver_data *drv_data,
					 int cs)
{
	struct spim_cs_data *cs_cur = drv_data->cs_list;
	struct spim_cs_data *cs_prev = cs_cur;
	while (cs_cur != NULL && cs_cur->cs != cs) {
		cs_prev = cs_cur;
		cs_cur = cs_cur->next;
	}
	if (cs_cur) {
		cs_prev->next = cs_cur->next;
		cs_cur->next = NULL;
	}
}
static inline void __pi_spim_cs_data_add(struct spim_driver_data *drv_data,
					 struct spim_cs_data *cs_data)
{
	// head insert, most recently allocated should be most recently used
	cs_data->drv_data = drv_data;
	cs_data->next = drv_data->cs_list;
	drv_data->cs_list = cs_data->next;
}

static uint32_t __pi_spi_clk_div_get(uint32_t spi_freq)
{
	uint32_t periph_freq = pi_freq_get(PI_FREQ_DOMAIN_PERIPH);
	if (spi_freq < periph_freq) {
		uint32_t clk_div = 0;
		clk_div = (periph_freq + spi_freq - 1) / spi_freq;
		if (clk_div & 1) {
			clk_div += 1;
		}
		/* The SPIM always divide by 2 once we activate the divider,
	   thus increase by 1 in case it is even to not go above the max
	   frequency. */
		clk_div = clk_div >> 1;
		return clk_div;
	}
	return 0;
}

void pi_spi_conf_init(struct pi_spi_conf *conf)
{
	conf->wordsize = PI_SPI_WORDSIZE_8;
	conf->big_endian = 0;
	conf->max_baudrate = 10000000;
	conf->cs = -1;
	conf->itf = 0;
	conf->polarity = 0;
	conf->phase = 0;
}

// TODO: prepare pseudo exec for delegate
void __pi_spim_execute_callback(void *arg)
{
	return;
}

void __pi_spim_exec_next_transfer(pi_task_t *task)
{
	if (task->data[5] == 1) { // if is send
		// cs data | data buffer | len | flags | end of transfer task
		__pi_spi_send_async((struct spim_cs_data *)task->data[0],
				    (void *)task->data[1], task->data[2],
				    task->data[3], (pi_task_t *)task->data[4]);
	} else if (task->data[5] == 0) {
		// cs data | data buffer | len | flags | end of transfer task
		__pi_spi_receive_async((struct spim_cs_data *)task->data[0],
				       (void *)task->data[1], task->data[2],
				       task->data[3],
				       (pi_task_t *)task->data[4]);
	} else { // task->data[5] contains rx data addr
		// cs data | tx buffer | rx buffer| len | flags | end of
		// transfer task
		__pi_spi_xfer_async((struct spim_cs_data *)task->data[0],
				    (void *)task->data[5],
				    (void *)task->data[1], task->data[2],
				    task->data[3], (pi_task_t *)task->data[4]);
	}
}

extern struct pmsis_event_kernel_wrap *default_sched;

void spim_eot_handler(void *arg)
{
	uint32_t event = (uint32_t)arg;
	uint32_t channel = event & 0x1;
	/* TODO: remove is garbage */
	// EOT is simply 22 + id in GAP8
	uint32_t periph_id = (event - SOC_EVENT_UDMA_SPIM_EOT(0));

	struct spim_driver_data *drv_data = __g_spim_drv_data[periph_id];

	if (drv_data->repeat_transfer) {
		DBG_PRINTF("%s:%d\n", __func__, __LINE__);
		// TODO: reenqueue the rest of the transfer
		DBG_PRINTF("Large transfers (>8k) are not implemented yet\n");
		return;
	}
	pi_task_t *task = drv_data->end_of_transfer;
	DBG_PRINTF("%s:%d\n", __func__, __LINE__);
	if (task != NULL) {
		if (task->id == PI_TASK_NONE_ID) {
			DBG_PRINTF("%s:%d release task %p\n", __func__,
				   __LINE__, task);
			pi_task_release(task);
		} else {
			/* TODO: hacked away */
			/* DBG_PRINTF("%s:%d push task %p with id:%x in
			 * sched%p\n", */
			/* 	   __func__, __LINE__, task, task->id, */
			/* 	   default_sched); */
			DBG_PRINTF("%s:%d periph id:%x\n", __func__, __LINE__,
				   periph_id);
			/* TODO: hacked away */
			/* pmsis_event_push(default_sched, task); */
			DBG_PRINTF("%s:%d\n", __func__, __LINE__);
		}
		drv_data->end_of_transfer = NULL;
	}
#ifdef DEBUG
	else {
		DBG_PRINTF("%s:%d null task %p\n", __func__, __LINE__, task);
	}
#endif
	task = __pi_spim_drv_fifo_pop(drv_data);
	if (task) {
		__pi_spim_exec_next_transfer(task);
	}
}

/* TODO: REMOVE THOSE */
void spim_tx_handler(void *arg)
{ // if we're here, it's cs keep related
	uint32_t event = (uint32_t)arg;
	uint32_t channel = event & 0x1;
	uint32_t periph_id =
		(event >> UDMA_CHANNEL_NB_EVENTS_LOG2) - UDMA_SPIM_ID(0);
	hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_SPIM_TX(periph_id));
	arg = (void *)(SOC_EVENT_UDMA_SPIM_EOT(0) + periph_id);
	DBG_PRINTF("%s:%d periph_id %x arg:%p\n", __func__, __LINE__, periph_id,
		   arg);
	spim_eot_handler(arg);
	DBG_PRINTF("%s:%d\n", __func__, __LINE__);
}

/* TODO: REMOVE THOSE and the handler */
void spim_rx_handler(void *arg)
{ // if we're here, it's cs keep related
	uint32_t event = (uint32_t)arg;
	uint32_t channel = event & 0x1;
	uint32_t periph_id =
		(event >> UDMA_CHANNEL_NB_EVENTS_LOG2) - UDMA_SPIM_ID(0);
	hal_soc_eu_clear_fc_mask(SOC_EVENT_UDMA_SPIM_RX(periph_id));
	arg = (void *)(SOC_EVENT_UDMA_SPIM_EOT(0) + periph_id);
	DBG_PRINTF("%s:%d periph_id %x arg:%p\n", __func__, __LINE__, periph_id,
		   arg);
	spim_eot_handler(arg);
	DBG_PRINTF("%s:%d\n", __func__, __LINE__);
}


void __pi_spi_receive_async(struct spim_cs_data *cs_data, void *data,
			    size_t len, pi_spi_flags_e flags, pi_task_t *task)
{
	struct spim_driver_data *drv_data = SPIM_CS_DATA_GET_DRV_DATA(cs_data);
	int qspi = (flags & (0x3 << 2)) == PI_SPI_LINES_QUAD;
	int cs_mode = (flags >> 0) & 0x3;

	int device_id = drv_data->device_id;
	uint32_t cfg = cs_data->cfg;
	DBG_PRINTF(
		"%s:%d: core clock:%d, baudrate:%d, div=%d, udma_cmd cfg =%lx, qpi=%lx\n",
		__func__, __LINE__, system_core_clock_get(),
		cs_data->max_baudrate,
		system_core_clock_get() / cs_data->max_baudrate, cfg, qspi);
	uint32_t byte_align = (cs_data->wordsize == PI_SPI_WORDSIZE_32) &&
			      cs_data->big_endian;
	int buffer_size = (len + 7) >> 3;

	if (len > 8192 * 8) {
		DBG_PRINTF(
			"%s:%d: Transaction splitting unimplemented, too large",
			__func__, __LINE__);
		abort(); /* TODO: unimplemented transaction splitting */
	}

	int irq = __disable_irq();

	if (!drv_data->end_of_transfer) {
		cs_data->udma_cmd[0] = cfg;
		cs_data->udma_cmd[1] = SPI_CMD_SOT(cs_data->cs);
		cs_data->udma_cmd[2] =
			SPI_CMD_RX_DATA(len / 32, SPI_CMD_1_WORD_PER_TRANSF, 32,
					qspi, SPI_CMD_MSB_FIRST);
		cs_data->udma_cmd[3] =
			SPI_CMD_EOT(1, cs_mode == PI_SPI_CS_KEEP);
		drv_data->end_of_transfer = task;
		drv_data->repeat_transfer = NULL;

		uint32_t rx_conf =
			UDMA_CORE_TX_CFG_EN(1) |
			UDMA_CORE_TX_CFG_DATASIZE(UDMA_CORE_CFG_DATASIZE_32);

		/* receive data stream with 32-bit data size */
		spim_enqueue_channel(SPIM(device_id), (uint32_t)data,
				     buffer_size, rx_conf, RX_CHANNEL);

		uint32_t cmd_conf =
			UDMA_CORE_TX_CFG_EN(1) |
			UDMA_CORE_TX_CFG_DATASIZE(UDMA_CORE_CFG_DATASIZE_32);
		/* send command stream with 32-bit data size */
		spim_enqueue_channel(SPIM(device_id),
				     (uint32_t)cs_data->udma_cmd,
				     4 * sizeof(uint32_t), cmd_conf,
				     COMMAND_CHANNEL);
	} else {
		struct spim_transfer transfer;
		transfer.data = data;
		transfer.flags = flags;
		transfer.len = len;
		transfer.cfg_cmd = cfg;
		transfer.byte_align = byte_align;
		transfer.is_send = 0;
		__pi_spim_drv_fifo_enqueue(cs_data, &transfer, task);
	}
	restore_irq(irq);
}

void __pi_spi_receive_async_with_ucode(struct spim_cs_data *cs_data, void *data,
				       size_t len, pi_spi_flags_e flags,
				       int ucode_size, void *ucode,
				       pi_task_t *task)
{
	/* TODO: port spi_async with ucode */
	abort();
#if 0
    struct spim_driver_data *drv_data = SPIM_CS_DATA_GET_DRV_DATA(cs_data);
    int qspi    = ((flags >> 2) & 0x3) == ((PI_SPI_LINES_QUAD>>2) & 0x03);
    int cs_mode = (flags >> 0) & 0x3;

    int device_id = drv_data->device_id;
    uint32_t byte_align = (cs_data->wordsize == PI_SPI_WORDSIZE_32)
        && cs_data->big_endian;
    uint32_t cfg = cs_data->cfg;
    DBG_PRINTF("%s:%d: core clock:%d, baudrate:%d, div=%d,  byte_align =%lx, cfg= %lx, qspi=%lx\n",
            __func__,__LINE__,system_core_clock_get(),cs_data->max_baudrate,
            system_core_clock_get() / cs_data->max_baudrate,byte_align,cfg,qspi);
    int size = (len + 7) >> 3;

    int cmd_id = 0;

    int irq = __disable_irq();
    if(!drv_data->end_of_transfer)
    {
        if(cs_mode != PI_SPI_CS_AUTO)
        {
            hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_SPIM_RX(device_id));
        }
        drv_data->end_of_transfer = task;
        drv_data->repeat_transfer = NULL;
        if(((0xFFF00000 & (uint32_t)ucode)!= 0x1c000000))
        {
            memcpy(&(cs_data->udma_cmd[0]), ucode, ucode_size);
            spim_enqueue_channel(SPIM(device_id), (uint32_t)data, size,
                    UDMA_CORE_RX_CFG_EN(1) | (2<<1), RX_CHANNEL);
            spim_enqueue_channel(SPIM(device_id), (uint32_t)cs_data->udma_cmd,
                    ucode_size, UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
        }
        else
        {
            spim_enqueue_channel(SPIM(device_id), (uint32_t)data, size,
                    UDMA_CORE_RX_CFG_EN(1) | (2<<1), RX_CHANNEL);
            spim_enqueue_channel(SPIM(device_id), (uint32_t)ucode,
                    ucode_size, UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
        }
    }
    else
    {
#if 0
        struct spim_transfer transfer;
        transfer.data = data;
        transfer.flags = flags;
        transfer.len = len;
        transfer.cfg_cmd = cfg;
        transfer.byte_align = byte_align;
        transfer.is_send = 0;
        __pi_spim_drv_fifo_enqueue(cs_data, &transfer, task);
#endif
    }
    restore_irq(irq);
#endif
}

void __pi_spi_send_async_with_ucode(struct spim_cs_data *cs_data, void *data,
				    size_t len, pi_spi_flags_e flags,
				    int ucode_size, void *ucode,
				    pi_task_t *task)
{
	/* TODO: port spi_async with ucode */
	abort();
#if 0
    struct spim_driver_data *drv_data = SPIM_CS_DATA_GET_DRV_DATA(cs_data);
    int qspi    = ((flags >> 2) & 0x3) == ((PI_SPI_LINES_QUAD>>2) & 0x03);
    int cs_mode = (flags >> 0) & 0x3;

    int device_id = drv_data->device_id;
    uint32_t byte_align = (cs_data->wordsize == PI_SPI_WORDSIZE_32)
        && cs_data->big_endian;
    uint32_t cfg = cs_data->cfg;
    DBG_PRINTF("%s:%d: core clock:%d, baudrate:%d, div=%d,  byte_align =%lx, cfg= %lx, qspi=%lx\n",
            __func__,__LINE__,system_core_clock_get(),cs_data->max_baudrate,
            system_core_clock_get() / cs_data->max_baudrate,byte_align,cfg,qspi);
    int size = (len + 7) >> 3;

    int cmd_id = 0;

    int irq = __disable_irq();
    if(!drv_data->end_of_transfer)
    {
        if(cs_mode != PI_SPI_CS_AUTO)
        {
            hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_SPIM_TX(device_id));
        }
        drv_data->end_of_transfer = task;
        drv_data->repeat_transfer = NULL;

        spim_enqueue_channel(SPIM(device_id), (uint32_t)ucode,
                ucode_size, UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
        pi_time_wait_us(1000);
        spim_enqueue_channel(SPIM(device_id), (uint32_t)data,
                size, UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
        if(cs_mode == PI_SPI_CS_AUTO)
        {
            // wait until channel is free
            while((hal_read32((void*)&(SPIM(device_id)->udma.tx_cfg))
                        & (1<<5))>>5)
            {
                DBG_PRINTF("%s:%d\n",__func__,__LINE__);
            }

            // enqueue EOT
            cs_data->udma_cmd[0] = SPI_CMD_EOT(1);
            spim_enqueue_channel(SPIM(device_id),
                    (uint32_t)&cs_data->udma_cmd[0], 1*(sizeof(uint32_t)),
                    UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
        }
    }
    else
    {
#if 0
        struct spim_transfer transfer;
        transfer.data = data;
        transfer.flags = flags;
        transfer.len = len;
        transfer.cfg_cmd = cfg;
        transfer.byte_align = byte_align;
        transfer.is_send = 0;
        __pi_spim_drv_fifo_enqueue(cs_data, &transfer, task);
#endif
    }
    restore_irq(irq);
#endif
}

void __pi_spi_send_async(struct spim_cs_data *cs_data, void *data, size_t len,
			 pi_spi_flags_e flags, pi_task_t *task)
{
	struct spim_driver_data *drv_data = SPIM_CS_DATA_GET_DRV_DATA(cs_data);
	int qspi = (flags & (0x3 << 2)) == PI_SPI_LINES_QUAD;
	int cs_mode = (flags >> 0) & 0x3;

	int device_id = drv_data->device_id;
	uint32_t cfg = cs_data->cfg;
	DBG_PRINTF(
		"%s:%d: core clock:%d, baudrate:%d, div=%d, udma_cmd cfg =%lx, qpi=%lx\n",
		__func__, __LINE__, system_core_clock_get(),
		cs_data->max_baudrate,
		system_core_clock_get() / cs_data->max_baudrate, cfg, qspi);
	uint32_t byte_align = (cs_data->wordsize == PI_SPI_WORDSIZE_32) &&
			      cs_data->big_endian;

	/* buffer size */
	int buffer_size = (len + 7) >> 3;

	if (len > 8192 * 8) {
		DBG_PRINTF(
			"%s:%d: Transaction splitting unimplemented, too large",
			__func__, __LINE__);
		abort(); /* TODO: unimplemented transaction splitting */
	}

	DBG_PRINTF("%s:%d: udma_cmd=%p\n", __func__, __LINE__,
		   &(cs_data->udma_cmd[0]));
	int irq = disable_irq();
	/* check if we already have a transfer ongoing */
	if (!drv_data->end_of_transfer) { /* enqueue the transfer */
		cs_data->udma_cmd[0] = cfg;
		cs_data->udma_cmd[1] = SPI_CMD_SOT(cs_data->cs);
		cs_data->udma_cmd[2] =
			SPI_CMD_TX_DATA(len / 32, SPI_CMD_1_WORD_PER_TRANSF, 32,
					qspi, SPI_CMD_MSB_FIRST);
		cs_data->udma_cmd[3] =
			SPI_CMD_EOT(1, cs_mode == PI_SPI_CS_KEEP);
		DBG_PRINTF("%s:%d\n", __func__, __LINE__);
		drv_data->end_of_transfer = task;
		drv_data->repeat_transfer = NULL;

		/* First enqueue the header with SPI config, cs, and send
		 * command. The rest will be sent by the assembly code. First
		 * the user data and finally an epilogue with the EOT command.
		 */
		uint32_t cmd_conf =
			UDMA_CORE_TX_CFG_EN(1) |
			UDMA_CORE_TX_CFG_DATASIZE(UDMA_CORE_CFG_DATASIZE_32);
		/* send command stream with 32-bit data size */
		spim_enqueue_channel(SPIM(device_id),
				     (uint32_t)cs_data->udma_cmd,
				     4 * sizeof(uint32_t), cmd_conf,
				     COMMAND_CHANNEL);

		uint32_t tx_conf =
			UDMA_CORE_TX_CFG_EN(1) |
			UDMA_CORE_TX_CFG_DATASIZE(UDMA_CORE_CFG_DATASIZE_32);

		/* send data stream with 32-bit data size */
		spim_enqueue_channel(SPIM(device_id), (uint32_t)data,
				     buffer_size, tx_conf, TX_CHANNEL);
	} else { /* a transfer is running, append to pending transfers queue */
		struct spim_transfer transfer;
		transfer.data = data;
		transfer.flags = flags;
		transfer.len = len;
		transfer.cfg_cmd = cfg;
		transfer.byte_align = byte_align;
		transfer.is_send = 1;
		__pi_spim_drv_fifo_enqueue(cs_data, &transfer, task);
	}
	__restore_irq(irq);
}

void __pi_spi_xfer_async(struct spim_cs_data *cs_data, void *tx_data,
			 void *rx_data, size_t len, pi_spi_flags_e flags,
			 pi_task_t *task)
{
	/* TODO: port spi_xfer async */
	abort();
#if 0
    struct spim_driver_data *drv_data = SPIM_CS_DATA_GET_DRV_DATA(cs_data);
    int qspi    = (flags & (0x3 << 2)) == PI_SPI_LINES_QUAD;
    int cs_mode = (flags >> 0) & 0x3;

    int device_id = drv_data->device_id;
    uint32_t cfg = cs_data->cfg;
    DBG_PRINTF("%s:%d: core clock:%d, baudrate:%d, div=%d, udma_cmd cfg =%lx\n",
            __func__,__LINE__,system_core_clock_get(),cs_data->max_baudrate,
            system_core_clock_get() / cs_data->max_baudrate,cfg);
    uint32_t byte_align = (cs_data->wordsize == PI_SPI_WORDSIZE_32)
        && cs_data->big_endian;
    int size = (len + 7) >> 3;

    int cmd_id = 0;

    int irq = __disable_irq();
    if(!drv_data->end_of_transfer)
    {
        cs_data->udma_cmd[0] = cfg;
        cs_data->udma_cmd[1] = SPI_CMD_SOT(cs_data->cs);
        cs_data->udma_cmd[2] = SPI_CMD_FULL_DUPL(len, byte_align);
        drv_data->end_of_transfer = task;
        drv_data->repeat_transfer = NULL;
        if(cs_mode == PI_SPI_CS_AUTO)
        {
            spim_enqueue_channel(SPIM(device_id), (uint32_t)cs_data->udma_cmd,
                    3*(sizeof(uint32_t)), UDMA_CORE_TX_CFG_EN(1),
                    TX_CHANNEL);
            spim_enqueue_channel(SPIM(device_id), (uint32_t)rx_data, size,
                    UDMA_CORE_RX_CFG_EN(1) | (2<<1), RX_CHANNEL);
            spim_enqueue_channel(SPIM(device_id), (uint32_t)tx_data,
                    size, UDMA_CORE_TX_CFG_EN(1),
                    TX_CHANNEL);
            // wait until TX channel is free
            while((hal_read32((void*)&(SPIM(device_id)->udma.tx_cfg))
                        & (1<<5))>>5)
            {
                DBG_PRINTF("%s:%d\n",__func__,__LINE__);
            }
            // send EOT
            cs_data->udma_cmd[3] = SPI_CMD_EOT(1);
            spim_enqueue_channel(SPIM(device_id),
                    (uint32_t)&cs_data->udma_cmd[3], sizeof(uint32_t),
                    UDMA_CORE_TX_CFG_EN(1), TX_CHANNEL);
        }
        else
        {
            spim_enqueue_channel(SPIM(device_id), (uint32_t)cs_data->udma_cmd,
                    3*(sizeof(uint32_t)), UDMA_CORE_TX_CFG_EN(1),
                    TX_CHANNEL);
            // wait until TX channel is free
            while((hal_read32((void*)&(SPIM(device_id)->udma.tx_cfg))
                        & (1<<5))>>5)
            {
                DBG_PRINTF("%s:%d\n",__func__,__LINE__);
            }
            // activate rx event
            hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_SPIM_RX(device_id));
            // NOTE: both transfers have the same size
            // does not matter which one we wait
            spim_enqueue_channel(SPIM(device_id), (uint32_t)rx_data, size,
                    UDMA_CORE_RX_CFG_EN(1) | (2<<1), RX_CHANNEL);
            spim_enqueue_channel(SPIM(device_id), (uint32_t)tx_data,
                    size, UDMA_CORE_TX_CFG_EN(1),
                    TX_CHANNEL);
        }

    }
    else
    {
        struct spim_transfer transfer;
        transfer.data = rx_data;
        transfer.flags = flags;
        transfer.len = len;
        transfer.cfg_cmd = cfg;
        transfer.byte_align = byte_align;
        transfer.is_send = (uint32_t) tx_data; // sending a pointer means xfer
        __pi_spim_drv_fifo_enqueue(cs_data, &transfer, task);
    }
    restore_irq(irq);
#endif
}

int pi_spi_open(struct pi_device *device)
{
	struct pi_spi_conf *conf = (struct pi_spi_conf *)device->config;
	int irq = __disable_irq();
	/* int status = __pi_spi_open((struct spim_cs_data **)(&device->data),
	 * conf); */

	/* TODO: hacked */
	int status = 0;
	struct spim_cs_data **cs_data = (struct spim_cs_data **)(&device->data);
	/* struct pi_spi_conf *conf = conf; */
	/* TODO: paste beg */
	// disable clock gating for said device
	udma_ctrl_cg_disable(UDMA_SPIM_ID(conf->itf));
	hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_SPIM_EOT(conf->itf));
	pi_fc_event_handler_set(SOC_EVENT_UDMA_SPIM_EOT(conf->itf),
				spim_eot_handler);
	pi_fc_event_handler_set(SOC_EVENT_UDMA_SPIM_TX(conf->itf),
				spim_tx_handler);
	pi_fc_event_handler_set(SOC_EVENT_UDMA_SPIM_RX(conf->itf),
				spim_rx_handler);

	// Take care of driver data
	struct spim_driver_data *drv_data;
	if (__g_spim_drv_data[conf->itf]) {
		drv_data = __g_spim_drv_data[conf->itf];
	} else {
		__g_spim_drv_data[conf->itf] =
			pi_default_malloc(sizeof(struct spim_driver_data));
		memset(__g_spim_drv_data[conf->itf], 0,
		       sizeof(struct spim_driver_data));
		drv_data = __g_spim_drv_data[conf->itf];
		drv_data->drv_fifo =
			pi_default_malloc(sizeof(struct spim_drv_fifo));
		memset(drv_data->drv_fifo, 0, sizeof(struct spim_drv_fifo));
		if (!drv_data->drv_fifo) {
			restore_irq(irq);
			return -1;
		}
		drv_data->device_id = conf->itf;
	}
	drv_data->nb_open++;

	// Take care of cs data
	*cs_data = __pi_spim_get_cs_data(drv_data, conf->cs);
	if (!*cs_data) {
		uint32_t clk_div = __pi_spi_clk_div_get(conf->max_baudrate);
		// alloc a cs data, need to be in udma reachable ram
		struct spim_cs_data *_cs_data =
			pi_data_malloc(sizeof(struct spim_cs_data));
		if (_cs_data == NULL) {
			DBG_PRINTF("[%s] _cs_data alloc failed\n", __func__);
			restore_irq(irq);
			return -2;
		}
		if (clk_div > 0xFF) {
			DBG_PRINTF(
				"[%s] clk_div, %x, does not fit into 8 bits. SoC frequency too high.\n",
				__func__, clk_div);
			restore_irq(irq);
			return -3;
		}
		memset(_cs_data, 0, sizeof(struct spim_cs_data));
		_cs_data->max_baudrate = conf->max_baudrate;
		_cs_data->polarity = conf->polarity;
		_cs_data->phase = conf->phase;
		_cs_data->big_endian = conf->big_endian;
		_cs_data->wordsize = conf->wordsize;
		_cs_data->cs = conf->cs;
		_cs_data->cfg = SPI_CMD_CFG(clk_div, _cs_data->phase,
					    _cs_data->polarity);
		*cs_data = _cs_data;
		__pi_spim_cs_data_add(drv_data, _cs_data);
	}
	DBG_PRINTF("%s:%d\n", __func__, __LINE__);

	/* TODO: paste end */

	restore_irq(irq);
	return status;
}

void pi_spi_close(struct pi_device *device)
{
	struct pi_spi_conf *conf = (struct pi_spi_conf *)device->config;
	int irq = __disable_irq();
	/* TODO: paste beg */
	struct spim_cs_data *cs_data = device->data;
	struct spim_driver_data *drv_data = cs_data->drv_data;
	__pi_spim_cs_data_del(drv_data, cs_data->cs);
	drv_data->nb_open--;
	if (drv_data->nb_open == 0) {
		/* reactivate clock gating for said device */
		udma_ctrl_cg_enable(UDMA_SPIM_ID(drv_data->device_id));
		hal_soc_eu_clear_fc_mask(
			SOC_EVENT_UDMA_SPIM_EOT(drv_data->device_id));
		pi_fc_event_handler_clear(
			SOC_EVENT_UDMA_SPIM_EOT(drv_data->device_id));
		__g_spim_drv_data[drv_data->device_id] = NULL;
		pi_default_free(drv_data->drv_fifo, sizeof(drv_data->drv_fifo));
		pi_default_free(drv_data, sizeof(drv_data));

		restore_irq(irq);
		return 0;
	}
	pi_data_free(cs_data, sizeof(cs_data));
	/* TODO: moved to end return drv_data->nb_open; */
	/* TODO: paste end */

	restore_irq(irq);
	return drv_data->nb_open;
}

void pi_spi_ioctl(struct pi_device *device, uint32_t cmd, void *arg)
{
	/* TODO */
}

void pi_spi_send(struct pi_device *device, void *data, size_t len,
		 pi_spi_flags_e flag)
{
	pi_task_t task_block;
	pi_task_block(&task_block);
	DEBUG_PRINTF("%s:%d\n", __func__, __LINE__);
	pi_spi_send_async(device, data, len, flag, &task_block);
	DEBUG_PRINTF("%s:%d\n", __func__, __LINE__);
	pi_task_wait_on(&task_block);
	DEBUG_PRINTF("%s:%d\n", __func__, __LINE__);
	pi_task_destroy(&task_block);
	DEBUG_PRINTF("%s:%d\n", __func__, __LINE__);
}

void pi_spi_send_async(struct pi_device *device, void *data, size_t len,
		       pi_spi_flags_e flag, pi_task_t *task)
{
	__pi_spi_send_async(device->data, data, len, flag, task);
}

void pi_spi_receive(struct pi_device *device, void *data, size_t len,
		    pi_spi_flags_e flag)
{
	pi_task_t task_block;
	pi_task_block(&task_block);
	DEBUG_PRINTF("%s:%d\n", __func__, __LINE__);
	pi_spi_receive_async(device, data, len, flag, &task_block);
	DEBUG_PRINTF("%s:%d\n", __func__, __LINE__);
	pi_task_wait_on(&task_block);
	DEBUG_PRINTF("%s:%d\n", __func__, __LINE__);
	pi_task_destroy(&task_block);
	DEBUG_PRINTF("%s:%d\n", __func__, __LINE__);
}

void pi_spi_receive_async(struct pi_device *device, void *data, size_t len,
			  pi_spi_flags_e flag, pi_task_t *task)
{
	__pi_spi_receive_async(device->data, data, len, flag, task);
}

void pi_spi_receive_with_ucode(struct pi_device *device, void *data, size_t len,
			       pi_spi_flags_e flags, int ucode_size,
			       void *ucode)
{
	pi_task_t task_block;
	pi_task_block(&task_block);
	__pi_spi_receive_async_with_ucode(device->data, data, len, flags,
					  ucode_size, ucode, &task_block);
	pi_task_wait_on(&task_block);
	pi_task_destroy(&task_block);
}

void pi_spi_send_with_ucode(struct pi_device *device, void *data, size_t len,
			    pi_spi_flags_e flags, int ucode_size, void *ucode)
{
	pi_task_t task_block;
	pi_task_block(&task_block);
	__pi_spi_send_async_with_ucode(device->data, data, len, flags,
				       ucode_size, ucode, &task_block);
	pi_task_wait_on(&task_block);
	pi_task_destroy(&task_block);
}

uint32_t pi_spi_get_config(struct pi_device *device)
{
	return __pi_spi_get_config(device->data);
}

void pi_spi_transfer(struct pi_device *device, void *tx_data, void *rx_data,
		     size_t len, pi_spi_flags_e flag)
{
	/* TODO */
	pi_task_t task_block;
	pi_task_block(&task_block);
	DEBUG_PRINTF("%s:%d\n", __func__, __LINE__);
	pi_spi_transfer_async(device, tx_data, rx_data, len, flag, &task_block);
	DEBUG_PRINTF("%s:%d\n", __func__, __LINE__);
	pi_task_wait_on(&task_block);
	DEBUG_PRINTF("%s:%d\n", __func__, __LINE__);
	pi_task_destroy(&task_block);
	DEBUG_PRINTF("%s:%d\n", __func__, __LINE__);
}

void pi_spi_transfer_async(struct pi_device *device, void *tx_data,
			   void *rx_data, size_t len, pi_spi_flags_e flag,
			   pi_task_t *task)
{
	/* TODO */
	__pi_spi_xfer_async(device->data, tx_data, rx_data, len, flag, task);
}
