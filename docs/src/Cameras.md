# Cameras

Cameras display the world of the game as well as provide the means to move within it, translating the location, shape, and color of game objects into your screen.

## LinceCamera

These fields may be modified by the user to change the camera's view of the world:
- `float scale`
- `float zoom`
	- Zoom value of the camera, where a greater value translates to zooming in.
- `float rotation`
	- Clockwise rotation of the camera in degrees.
- `vec3 pos`
	- Position of the camera in the world

These fields are internal and are automatically updated with the functions provided below.
- `mat4 proj`
	- Projection matrix that calculates what objects are seen by the camera.
- `mat4 view`
	- View matrix that translates the camera's eye to a flat screen view.
- `mat4 view_proj`
	- View-projection matrix, calculated by multiplying the view and projection.
- `mat4 view_proj_inv`
	- Inverse view-projection matrix.
- `float aspect_ratio`
	- Aspect ratio of the window. Using an incorrect value will result in a distorted view of the world.

## LinceCalculateProjection
```c
void LinceCalculateProjection(
	mat4 result,
	float left, float right,
	float bottom, float top
)
```
Calculates the projection matrix given the dimensions of the frustrum: left, right, bottom, and top. The near and far coordinates are hard coded as -1 and 1, respectively. This means that z values outside this range are not rendered.

## LinceCreateCamera
```c
LinceCamera* LinceCreateCamera(float aspect_ratio)
```
Creates a new camera by calcualting the projection matrix from the window's aspect ratio.

## LinceCreateCameraFromProj
```c
LinceCamera* LinceCreateCameraFromProj(mat4 proj)
```
Creates a new camera using a custom provided projection matrix.

## LinceSetCameraProj
```c
void LinceSetCameraProj(LinceCamera* cam, mat4 proj)
```
Manually updates the projection matrix of an existing camera with the provided one.

## LinceUpdateCamera
```c
void LinceUpdateCamera(LinceCamera* cam)
```
Recalculates the view-projection matrix using the values for scale, zoom, and rotation.

## LinceResizeCameraView
```c
void LinceResizeCameraView(LinceCamera* cam, float aspect_ratio)
```
Updates the view matrix using the provided aspect ratio. This function should be called when the window is resized. 

## LinceDeleteCamera
```c
void LinceDeleteCamera(LinceCamera* cam)
```
Frees allocated camera data.

