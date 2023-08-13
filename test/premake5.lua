
externalproject "cmocka"
    -- Run CMake for cmocka
    location "%{wks.location}/build/%{prj.name}"
    os.executef(
        "cmake -B %s/../build/cmocka -S %s/../deps/cmocka",
        os.getcwd(), os.getcwd()
    )
    uuid "57940020-8E99-AEB6-271F-61E0F7F6B73B"
    kind "SharedLib"
    language "C"
    targetdir ("%{wks.location}/build/cmocka/src")


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
        "%{wks.location}/deps/cmocka/include",
        "%{prj.name}",
        "%{prj.name}/src",
        "%{wks.location}/%{LinceIncludeDir.lince}",
        "%{wks.location}/%{LinceIncludeDir.glfw}",
        "%{wks.location}/%{LinceIncludeDir.glad}",
        "%{wks.location}/%{LinceIncludeDir.cglm}",
    }

    links {
        "cmocka",
        "lince",
        "glad",
        "glfw",
        "cglm",
        "nuklear",
        "stb"
    }

    libdirs {
        "%{wks.location}/build/cmocka/src",
        "%{wks.location}/bin/" .. LinceOutputDir .. "/lince"
    }

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
