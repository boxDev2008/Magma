<p align="center">
  <img width="800" src="content/logo.png">
</p>

<br>
Magma is yet another set of tools for game development in C.

# Project Generation
To generate the `Sandbox` project you can use premake in the `tools` folder or install it yourself.

Run ```.\tools\premake5\premake5.exe vs2022``` to generate a `Visual Studio 2022` project (On Windows)

# Compilation
Run ```MSBuild.exe Magma.sln -property:Configuration=Release``` and it will build (On Windows)

> [!NOTE]
> You need to have the Vulkan SDK installed.
