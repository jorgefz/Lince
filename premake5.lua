

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

include "deps.lua"

workspace "lince"
    architecture "x86_64"
    startproject "lince"
    warnings "Extra"
    
    -- ## YOUR DEFINES HERE ## --
    -- defines {"LINCE_PROFILE"}
    -- defines {"LINCE_DEBUG_MEMCHECK"}
    -- defines {"LINCE_ASSETS_PATH=\"../../../lince/assets\""}
    defines {"LINCE_DIR=\"" .. os.getcwd() .. "/\""}
    -----------------------------
    
    configurations {"Debug", "Release"}
    flags {"MultiProcessorCompile"}

    group "deps"
        include "deps/premake5.lua"
    group ""
    
    group "test"
        include "test/premake5.lua"
        include "benchmark/premake5.lua"
    group ""

    group "core"
        include "lince"
    group ""

    group "tools"
        include "editor/premake5.lua"
        include "sandbox"
    group ""

    group "demos"
        include "demos/premake5.lua"
    group ""