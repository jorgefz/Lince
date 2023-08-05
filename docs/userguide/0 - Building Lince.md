# User Guide Issue #0: Building Lince

The build system for Lince is generated with `premake5` using the script file `premake5.lua` in the top-level directory of this repository.

To build and compile Lince into a static library, run `premake5` followed by your target build system, e.g. `premake5 gmake2` for linux Makefiles or `premake5 vs2022` for Windows Visual Studio.

This will generate either Makefiles, which you can run with the command `make lince`, or a Visual Studio solution `{solution}.sln`. After compilation, you will find the static library at `bin/{config}-{OS}-{architecture}/lince/liblince.a` which you may link with your project. Additionally, the include directory where the header files are located is at `lince/src/lince.h`.


```
my_game/
    lince/
        *this repository*
        premake5.lua
    src/
    premake5.lua
```
