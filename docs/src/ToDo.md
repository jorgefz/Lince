# To Do

## Symbol legend
| Symbol | Meaning |
| ------ | ------- |
|   ✅   | Completed |
|   💛   | In-dev    |
|   🟠   | Delayed   |
|   🔷   | Planned   |


## Roadmap
1. Basic 2D Renderer ✅
	1. Vertex Arrays ✅
	2. Shaders ✅
	3. Textures ✅
	4. Cameras ✅
	5. Renderer2D API ✅
2. Renderer Improvements ✅
	1. Batch renderer ✅
3. Tiles 💛
	1. Tilesets and texture atlases ✅
	2. Tile animations 💛
	3. Tilemaps 💛
4. Audio 🔷
	1. Integrate OpenAL
	2. Build audio API

## Project Structure
1. 💛 Embed engine resources within the code (e.g. default shaders and textures)
2. 🟠 Create test suite for the engine
3. 🟠 Improve documentation - perhaps move to a separate github repo.
4. 🔷 Restructure engine to be in a separate folder, that can be included into the user's main game project.
5. 💛 Add benchmarking and identify slowest pieces of code that can be optimized.
6. 🔷 Improve code speed & efficiency, e.g. place shader uniforms in a key-value map

## Data
1. 🔷 Add data structures, e.g. linked lists, hashmaps, etc. 

## Audio
1. 🔷 Integrate an audio library into the project (OpenAL or simpler library)

## Physics
1. 💛 Add simple box colliders and algorithm to check

## Scenes
1. 🔷 Add static and parallax backgrounds
2. 🔷 Create scenes which can hold quads, and different scenes represent different screens/menus, etc. E.g. LinceScene, LinceScenePushQuad, LinceDrawScene, etc.
3. ✅ `LinceLoadTexture` function that takes extra flags, such as flipping on the Y axis.

## Tile System
1. ✅ **Develop tile-based sprite animations**
2. ✅ **Improve tile animations with callbacks and custom tile order**
2. 💛 Add tilemaps

## 2D Renderer
1. ✅ **Add basic 2D renderer using immediate-mode scenes and quads**
2. ✅ **Add batching to 2D renderer**
3. ✅ **Add z-order with transparency support**
4. 🟠 Add z-order with translucency support
5. ✅ **Move renderer initialisation and termination to the engine, away from the user**

## GUI
1. ✅ **Add Nuklear to project with appropriate backend**
2. ✅ **Add Nuklear layer and render GUI**
3. ✅ **Sort out issues with event callbacks and GLFW user pointer**
4. ✅ **Stop Nuklear API from exposing GLFW and OpenGL API to the user**
5. ✅ **Move Nuklear rendering layer to the engine**
6. 🔷 Provide UI callback to layers
7. 🟠 Improve LinceUIText

## API ease of use
1. 💛 Add more user settings prior to calling `LinceRun`
2. 🟠 Add `LinceLoadTexture` function with an extra argument for flags, i.e. flipping by the y axis on load.
3. 🟠 Add memory management functions, e.g. LinceMalloc, LinceNewCopy, etc

## Bugs
1. ✅ **(Solved)** Missile Command: some quads persist on the screen
2. 🟠 Draw text overlay without consuming click events

## Next goals
1. 💛 Add UI library: [Nuklear](https://github.com/Immediate-Mode-UI/Nuklear/)
2. 💛 Add tilesets
3. 🔷 Add tilemaps
4. 🔷 Make small games to test the engine
5. 🔷 Make a tilemap editor
6. 🔷 Add audio library, e.g. [Miniaudio](https://github.com/mackron/miniaudio)

## Extra/recurrent goals
1. 🟠 Improve documentation
2. 🟠 Add tests
3. 🟠 Add logging
4. 🟠 Fix issues with compiling on Windows
