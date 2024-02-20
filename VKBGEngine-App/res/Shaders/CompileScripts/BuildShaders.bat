@echo off

echo "Building Shaders..."

set GLSLC="C:\VulkanSDK\1.3.268.0\Bin\glslc.exe"
set SHADER_PATH="res\Shaders"
set COMPILED_PATH="res\Shaders\Compiled"

for %%f in (%SHADER_PATH%\*.vert) do (
    %GLSLC% %%f -o %COMPILED_PATH%\%%~nf.vert.spv
)

for %%f in (%SHADER_PATH%\*.frag) do (
    %GLSLC% %%f -o %COMPILED_PATH%\%%~nf.frag.spv
)

echo "Shaders Built!"
