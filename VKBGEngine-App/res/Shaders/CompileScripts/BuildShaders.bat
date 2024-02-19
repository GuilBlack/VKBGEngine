@echo off

echo "Building Shaders..."

set GLSLC="glslc.exe"
set SHADER_PATH=".."
set COMPILED_PATH="..\Compiled"

for %%f in (%SHADER_PATH%\*.vert) do (
    %GLSLC% %%f -o %COMPILED_PATH%\%%~nf.vert.spv
)

for %%f in (%SHADER_PATH%\*.frag) do (
    %GLSLC% %%f -o %COMPILED_PATH%\%%~nf.frag.spv
)

echo "Shaders Built!"
