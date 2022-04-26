# Development Diary


## To-Do

* Make user application a separate project that links to lince static library.
* Test vertex arrays and buffer layouts
* BUG: smaller windows have shorter delta-time
* Add application settings and window flags


## Roadmap

1. Basic 2D Renderer
	1. Vertex Arrays
	2. Shaders
	3. Textures
	4. Renderer2D API


## 23 Apr 2022
So far, this project has been an attemt at recreating the C++ game engine built by TheCherno on his youtube series, but in C. Natuarlly, many of the features C++ has don't exist in C. This ended up making this C code look more OOP-like than procedural. My goal is to build a simple game engine using procedural programming.

This migration will be carried out by following these steps:

* Renaming API functions, e.g. "LinceApp\_OnEvent", which refers to an object "App", to "LinceOnEvent", since there it's a general function called when any event takes place during runtime.
* Simplify event and callback systems


