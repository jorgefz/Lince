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


## Tile System
1. ✅Develip tile-based sprite animations
2. 💛 Improve tile animations with callbacks and custom tile order
2. 🔷 Add tilemap system

## 2D Renderer
1. ✅ **Add basic 2D renderer using immediate-mode scenes and quads**
2. ✅ **Add batching to 2D renderer**
3. ✅ **Add z-order with transparency support**
4. 🟠 Add z-order with translucency support
5. ✅ **Move renderer initialisation and termination to the engine, away from the user**

## Nuklear
1. ✅ **Add Nuklear to project with appropriate backend**
2. ✅ **Add Nuklear layer and render GUI**
3. ✅ **Sort out issues with event callbacks and GLFW user pointer**
4. ✅ **Stop Nuklear API from exposing GLFW and OpenGL API to the user**
5. ✅ **Move Nuklear rendering layer to the engine**
6. 🔷 Provide UI callback to layers

## API ease of use
1. 💛 Add more user settings prior to calling `LinceRun`

## Bugs
1. Missile Command: some quads persist on the screen

## Next goals
1. 💛 Add UI library: [Nuklear](https://github.com/Immediate-Mode-UI/Nuklear/)
2. 💛 Add tilesets
3. 🔷 Add tilemaps
4. 🔷 Make small games to test the engine
5. 🔷 Add audio library, e.g. [Miniaudio](https://github.com/mackron/miniaudio)

## Extra/recurrent goals
1. 🟠 Improve documentation
2. 🟠 Add tests
3. 🟠 Add logging
4. 🟠 Fix issues with compiling on Windows
