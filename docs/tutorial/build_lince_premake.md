# Build Lince with Premake

Premake5 is a tool to generate project files for different platforms and build systems.

### 1. Download and install Premake5

Download Premake5 by following the link https://premake.github.io/.
Download the binary file appropriate to your system and place it
in a location easily accessible from 

### 2. Generate build files with Premake

#### Windows

1. Run `premake5 vs2022` from the top-level directory
2. Open the output VS solution `lince.sln` with Visual Studio 2022

#### Linux

1. Install the following libraries:
```bash
sudo apt-get install make libx11-dev libxcursor-dev libxrandr-dev libxinerama-dev libxtst-dev libgl-dev uuid-dev`
```
2. Run `premake5 gmake2` to generate the Makefiles

### 3. Compile lince into a static library

#### Windows

1. Open `lince.sln` with Visual Studio 2022
2. Right click on the project `core/lince` and select "Build"
3. You will find the static library in `bin/Debug-windows-x86_64/lince/lince.lib`

#### Lince

1. Compile with GNU Make by running `make lince`
2. You will find the static library in `bin/Debug-linux-x86_64/lince/liblince.a`
3. Compile it with `make lince`

### 4. Compile and test the demos

#### Windows

1. Open `lince.sln` with Visual Studio 2022
2. Right click on the `demos` group and select "Build"
3. You will find the executables in `bin/Debug-windows-x86_64/xxx_demo/xxx_demo.exe` where `xxx` is the name of each demo.
4. Run one of the demo executables to test it out.

### Lince

1. Compile the demos with GNU Make by running `make demo`
2. You will find the executables in `bin/Debug-linux-x86_64/xxx_demo/xxx_demo.exe` where `xxx` is the name of each demo.

