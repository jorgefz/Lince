# Lince 0.5.0

A 2D OpenGL game engine written in C, currently in development.

It uses GLFW for the windowing system, GLAD for the OpenGL loader, Premake5 as the build system, and CGLM for vector and matrix operations.

## Features

* 2D graphics
* Cross-platform (Windows and Linux)
* Integrated GUI (nuklear)
* Audio system (Miniaudio)
* Event system for user input
* Tilemaps and sprite animations

## Documentation

The documentation is available [here](./docs/src/SUMMARY.md) as Markdown, or at `docs/book/index.html` which you can open with a web browser.

## Dependencies

The project uses Premake5 by default as the build system (https://premake.github.io/).

The project requires OpenGL 4.0 or greater.

On Linux, the following packages are required:
```bash
sudo apt install make libx11-dev libxcursor-dev libxrandr-dev libxinerama-dev libxtst-dev libgl-dev`
```

If you are using Windows Subsystem for Linux on Windows 11 (WSLg), as of December 2022, you must manually connect ALSA to PulseAudio following the steps detailed on this thread: https://github.com/microsoft/wslg/issues/864.

On Windows, Visual Studo 2022 is recommended.


## Building Lince

On the root folder (where this README.md file is located), run `premake5` to produce the build configuration files appropriate to your building tool:

* For Unix: `premake5 gmake2`, then run `make`.
* For Windows: `premake5 vs2020`, then load the Visual Studio solution and compile.

The game executable will be located at `bin/{configuration}-{system}-{architecture}/{project}`, e.g. `bin/Debug-Linux-x86_64/pong`. Currently, this project is being tested on Windows 11 (x64) and WSLg GNU/Linux Ubuntu-22.04 (x64).

The project contains the source code for the following packages, which are built into binaries as part of the compilation process:

* GLFW: windowing system (https://glfw.org)
* GLAD: OpenGL loader (https://glad.dav1d.de/)
* CGLM: OpenGL vector and matrix mathematics (https://cglm.readthedocs.io/en/latest)
* stb\_image.h: Single-header image loader by Sean Barret (https://github.com/nothings/stb)
* Nucklear: ANSI C single-header UI library (https://github.com/Immediate-Mode-UI/Nuklear)

The project makes use of `mdbook` to generate the documentation website (https://github.com/rust-lang/mdBook).


## Credit

This repository contains audio files obtained from freesound.org, which are licensed under the Creative Commons Attribution 4.0 license (https://creativecommons.org/licenses/by/4.0/)
