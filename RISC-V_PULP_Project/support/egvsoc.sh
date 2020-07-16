#!/usr/bin/env bash
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
