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

#ifndef _PMSIS_UDMA_UART_H_
#define _PMSIS_UDMA_UART_H_

#include "pmsis/targets/target.h"
#include "pmsis/implem/hal/udma/udma_core.h"
#include "pmsis/implem/hal/udma/udma_ctrl.h"


/*! UART udma configuration. */
static inline void uart_udma_channel_set(uint32_t device_id, uint32_t l2_buf,
                                         uint32_t size, uint32_t cfg,
                                         udma_channel_e channel)
{
	switch(channel)
	{
        case(RX_CHANNEL):
		udma_enqueue_channel(&(uart(device_id)->rx), l2_buf, size, cfg);
		break;
        case(TX_CHANNEL):
		udma_enqueue_channel(&(uart(device_id)->tx), l2_buf, size, cfg);
		break;
        default:
		break;
	}
}

/*! Status Register. */
static inline uint32_t uart_status_get(uint32_t device_id)
{
    return hal_read32(&(uart(device_id)->status));
}


/*! Setup Register. */
static inline void uart_setup_set(uint32_t device_id, uint32_t setup)
{
    hal_write32(&(uart(device_id)->setup), setup);
}

static inline uint32_t uart_setup_get(uint32_t device_id)
{
    return hal_read32(&(uart(device_id)->setup));
}

/*! SETUP. */
/* Parity setup. */
static inline void hal_uart_parity_enable(uint32_t device_id)
{
    uart_setup_set(device_id, (uart_setup_get(device_id) | UART_SETUP_PARITY_ENA(1)));
}

static inline void hal_uart_parity_disable(uint32_t device_id)
{
    uart_setup_set(device_id, (uart_setup_get(device_id) & ~UART_SETUP_PARITY_ENA(1)));
}

/* Word size. */
static inline void hal_uart_bit_length_set(uint32_t device_id, uint8_t bit_length)
{
    uart_setup_set(device_id, (uart_setup_get(device_id) & ~UART_SETUP_BIT_LENGTH_MASK)
                              | UART_SETUP_BIT_LENGTH(bit_length));
}

/* Stop bits. */
static inline void hal_uart_stop_bits_set(uint32_t device_id, uint8_t stop_bits)
{
    uart_setup_set(device_id, (uart_setup_get(device_id) & ~UART_SETUP_STOP_BITS_MASK)
                              | UART_SETUP_STOP_BITS(stop_bits));
}

/* TX enable. */
static inline void hal_uart_tx_enable(uint32_t device_id)
{
    uart_setup_set(device_id, (uart_setup_get(device_id) | UART_SETUP_TX_ENA(1)));
}

static inline void hal_uart_tx_disable(uint32_t device_id)
{
    uart_setup_set(device_id, (uart_setup_get(device_id) & ~UART_SETUP_TX_ENA(1)));
}

/* RX enable. */
static inline void hal_uart_rx_enable(uint32_t device_id)
{
    uart_setup_set(device_id, (uart_setup_get(device_id) | UART_SETUP_RX_ENA(1)));
}

static inline void hal_uart_rx_disable(uint32_t device_id)
{
    uart_setup_set(device_id, (uart_setup_get(device_id) & ~UART_SETUP_RX_ENA(1)));
}

/* Clock divider setup. */
static inline void hal_uart_clkdiv_set(uint32_t device_id, uint16_t clk_div)
{
    uart_setup_set(device_id, (uart_setup_get(device_id) & ~UART_SETUP_CLKDIV_MASK)
                              | UART_SETUP_CLKDIV(clk_div));
}

/* Setup UART. */
static inline void hal_uart_setup_set(uint32_t device_id, uint16_t clk_div,
                                      uint8_t rx_ena, uint8_t tx_ena,
                                      uint8_t stop_bits, uint8_t bit_length,
                                      uint8_t parity_ena)
{
    uint32_t setup = UART_SETUP_CLKDIV(clk_div) |
                     UART_SETUP_RX_ENA(rx_ena) |
                     UART_SETUP_TX_ENA(tx_ena) |
                     UART_SETUP_STOP_BITS(stop_bits) |
                     UART_SETUP_BIT_LENGTH(bit_length) |
                     UART_SETUP_PARITY_ENA(parity_ena);
    uart_setup_set(device_id, setup);
}


/*! STATUS.  */
static inline uint32_t hal_uart_tx_status_get(uint32_t device_id)
{
    return (uart_status_get(device_id) & UART_STATUS_TX_BUSY_MASK);
}

static inline uint32_t hal_uart_rx_status_get(uint32_t device_id)
{
    return (uart_status_get(device_id) & UART_STATUS_RX_BUSY_MASK);
}

static inline uint32_t hal_uart_rx_parity_error_get(uint32_t device_id)
{
    return (uart_status_get(device_id) & UART_STATUS_RX_PE_MASK);
}


/*! UDMA. */
static inline void hal_uart_enqueue(uint32_t device_id, uint32_t l2_buf,
                                    uint32_t size, uint32_t cfg, udma_channel_e channel)
{
    cfg |= UDMA_CORE_RX_CFG_EN(1);
    uart_udma_channel_set(device_id, l2_buf, size, cfg, channel);
}

static inline void hal_uart_rx_clear(uint32_t device_id)
{
    udma_channel_clear(&(uart(device_id)->rx));
}

static inline void hal_uart_tx_clear(uint32_t device_id)
{
    udma_channel_clear(&(uart(device_id)->tx));
}

#endif
