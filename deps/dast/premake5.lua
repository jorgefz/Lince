workspace "dast"
    configurations {
        "Arch32",
        "Arch32-NoSTD",
        "Arch64",
        "Arch64-NoSTD"
    }

    flags {"MultiProcessorCompile"}
    startproject "test"
    warnings "Extra"

    filter "configurations:Arch32*"
        architecture "x86"

    filter "configurations:Arch64*"
        architecture "x86_64"

    filter "configurations:*NoSTD*"
        defines { "DAST_NO_STDLIB" }

    filter ""

    OutputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "dast"
    kind "StaticLib"
    language "C"
    cdialect "C99"
    staticruntime "on"
    location "build/%{prj.name}"
    objdir ("obj/" .. OutputDir .. "/%{prj.name}" )
    targetdir ("bin/" .. OutputDir .. "/%{prj.name}" )
    files { "src/**.c", "include/**.h" }
    includedirs { "include" }
    filter { "system:linux", "action:gmake2" }
        buildoptions {"-pedantic" }

project "test"
    kind "ConsoleApp"
    language "C"
    cdialect "C99"
    location "build/%{prj.name}"
    objdir ("obj/" .. OutputDir .. "/%{prj.name}" )
    targetdir ("bin/" .. OutputDir .. "/%{prj.name}" )
    files { "test/**.c", "test/**.h" }
    links { "dast" }
    includedirs { "include" }

    filter "system:linux"
        links {"cmocka"}
    filter "system:windows"
        links {"cmocka.dll"}

project "extras"
    kind "ConsoleApp"
    language "C"
    cdialect "C99"
    location "build/%{prj.name}"
    objdir ("obj/" .. OutputDir .. "/%{prj.name}" )
    targetdir ("bin/" .. OutputDir .. "/%{prj.name}" )
    files { "extras/**.c", "extras/**.h" }
    links { "dast" }
    includedirs { "include" }
