# Lince 0.1.0

An OpenGL game engine written in C, currently in development. It uses GLFW for the windowing system, GLAD for the OpenGL loader, Premake5 as the build system, and CGLM for vector and matrix operations.

## Documentation

The documentation is available [here](./docs/src/SUMMARY.md) as Markdown, or at `docs/book/index.html` which you can open with your favourite web browser.

## Dependencies

The project uses Premake5 as the build system (https://premake.github.io/).

On Linux, the following are required:

* make
* libx11-dev
* libxcursor-dev
* libxrandr-dev
* libxinerama-dev
* libxtst-dev
* libgl-dev

On Windows, Windows SDK is required.

## Building Lince

On the root folder (where this README.md file is located), run `premake5` to output the building tool appropriate to your OS:

* For Unix: `premake5 gmake2`, then run `make`.
* For Windows: `premake5 vs2019`, then load the Visual Studio solution and compile.

The game executable will be located at `bin/{configuration}-{system}-{architecture}/lince`, e.g. `bin/Debug-Linux-x86_64/lince`. Currently, this project is being tested on Windows 10 (x64) and GNU/Linux Ubuntu-20.04 on WSL2 (x64).

The project contains the source code for the following packages, which are built into binaries as part of the compilation process:

* GLFW: windowing system (https://glfw.org)
* GLAD: OpenGL loader (https://glad.dav1d.de/)
* CGLM: OpenGL vector and matrix mathematics (https://cglm.readthedocs.io/en/latest)
* stb\_image.h: Single-header image loader by Sean Barret (https://github.com/nothings/stb)
* Nucklear: ANSI C single-header UI library (https://github.com/Immediate-Mode-UI/Nuklear)

The project makes use of `mdbook` to generate the documentation website (https://github.com/rust-lang/mdBook)



