#!/usr/bin/env bash

set -e

CONFIG=Release
OUT=bin/$CONFIG

mkdir -p "$OUT"

clang++ \
    -std=c++20 \
    -O2 \
    -DNDEBUG \
    -Ivendor \
    magma-shdc/main.cpp \
    magma-shdc/vendor/pystring/pystring.cpp \
    -o "$OUT/magma-shdc" \
    -lglslang \
    -lglslang-default-resource-limits \
    -lSPIRV \
    -lSPIRV-Tools \
    -lSPIRV-Tools-opt \
    -lspirv-cross-glsl \
    -lspirv-cross-hlsl \
    -lspirv-cross-msl \
    -lspirv-cross-core