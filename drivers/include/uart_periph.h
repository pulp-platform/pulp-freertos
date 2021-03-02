/* THIS FILE HAS BEEN GENERATED, DO NOT MODIFY IT. */
/*
 * Copyright (C) 2019 ETH Zurich, University of Bologna
 * and GreenWaves Technologies
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
 */

#ifndef __UART_PERIPH_H__
#define __UART_PERIPH_H__

#include "udma.h"

/** UART_Type Register Layout Typedef */
typedef struct {
	udma_channel_t rx;	  /**< UDMA RX channels struct. */
	udma_channel_t tx;	  /**< UDMA TX channels struct. */
	volatile uint32_t status; /**< Status register */
	volatile uint32_t setup;  /**< Configuration register */
} uart_t;


/*! @name STATUS */
/* TX busy status flag */
#define UART_STATUS_TX_BUSY_MASK  (0x1)
#define UART_STATUS_TX_BUSY_SHIFT (0)

/* RX busy status flag */
#define UART_STATUS_RX_BUSY_MASK  (0x2)
#define UART_STATUS_RX_BUSY_SHIFT (1)

/* RX parity error status flag */
#define UART_STATUS_RX_PE_MASK	(0x4)
#define UART_STATUS_RX_PE_SHIFT (2)


/*! @name SETUP */
/* Set parity generation and check:
  - 1'b0: disable
  - 1'b1: enable */
#define UART_SETUP_PARITY_ENA_MASK  (0x1)
#define UART_SETUP_PARITY_ENA_SHIFT (0)

/* Set character length:
  - 2'b00: 5 bits
  - 2'b01: 6 bits
  - 2'b10: 7 bits
  - 2'b11: 8 bits */
#define UART_SETUP_BIT_LENGTH_MASK  (0x6)
#define UART_SETUP_BIT_LENGTH_SHIFT (1)

/* Set stop bits length:
  - 2'b0: 1 stop bit
  - 2'b1: 2 stop bits */
#define UART_SETUP_STOP_BITS_MASK  (0x8)
#define UART_SETUP_STOP_BITS_SHIFT (3)

/* Set TX transceiver state:
  - 1'b0: disable
  - 1'b1: enable */
#define UART_SETUP_TX_ENA_MASK	(0x100)
#define UART_SETUP_TX_ENA_SHIFT (8)

/* Set RX transceiver state:
  - 1'b0: disable
  - 1'b1: enable */
#define UART_SETUP_RX_ENA_MASK	(0x200)
#define UART_SETUP_RX_ENA_SHIFT (9)

/* Sets the clock divider ratio for the baud rate generator. */
#define UART_SETUP_CLKDIV_MASK	(0xffff0000)
#define UART_SETUP_CLKDIV_SHIFT (16)


#define uart(id) ((uart_t *)UDMA_UART(id))

#endif /* __UART_PERIPH_H__ */
