# Development Diary

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


