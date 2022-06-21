# Development Diary


## To-Do

* Make user application a separate project that links to lince static library.
* Test vertex arrays and buffer layouts
* BUG: smaller windows have shorter delta-time
* Add application settings and window flags


## Roadmap

1. Basic 2D Renderer
	1. Vertex Arrays - DONE
	2. Shaders
	3. Textures
	4. Cameras
	5. Renderer2D API
2. Renderer Improvements
	1. Batch renderer
	2. Tilemaps
3. Audio


## 29 Apr 2022
Planning the shader API. I'm not sure what data structure to use for shader uniforms, which hold a string name and an OpenGL location integer ID. On the C++ code, an object like `std::unordered_map<std::string, int>` was used, and an equivalent C implementation is not so simple to implement. A simple implementation would store a list of string names (the shader uniform variable names) as well as another list of the same length with the location IDs. When an uniform set, the function searches for the uniform name in the list (either with string comparison or by hashing and comparing integer values); then it either changes its value if it exists, or creates a new uniform if it doesn't. I think I might just implement a string comparison for value searching, and then implement a more advanced algorithm later on.

## 27 Apr 2022
Having updated the API to be more C-like, I have now moved to expanding it. The best course of action is to get a simple share rendered on the screen to check that the system is indeed working as expected. After solving a couple of headache-inducing bugs, I got a multi-coloured square on screen, and followed that up with an API for managing vertex arrays and buffer layouts. Next steps are the the shader and renderer2D API, and perhaps, a camera API, which will include matrix math.


## 23 Apr 2022
So far, this project has been an attemt at recreating the C++ game engine built by TheCherno on his youtube series, but in C. Natuarlly, many of the features C++ has don't exist in C. This ended up making this C code look more OOP-like than procedural. My now goal is to build a simple game engine using procedural programming.

This migration will be carried out by following these steps:

* Renaming API functions, e.g. "LinceApp\_OnEvent", which refers to an object "App", to "LinceOnEvent", since there it's a general function called when any event takes place during runtime.
* Simplify event and callback systems


