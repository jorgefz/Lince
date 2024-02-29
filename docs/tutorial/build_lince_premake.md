# Build Lince with Premake

Premake5 is a tool to generate project files for different platforms and build systems.

### 1. Download and install Premake5 from https://premake.github.io/.

### 2. Run premake to generate build files

To build Lince into a static library with Premake, run `premake5` from the top-level directory with one of the following options:

* On Windows, run `premake5 vs2022` and open `lince.sln` with Visual Studio 2022.
* On Linux, run `premake5 game2` to generate the Makefiles.

### 3. Install dependencies

On Linux, you'll need to install the following libraries:
```bash
sudo apt-get install make libx11-dev libxcursor-dev libxrandr-dev libxinerama-dev libxtst-dev libgl-dev uuid-dev`
```

On Windows, Visual Studio 2022 is required.

### 4. Compile Lince

On Windows, open `lince.sln` with Visual Studio 2022, right click on the `core/lince` project, and click Build.

On Linux, execute the command `make lince`.

After compilation, you will find the static library at `bin/{config}-{OS}-{architecture}/lince`
as either `liblince.a` on Linux or `lince.lib` on Windows.

Additionally, the main header file to include in your project is `lince/src/lince.h`.

