local HOST = os.host()
if HOST == "windows" then
	VULKAN_SDK = os.getenv("VULKAN_SDK")
else
	VULKAN_SDK = "."
end

workspace "Magma"
	configurations { "Release" }

project "Magma-shdc"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	targetdir "bin/%{cfg.buildcfg}"

	architecture "x64"

	files {
        "**.cpp"
    }

	includedirs { 
		"vendor",
		VULKAN_SDK .. "\\Include"
	}

	libdirs { 
		"bin/%{cfg.buildcfg}", 
		VULKAN_SDK .. "\\Lib"
	}

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