# Development Diary


## 24 Aug 2023

I have made some progress in devising ways of implementing two systems:

1. The **scene system** could be implemented as both a hashmap and an array. It doesn't make much sense to implement it as a stack where you can only push or pop. The hashmap would be used to access scenes using a string name, and the array would serve to store the scene data (e.g. callbacks) in contiguous memory. Moreover, this allows storing scene callbacks without initialising the scene, and initialised scenes. You could first load up all scene callbacks into the hashmap and thus the array with a new function (e.g. `LincePreloadScene` or `LinceRegisterScene`). You could tell where an scene has been initalised with an extra boolean parameter - you wouldn't be able to trust whether `scene->data` is NULL because its `on_init` method could be modifying some other global state without using `scene->data`. You'd also have more control over cached scenes, choosing when to free up some memory when a scene hasn't been accessed in some time.

2. When **resolving directories**, allow the user to push custom search paths to a stack.


## 21 Aug 2023

In the last year, I have implemented several major components: a sound system (using Miniaudio), a testing framework (CMocka), an entity-component-system (ECS), a scene system, and UUIDs. I have also improve improved tilemaps.

Regarding the **tilemaps**, I decided to reduce their complexity down to a simple grid of tiles. My implementation from 2022 was attempting to define a full game map with characters, trees, and multiple layers of tiles. Now, a tilemap is simply one such layer of tiles.

Now onto the **ECS**.
Here, an entity is simply the 'glue' that associates a bunch of components, which hold data. A system is simply a function, which takes components and modifies them.
I have implemented it using a sparse set to take advantage of the speed-up from the CPU cache. This implementation has an array with all the component data for each entity (the dense set), and an array with bitmasks that indicate what components an entity has (the sparse set). Thus, an entity, which is represented by an integer, serves as the index to the sparse set. So one can get the bitmask with `sparse[entity]`, and the component data for an entity with `dense[sparse[entity]]`, and, finally, the data for a specific component using the appropriate offset from that location. This implementation is very cache-friendly because all the data is nicely laid out in a single continuous array.
It also gives rise to some questions.

1. The implementation, however, is not very memory-friendly, because every entity will actually have enough memory allocated to have all of the components. This is fine if most of your entities have the same components (e.g. sprite, transform, etc). But there might be some entities that are placeholders for specialised components (e.g. camera, tilemap) which all other entities will rarely use, despite that they will have the memory allocated for them. A solution could be to refrain from using the ECS for everything. If a scene will only have one camera, one shader, and one tilmemap, these could be stored not as entities, but as plain scene data.

2. Serialising. Perhaps components could each have a set of callbacks to initialise, update, delete, save, and load them.

3. Components that need to reference other entities. Storing entity IDs or pointers to other components is a bad idea because these may change or be deleted. A better idea is to reference another entity by its UUID, which would be a "mandatory" component generated when the entity is created. 
As a matter of fact, Cherno defined a list of mandatory component every entity starts out with: UUID, tag, transform, and sprite. It might be worth doing this if I'm sure that very few entities will be abstract enough to not need them, but also not too 'un-ECS' to be stored separately as scene data.

Finally, the **scene system**.
A scene is simply a collection of entities. I have implemented it as a stack, on which one can push and pop scenes. Only the topmost scene is rendered and updated.
Each scene has three callbacks defined: `on_init`, `on_update`, and `on_delete`.
For example, say the player is in a town. This is a scene with a tilemap, and other characters walking around. The player then enters a house through a door. Here, a new scene, the interior of the house, is pushed onto the stack. The state of the previous town scene is effectively paused. If we pop the current scene (we leave the house), the town scene resumes.
This raises a number of questions.

1. What do we need to 'push' a scene onto the stack? We'd need to define its callbacks. If we push the house interior scene from the town scene, the town scene will need to include the header for the house interior. This enforces a 'hierarchy' of scenes, otherwise you might run into spaghettified code and recursive imports. A solution could be caching all scene callbacks into a hashmap in the main game `on_init` function, and on each scene, retrieving the game data and the desired scene from the hashmap using a string identifier.

2. Say the player enters the house, loots a chest, and leaves the house. The scene would be popped from the stack and its data freed, effectively resetting the scene. The next time the player enters the house, they'll find the chest full of riches once again. Solving this issue requires some sort of caching of scene data. Maybe scenes that are popped from the stack can be saved into the hashmap cache for future use - you'd need to avoid calling its `on_delete` method.
If the cached scene is pushed again onto the stack, one could check whether `scene->data` points to valid memory (which would indicate its data is in the heap), and if so, skip calling its `on_init` method (as it is already initialised). Resetting a scene would be as simple as calling the `on_delete` and `on_init` methods in that order.

3. Serialising. I have though about defining further callbacks for scenes: `on_save` for writing the scene data to disk, and `on_load` for reading from disk. When re-loading a save, the `on_init` method is skipped in favour of `on_save` in order to restore its previous state.

One final feature I have been worrying about is the issue of **resolving directories**. It would seem that using raw directories to load resources (e.g. `LoadTexture("path/to/texture.png")`) is a bad idea, as it is dependent on the location from which the executable is run. It has been suggested online that an asset manager should be able to solve this issue. Essentially, it has a predefined location for textures, shaders, etc (e.g. "assets/textures", "assets/shaders"). One would only need to provide the filename (e.g. "player.png"), then the asset manager would look at the extension ("png"), realise it is a texture, and retrieve it from the predefined location for textures. For this, it's necessary for the asset manager to know how to get from the working directory to the asset directory, which is the part I'm not so sure about.
Finding the answer would solve an issue with loading fonts in the engine (the only resource so far that the engine ships with).
A solution could be to support only a reduced number of 'locations' from which to run the executable:

