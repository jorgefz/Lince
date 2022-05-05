#include "renderer/camera.h"
#include "core/core.h"
#include <cglm/cam.h>
#include <cglm/mat4.h>
#include <cglm/affine.h>

static const LinceCamera default_camera = {
	.scale = 1.0,
	.rotation = 0.0,
	.pos = (vec3){0.0, 0.0, 0.0},
	.proj = GLM_MAT4_IDENTITY_INIT,
	.view = GLM_MAT4_IDENTITY_INIT,
	.view_proj = GLM_MAT4_IDENTITY_INIT,
	.aspect_ratio = 1.0,
};

void LinceCalculateProjection(
	mat4 result,
	float left, float right,
	float bottom, float top
) {
	float near = -1.0, far = 1.0;
	glm_ortho(left, right, bottom, top, near, far, result);
}

LinceCamera* LinceCreateCamera(float aspect_ratio){

	LinceCamera *cam;
	mat4 proj;
	LinceCalculateProjection(proj, -aspect_ratio, aspect_ratio, -1.0, 1.0);
	cam = LinceCreateCameraFromProj(proj);
	cam->aspect_ratio = aspect_ratio;

	return cam;
}

LinceCamera* LinceCreateCameraFromProj(mat4 proj){
	
	LinceCamera *cam;
	cam = malloc(sizeof(LinceCamera));
	LINCE_ASSERT_ALLOC(cam, sizeof(LinceCamera));
	memcpy(cam, &default_camera, sizeof(LinceCamera));
	glm_mat4_copy(proj, cam->proj);
	glm_mat4_mul(cam->proj, cam->view, cam->view_proj);

	return cam;
}

void LinceSetCameraProj(LinceCamera* cam, mat4 proj){
	if(!cam) return;
	glm_mat4_copy(proj, cam->proj);
}

void LinceUpdateCamera(LinceCamera* cam) {
	if(!cam) return;

	mat4 result = GLM_MAT4_IDENTITY_INIT;
	vec3 scale_m = {cam->scale, cam->scale, 1.0};
	vec3 rot_axis = {0.0, 0.0, 1.0};

	glm_scale(result, scale_m);
	glm_translate(result, cam->pos);
	glm_rotate(result, glm_rad(cam->rotation), rot_axis);
	glm_mat4_inv(result, cam->view);
	glm_mat4_mul(cam->proj, cam->view, cam->view_proj);
}

void LinceResizeCameraView(LinceCamera* cam, float aspect_ratio){
	cam->aspect_ratio = aspect_ratio;
	LinceCalculateProjection(
		 cam->proj,
		-cam->aspect_ratio * cam->scale,
		 cam->aspect_ratio * cam->scale,
		-cam->scale, cam->scale
	);
}

void LinceDeleteCamera(LinceCamera* cam){
	if(!cam) return;
	free(cam);
}