workspace "Hazel"
    architecture "x64"
    startproject "Sandbox"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

vectorextensions "AVX2"
buildoptions {
    "/Qpar", "/Qpar-report:1", "/Qvec-report:1"
}

filter "configurations:Release"
    linkoptions {
        "/LTCG"
    }
filter "configurations:Dist"
    linkoptions {
        "/LTCG"
    }

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Hazel/vendor/GLFW/include"
IncludeDir["Glad"] = "Hazel/vendor/Glad/include"
IncludeDir["ImGui"] = "Hazel/vendor/imgui"
IncludeDir["glm"] = "Hazel/vendor/glm"
IncludeDir["stb_image"] = "Hazel/vendor/stb_image"
IncludeDir["HazelAudio"] = "Hazel/vendor/HazelAudio/HazelAudio/src"

include "Hazel/vendor/GLFW"
include "Hazel/vendor/Glad"
include "Hazel/vendor/imgui"
include "Hazel/vendor/HazelAudio"

project "Hazel"
    location "Hazel"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "hzpch.h"
    pchsource "Hazel/src/hzpch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/stb_image/**.cpp",
        "%{prj.name}/vendor/stb_image/**.h",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.HazelAudio}"
    }

    links 
    { 
        "GLFW",
        "Glad",
        "ImGui",
        "HazelAudio",
        "opengl32.lib"
    }

    defines {
        "_CRT_SECURE_NO_WARNINGS"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "HZ_BUILD_DLL",
            "GLFW_INCLUDE_NONE"
        }

    filter "configurations:Debug"
        defines "HZ_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "HZ_RELEASE"
        runtime "Release"
        optimize "on"
        buildoptions {
            "/O2", "/Ob2", "/Ot", "/Oy", "/GT", "/LTCG", "/GL"
        }

    filter "configurations:Dist"
        defines "HZ_DIST"
        runtime "Release"
        optimize "on"
        buildoptions {
            "/O2", "/Ob2", "/Ot", "/Oy", "/GT", "/LTCG", "/GL"
        }

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/assets/**.**"
    }

    includedirs
    {
        "Hazel/vendor/spdlog/include",
        "Hazel/src",
        "Hazel/vendor",
        "%{IncludeDir.glm}",
        "%{IncludeDir.HazelAudio}"
    }

    links
    {
        "Hazel"
    }

    filter "system:windows"
        systemversion "latest"


    filter "configurations:Debug"
        defines "HZ_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "HZ_RELEASE"
        runtime "Release"
        optimize "on"
        buildoptions {
            "/O2", "/Ob2", "/Ot", "/Oy", "/GT", "/LTCG", "/GL"
        }

    filter "configurations:Dist"
        defines "HZ_DIST"
        runtime "Release"
        optimize "on"
        buildoptions {
            "/O2", "/Ob2", "/Ot", "/Oy", "/GT", "/LTCG", "/GL"
        }


project "GeometryDash"
    location "GeometryDash"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/assets/**.**"
    }

    includedirs
    {
        "Hazel/vendor/spdlog/include",
        "Hazel/src",
        "Hazel/vendor",
        "%{IncludeDir.glm}",
        "%{IncludeDir.HazelAudio}"
    }

    links
    {
        "Hazel"
    }

    filter "system:windows"
        systemversion "latest"


    filter "configurations:Debug"
        defines "HZ_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "HZ_RELEASE"
        runtime "Release"
        optimize "on"
        buildoptions {
            "/O2", "/Ob2", "/Ot", "/Oy", "/GT", "/LTCG", "/GL"
        }

    filter "configurations:Dist"
        defines "HZ_DIST"
        runtime "Release"
        optimize "on"
        buildoptions {
            "/O2", "/Ob2", "/Ot", "/Oy", "/GT", "/LTCG", "/GL"
        }
