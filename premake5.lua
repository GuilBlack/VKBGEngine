workspace "VKBGEngine"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

OutputDir = "%{cfg.buildcfg}/%{cfg.system}-%{cfg.architecture}"

-- IncludeDir = {}
-- IncludeDir["GLFW"] = "%{wks.location}/%{ProjectNames.Core}/vendor/GLFW/include"

-- group "Dependencies"
--     include "%{ProjectNames.Core}/vendor/GLFW"
-- group ""

include "VKBGEngine-Core"
include "VKBGEngine-App"