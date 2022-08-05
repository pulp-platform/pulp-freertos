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
#define UART_STATUS_TX_BUSY_MASK  (0x1ul)
#define UART_STATUS_TX_BUSY_SHIFT (0ul)

/* RX busy status flag */
#define UART_STATUS_RX_BUSY_MASK  (0x2ul)
#define UART_STATUS_RX_BUSY_SHIFT (1ul)

/* RX parity error status flag */
#define UART_STATUS_RX_PE_MASK	(0x4ul)
#define UART_STATUS_RX_PE_SHIFT (2ul)


/*! @name SETUP */
/* Set parity generation and check:
  - 1'b0: disable
  - 1'b1: enable */
#define UART_SETUP_PARITY_ENA_MASK  (0x1ul)
#define UART_SETUP_PARITY_ENA_SHIFT (0ul)

/* Set character length:
  - 2'b00: 5 bits
  - 2'b01: 6 bits
  - 2'b10: 7 bits
  - 2'b11: 8 bits */
#define UART_SETUP_BIT_LENGTH_MASK  (0x6ul)
#define UART_SETUP_BIT_LENGTH_SHIFT (1ul)

/* Set stop bits length:
  - 2'b0: 1 stop bit
  - 2'b1: 2 stop bits */
#define UART_SETUP_STOP_BITS_MASK  (0x8ul)
#define UART_SETUP_STOP_BITS_SHIFT (3ul)

/* Set polling mode:
  - 1'b0: disable
  - 1'b1: Enable polling data transfer. Wait until valid register signals ok,
    then read data register. Valid register automatically cleared after read.*/
#define UART_SETUP_POLLING_EN_MASK  (0x10ul)
#define UART_SETUP_POLLING_EN_SHIFT (4ul)

/* Set clean fifo mode:
  - 1'b0: disable
  - 1'b1: clean the rx fifo */
#define UART_SETUP_CLEAN_FIFO_MASK  (0x20ul)
#define UART_SETUP_CLEAN_FIFO_SHIFT (5ul)

/* Set TX transceiver state:
  - 1'b0: disable
  - 1'b1: enable */
#define UART_SETUP_TX_ENA_MASK	(0x100ul)
#define UART_SETUP_TX_ENA_SHIFT (8ul)

/* Set RX transceiver state:
  - 1'b0: disable
  - 1'b1: enable */
#define UART_SETUP_RX_ENA_MASK	(0x200ul)
#define UART_SETUP_RX_ENA_SHIFT (9ul)

/* Sets the clock divider ratio for the baud rate generator. */
#define UART_SETUP_CLKDIV_MASK	(0xffff0000ul)
#define UART_SETUP_CLKDIV_SHIFT (16ul)

/*! @name ERROR register */

/* Reports overflow error:
  - 1'b0: no error
  - 1'b1: rx overflow error */
#define UART_ERROR_RX_ERR_OVERFLOW_MASK	 (0x1ul)
#define UART_ERROR_RX_ERR_OVERFLOW_SHIFT (0ul)

/* Reports parity error:
  - 1'b0: no error
  - 1'b1: rx parity error */
#define UART_ERROR_RX_ERR_PARITY_MASK  (0x2ul)
#define UART_ERROR_RX_ERR_PARITY_SHIFT (1ul)

/*! @name IRQ_EN register */

/* Rx interrupt in enable flag:
  - 1’b0: RX IRQ disable
  - 1’b1: RX IRQ enable */
#define UART_IRQ_EN_RX_MASK  (0x1ul)
#define UART_IRQ_EN_RX_SHIFT (0ul)

/* Error interrupt in enable flag:
  - 1’b0: Error IRQ disable
  - 1’b1: Error IRQ enable*/
#define UART_IRQ_EN_ERROR_MASK	(0x2ul)
#define UART_IRQ_EN_ERROR_SHIFT (1ul)

/*! @name READY register */

/* Used only in RX polling method to indicate data is ready for read:
  - 1’b0: Data is not ready to read
  - 1’b1: Data is ready to read */
#define UART_VALID_READY_MASK  (0x1ul)
#define UART_VALID_READY_SHIFT (0ul)

/* RX read data for polling or interrupt */
#define UART_DATA_MASK	(0xfful)
#define UART_DATA_SHIFT (0ul)


#define uart(id) ((uart_t *)UDMA_UART(id))

#endif /* __UART_PERIPH_H__ */
