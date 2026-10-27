#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
: "${PICO_SDK_PATH:?Exporta PICO_SDK_PATH apuntando a Pico SDK 2.2.0}"
for board in pico pico2_w; do
  cmake -S . -B "build_${board}" -DPICO_BOARD="$board"
  cmake --build "build_${board}" -j
 done
