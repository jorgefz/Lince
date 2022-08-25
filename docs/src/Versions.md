# Version History

Version codes follow [Semantic Versioning](https://semver.org/).

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