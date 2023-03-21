/** @file camera.h
* Cameras display the world of the game
* as well as provide the means to move within it,
* translating the location, shape, and color of game objects into your screen.
*/

#ifndef LINCE_CAMERA_H
#define LINCE_CAMERA_H

#include "cglm/types.h"

/** @struct LinceCamera
* @brief Holds spatial information of a camera.
* 
* Modify scale, zoom, rotation, and position to interact with camera,
* and call `LinceUpdateCamera` for the changes to have effect in the world.
*/
typedef struct LinceCamera {
	float scale;		///< Scale of the world
	float zoom;			///< Zoom level
	float rotation; 	///< Clockwise rotation in degrees
	vec3 pos;       	///< 3D position of the camera in the world

	mat4 proj;      	///< Projection matrix
	mat4 view;      	///< View matrix
	mat4 view_proj; 	///< View-projection matrix
	mat4 view_proj_inv; ///< Inverted view-projection matrix
	float aspect_ratio; ///< Aspect ratio of the window
} LinceCamera;

/** @brief Calculates perspective projection matrix from screen dimensions
* @param result Return projection matrix
* @param left   Min x value of clip space
* @param right  Max x value of clip space
* @param bottom Min y value of clip space
* @param top    Max y value of clip space
*/
void LinceCalculateProjection(
	mat4 result,
	float left, float right,
	float bottom, float top
);

/** @brief Creates camera calculating projection from window aspect ratio */
LinceCamera* LinceCreateCamera(float aspect_ratio);

/** @brief Creates camera with custom projection matrix */
LinceCamera* LinceCreateCameraFromProj(mat4 proj);

/** @brief Manually sets projection matrix */
void LinceSetCameraProj(LinceCamera* cam, mat4 proj);

/** @brief Transforms view matrix using camera position, scale, and rotation,
* and recalculates view-projection matrix and its inverse.
*/
void LinceUpdateCamera(LinceCamera* cam);

/** @brief Adapts projection to changes in window size */
void LinceResizeCameraView(LinceCamera* cam, float aspect_ratio);

/** @brief Deallocates a camera */
void LinceDeleteCamera(LinceCamera* cam);

#endif /* LINCE_CAMERA_H */