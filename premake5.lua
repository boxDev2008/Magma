local VULKAN_LIB_PATH = os.getenv("VULKAN_SDK").."\\Lib";
print(VULKAN_LIB_PATH)

workspace "Magma"
   configurations { "Release" }

project "Magma"
   kind "SharedLib"
   language "C"
   targetdir "bin/%{cfg.buildcfg}"

   architecture "x64"

   files { "magma/**.h", "magma/**.c" }
   includedirs { ".", "magma", "magma/vendor" }

   filter { "system:windows" }
      libdirs { VULKAN_LIB_PATH }
      links { "user32", "gdi32", "vulkan-1", "opengl32" }
      filter "configurations:Release"
         defines { "NDEBUG", "VK_USE_PLATFORM_WIN32_KHR" }

project "Sandbox"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   targetdir "bin/%{cfg.buildcfg}"

   architecture "x64"

   files { "sandbox/**.h", "sandbox/**.cpp" }
   includedirs { ".", "magma", "magma/vendor", "sandbox" }

   links { "Magma" }
   libdirs { "bin/%{cfg.buildcfg}" }

   filter "configurations:Release"
      defines { "NDEBUG" }