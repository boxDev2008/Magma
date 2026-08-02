@echo off
setlocal

set CONFIG=Release
set OUT=bin\%CONFIG%

if not exist %OUT% mkdir %OUT%

clang++ ^
    -std=c++20 ^
    -O2 ^
    -DNDEBUG ^
    -Ivendor ^
    -I"%VULKAN_SDK%\Include" ^
    magma-shdc\main.cpp ^
    magma-shdc\vendor\pystring\pystring.cpp ^
    -o "%OUT%\magma-shdc.exe" ^
    -L"%VULKAN_SDK%\Lib" ^
    -lglslang ^
    -lglslang-default-resource-limits ^
    -lGenericCodeGen ^
    -lMachineIndependent ^
    -lOSDependent ^
    -lSPIRV ^
    -lSPIRV-Tools ^
    -lSPIRV-Tools-opt ^
    -lspirv-cross-glsl ^
    -lspirv-cross-hlsl ^
    -lspirv-cross-msl ^
    -lspirv-cross-core

endlocal