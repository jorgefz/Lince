
project "cglm"
    
    kind "StaticLib"
    language "C"
    staticruntime "On"

    targetdir ("bin/" .. outputdir)
    objdir ("obj/" .. outputdir)

    files {
        "src/**.c",
        "include/cglm/**.h"
    }

    includedirs {
        "include"
    }

    defines {
        "CGLM_STATIC"
    }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"


