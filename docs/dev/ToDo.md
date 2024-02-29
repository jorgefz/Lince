# To Do

## Symbol legend
| Symbol | Meaning |
| ------ | ------- |
|   ✅   | Completed |
|   💛   | In-dev    |
|   🟠   | Next up   |
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
3. ECS 💛
	1. Develop ECS API ✅
	2. Improve to use archetypes ✅
	3. Cache queries 🔷
	4. Add more pre-defined components 🔷
4. Tiles 💛
	1. Tilesets and texture atlases ✅
	2. Tile animations 💛
	3. Tilemaps 💛
5. Audio 💛
	1. Integrate Miniaudio ✅
	2. Build audio API ✅
	3. Improve: rename `AudioManager`.
6. Physics
	1. Add simple box colliders ✅
	2. Add an efficient nearest-neighbour algorithm
7. Editor 🔷
	1. Program to create and modify tilemaps


## Project Structure
1. 💛 Embed engine resources within the code (e.g. default shaders and textures)
2. 🟠 Create test suite for the engine
3. 🟠 Improve documentation - perhaps move to a separate github repo.
4. 💛 Restructure engine to be in a separate folder, that can be included into the user's main game project.
	* Feasible, but requires user to copy settings on premake5 file into their own build script, and re-define custom LINCE\_DIR.
5. 💛 Add benchmarking and identify slowest pieces of code that can be optimized.
6. 💛 Improve code speed & efficiency

## Data structures
1. ✅ **Add array (contiguous memory)**
2. ✅ **Add hashmap**
3. ✅ **Add linked lists** 

## Audio
1. ✅ **Integrate Miniaudio library into the project**
2. ✅ **Develop API layer for Miniaudio**

## Physics
1. ✅ **Add simple rectangle colliders and algorithm to check for collision**
2. 💛 Improve nearest-neighbour search algorith.

## Scenes
1. 🔷 Add static and parallax backgrounds
2. 🔷 Create scenes which can hold quads, and different scenes represent different screens/menus, etc. E.g. LinceScene, LinceScenePushQuad, LinceDrawScene, etc.
3. ✅ `LinceLoadTexture` function that takes extra flags, such as flipping on the Y axis.

## Tile System
1. ✅ **Develop tile-based sprite animations**
2. ✅ **Improve tile animations with callbacks and custom tile order**
2. 💛 Add tilemaps

## Editor
1. Transition to using entities instead of bare quads.

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
6. 🟠 Develop documentation for nuklear
7. 🔷 Provide UI callback to layers

## API ease of use
1. 💛 Add more user settings prior to calling `LinceRun`
2. ✅ **Add `LinceLoadTexture` function with an extra argument for flags, i.e. flipping by the y axis on load.**
3. ✅ **Add memory management functions (e.g. LinceMalloc, LinceNewCopy)**

## Bugs
1. ✅ **(Solved) Missile Command: some quads persist on the screen**
2. 🟠 Draw text overlay without consuming click events

## Extra/recurrent goals
1. 🟠 Improve documentation
2. 🟠 Add tests
3. 🟠 Add logging
4. 🟠 Fix issues with compiling on Windows
