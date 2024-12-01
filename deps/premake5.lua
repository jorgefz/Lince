

project "glfw"
    kind "StaticLib"
    language "C"
    staticruntime "on"
    location "%{wks.location}/build/%{prj.name}"

    targetdir ("%{wks.location}/bin/" .. LinceOutputDir .. "/%{prj.name}")
    objdir ("%{wks.location}/obj/" .. LinceOutputDir .. "/%{prj.name}")

    local SrcDir = "%{prj.name}/src"
    
    files {
        "%{prj.name}/include/GLFW/glfw3.h",
        "%{prj.name}/include/GLFW/glfw3native.h",
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
    location "%{wks.location}/build/%{prj.name}"

    targetdir ("%{wks.location}/bin/" .. LinceOutputDir .. "/%{prj.name}")
    objdir ("%{wks.location}/obj/" .. LinceOutputDir .. "/%{prj.name}")

    files {
        "%{prj.name}/include/glad/glad.h",
        "%{prj.name}/include/KHR/khrplatform.h",
        "%{prj.name}/src/glad.c"
    }

    includedirs {"%{prj.name}/include"}

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
    location "%{wks.location}/build/%{prj.name}"

    targetdir ("%{wks.location}/bin/" .. LinceOutputDir .. "/%{prj.name}")
    objdir ("%{wks.location}/obj/" .. LinceOutputDir .. "/%{prj.name}")

    files {
        "%{prj.name}/src/**.c",
        "%{prj.name}/include/cglm/**.h"
    }

    includedirs {"%{prj.name}/include"}

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
    location "%{wks.location}/build/%{prj.name}"

    targetdir ("%{wks.location}/bin/" .. LinceOutputDir .. "/%{prj.name}")
    objdir ("%{wks.location}/obj/" .. LinceOutputDir .. "/%{prj.name}")

    files {
        "%{prj.name}/src/**.c",
        "%{prj.name}/include/**.h"
    }

    includedirs {"%{prj.name}/include"}

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
    location "%{wks.location}/build/%{prj.name}"

    objdir ("%{wks.location}/obj/" .. LinceOutputDir .. "/%{prj.name}" )
    targetdir ("%{wks.location}/bin/" .. LinceOutputDir .. "/%{prj.name}" )
    
    defines "GLFW_INCLUDED_NONE"

    files {
        "%{prj.name}/src/**.c",
        "%{prj.name}/include/**.h",
    }
    
    includedirs {
        "%{prj.name}",
        "%{prj.name}/src",
        "%{wks.location}/%{LinceIncludeDir.nuklear}",
        "%{wks.location}/%{LinceIncludeDir.glfw}",
        "%{wks.location}/%{LinceIncludeDir.glad}",
    }

    links {"glad", "glfw"}

    libdirs {
        "%{wks.location}/bin/" .. LinceOutputDir .. "/glad",
        "%{wks.location}/bin/" .. LinceOutputDir .. "/glfw",
    }


project "miniaudio"
    kind "StaticLib"
    language "C"
    staticruntime "on"
    location "%{wks.location}/build/%{prj.name}"

    objdir ("%{wks.location}/obj/" .. LinceOutputDir .. "/%{prj.name}" )
    targetdir ("%{wks.location}/bin/" .. LinceOutputDir .. "/%{prj.name}" )
    
    files {
        "%{prj.name}/src/**.c",
        "%{prj.name}/include/**.h",
    }
    
    includedirs {
        "%{prj.name}/include",
    }

    links {"glad", "glfw"}

    libdirs {
        "%{wks.location}/bin/" .. LinceOutputDir .. "/glad",
        "%{wks.location}/bin/" .. LinceOutputDir .. "/glfw",
    }


project "toml"
    kind "StaticLib"
    language "C"
    staticruntime "on"
    location "%{wks.location}/build/%{prj.name}"

    objdir ("%{wks.location}/obj/" .. LinceOutputDir .. "/%{prj.name}" )
    targetdir ("%{wks.location}/bin/" .. LinceOutputDir .. "/%{prj.name}" )
    
    files {
        "%{prj.name}/src/**.c",
        "%{prj.name}/include/**.h",
    }
    
    includedirs {
        "%{prj.name}/include",
    }

    links {"glad", "glfw"}

    libdirs {
        "%{wks.location}/bin/" .. LinceOutputDir .. "/glad",
        "%{wks.location}/bin/" .. LinceOutputDir .. "/glfw",
    }
