# Version History

Version codes follow [Semantic Versioning](https://semver.org/).

## To-Do

- Rename asset manager to e.g. asset cache.
- Create functions that don't depend on OpenGL, e.g. `LinceImage` for storing image data, `LinceClock` for timers, etc. 
- Rename functions in `app.c` to begin with `LinceApp...`.
- Make font system more dynamic

- Merge all key events into single struct. Add 'mods' to keys (e.g. Ctrl)
- Move misc functions to separate files (`LinceReadFile`, `LinceGetTimeMillis`). 
- Nuklear UI wrapper and/or custom docs.


## v0.8.1 (WIP)
- Added new ECS 
- Added LinceTransform and transform functions
- Added LincePoint and coordinate conversion functions
- Added tests
- Replaced usage of vec2 in app.h functions with LincePoint
- Added function to hash arbitrary bytes, in preparation to make a hashmap of bitmasks to introduce archetypes to ECS.
- Added functionality to hashmap to use byte keys of arbitrary length.
- Improved benchmarking macros in testing
- Moved benchmarking to separate project
- Added benchmarking of new and old ECS, array, and hashmap.
- Added demos
- Added tutorials
- Renames `LinceBeginScene` and `LinceEndScene` to `LinceBeginRender` and `LinceEndRender`.
- Fixed bug on `GetMousePos` with float pointers treated as double pointers.
- Fixed `GetMouseX` and `GetMouseY` not returning a value.
- Added LinceClock
- Improved profiler
- Renamed some App functions
- Renamed asset manager to asset cache


## v0.8.0
- Added CMocka as the testing framework.
- Added UUIDs.
- Added z-sorting with translucency by sorting sprites before drawing
- Added scene system.
- Improved premake scripts. Placed separate 'premak5.lua' scripts in the folder of each project with all build commands necessary to build it. This allows users to create projects that depend on Lince much more easily.
- Added rudimentary asset manager. Assets are now sought from a stack of asset folders, whose locations are defined relative to the path of the running executable. This allows the executable to find the application assets when called from anywhere in the system.
- `LINCE_DIR` now generated in premake5 file automatically from any path.
- Changed scope of `LINCE_DIR` to the directory where output files are generated (e.g. logging, profiling). By default, this is the location from which the executable is called.
- Added functions to initialise camera without allocating heap memory.
- Improved error message when font fails to load
- Removed `LinceCreateTexture`
- Removed `name` parameter from `LinceTexture` functions.
- Removed `name` parameter from `LinceShader` functions.
- Change naming of event-related variable names from past tense (e.g. pressed) to present tense (.e.g press), and shortened "mouse button" to simply "mouse". This affects `LinceEventType` enum values, `LinceEventData` union members, event struct names, and event creation functions.
- Renamed application callback variables e.g. from `game_on_update` to `on_update`.
- Fixed bug where passing a NULL value as the filename for the log file would result in undefined behaviour.
- Added functions to open and close a profiling file, instead of providing a file handle directly.
- Added logging platform name in log file.
- Renamed `LINCE_STR_MAX` to `LINCE_TEXT_MAX`.
- Renamed function `LinceGetAppState` to `LinceGetApp`.
- Replaced instances of standard C functions `malloc`,`calloc`,`realloc`, and `free` with the engine versions.
- Renamed premake5 script variables `OutputDir` to `LinceOutputDir`, and `IncludeDir` to `LinceIncludeDir`.
- Fixed bug in editor where entity registry was initalised with the wrong number of components.
- Added 'clean' action to premake5 that removes build outputs
- Fixed comaptibility with Windows by enforcing old MSVC preprocessor
- Fixed minimum required OpenGL version to 4.5.
- Renamed layer callbacks following snake case.
- Moved input code, and key and mouse codes to 'input' folder
- Improved layer stack to be more cache friendly. From array of pointers to array of plain layers.
- Added `fileio.h` functions for file IO: `LinceIsDir`, `LinceIsFile`, `LinceLoadFile`, and `LinceLoadTextFile.`
- Removed `LinceGetTimeMillis`.
- Added to-screen transform and improved to-world transform.

## v0.7.0
- Added support for custom shaders in renderer
- Moved minigames to a separate github repo
- Added logging
- Improved tile animations and tilemaps
- Re-made the editor
- Added Doxygen docs to the project

## v0.6.0
- Added ECS
- Improved array and hashmap initialisation API
- Added OpenGL version requirement to README
- Moved glfw error callback before window creation.
- Added printing Miniaudio debug info in debug configuration
- Updated README with instructions for audio playing on WSLg and required OpenGL version

## v0.5.0
- Added basic audio API
- Integrated miniaudio into the engine
- Restructured build scripts to output binaries to common top-level folders `bin` and `obj`.
- Moved output build files to `build` folder.
- Separated nuklear and stb\_image to their own projects that build to a static library.
- Separated `game` project into `editor` and `sandbox` projects.


## v0.4.2
- Added benchmarking and profiling tools in Python
- Added basic test framework
- Added hashmap, array, and linked list containers
- Added caching of shader uniform locations with hashmap


## v0.4.1
- Added function `LinceLoadTexture`, similar to `LinceCreateTexture` but with extra `flags` argument.
- Added texture flag to flip texture data vertically on load `LinceTexture_FlipY`.
- Updated docs
- Fixed bug where Nuklear would respond to all scrolling events, even if no widget was active.
- Embedded the renderer's shader source in the code, and removed the source files.


## v0.4.0
- Added simple tilemaps - `LinceTilemap`
- Simplified premake5 script somewhat
- Refactored Missile Command debug text as a movable and minimizable Nuklear window, as the text boxes where consuming the click events
- Added memory management functions: LinceMalloc, LinceNewCopy, etc
- Added allocation macros allowing for custom allocation functions - `LINCE_MALLOC`, `LINCE_CALLOC`, `LINCE_REALLOC`, and `LINCE_FREE`
- Added `LinceForeach` macro for iterating over array items
- Added calculation for z-order based on the y coordinate `LinceYSortedZ`

## v0.3.1
- Renamed and moved the 2D renderer shader files to 'engine/assets/shaders'
- Updated premake5 lua script
- Moved Pong and Missile Command to their own projects and folders
- Solved bug in Missile Command where texture would be freed twice
- Updated docs

## v0.3.0
- Added calculation of inverse view-projection matrix on camera update.
- Added transform from screen to world coordinates.
- Added function to retrieve screen size directly.
- Added function to retrieve mouse position in world coordinates.
- Solved bug where Nuklear UI events were not being flagged as handled.
- Moved tile animation and tileset headers to 'tiles' folder.
- Added tile animation and tileset headers to `lince.h`.
- Renamed 'lince' folder to 'engine'.
- Added assertion to ensure fonts are correctly loaded from file.
- Moved engine source to 'lince' folder within 'engine/src'.
- Fixed bug in 2D renderer where quads that were no longer being rendered persisted in the scene.

## v0.2.2
- Improved tile animations with custom animation orders.
- Improved documentation on tile animations

## v0.2.1
- Renamed `LinceDeleteAnim` to `LinceDeleteTileAnim`
- LinceTileAnim: added optional maximum number of repeats
- LinceTileAnim: added `on_repeat` and `on_finish` callbacks
- Solved issue with test tilemap where black lines appear between tiles
- Rebuilt html docs

## v0.2.0
- Added basic tile animations

## v0.1.0 (alpha)
- Basic 2D Renderer for quads
- Basic texturing and tilesets
- Event system for user input
