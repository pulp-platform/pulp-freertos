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

SRCS += $(dir)/syscalls.c
SRCS += $(dir)/pulp_malloc.c
ifeq ($(CONFIG_CLUSTER),y)
SRCS += $(dir)/malloc/cl_l1_malloc.c
SRCS += $(dir)/malloc/malloc_internal.c
CV_CPPFLAGS += -I"$(FREERTOS_PROJ_ROOT)/$(dir)/malloc/include"
endif
CV_CPPFLAGS += -DSTDIO_FAKE=2
CV_CPPFLAGS += -DSTDIO_UART=1
CV_CPPFLAGS += -DSTDIO_NULL=0
ifeq ($(CONFIG_STDIO),fake)
CV_CPPFLAGS += -DCONFIG_STDIO=2
else ifeq ($(CONFIG_STDIO),uart)
CV_CPPFLAGS += -DCONFIG_STDIO=1
CV_CPPFLAGS += -DSTDIO_UART_DEVICE_ID=$(CONFIG_STDIO_UART_DEVICE_ID)
CV_CPPFLAGS += -DSTDIO_UART_BAUDRATE=$(CONFIG_STDIO_UART_BAUDRATE)
CV_CPPFLAGS += -DSTDIO_UART_BUFSIZE=$(CONFIG_STDIO_UART_BUFSIZE)
else ifeq ($(CONFIG_STDIO),null)
CV_CPPFLAGS += -DCONFIG_STDIO=0
endif
