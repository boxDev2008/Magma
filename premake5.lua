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
		end
		links { "user32", "gdi32", "vulkan-1", "opengl32" }
		defines { "VK_USE_PLATFORM_WIN32_KHR", "NDEBUG" }

	filter { "system:linux" }
		includedirs { ".", "magma", "magma/vendor" }
		links { "vulkan", "X11", "Xcursor" }
		defines { "NDEBUG", "VK_USE_PLATFORM_XCB_KHR" }
		buildoptions { "-fpermissive" }

project "Sandbox"
	kind "ConsoleApp"
	language "C"
	targetdir "bin/%{cfg.buildcfg}"

	architecture "x64"

	files { "sandbox/**.h", "sandbox/**.c" }
	includedirs { ".", "magma", "magma/vendor", "sandbox" }

	links { "Magma" }
	libdirs { "bin/%{cfg.buildcfg}" }

   optimize "On"

	filter "configurations:Release"
		defines { "NDEBUG" }