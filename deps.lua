

project "glfw"
    kind "StaticLib"
    language "C"
    staticruntime "on"
    location "build/glfw"

    targetdir ("bin/" .. LinceOutputDir .. "/%{prj.name}")
    objdir ("obj/" .. LinceOutputDir .. "/%{prj.name}")

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

    targetdir ("bin/" .. LinceOutputDir .. "/%{prj.name}")
    objdir ("obj/" .. LinceOutputDir .. "/%{prj.name}")

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

    targetdir ("bin/" .. LinceOutputDir .. "/%{prj.name}")
    objdir ("obj/" .. LinceOutputDir .. "/%{prj.name}")

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

    targetdir ("bin/" .. LinceOutputDir .. "/%{prj.name}")
    objdir ("obj/" .. LinceOutputDir .. "/%{prj.name}")

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

    objdir ("obj/" .. LinceOutputDir .. "/%{prj.name}" )
    targetdir ("bin/" .. LinceOutputDir .. "/%{prj.name}" )
    
    defines "GLFW_INCLUDED_NONE"

    files {
        "deps/nuklear/src/**.c",
        "deps/nuklear/include/**.h",
    }
    
    includedirs {
        "deps/nuklear",
        "deps/nuklear/src",
        "%{LinceIncludeDir.nuklear}",
        "%{LinceIncludeDir.glfw}",
        "%{LinceIncludeDir.glad}",
    }

    links {
        "GLAD",
        "glfw",
    }

    libdirs {
        "bin/" .. LinceOutputDir .. "/glad",
        "bin/" .. LinceOutputDir .. "/glfw",
    }


project "miniaudio"
    kind "StaticLib"
    language "C"
    staticruntime "on"
    location "build/miniaudio"

    objdir ("obj/" .. LinceOutputDir .. "/%{prj.name}" )
    targetdir ("bin/" .. LinceOutputDir .. "/%{prj.name}" )
    
    files {
        "deps/miniaudio/src/**.c",
        "deps/miniaudio/include/**.h",
    }
    
    includedirs {
        "deps/miniaudio/include",
    }

    links {
        "GLAD",
        "glfw",
    }

    libdirs {
        "bin/" .. LinceOutputDir .. "/glad",
        "bin/" .. LinceOutputDir .. "/glfw",
    }


project "lince"
    kind "StaticLib"
    language "C"
    staticruntime "on"
    location "build/lince"

    targetdir ("bin/" .. LinceOutputDir .. "/%{prj.name}")
    objdir ("obj/" .. LinceOutputDir .. "/%{prj.name}")

    defines "GLFW_INCLUDED_NONE"

    files {
        "lince/src/**.c",
        "lince/src/**.h",
    }
    
    includedirs {
        "lince",
		"lince/src",
        "lince/src/lince",
        "%{LinceIncludeDir.glfw}",
        "%{LinceIncludeDir.glad}",
        "%{LinceIncludeDir.cglm}",
        "%{LinceIncludeDir.nuklear}",
        "%{LinceIncludeDir.stb}",
        "%{LinceIncludeDir.miniaudio}"
    }

    links {
        "glad",
        "glfw",
        "cglm",
        "nuklear",
        "stb",
        "miniaudio"
    }

    libdirs {
        "bin/" .. LinceOutputDir .. "/glad",
        "bin/" .. LinceOutputDir .. "/glfw",
        "bin/" .. LinceOutputDir .. "/nuklear",
        "bin/" .. LinceOutputDir .. "/cglm",
        "bin/" .. LinceOutputDir .. "/stb",
        "bin/" .. LinceOutputDir .. "/miniaudio",
    }
