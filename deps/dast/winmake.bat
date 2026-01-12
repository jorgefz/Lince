rem Helper script to compile DAST with VStudio on Windows.
rem ---
rem Requires setting up MSVC in the command line,
rem either by running from a "Developer Command Prompt"
rem or by setting up the environment variables.
rem See: https://learn.microsoft.com/en-us/cpp/build/building-on-the-command-line
rem --- 
rem USAGE: winmake.bat [PROJECT] ([ARCH]) (NoSTD)
rem     Project: project name
rem     Architecture: (optional) 32 or 64, default is 64
rem     NoSTDLib: (optional) exclude C standard library.
rem         Any value is valid; leave empty to include the std lib.
rem ---
rem EXAMPLES:
rem     winmake.bat test 32
rem     winmake.bat test 64 NoSTD

@echo off

set "proj=%1"
IF not defined proj GOTO error_proj

set "arch=%2"
IF not defined arch set "arch=64"
set "bad_arch=1"
IF %arch%==32 goto setup_arch32
IF %arch%==64 goto setup_arch64
goto setup_arch_end

:setup_arch32
    set "bad_arch=0"
    set "arch2=Win32"
    goto setup_arch_end
:setup_arch64
    set "bad_arch=0"
    set "arch2=x64"
    goto setup_arch_end
:setup_arch_end

IF %bad_arch%==1 GOTO error_arch

set "stdlib=%3"
IF defined stdlib set "stdlib=-NoSTD"

set "projfile=build/%proj%/%proj%.vcxproj"
IF NOT EXIST "%projfile%" GOTO error_file

set "conf=Arch%arch%%stdlib%|%arch2%"
set conf="%conf%"

echo Building %proj% with configuration %conf%
devenv dast.sln /Project build/%proj%/%proj%.vcxproj /Build %conf%
exit /b

:error_proj
echo Usage: winmake.bat [Project] ([Architecture]) ([NoSTDLib])
echo     Project: project name
echo     Architecture: (optional) 32 or 64, default is 64
echo     NoSTDLib: (optional) any value, exclude C standard library (leave empty to include it)
exit /b

:error_file
echo Error: project '%proj%' does not exist
exit /b

:error_arch
echo Error: architecture must be '32' or '64' ('%arch%' given)
exit /b