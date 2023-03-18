# Lince 0.7.0

Lince is a game engine written in C for making 2D games.
Lince is currently in very early development.

## Features

* 2D graphics
* Cross-platform (Windows and Linux)
* Integrated GUI (nuklear)
* Audio system (Miniaudio)
* Event system for user input
* Tilemaps and sprite animations
* Lightweight Entity Component System (ECS) with unlimited entities and components
* Tilemaps and sprite animations
* Collision system for rectangles.
* Built-in level and scene editor

## Documentation

You need to have Doxygen installed from https://www.doxygen.nl/ or via the command line on Linux:
```
sudo apt install doxygen
```
Then, simply run `doxygen`, and check out the HTML documentation on `doxydocs/html/index.html`.

## Dependencies

The project uses Premake5 by default as the build system (https://premake.github.io/).
It also requires OpenGL 4.0 or greater.

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

The game executable will be located at `bin/{configuration}-{system}-{architecture}/{project}`, e.g. `bin/Debug-Linux-x86_64/editor`. Currently, this project is being tested on Windows 11 (x64) and WSLg GNU/Linux Ubuntu-22.04 (x64).

This repository contains the source code for the following projects, which are built into binaries as part of the compilation process:

* GLFW: windowing system (https://glfw.org)
* GLAD: OpenGL loader (https://glad.dav1d.de/)
* CGLM: OpenGL vector and matrix mathematics (https://cglm.readthedocs.io/en/latest)
* stb\_image.h: Single-header image loader by Sean Barret (https://github.com/nothings/stb)
* Nucklear: ANSI C single-header UI library (https://github.com/Immediate-Mode-UI/Nuklear)
* Miniaudio: Audio playback and capture library (https://github.com/mackron/miniaudio)

The project makes use of `mdbook` to generate the documentation website (https://github.com/rust-lang/mdBook).


## Credit

This repository contains audio files obtained from freesound.org, which are licensed under the Creative Commons Attribution 4.0 license (https://creativecommons.org/licenses/by/4.0/)
