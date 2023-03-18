

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
        "%{IncludeDir.stb}",
        "%{IncludeDir.miniaudio}"
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

    libdirs {"bin/" .. OutputDir .. "/lince"}