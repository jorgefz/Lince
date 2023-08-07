
externalproject "cmocka"
    -- Run CMake for cmocka
    os.execute("cmake -B "..os.getcwd().."/build/cmocka -S "..os.getcwd().."/deps/cmocka")   
    location "build/cmocka"
    uuid "57940020-8E99-AEB6-271F-61E0F7F6B73B"
    kind "SharedLib"
    language "C"
    targetdir ("build/cmocka/src")


project "tests"
    kind "ConsoleApp"
    language "C"
    staticruntime "on"
    location "build/tests"

    targetdir ("bin/" .. LinceOutputDir .. "/%{prj.name}")
    objdir ("obj/" .. LinceOutputDir .. "/%{prj.name}")

    files {
        "%{prj.name}/src/**.c",
        "%{prj.name}/src/**.h",
    }
    
    includedirs {
        "deps/cmocka/include",
        "%{prj.name}",
        "%{prj.name}/src",
        "%{LinceIncludeDir.lince}",
        "%{LinceIncludeDir.glfw}",
        "%{LinceIncludeDir.glad}",
        "%{LinceIncludeDir.cglm}",
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
        "build/cmocka/src",
        "bin/" .. LinceOutputDir .. "/lince"
    }


project "editor"
    kind "ConsoleApp"
    language "C"
    staticruntime "on"
    location "build/editor"

    targetdir ("bin/" .. LinceOutputDir .. "/%{prj.name}")
    objdir ("obj/" .. LinceOutputDir .. "/%{prj.name}")

    files {
        "%{prj.name}/src/**.c",
        "%{prj.name}/src/**.h",
    }
    
    includedirs {
        "%{prj.name}",
		"%{prj.name}/src",
        "%{LinceIncludeDir.lince}",
        "%{LinceIncludeDir.glfw}",
        "%{LinceIncludeDir.glad}",
        "%{LinceIncludeDir.cglm}",
        "%{LinceIncludeDir.nuklear}",
        "%{LinceIncludeDir.stb}"
    }

    links {
        "lince",
        "glad",
        "glfw",
        "cglm",
        "stb",
        "nuklear"
    }

    libdirs {"bin/" .. LinceOutputDir .. "/lince"}


project "sandbox"
    kind "ConsoleApp"
    language "C"
    staticruntime "on"
    location "build/sandbox"
    
    targetdir ("bin/" .. LinceOutputDir .. "/%{prj.name}")
    objdir ("obj/" .. LinceOutputDir .. "/%{prj.name}")

    files {
        "%{prj.name}/src/**.c",
        "%{prj.name}/src/**.h",
    }
    
    includedirs {
        "%{prj.name}",
		"%{prj.name}/src",
        "%{LinceIncludeDir.lince}",
        "%{LinceIncludeDir.glfw}",
        "%{LinceIncludeDir.glad}",
        "%{LinceIncludeDir.cglm}",
        "%{LinceIncludeDir.nuklear}",
        "%{LinceIncludeDir.stb}",
        "%{LinceIncludeDir.miniaudio}"
    }

    links {
        "lince",
        "glad",
        "glfw",
        "cglm",
        "stb",
        "nuklear",
        "miniaudio"
    }

    libdirs {"bin/" .. LinceOutputDir .. "/lince"}