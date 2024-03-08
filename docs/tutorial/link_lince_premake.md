# Create a Lince project with Premake

To create a new application that uses Lince with Premake:

### 1. Create a new folder where your project will be located

```
lince/
docs/
demos/
...
my_project/
```

### 2. Copy the contents of the blank (or any other) demo located in the folder `demos` to your project folder

The top-level directory should look like this:

```
lince/
docs/
demos/
...
my_project/
	|-> premake5.lua
	|-> src/
		|-> main.c
```

### 3. In `my_project/premake5.lua`, change the name of the project to that of your project folder

The first line of the file `my_project/premake5.lua` should look like this:
```
project "my_project"
	...
```

### 4. Include the location of your project's premake script at the bottom of the top-level premake script

The last line of the file `premake5.lua` should look like this:
```
...
include "my_project/premake5.lua"
```

### 5. Run `premake5` again and build your project.

Follow the instructions in [this tutorial](build_lince_premake.md) to compile your Premake5 project.
You will find your project compiled into an executable in the folder `bin/{config}-{os}-{arch}/my_project/my_project.exe`.

Additionally, the main Lince header file to include in your project is in `lince/src/lince.h`.
