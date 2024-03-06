@echo off

set "proj=%1"
IF not defined proj GOTO error

set "projfile=build/%proj%/%proj%.vcxproj"
IF not exist projfile GOTO error2

echo Building project %proj%
devenv lince.sln /Project build/%proj%/%proj%.vcxproj /Build "Debug|x64"
exit /b

:error
echo Error: provide project name
exit /b

:error2
echo Error: project '%proj%' does not exist
exit /b