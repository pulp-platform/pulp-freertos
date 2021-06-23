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
ifeq ($(CONFIG_DRIVER_FLL),y)
SRCS += $(dir)/fll.c
else ifeq ($(CONFIG_DRIVER_CLKDIV),y)
SRCS += $(dir)/clkdiv.c
else
$(error no driver for clock configuration enabled. Choose FLL or CLKDIV)
endif
SRCS += $(dir)/timer_irq.c
SRCS += $(dir)/irq.c
SRCS += $(dir)/soc_eu.c
SRCS += $(dir)/gpio.c
SRCS += $(dir)/pinmux.c
SRCS += $(dir)/fc_event.c

SRCS += $(dir)/pmsis_task.c
SRCS += $(dir)/device.c

CV_CPPFLAGS += -I"$(dir)/include"
