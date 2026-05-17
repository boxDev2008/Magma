local HOST = os.host()
VULKAN_SDK = os.getenv("VULKAN_SDK") or ""

workspace "Magma"
    configurations { "Release" }

project "magma-shdc"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "bin/%{cfg.buildcfg}"
    architecture "x64"

    files {
        "magma-shdc/**.cpp"
    }

    includedirs { "vendor" }
    libdirs { "bin/%{cfg.buildcfg}" }

    if VULKAN_SDK ~= "" then
        includedirs { path.join(VULKAN_SDK, "include") }
        libdirs    { path.join(VULKAN_SDK, "lib") }
    end

    links { 
        "glslang",
        "glslang-default-resource-limits",
        "GenericCodeGen",
        "MachineIndependent",
        "OSDependent",
        "SPIRV",
        "spirv-cross-core",
        "spirv-cross-glsl",
        "spirv-cross-hlsl",
        "spirv-cross-msl",
        "SPIRV-Tools",
        "SPIRV-Tools-opt"
    }

    optimize "On"
    filter "configurations:Release"
        defines { "NDEBUG" }