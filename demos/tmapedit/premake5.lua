
project "nativefiledialog"
    kind "StaticLib"
    staticruntime "on"
    location "%{wks.location}/build/tmapedit"

    targetdir ("%{wks.location}/bin/" .. LinceOutputDir .. "/tmapedit")
    objdir ("%{wks.location}/obj/" .. LinceOutputDir .. "/tmapedit")

    files {"nativefiledialog/src/*.h",
           "nativefiledialog/src/include/*.h",
           "nativefiledialog/src/nfd_common.c",
    }

    includedirs {"nativefiledialog/src/include/"}

    warnings "extra"

    filter "system:windows"
      language "C++"
      files {"nativefiledialog/src/nfd_win.cpp"}

    filter {"system:linux"}
      language "C"
      files {"nativefiledialog/src/nfd_zenity.c"}

    filter {"action:gmake"}
      buildoptions {"-fno-exceptions"}

    filter "configurations:Debug"
        defines {"DEBUG"}
        symbols "On"

    filter "configurations:Release"
        defines {"NDEBUG"}
        optimize "On"

    -- visual studio filters
    filter "action:vs*"
      defines { "_CRT_SECURE_NO_WARNINGS" }  



project "tmapedit"
    kind "ConsoleApp"
    language "C"
    staticruntime "on"
    location "%{wks.location}/build/%{prj.name}"
    
    targetdir ("%{wks.location}/bin/" .. LinceOutputDir .. "/%{prj.name}")
    objdir ("%{wks.location}/obj/" .. LinceOutputDir .. "/%{prj.name}")

    files {
        "src/**.c",
        "src/**.h",
    }
    
    includedirs {
		"src",
        "%{wks.location}/%{LinceIncludeDir.lince}",
        "%{wks.location}/%{LinceIncludeDir.glfw}",
        "%{wks.location}/%{LinceIncludeDir.glad}",
        "%{wks.location}/%{LinceIncludeDir.cglm}",
        "%{wks.location}/%{LinceIncludeDir.nuklear}",
        "%{wks.location}/%{LinceIncludeDir.stb}",
        "%{wks.location}/%{LinceIncludeDir.miniaudio}",
        "%{wks.location}/%{LinceIncludeDir.dast}",
        "%{wks.location}/%{LinceIncludeDir.toml}",
        "%{wks.location}/demos/tmapedit/nativefiledialog/src/include"
    }

    links {
        "lince",
        "glad",
        "glfw",
        "cglm",
        "stb",
        "nuklear",
        "miniaudio",
        "dast",
        "toml",
        "nativefiledialog"
    }

    libdirs {"%{wks.location}/bin/" .. LinceOutputDir .. "/lince"}

    filter "system:windows"
        systemversion "latest"
        defines {"_CRT_SECURE_NO_WARNINGS", "LINCE_WINDOWS"}
        buildoptions {"/Zc:preprocessor"}
        links {"opengl32"}

    filter "system:linux"
        systemversion "latest"    
        links {"GL","rt","m","dl","pthread","X11","uuid"}
        defines {"LINCE_LINUX"}
        
    filter "configurations:Debug"
        symbols "on"
        defines {"LINCE_DEBUG"}

    filter "configurations:Release"
        optimize "on"
        defines {"LINCE_RELEASE"}