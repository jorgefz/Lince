
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

    defines "GLFW_INCLUDED_NONE"

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

    libdirs {
        "vendor/glad/bin/" .. outputdir .. "/glad",
        "vendor/glfw/bin/" .. outputdir .. "/glfw",
    }

    filter "system:linux"
        systemversion "latest"
        defines {"LINCE_LINUX"}
        links {"GL","rt","m","dl","pthread","X11"}
    
    filter "system:windows"
        systemversion "latest"
        defines {"LINCE_WINDOWS", "_CRT_SECURE_NO_WARNINGS"}
        links {"opengl32"}

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

    libdirs {
        "vendor/glad/bin/" .. outputdir .. "/glad",
        "vendor/glfw/bin/" .. outputdir .. "/glfw",
        "bin/" .. outputdir .. "/lince"
    }

    filter "system:windows"
        systemversion "latest"
        defines {"_CRT_SECURE_NO_WARNINGS"}
        links {"opengl32"}

    filter "system:linux"
        systemversion "latest"    
        links {"GL","rt","m","dl","pthread","X11"}
        
    filter "configurations:Debug"
        symbols "on"
        defines {"LINCE_DEBUG"}

    filter "configurations:Release"
        optimize "on"
        defines {"LINCE_RELEASE"}


project "pong"
    location "games/pong"
    kind "ConsoleApp"
    language "C"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    files {
        "games/%{prj.name}/src/**.c",
        "games/%{prj.name}/src/**.h",
    }
    
    includedirs {
        "games/%{prj.name}",
        "games/%{prj.name}/src",
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

    libdirs {
        "vendor/glad/bin/" .. outputdir .. "/glad",
        "vendor/glfw/bin/" .. outputdir .. "/glfw",
        "bin/" .. outputdir .. "/lince"
    }

    filter "system:windows"
        systemversion "latest"
        defines {"_CRT_SECURE_NO_WARNINGS"}
        links {"opengl32"}

    filter "system:linux"
        systemversion "latest"    
        links {"GL","rt","m","dl","pthread","X11"}
            
    filter "configurations:Debug"
        symbols "on"
        defines {"LINCE_DEBUG"}

    filter "configurations:Release"
        optimize "on"
        defines {"LINCE_RELEASE"}

    
project "mcommand"
    location "games/mcommand"
    kind "ConsoleApp"
    language "C"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    files {
        "games/%{prj.name}/src/**.c",
        "games/%{prj.name}/src/**.h",
    }
    
    includedirs {
        "games/%{prj.name}",
        "games/%{prj.name}/src",
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

    libdirs {
        "vendor/glad/bin/" .. outputdir .. "/glad",
        "vendor/glfw/bin/" .. outputdir .. "/glfw",
        "bin/" .. outputdir .. "/lince"
    }

    filter "system:windows"
        systemversion "latest"
        defines {"_CRT_SECURE_NO_WARNINGS"}
        links {"opengl32"}

    filter "system:linux"
        systemversion "latest"    
        links {"GL","rt","m","dl","pthread","X11"}
            
    filter "configurations:Debug"
        symbols "on"
        defines {"LINCE_DEBUG"}

    filter "configurations:Release"
        optimize "on"
        defines {"LINCE_RELEASE"}
