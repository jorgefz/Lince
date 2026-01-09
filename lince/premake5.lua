
project "lince"
    kind "StaticLib"
    language "C"
    staticruntime "on"
    location "%{wks.location}/build/%{prj.name}"

    targetdir ("%{wks.location}/bin/" .. LinceOutputDir .. "/%{prj.name}")
    objdir ("%{wks.location}/obj/" .. LinceOutputDir .. "/%{prj.name}")

    defines {"GLFW_INCLUDED_NONE"}

    files {
        "src/**.c",
        "src/**.h",
    }

    removefiles {
        "src/%{prj.name}/platform/**"
    }
    
    includedirs {
        ".",
		"src",
        "src/lince",
        "%{wks.location}/%{LinceIncludeDir.glfw}",
        "%{wks.location}/%{LinceIncludeDir.glad}",
        "%{wks.location}/%{LinceIncludeDir.cglm}",
        "%{wks.location}/%{LinceIncludeDir.nuklear}",
        "%{wks.location}/%{LinceIncludeDir.stb}",
        "%{wks.location}/%{LinceIncludeDir.miniaudio}",
        "%{wks.location}/%{LinceIncludeDir.toml}",
        "%{wks.location}/%{LinceIncludeDir.dast}",
    }

    links {
        "glad",
        "glfw",
        "cglm",
        "nuklear",
        "stb",
        "miniaudio",
        "toml",
        "dast"
    }

    libdirs {
        "%{wks.location}/bin/" .. LinceOutputDir .. "/glad",
        "%{wks.location}/bin/" .. LinceOutputDir .. "/glfw",
        "%{wks.location}/bin/" .. LinceOutputDir .. "/nuklear",
        "%{wks.location}/bin/" .. LinceOutputDir .. "/cglm",
        "%{wks.location}/bin/" .. LinceOutputDir .. "/stb",
        "%{wks.location}/bin/" .. LinceOutputDir .. "/miniaudio",
        "%{wks.location}/bin/" .. LinceOutputDir .. "/toml",
        "%{wks.location}/bin/" .. LinceOutputDir .. "/dast",
    }

    filter "system:windows"
        systemversion "latest"
        defines {"_CRT_SECURE_NO_WARNINGS", "LINCE_WINDOWS"}
        buildoptions {"/Zc:preprocessor"}
        links {"opengl32","rpcrt4"}
        files {
            "src/%{prj.name}/platform/windows/**.c",
            "src/%{prj.name}/platform/windows/**.h"
        }

    filter "system:linux"
        systemversion "latest"    
        links {"GL","rt","m","dl","pthread","X11","uuid"}
        defines {"LINCE_LINUX"}
        files {
            "src/%{prj.name}/platform/linux/**.c",
            "src/%{prj.name}/platform/linux/**.h"
        }
        
    filter "configurations:Debug"
        symbols "on"
        defines {"LINCE_DEBUG"}

    filter "configurations:Release"
        optimize "on"
        defines {"LINCE_RELEASE"}