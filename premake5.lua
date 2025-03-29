workspace "Magma"
	configurations { "Release" }
	startproject "Sandbox"

project "Magma"
	kind "SharedLib"
	language "C"
	targetdir "bin/%{cfg.buildcfg}"

	architecture "x64"

	files { "magma/**.h", "magma/**.c" }

	optimize "On"

	filter { "system:windows" }
		local VULKAN_SDK = os.getenv("VULKAN_SDK")
		if VULKAN_SDK then
			includedirs { ".", "magma", "magma/vendor", VULKAN_SDK.."\\Include" }
			libdirs { VULKAN_SDK.."\\Lib" }
			defines { "NDEBUG" }
		end
		defines { "NDEBUG" }

	filter { "system:linux" }
		includedirs { ".", "magma", "magma/vendor" }
		links { "vulkan", "X11", "Xcursor" }
		defines { "NDEBUG" }
		buildoptions { "-fpermissive" }

project "Magma-shdc"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	targetdir "bin/%{cfg.buildcfg}"

	architecture "x64"

	files { "magma-shdc/**.h", "magma-shdc/**.cpp" }
	includedirs { 
		"magma-shdc",
		"magma-shdc/vendor",
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

project "Sandbox"
	kind "ConsoleApp"
	language "C"
	targetdir "bin/%{cfg.buildcfg}"

	architecture "x64"

	files { "sandbox/*.c" }

	includedirs {
		".",
		"magma",
		"magma/vendor"
	}

	links { "Magma" }
	libdirs { "bin/%{cfg.buildcfg}" }

	optimize "On"

	filter "configurations:Release"
		defines { "NDEBUG" }