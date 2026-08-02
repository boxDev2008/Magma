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
    ${VULKAN_SDK:+-I"$VULKAN_SDK/include"} \
    magma-shdc/main.cpp \
    magma-shdc/vendor/pystring/pystring.cpp \
    -o "$OUT/magma-shdc" \
    ${VULKAN_SDK:+-L"$VULKAN_SDK/lib"} \
    -lglslang \
    -lglslang-default-resource-limits \
    -lGenericCodeGen \
    -lMachineIndependent \
    -lOSDependent \
    -lSPIRV \
    -lSPIRV-Tools \
    -lSPIRV-Tools-opt \
    -lspirv-cross-glsl \
    -lspirv-cross-hlsl \
    -lspirv-cross-msl \
    -lspirv-cross-core