# Renderer

## LinceInitRenderer
```c
void LinceInitRenderer()
```
Initialises the rendering state: allocates vertex batch, initialises default shader and texture, etc.
This should be called only once. 

## LinceTerminateRenderer
```c
void LinceTerminateRenderer()
```
Frees allocated memory and destroys OpenGL objects.


## LinceQuadProps

This struct contains the information to render a "quad" (rectangle) on screen, such as location, size, and color.

- `float x`
	- X-axis position in the world
- `float y`
	- Y-axis position in the world
- `float w`
	- Width, size in the x-axis.
- `float h`
	- Height, size in the y-axis.
- `float zorder`
	- Order for rendering overlapping quads. A higher number results in a quad being rendered above others.
- `float rotation`
	- Clockwise rotation in degrees
- `float color[4]`
	- RGBA color as floats with values between 0 and 1.
- `LinceTexture* texture`
	- Texture to apply on the quad. If `NULL`, a default white texture is used instead.

Note that for ease of use one may use a designated initialiser list, where an struct is initialised enclosed in curly brakets where specific fields are given values `{.x = 1, .y = 2}`.

Note that any field that is left uninitialised will be set to zero of the appropriate type. For instance, defining only the x position `{.x=1.0}` will make `y = 0` and the texture `NULL`, however it will also make the color completely transparent (`color = {0, 0, 0, 0}`, where alpha is zero) as well as the size (width and height) zero.

This example describes a red square at origin:
```c
LinceQuadProps props = {
	.w=1.0, .h=1.0,
	.color={1.0,0.0,0.0,1.0}
};
```

## LinceBeginScene
```c
void LinceBeginScene(LinceCamera* camera)
```
Prepares a new batch for rendering.

## LinceEndScene
```c
void LinceEndScene()
```
Flushes the vertex batch and renders the geometry.

## LinceDrawQuad
```c
void LinceDrawQuad(LinceQuadProps props)
```
Submits a quad (rectangle) for rendering. This function should only be called between `LinceBeginScene` and `LinceEndScene` calls.

This example below shows how to use the rendering API to render quads to the screen. This code snippet should be placed on your `OnUpdate` function and should be called every frame.
```c
LinceBeginScene(camera);

LinceDrawQuad((LinceQuadProps){
	.w = 1.0, .h = 1.0,
	.color = {1.0, 0.0, 0.0, 1.0}
});

LinceEndScene();
```

## LinceDrawIndexed
```c
void LinceDrawIndexed(
	LinceShader* shader,
	LinceVertexArray* va,
	LinceIndexBuffer vb
)
```
Mannually renders geometry with provided OpenGL objects.

## LinceClear
```c
void LinceClear()
```
Clears the screen.

## LinceSetClearColor
```c
void LinceSetClearColor(float r, float g, float b, float a)
```
Sets the color to which the screen is cleared.
