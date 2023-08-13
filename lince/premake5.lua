
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
    
    includedirs {
        ".",
		"src",
        "src/lince",
        "%{wks.location}/%{LinceIncludeDir.glfw}",
        "%{wks.location}/%{LinceIncludeDir.glad}",
        "%{wks.location}/%{LinceIncludeDir.cglm}",
        "%{wks.location}/%{LinceIncludeDir.nuklear}",
        "%{wks.location}/%{LinceIncludeDir.stb}",
        "%{wks.location}/%{LinceIncludeDir.miniaudio}"
    }

    links {
        "glad",
        "glfw",
        "cglm",
        "nuklear",
        "stb",
        "miniaudio",
        "uuid"
    }

    libdirs {
        "%{wks.location}/bin/" .. LinceOutputDir .. "/glad",
        "%{wks.location}/bin/" .. LinceOutputDir .. "/glfw",
        "%{wks.location}/bin/" .. LinceOutputDir .. "/nuklear",
        "%{wks.location}/bin/" .. LinceOutputDir .. "/cglm",
        "%{wks.location}/bin/" .. LinceOutputDir .. "/stb",
        "%{wks.location}/bin/" .. LinceOutputDir .. "/miniaudio",
    }

    filter "system:windows"
        systemversion "latest"
        defines {"_CRT_SECURE_NO_WARNINGS", "LINCE_WINDOWS"}
        buildoptions {"/Zc:preprocessor"}
        links {"opengl32"}

    filter "system:linux"
        systemversion "latest"    
        links {"GL","rt","m","dl","pthread","X11"}
        defines {"LINCE_LINUX"}
        
    filter "configurations:Debug"
        symbols "on"
        defines {"LINCE_DEBUG"}

    filter "configurations:Release"
        optimize "on"
        defines {"LINCE_RELEASE"}