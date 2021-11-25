# Copyright 2020 ETH Zurich
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# SPDX-License-Identifier: Apache-2.0
# Author: Robert Balas (balasr@iis.ee.ethz.ch)

SRCS += $(dir)/uart.c
SRCS += $(dir)/spi.c
SRCS += $(dir)/i2c.c
ifeq ($(CONFIG_UDMA_I2C_ACK),y)
CV_CPPFLAGS += -DCONFIG_UDMA_I2C_ACK
endif
ifeq ($(CONFIG_UDMA_I2C_EOT),y)
CV_CPPFLAGS += -DCONFIG_UDMA_I2C_EOT
endif
ifeq ($(CONFIG_DRIVER_FLL),y)
SRCS += $(dir)/fll.c
else ifeq ($(CONFIG_DRIVER_CLKDIV),y)
SRCS += $(dir)/clkdiv.c
else ifeq ($(CONFIG_DRIVER_CLKCONST),y)
SRCS += $(dir)/clkconst.c
else
$(error no driver for clock configuration enabled.)
endif
ifeq ($(CONFIG_CLUSTER),y)
SRCS += $(dir)/cluster/cl_to_fc_delegate.c
SRCS += $(dir)/cluster/fc_to_cl_delegate.c
endif

SRCS += $(dir)/timer_irq.c
ifeq ($(CONFIG_DRIVER_INT),pclint)
SRCS += $(dir)/pclint.c
else ifeq ($(CONFIG_DRIVER_INT),clic)
SRCS += $(dir)/clic.c
CV_CPPFLAGS += -DCONFIG_CLIC
else
$(error no driver for interrupt controller enabled. Set CONFIG_DRIVER_INT)
endif

SRCS += $(dir)/soc_eu.c
SRCS += $(dir)/gpio.c
SRCS += $(dir)/pinmux.c
SRCS += $(dir)/fc_event.c

SRCS += $(dir)/pmsis_task.c
SRCS += $(dir)/device.c

CV_CPPFLAGS += -I"$(FREERTOS_PROJ_ROOT)/$(dir)/include"
ifeq ($(CONFIG_CLUSTER),y)
CV_CPPFLAGS += -I"$(FREERTOS_PROJ_ROOT)/$(dir)/include/cluster"
CV_CPPFLAGS += -DCONFIG_CLUSTER
endif
