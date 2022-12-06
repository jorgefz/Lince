

OutputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["glfw"]    = "deps/glfw/include"
IncludeDir["glad"]    = "deps/glad/include"
IncludeDir["cglm"]    = "deps/cglm/include"
IncludeDir["nuklear"] = "deps/nuklear/include"
IncludeDir["stb"]     = "deps/stb/include"
IncludeDir["lince"]   = "lince/src"


project "glfw"
    kind "StaticLib"
    language "C"
    staticruntime "on"
    location "build/glfw"

    targetdir ("bin/" .. OutputDir .. "/%{prj.name}")
    objdir ("obj/" .. OutputDir .. "/%{prj.name}")

    SrcDir = "deps/glfw/src"
    
    files {
        "deps/glfw/include/GLFW/glfw3.h",
        "deps/glfw/include/GLFW/glfw3native.h",
        SrcDir .. "/context.c",
        SrcDir .. "/init.c",
        SrcDir .. "/input.c",
        SrcDir .. "/monitor.c",
        SrcDir .. "/vulkan.c",
        SrcDir .. "/window.c",
        SrcDir .. "/osmesa_context.c",
    }
    
    filter "system:linux"
        systemversion "latest"
        files {
            SrcDir .. "/posix_time.c",
            SrcDir .. "/posix_thread.c",
            SrcDir .. "/x11_init.c",
            SrcDir .. "/x11_monitor.c",
            SrcDir .. "/x11_window.c",
            SrcDir .. "/egl_context.c",
            SrcDir .. "/xkb_unicode.c",
            SrcDir .. "/glx_context.c",
            SrcDir .. "/linux_joystick.c",
        }
        defines {"_GLFW_X11"}
    
    filter "system:windows"
        files{
            SrcDir .. "/win32_init.c",
            SrcDir .. "/win32_joystick.c",
            SrcDir .. "/win32_monitor.c",
            SrcDir .. "/win32_time.c",
            SrcDir .. "/win32_thread.c",
            SrcDir .. "/win32_window.c",
            SrcDir .. "/wgl_context.c",
            SrcDir .. "/egl_context.c",
        }

        defines{
            "_GLFW_WIN32",
            "_CRT_SECURE_NO_WARNINGS"
        }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"


project "glad"
    kind "StaticLib"
    language "C"
    staticruntime "On"
    location "build/glad"

    targetdir ("bin/" .. OutputDir .. "/%{prj.name}")
    objdir ("obj/" .. OutputDir .. "/%{prj.name}")

    files {
        "deps/glad/include/glad/glad.h",
        "deps/glad/include/KHR/khrplatform.h",
        "deps/glad/src/glad.c"
    }

    includedirs {"deps/glad/include"}

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"


project "cglm"
    kind "StaticLib"
    language "C"
    staticruntime "On"
    location "build/cglm"

    targetdir ("bin/" .. OutputDir .. "/%{prj.name}")
    objdir ("obj/" .. OutputDir .. "/%{prj.name}")

    files {
        "deps/cglm/src/**.c",
        "deps/cglm/include/cglm/**.h"
    }

    includedirs {"deps/cglm/include"}

    defines {"CGLM_STATIC"}

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"


project "stb"
    kind "StaticLib"
    language "C"
    staticruntime "On"
    location "build/stb"

    targetdir ("bin/" .. OutputDir .. "/%{prj.name}")
    objdir ("obj/" .. OutputDir .. "/%{prj.name}")

    files {
        "deps/stb/src/**.c",
        "deps/stb/include/**.h"
    }

    includedirs {"deps/stb/include"}

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"


project "nuklear"
    kind "StaticLib"
    language "C"
    staticruntime "on"
    location "build/nuklear"

    objdir ("obj/" .. OutputDir .. "/%{prj.name}" )
    targetdir ("bin/" .. OutputDir .. "/%{prj.name}" )
    
    defines "GLFW_INCLUDED_NONE"

    files {
        "deps/nuklear/src/**.c",
        "deps/nuklear/include/**.h",
    }
    
    includedirs {
        "deps/nuklear",
        "deps/nuklear/src",
        "%{IncludeDir.nuklear}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.glad}",
    }

    links {
        "GLAD",
        "glfw",
    }

    libdirs {
        "bin/" .. OutputDir .. "/glad",
        "bin/" .. OutputDir .. "/glfw",
    }


