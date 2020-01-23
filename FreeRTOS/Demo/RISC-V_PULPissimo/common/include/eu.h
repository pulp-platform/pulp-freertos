/*
 * Copyright (C) 2019 ETH Zurich and University of Bologna
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

/* Author: Robert Balas (balasr@iis.ee.ethz.ch)
 *         Germain Haugou (germain.haugou@iis.ee.ethz.ch)
 */

#ifndef __EU_H
#define __EU_H
/*
 * SOC EVENTS
 */

#define EU_SOC_EVENT_PERIPH_EVT_NB 160

#define EU_SOC_EVENT_SW_NB (8)

#define EU_SOC_EVENT_NB_TOTAL 256

#define EU_SOC_EVENT_UDMA_NB_CHANNEL_EVT_LOG2 2
#define EU_SOC_EVENT_UDMA_NB_CHANNEL_EVT                                       \
	(1 << EU_SOC_EVENT_UDMA_NB_CHANNEL_EVT_LOG2)
#define EU_SOC_EVENT_UDMA_FIRST_EVT 0
#define EU_SOC_EVENT_UDMA_NB_EVT                                               \
	(EU_SOC_EVENT_UDMA_NB_CHANNEL_EVT * EU_NB_PERIPH)
#define EU_SOC_EVENT_UDMA_NB_TGEN_EVT 6

#define EU_SOC_EVENT_PERIPH_FIRST_EVT(x) ((x)*EU_SOC_EVENT_UDMA_NB_CHANNEL_EVT)

#define EU_SOC_EVENT_UART0_RX	   0
#define EU_SOC_EVENT_UART0_TX	   1
#define EU_SOC_EVENT_UART0_EOT	   2
#define EU_SOC_EVENT_UART0_RX_DATA 3

#define EU_SOC_EVENT_SPIM0_RX  4
#define EU_SOC_EVENT_SPIM0_TX  5
#define EU_SOC_EVENT_SPIM0_CMD 6
#define EU_SOC_EVENT_SPIM0_EOT 7

#define EU_SOC_EVENT_I2C0_RX 8
#define EU_SOC_EVENT_I2C0_TX 9

#define EU_SOC_EVENT_I2C1_RX 12
#define EU_SOC_EVENT_I2C1_TX 13

#define EU_SOC_EVENT_SDIO0_RX 16
#define EU_SOC_EVENT_SDIO0_TX 17

#define EU_SOC_EVENT_I2S0_RX 20
#define EU_SOC_EVENT_I2S0_TX 21

#define EU_SOC_EVENT_CPI0_RX 24

#define EU_SOC_EVENT_FILTER0_RX 28
#define EU_SOC_EVENT_FILTER0_TX 29

#define EU_SOC_EVENT_CLUSTER_ON_OFF   31
#define EU_SOC_EVENT_MSP	      37
#define EU_SOC_EVENT_ICU_MODE_CHANGED 37
#define EU_SOC_EVENT_ICU_OK	      37
#define EU_SOC_EVENT_ICU_DELAYED      37
#define EU_SOC_EVENT_CLUSTER_CG_OK    35
#define EU_SOC_EVENT_PICL_OK	      36
#define EU_SOC_EVENT_SCU_OK	      37
#define EU_SOC_EVENT_PMU_FIRST_EVENT  EU_SOC_EVENT_CLUSTER_ON_OFF
#define EU_SOC_EVENT_PMU_NB_EVENTS    7

#define EU_SOC_EVENT_GPIO 42

#define EU_SOC_EVENT_NB_I2S_CHANNELS  4
#define EU_SOC_EVENT_NB_UDMA_CHANNELS 19

#define EU_SOC_EVENT_FCHWPE0 140
#define EU_SOC_EVENT_FCHWPE1 141

#define EU_SOC_EVENT_SW_EVENT0 48
#define EU_SOC_EVENT_SW_EVENT1 49
#define EU_SOC_EVENT_SW_EVENT2 50
#define EU_SOC_EVENT_SW_EVENT3 51
#define EU_SOC_EVENT_SW_EVENT4 52
#define EU_SOC_EVENT_SW_EVENT5 53
#define EU_SOC_EVENT_SW_EVENT6 54
#define EU_SOC_EVENT_SW_EVENT7 55

#define EU_SOC_EVENT_NB 8

#define EU_SOC_EVENT_REF_CLK_RISE 56

#endif
