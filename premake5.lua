
-- Folder name for compilation outputs inside 'build/' and 'bin/'.
LinceOutputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Location of dependencies
LinceIncludeDir = {}
LinceIncludeDir["glfw"]    = "deps/glfw/include"
LinceIncludeDir["glad"]    = "deps/glad/include"
LinceIncludeDir["cglm"]    = "deps/cglm/include"
LinceIncludeDir["nuklear"] = "deps/nuklear/include"
LinceIncludeDir["stb"]     = "deps/stb/include"
LinceIncludeDir["miniaudio"] = "deps/miniaudio/include"
LinceIncludeDir["lince"]   = "lince/src"


if _ACTION == "clean" then
    os.rmdir(os.getcwd() .. "/build")
    os.rmdir(os.getcwd() .. "/bin")
    os.rmdir(os.getcwd() .. "/obj")
    os.rmdir(os.getcwd() .. "/Makefile")
    os.rmdir(os.getcwd() .. "/docs/html")
    os.rmdir(os.getcwd() .. "/docs/latex")
    print("Action 'clean' finished")
    os.exit()
end


workspace "lince"
    architecture "x86_64"
    startproject "editor"
    warnings "Extra"
    
    -- defines{"LINCE_PROFILE"}
    defines {"LINCE_DIR=\"" .. os.getcwd() .. "/\""}
    print("LINCE_DIR=\"" .. os.getcwd() .. "/\"")
    
    
    configurations {"Debug", "Release"}
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

    include "deps.lua"
    include "projects.lua"
