@echo off
setlocal

set CONFIG=Release
set OUT=bin\%CONFIG%

if not exist %OUT% mkdir %OUT%

clang-cl ^
    /std:c++20 ^
    /O2 ^
    /DNDEBUG ^
    /MD ^
    /EHsc ^
    -Ivendor ^
    -I"%VULKAN_SDK%\Include" ^
    magma-shdc\main.cpp ^
    magma-shdc\vendor\pystring\pystring.cpp ^
    /Fe:"%OUT%\magma-shdc.exe" ^
    /link ^
    /LIBPATH:"%VULKAN_SDK%\Lib" ^
    glslang.lib ^
    glslang-default-resource-limits.lib ^
    GenericCodeGen.lib ^
    MachineIndependent.lib ^
    OSDependent.lib ^
    SPIRV.lib ^
    SPIRV-Tools.lib ^
    SPIRV-Tools-opt.lib ^
    spirv-cross-glsl.lib ^
    spirv-cross-hlsl.lib ^
    spirv-cross-msl.lib ^
    spirv-cross-core.lib

endlocal