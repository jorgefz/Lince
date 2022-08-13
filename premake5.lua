
workspace "lince"
    architecture "x86_64"

    configurations {
        "Debug",
        "Release"
    }

    startproject "game"

    warnings "Extra"


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["glfw"] = "vendor/glfw/include"
IncludeDir["glad"] = "vendor/glad/include"
IncludeDir["cglm"] = "vendor/cglm/include"

include "vendor/glfw"
include "vendor/glad"
include "vendor/cglm"

project "lince"
    location "engine"
    kind "StaticLib"
    language "C"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    defines {
        "GLFW_INCLUDED_NONE"
    }

    files {
        "engine/src/**.c",
        "engine/src/**.h",
    }
    
    includedirs {
        "engine",
		"engine/src",
        "engine/src/lince",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.cglm}"
    }

    links {
        "GLAD",
        "glfw",
        "cglm"
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
            "pthread",
            "X11"
        }

    libdirs {
        "vendor/glad/bin/" .. outputdir .. "/glad",
        "vendor/glfw/bin/" .. outputdir .. "/glfw",
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
        optimize "on"


project "game"
    location "game"
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
        "%{prj.name}",
		"%{prj.name}/src",
        "engine",
        "engine/src",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.cglm}"
    }

    links {
        "lince",
        "GLAD",
        "glfw",
        "cglm",
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
            "pthread",
            "X11"
        }

    libdirs {
        "vendor/glad/bin/" .. outputdir .. "/glad",
        "vendor/glfw/bin/" .. outputdir .. "/glfw",
        "bin/" .. outputdir .. "/lince"
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
        optimize "on"
