@echo off

set "proj=%1"
IF not defined proj GOTO error

echo Building project %proj%
devenv lince.sln /Project build/%proj%/%proj%.vcxproj /Build "Debug|x64"

:error
echo Error: provide project name
exit /b