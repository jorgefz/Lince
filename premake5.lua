
workspace "lince"
    architecture "x86_64"

    configurations {
        "Debug",
        "Release"
    }

    platforms { "Win64", "Linux" }

    startproject "lince"

    warnings "Extra"


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["glfw"] = "vendor/glfw/include"
IncludeDir["glad"] = "vendor/glad/include"

include "vendor/glfw"
include "vendor/glad"

project "lince"
    location "lince"
    kind "ConsoleApp"
    language "C"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    defines {
        "GLFW_INCLUDED_NONE"
    }

    files {
        "%{prj.name}/src/**.c",
        "%{prj.name}/src/**.h",
    }
    
    includedirs {
        "%{prj.name}/src",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.glad}",
    }

    links {
        "GLAD",
        "glfw",
    }

    filter "system:windows"
        links {
            "opengl32"
        }

    filter "system:linux"
        links {
            "GL",
            "rt",
            "m",
            "dl",
            "pthread"
        }

    libdirs {
        "vendor/glad/bin/" .. outputdir .. "/glad",
        "vendor/glad/bin/" .. outputdir .. "/glfw",
    }

    defines {
        "GLFW_INCLUDE_NONE"
    }

    filter "system:linux"
        systemversion "latest"
        defines {"LINCE_LINUX"}
    
    filter "system:windows"
        systemversion "latest"
        defines {"LINCE_WINDOWS", "_CRT_SECURE_NO_WARNINGS"}
    
    filter "configurations:Debug"
        defines "LINCE_DEBUG"
        symbols "on"

    filter "configurations:Release"
        defines "LINCE_RELEASE"
        symbols "on"
