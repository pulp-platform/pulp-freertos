#!/usr/bin/env bash

# var that points to this project's root
ROOT=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
export FREERTOS_PROJ_ROOT="$ROOT"
export FREERTOS_CONFIG_FAMILY="pulpissimo"
