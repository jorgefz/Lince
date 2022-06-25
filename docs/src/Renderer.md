# Renderer

## LinceRendererState


## LinceGetRendererState
```c
LinceRendererState* LinceGetRendererState()
```
## LinceQuadProps
- `float x`
- `float y`
- `float zorder`
- `float color[4]`
- `LinceTexture* texture`

## LinceInitRenderer
```c
void LinceInitRenderer(LinceWindow* window)
```
## LinceTerminateRenderer
```c
void LinceTerminateRenderer()
```
## LinceBeginScene
```c
void LinceBeginScene()
```
## LinceEndScene
```c
void LinceEndScene()
```
## LinceDrawQuad
```c
void LinceDrawQuad(LinceQuadProps props)
```
## LinceDrawIndexed
```c
void LinceDrawIndexed(
	LinceShader* shader,
	LinceVertexArray* va,
	LinceIndexBuffer vb
)
```
## LinceClear
```c
void LinceClear()
```
## LinceSetClearColor
```c
void LinceSetClearColor(float r, float g, float b, float a)
```
