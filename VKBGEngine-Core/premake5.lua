project "VKBGEngine-Core"
    kind "StaticLib"
    language "C++"

    targetdir ("%{wks.location}/bin/" .. OutputDir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-inter/" .. OutputDir .. "/%{prj.name}")

    files 
    {
        "src/**.h",
        "src/**.cpp",
    }

    includedirs
    {
        "src",
    }

    links
    {
    }

    filter "system:windows"
        cppdialect "C++20"
        staticruntime "On"
        systemversion "latest"
        defines "PLATFORM_WINDOWS"

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
        defines { "NDEBUG" }