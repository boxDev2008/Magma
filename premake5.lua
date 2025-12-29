include "config.lua"

local host = os.host()
if host == "windows" then
	VULKAN_SDK = os.getenv("VULKAN_SDK")
else
	VULKAN_SDK = "."
end

workspace "Magma"
	configurations { "Release" }
	startproject "SandboxApp"

project "Magma"
	kind "SharedLib"
	language "C"
	targetdir "bin/%{cfg.buildcfg}"

	architecture "x64"

	files { "magma/**.h", "magma/**.c" }

	optimize "On"

	defines { "MG_EXPORT" }

	includedirs {
		"magma",
		"magma/vendor"
	}

	filter "configurations:Release"
		defines { "NDEBUG" }

	filter { "system:windows" }
		includedirs { VULKAN_SDK.."\\Include" }
		libdirs { VULKAN_SDK.."\\Lib" }

	filter { "system:linux" }
		links { "vulkan", "X11", "m" }

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

project(PROJECT_NAME)
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	targetdir "bin/%{cfg.buildcfg}"

	architecture "x64"

	local utils_files = {}
	local additional_includes = {}
	local additional_links = {}

	if PROJECT_UTILS then
		for _, dir in ipairs(PROJECT_UTILS) do
			table.insert(utils_files, "magma-utils/" .. dir .. "/**.c")
			table.insert(utils_files, "magma-utils/" .. dir .. "/**.cpp")
		end
	end

	if ADDITIONAL_INCLUDES then
		additional_includes = ADDITIONAL_INCLUDES
	end

	if ADDITIONAL_LINKS then
		additional_links = ADDITIONAL_LINKS
	end

	files {
		SOURCE_DIR.."/**.c",
		SOURCE_DIR.."/**.cpp",
		table.unpack(utils_files)
	}

	includedirs {
		".",
		"magma",
		"magma/vendor",
		"magma-utils",
		SOURCE_DIR,
		table.unpack(additional_includes)
	}

	libdirs { "bin/%{cfg.buildcfg}" }
	links {
		"Magma",
		table.unpack(additional_links)
	}

	optimize "On"

	filter "configurations:Release"
		defines { "NDEBUG" }

	filter { "system:linux" }
		links { "EGL" }