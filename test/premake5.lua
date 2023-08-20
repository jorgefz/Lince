
-- newoption {
--     trigger = "vcpkg-root",
--     value       = "path",
--     description = "Root folder of VCPKG"
-- }
-- 
-- if _OPTIONS['vcpkg-root'] then
--     local cmocka_dll = _OPTIONS['vcpkg-root'] .. "/installed/x64-windows/debug/bin"
--     local cmocka_include = _OPTIONS['vcpkg-root'] .. "/installed/x64-windows/include"
--     if os.isfile(cmocka_dll .. "/cmocka.dll") == false then
--         print("Failed to locate 'cmocka.dll'")
--         print("Expected at '%s'", cmocka_dll .. "/cmocka.dll")
--         cmocka_dll = ""
--     end
--     if os.isfile(cmocka_include .. "/cmocka.h") == false then
--         print("Failed to locate 'cmocka.h'")
--         print("Expected at '%s'", cmocka_include .. "/cmocka.h")
--     end
-- end


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
