
workspace "lince"
    architecture "x86_64"
    startproject "editor"
    warnings "Extra"
    
    defines {"LINCE_DIR=\"" .. os.getcwd() .. "/\""}
    print("LINCE_DIR=\"" .. os.getcwd() .. "/\"")
    
    configurations {"Debug", "Release"}
    filter "system:windows"
        systemversion "latest"
        defines {"_CRT_SECURE_NO_WARNINGS", "LINCE_WINDOWS"}
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