#ifndef LINCE_CAMERA_H
#define LINCE_CAMERA_H

#include "cglm/types.h"

typedef struct LinceCamera {
	float scale;
	float rotation; // clockwise rotation in degrees
	vec3 pos;       // 3D position
	mat4 proj;      // projection matrix
	mat4 view;      // view matrix
	mat4 view_proj; // view-projection matrix

	float aspect_ratio;
} LinceCamera;

/* Calculates perspective projection matrix from screen distances */
void LinceCalculateProjection(
	mat4 result,
	float left, float right,
	float bottom, float top
);

/* Creates camera calculating projection from window aspect ratio */
LinceCamera* LinceCreateCamera(float aspect_ratio);

/* Creates camera with custom projection matrix */
LinceCamera* LinceCreateCameraFromProj(mat4 proj);

/* Manually sets projection matrix */
void LinceSetCameraProj(LinceCamera* cam, mat4 proj);

/* Transforms view matrix using camera position, scale, and rotation,
and recalculates view-projection matrix */
void LinceUpdateCamera(LinceCamera* cam);

/* Adapts projection to changes in window dimensions */
void LinceResizeCameraView(LinceCamera* cam, float aspect_ratio);

void LinceDeleteCamera(LinceCamera* cam);

#endif /* LINCE_CAMERA_H */