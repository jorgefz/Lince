
workspace "lince"
    architecture "x86_64"
    startproject "editor"
    warnings "Extra"
    
    configurations {"Debug", "Release"}
    filter "system:windows"
        systemversion "latest"
        defines {"_CRT_SECURE_NO_WARNINGS"}
        links {"opengl32"}

    filter "system:linux"
        systemversion "latest"    
        links {"GL","rt","m","dl","pthread","X11"}
        
    filter "configurations:Debug"
        symbols "on"
        defines {"LINCE_DEBUG"}

    filter "configurations:Release"
        optimize "on"
        defines {"LINCE_RELEASE"}

    include "deps.lua"
    include "projects.lua"