project "lince"
    kind "StaticLib"
    language "C"
    staticruntime "on"
    location "build/lince"

    targetdir ("bin/" .. OutputDir .. "/%{prj.name}")
    objdir ("obj/" .. OutputDir .. "/%{prj.name}")

    defines "GLFW_INCLUDED_NONE"

    files {
        "lince/src/**.c",
        "lince/src/**.h",
    }
    
    includedirs {
        "lince",
		"lince/src",
        "lince/src/lince",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.cglm}",
        "%{IncludeDir.nuklear}",
        "%{IncludeDir.stb}"
    }

    links {
        "glad",
        "glfw",
        "cglm",
        "nuklear",
        "stb"
    }

    libdirs {
        "bin/" .. OutputDir .. "/glad",
        "bin/" .. OutputDir .. "/glfw",
        "bin/" .. OutputDir .. "/nuklear",
        "bin/" .. OutputDir .. "/cglm",
        "bin/" .. OutputDir .. "/stb"
    }


project "tests"
    kind "ConsoleApp"
    language "C"
    staticruntime "on"
    location "build/tests"

    targetdir ("bin/" .. OutputDir .. "/%{prj.name}")
    objdir ("obj/" .. OutputDir .. "/%{prj.name}")

    files {
        "%{prj.name}/src/**.c",
        "%{prj.name}/src/**.h",
    }
    
    includedirs {
        "%{prj.name}",
        "%{prj.name}/src",
        "%{IncludeDir.lince}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.cglm}"
    }

    links {
        "lince",
        "glad",
        "glfw",
        "cglm",
        "nuklear",
        "stb"
    }

    libdirs {"bin/" .. OutputDir .. "/lince"}


project "editor"
    kind "ConsoleApp"
    language "C"
    staticruntime "on"
    location "build/editor"

    targetdir ("bin/" .. OutputDir .. "/%{prj.name}")
    objdir ("obj/" .. OutputDir .. "/%{prj.name}")

    files {
        "%{prj.name}/src/**.c",
        "%{prj.name}/src/**.h",
    }
    
    includedirs {
        "%{prj.name}",
		"%{prj.name}/src",
        "%{IncludeDir.lince}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.cglm}",
        "%{IncludeDir.nuklear}",
        "%{IncludeDir.stb}"
    }

    links {
        "lince",
        "glad",
        "glfw",
        "cglm",
        "stb",
        "nuklear"
    }

    libdirs {"bin/" .. OutputDir .. "/lince"}


project "sandbox"
    kind "ConsoleApp"
    language "C"
    staticruntime "on"
    location "build/sandbox"
    
    targetdir ("bin/" .. OutputDir .. "/%{prj.name}")
    objdir ("obj/" .. OutputDir .. "/%{prj.name}")

    files {
        "%{prj.name}/src/**.c",
        "%{prj.name}/src/**.h",
    }
    
    includedirs {
        "%{prj.name}",
		"%{prj.name}/src",
        "%{IncludeDir.lince}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.cglm}",
        "%{IncludeDir.nuklear}",
        "%{IncludeDir.stb}"
    }

    links {
        "lince",
        "glad",
        "glfw",
        "cglm",
        "stb",
        "nuklear"
    }

    libdirs {"bin/" .. OutputDir .. "/lince"}


----------------------------
--------- MINIGAMES --------
----------------------------

project "pong"
    kind "ConsoleApp"
    language "C"
    staticruntime "on"
    location "build/pong"

    targetdir ("bin/" .. OutputDir .. "/%{prj.name}")
    objdir ("obj/" .. OutputDir .. "/%{prj.name}")

    files {
        "minigames/%{prj.name}/src/**.c",
        "minigames/%{prj.name}/src/**.h",
    }
    
    includedirs {
        "minigames/%{prj.name}",
        "minigames/%{prj.name}/src",
        "%{IncludeDir.lince}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.cglm}",
        "%{IncludeDir.nuklear}"
    }

    links {
        "lince",
        "glad",
        "glfw",
        "cglm",
        "nuklear",
        "stb"
    }

    libdirs {"bin/" .. OutputDir .. "/lince"}

    
project "mcommand"
    kind "ConsoleApp"
    language "C"
    staticruntime "on"
    location "build/mcommand"

    targetdir ("bin/" .. OutputDir .. "/%{prj.name}")
    objdir ("obj/" .. OutputDir .. "/%{prj.name}")

    files {
        "minigames/%{prj.name}/src/**.c",
        "minigames/%{prj.name}/src/**.h",
    }
    
    includedirs {
        "minigames/%{prj.name}",
        "minigames/%{prj.name}/src",
        "%{IncludeDir.lince}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.glad}",
        "%{IncludeDir.cglm}",
        "%{IncludeDir.nuklear}"
    }

    links {
        "lince",
        "glad",
        "glfw",
        "cglm",
        "nuklear",
        "stb"
    }

    libdirs {"bin/" .. OutputDir .. "/lince"}