1. Get current working directory
2. Check if any of the supported paths to the assets folder are here:
	* ./lince/assets
	* ./lince/lince/assets
    * etc
3. Build a path to the assets folder and append to the filename of the requested asset.

As for my next steps, it seems the most practical way of encountering problems to solve is trying to make a game. These include:

1. Porting the old games to the new 0.8 version, improving them to use the new systems.
2. Creating new games to test out play styles (e.g. platformer).
3. Creating a tilemap editor. This will become necessary to make more complex games that use maps.


## 12 Aug 2022

I have been working on the idea of a tilemap editor. I have simply setup a Nuklear GUI element with a grid of buttons and images where each is a tile from the tileset. Click events on these buttons save your tile choice, and then click events on the tilemap itself swap the old tile with the new one. These changes are not saved to disk, and the tilemap is reset each time you close the application. Regardless, this is a good start!

One problem I am facing is that the tile images rendered by Nuklear are flipped on their Y axis, which makes it harder to work with them.

But before I go on to implement this editor, I first need to figure out what features tilemaps will have:
1. Plain ground tiles (Implemented)
2. Overlay tiles with sorted z (Implemented)
3. Special tiles, e.g. doors, warps, animated tiles...

Next steps to build this tilemap editor:
1. Create separate folder and Premake5 project
2. Move the code over to the new project
3. Figure out a serialization system for tilemaps
4. Add create, save, and load functionality.

I'm thinking the serialization will consist on two files: a ground data file (array of ground tile indices and bit flags) and a metadata file (tileset name, location of overlays and special tiles, etc).

## 11 Aug 2022
Now that tile animations are setup, I'm moving onto tilemaps.
I have already made a bare-bones tilemap on the testing layer by loading a texture atlas, collecting some of its tiles in a separate tile array and addressing them using an enum. Then, I made a 2D array that stores indices of which tile to use, which constitutes the tilemap. A tilemap struct and API should simply cover these features.
Further features the tilemap should have are collision data, overlay tiles that are rendered over the player, doors and warps, etc.

Regarding these overlays (trees, etc), these in some cases must be drawn **over** the player, for instance when the player is standing behind. Drawing some quads over others can be achieved by changing their relative z coordinates. In the case of trees and other overlaid tiles, their `z` values must be greater than the player's when the player is behind them, and the opposite when the player is in front. This relation can be obtained in a purely arithmetic manner by calculating the `z` value of each quad based on their `y` coordinate: quads at higher `y` positions have lower `z` values because they are further away.

Finally, I will leave the exact serialization format for tilemaps for later development, along with the design of a future tilemap editor.


## 10 Aug 2022
After a three-month long hiatus, and developing the 2D renderer as well as two simple games on the engine (Pong and Missile Command), I am now developing tile-based sprite animations. The way it works is you take an array of tiles, and you loop through them, with a countdown in between. The chosen 'frame' at any given time is provided as a quad texture and rendered.

Perhaps the most common example of sprite animations is a character walking, which is why I decied to implement it and why I found out that my implementation is too simple, not flexible enough. Walking in a 2D ground requires a total of eight animations: both walking and idle for left, right, front, and back directions. Using the simple tile-looping model, I had to create eight different animations off the same spritesheet, which is wasteful and resulted in too much boilerplate. But this could be simpler. I have been toying with the idea of providing the animation data with additional logic: the order in with the tiles are animated. This way, I wouldn't need eight separate animations, but only one. I can give it the entire spritesheet, and then make it use a subset of the tiles in a specific order for each of the character animations. Then, I would switch this subset of indices for another when the character's direction changes.

**Update**: it worked! It's easiest when all the animations have the same number of frames. If not, you'll have to reallocate the order array for more/less space, which is slower.


## 29 Apr 2022
Planning the shader API. I'm not sure what data structure to use for shader uniforms, which hold a string name and an OpenGL location integer ID. On the C++ code, an object like `std::unordered_map<std::string, int>` was used, and an equivalent C implementation is not so simple to implement. A simple implementation would store a list of string names (the shader uniform variable names) as well as another list of the same length with the location IDs. When an uniform set, the function searches for the uniform name in the list (either with string comparison or by hashing and comparing integer values); then it either changes its value if it exists, or creates a new uniform if it doesn't. I think I might just implement a string comparison for value searching, and then implement a more advanced algorithm later on.

## 27 Apr 2022
Having updated the API to be more C-like, I have now moved to expanding it. The best course of action is to get a simple square rendered on the screen to check that the system is indeed working as expected. After solving a couple of headache-inducing bugs, I got a multi-coloured square on screen, and followed that up with an API for managing vertex arrays and buffer layouts. Next steps are the the shader and renderer2D API, and perhaps, a camera API, which will include matrix math.


## 23 Apr 2022
So far, this project has been an attemt at recreating the C++ game engine built by TheCherno on his youtube series, but in C. Naturally, many of the features C++ has don't exist in C. This ended up making this C code look more OOP-like than procedural. My now goal is to build a simple game engine using procedural programming.

This migration will be carried out by following these steps:

* Renaming API functions, e.g. "LinceApp\_OnEvent", which refers to an object "App", to "LinceOnEvent", since there it's a general function called when any event takes place during runtime.
* Simplify event and callback systems


