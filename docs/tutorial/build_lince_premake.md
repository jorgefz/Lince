# Build Lince with Premake

Premake5 is a tool to generate project files for different platforms and build systems.

### 1. Download and install Premake5 from https://premake.github.io/.

### 2. Run premake to generate build files

#### Windows

1. Run `premake5 vs2022` from the top-level directory
2. Open `lince.sln` with Visual Studio 2022
3. Compile it

You should find the 

#### Linux

1. Install the following libraries:
```bash
sudo apt-get install make libx11-dev libxcursor-dev libxrandr-dev libxinerama-dev libxtst-dev libgl-dev uuid-dev`
```

2. Run `premake5 game2`
3. Compile it with `make lince`

After compilation, you will find the static library at `bin/{config}-{OS}-{architecture}/lince`
as either `liblince.a` on Linux or `lince.lib` on Windows.

Additionally, the main header file to include in your project is `lince/src/lince.h`.

