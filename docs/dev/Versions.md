# Version History

Version codes follow [Semantic Versioning](https://semver.org/).

### To-Do
- Merge all key events into single struct.
- Improve variable name for macros like `LINCE_NAME_MAX`
- Change entity registry initialisation to take an array of component sizes instead of a list of varargs.
- Nuklear UI wrapper and/or custom docs.
- Create functions that don't depend on OpenGL, e.g. `LinceImage` for storing image data, `LinceClock` for timers, etc. 

## v0.7.1 (WIP)
- Added functions to initialise camera without allocating heap memory.
- Started working on the scene system on the sandbox
- Fixed z sorting with translucency
- Improved error message when font fails to load 
- Removed `LinceCreateTexture`
- Removed `name` parameter from `LinceTexture` functions.
- Removed `name` parameter from `LinceShader` functions.
- `LINCE_DIR` now generated in premake5 file automatically from any path.
- Change naming of event-related variable names from past tense (e.g. pressed) to present tense (.e.g press), and shortened "mouse button" to simply "mouse". This affects `LinceEventType` enum values, `LinceEventData` union members, event struct names, and event creation functions.
- Renamed application callback variables e.g. from `on_update` to `on_update`.


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
