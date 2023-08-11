-- Folder name for compilation outputs inside 'build/' and 'bin/'.
LinceOutputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Dependencies' include directories
LinceIncludeDir = {}
LinceIncludeDir["glfw"]      = "deps/glfw/include"
LinceIncludeDir["glad"]      = "deps/glad/include"
LinceIncludeDir["cglm"]      = "deps/cglm/include"
LinceIncludeDir["nuklear"]   = "deps/nuklear/include"
LinceIncludeDir["stb"]       = "deps/stb/include"
LinceIncludeDir["miniaudio"] = "deps/miniaudio/include"
LinceIncludeDir["lince"]     = "lince/src"
