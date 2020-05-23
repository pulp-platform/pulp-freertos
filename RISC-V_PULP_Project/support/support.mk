# SPDX-License-Identifier: Apache-2.0
# Author: Robert Balas (balasr@iis.ee.ethz.ch)
# Try to build a full dependency tree for all libraries

SUPPORT_LIB_DIR = $(SUPPORT_ROOT)/install/lib

$(SUPPORT_LIB_DIR)/libjson.a: \
$(shell find "$(SUPPORT_ROOT)/json-tools" -name '*.cpp' -o -name '*.hpp')
	INSTALL_DIR="$(SUPPORT_ROOT)/install" make -C "$(SUPPORT_ROOT)/json-tools" all install

$(SUPPORT_LIB_DIR)/libpulpdebugbridge.so: \
$(shell find "$(SUPPORT_ROOT)/pulp-debug-bridge" -name '*.cpp' -o -name '*.hpp')
	INSTALL_DIR="$(SUPPORT_ROOT)/install" make -C "$(SUPPORT_ROOT)/pulp-debug-bridge" build

$(SUPPORT_LIB_DIR)/libpulpperiph.so $(SUPPORT_LIB_DIR)/libpulpdpi.so &: \
$(SUPPORT_LIB_DIR)/libjson.a $(SUPPORT_LIB_DIR)/libpulpdebugbridge.so \
$(shell find "$(SUPPORT_ROOT)/dpi-models" -name '*.cpp' -o -name '*.hpp')
	INSTALL_DIR="$(SUPPORT_ROOT)/install" make -C "$(SUPPORT_ROOT)/dpi-models" build

