project "VKBGEngine-App"
    kind "ConsoleApp"
    language "C++"

    targetdir ("%{wks.location}/bin/" .. OutputDir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-inter/" .. OutputDir .. "/%{prj.name}")

    files 
    {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs
    {
        "src",
        "src/%{prj.name}",
        "%{wks.location}/VKBGEngine-Core/src",
        "%{IncludeDir.GLFW}"
    }

    links
    {
        "VKBGEngine-Core",
    }

    pchheader "pch.h"
    pchsource "src/pch.cpp"

    forceincludes "pch.h"

    filter "system:windows"
        cppdialect "C++20"
        staticruntime "On"
        systemversion "latest"
        defines "PLATFORM_WINDOWS"

        includedirs
        {
            "C:/VulkanSDK/1.3.268.0/Include"
        }

        postbuildcommands
        {
            "call " .. os.realpath("res\\Shaders\\CompileScripts\\BuildShaders.bat"),
            "{COPY} res/Shaders/ " .. "%{cfg.targetdir}/res/Shaders"
        }

    filter "configurations:Debug"
        symbols "On"
        optimize "Off"
        defines { "_DEBUG", "DEBUG", "VKBG_DEBUG" }

    filter "configurations:Release"
        symbols "On"
        optimize "On"
        defines { "VKBG_DEBUG" }

    filter "configurations:Release"
        symbols "Off"
        optimize "On"
        defines "NDEBUG"
