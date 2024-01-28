workspace "VKBGEngine"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

OutputDir = "%{cfg.buildcfg}/%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/VKBGEngine-Core/vendor/GLFW/include"
IncludeDir["GLM"] = "%{wks.location}/VKBGEngine-Core/vendor/GLM"

group "Dependencies"
    include "VKBGEngine-Core/vendor/GLFW"
group ""

include "VKBGEngine-Core"
include "VKBGEngine-App"