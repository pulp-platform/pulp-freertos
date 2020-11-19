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

# Try to build a full dependency tree for all libraries

SUPPORT_LIB_DIR = $(SUPPORT_ROOT)/install/lib

$(SUPPORT_LIB_DIR)/libjson.a: \
$(shell find "$(SUPPORT_ROOT)/json-tools" -name '*.cpp' -o -name '*.hpp')
	INSTALL_DIR="$(SUPPORT_ROOT)/install" $(MAKE) -C "$(SUPPORT_ROOT)/json-tools" all install

$(SUPPORT_LIB_DIR)/libpulpdebugbridge.so: $(SUPPORT_LIB_DIR)/libjson.a \
$(shell find "$(SUPPORT_ROOT)/pulp-debug-bridge" -name '*.cpp' -o -name '*.hpp')
	INSTALL_DIR="$(SUPPORT_ROOT)/install" $(MAKE) -C "$(SUPPORT_ROOT)/pulp-debug-bridge" build

$(SUPPORT_LIB_DIR)/libpulpperiph.so $(SUPPORT_LIB_DIR)/libpulpdpi.so: \
$(SUPPORT_LIB_DIR)/libjson.a $(SUPPORT_LIB_DIR)/libpulpdebugbridge.so \
$(shell find "$(SUPPORT_ROOT)/dpi-models" -name '*.cpp' -o -name '*.hpp')
	INSTALL_DIR="$(SUPPORT_ROOT)/install" $(MAKE) -C "$(SUPPORT_ROOT)/dpi-models" build

$(SUPPORT_ROOT)/install/bin/plpinfo:
	INSTALL_DIR="$(SUPPORT_ROOT)/install" $(MAKE) -C "$(SUPPORT_ROOT)/pulp-configs" all_scons

$(SUPPORT_ROOT)/install/bin/plp_mkflash:
	INSTALL_DIR="$(SUPPORT_ROOT)/install" $(MAKE) -C "$(SUPPORT_ROOT)/runner" sdk.build


## Build the gvsoc simulator
gvsoc: $(addprefix $(SUPPORT_LIB_DIR)/,libpulpvp.so libpulpvplauncher.so libpulpvp-debug.so)

# TODO: fix build configuration per chip thing
$(addprefix $(SUPPORT_LIB_DIR)/,libpulpvp.so libpulpvplauncher.so libpulpvp-debug.so): \
$(SUPPORT_LIB_DIR)/libjson.a $(SUPPORT_LIB_DIR)/libpulpdpi.so \
$(SUPPORT_ROOT)/install/bin/plpinfo $(SUPPORT_ROOT)/install/bin/plp_mkflash \
$(shell find "$(SUPPORT_ROOT)/gvsoc" -name '*.cpp' -o -name '*.hpp' -o -name '*.py')
	PULP_CURRENT_CONFIG=pulp@config_file=chips/pulp/pulp.json \
	PULP_CONFIGS_PATH=$(SUPPORT_ROOT)/pulp-configs/configs \
	PYTHONPATH=$(SUPPORT_ROOT)/install/python \
	INSTALL_DIR="$(SUPPORT_ROOT)/install" \
	$(MAKE) -C "$(SUPPORT_ROOT)/gvsoc" build ARCHI_DIR=$(SUPPORT_ROOT)/archi/include


# TODO proper clean targets that are not buggy...
clean-gvsoc:
	$(MAKE) -C "$(SUPPORT_ROOT)/gvsoc" clean
