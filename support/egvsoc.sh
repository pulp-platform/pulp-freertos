#!/usr/bin/env bash

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

SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

SUPPORT_DIR="$SCRIPTPATH"
export INSTALL_DIR="$SUPPORT_DIR/install"
# export PULP_CURRENT_CONFIG="pulp@config_file=chips/pulp/pulp.json"
# two paths because buggy lookup in gvsoc and related scripts
export PULP_CONFIGS_PATH="$SUPPORT_DIR/pulp-configs/configs:$SUPPORT_DIR/pulp-configs"

export PATH="$SUPPORT_DIR/install/bin:$PATH"
export PYTHONPATH="$SUPPORT_DIR/install/python"
# hack the ld path to make the the shared libraries find the shared libraries
# they want to load
export LD_LIBRARY_PATH="$SUPPORT_DIR/install/lib:$LD_LIBRARY_PATH"

# need to cd otherwise gvsoc.json has a bad path
cd "$SCRIPTPATH" && ./gwrap.py "$@"
