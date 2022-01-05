# Lince

An OpenGL game engine written in C, currently in development.

## Dependencies

* GLFW: windowing system (https://glfw.org)
* GLAD: OpenGL loader (https://glad.dav1d.de/)
* Premake5: build system
* CGLM: OpenGL vector and matrix mathematics (https://cglm.readthedocs.io/en/latest)

## Building Lince

On the root folder (where this README.md file is located), run `premake5` to output the building tool appropriate to your OS:

* For Unix: `premake5 gmake2`, then run `make`.
* For Windows: `premake5 vs2019`, then load the Visual Studio solution and compile.

The game executable will be located at `bin/{configuration}-{system}-{architexture}/lince`, e.g. `bin/Debug-Linux-x86_64/lince`. Currently, this project is being tested on Windows 10 (x64) and GNU/Linux Ubuntu-20.04 on WSL2 (x64).



