
project "test"
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
        "%{prj.name}",
        "%{prj.name}/src",
        "%{wks.location}/%{LinceIncludeDir.lince}",
        "%{wks.location}/%{LinceIncludeDir.glfw}",
        "%{wks.location}/%{LinceIncludeDir.glad}",
        "%{wks.location}/%{LinceIncludeDir.cglm}",
    }

    links {
        "lince",
        "glad",
        "glfw",
        "cglm",
        "nuklear",
        "stb"
    }
    libdirs {
        "%{wks.location}/bin/" .. LinceOutputDir .. "/lince"
    }

    filter "system:windows"
        systemversion "latest"
        defines {"_CRT_SECURE_NO_WARNINGS", "LINCE_WINDOWS"}
        buildoptions {"/Zc:preprocessor"}
        links {"opengl32","cmocka.dll"}

    filter "system:linux"
        systemversion "latest"
        defines {"LINCE_LINUX"}
        links {"GL","rt","m","dl","pthread","X11","uuid","cmocka"}
        
    filter "configurations:Debug"
        symbols "on"
        defines {"LINCE_DEBUG"}

    filter "configurations:Release"
        optimize "on"
        defines {"LINCE_RELEASE"}
