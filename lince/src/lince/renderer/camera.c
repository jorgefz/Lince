#include "renderer/camera.h"
#include "core/core.h"
#include "core/profiler.h"
#include "core/memory.h"
#include <cglm/cam.h>
#include <cglm/mat4.h>
#include <cglm/affine.h>

static const LinceCamera default_camera = {
	.scale = 1.0,
	.zoom = 1.0,
	.rotation = 0.0,
	.pos = GLM_VEC3_ZERO_INIT,
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


void LinceInitCamera(LinceCamera* cam, float aspect_ratio){
	mat4 proj;
	LinceCalculateProjection(proj, -aspect_ratio, aspect_ratio, -1.0, 1.0);
	memmove(cam, &default_camera, sizeof(LinceCamera));
	glm_mat4_copy(proj, cam->proj);
	glm_mat4_mul(cam->proj, cam->view, cam->view_proj);
	cam->aspect_ratio = aspect_ratio;
}

LinceCamera* LinceCreateCameraFromProj(mat4 proj){
	LINCE_PROFILER_START(timer);
	LinceCamera *cam;
	cam = LinceNewCopy(&default_camera, sizeof(LinceCamera));
	glm_mat4_copy(proj, cam->proj);
	glm_mat4_mul(cam->proj, cam->view, cam->view_proj);
	LINCE_PROFILER_END(timer);
	return cam;
}

LinceCamera* LinceCreateCamera(float aspect_ratio){
	LINCE_PROFILER_START(timer);
	LinceCamera *cam;
	mat4 proj;
	LinceCalculateProjection(proj, -aspect_ratio, aspect_ratio, -1.0, 1.0);
	cam = LinceCreateCameraFromProj(proj);
	cam->aspect_ratio = aspect_ratio;
	LINCE_PROFILER_END(timer);
	return cam;
}

void LinceSetCameraProj(LinceCamera* cam, mat4 proj){
	if(!cam) return;
	glm_mat4_copy(proj, cam->proj);
}

void LinceUpdateCamera(LinceCamera* cam) {
	if(!cam) return;
	LINCE_PROFILER_START(timer);

	mat4 result = GLM_MAT4_IDENTITY_INIT;
	vec3 scale_m = {cam->scale, cam->scale, 1.0};
	vec3 rot_axis = {0.0, 0.0, 1.0};

	glm_scale(result, scale_m);
	glm_translate(result, cam->pos);
	glm_rotate(result, glm_rad(cam->rotation), rot_axis);
	glm_mat4_inv(result, cam->view);
	glm_mat4_mul(cam->proj, cam->view, cam->view_proj);
	glm_mat4_inv(cam->view_proj, cam->view_proj_inv);

	LINCE_PROFILER_END(timer);
}

void LinceResizeCameraView(LinceCamera* cam, float aspect_ratio){
	cam->aspect_ratio = aspect_ratio;
	LinceCalculateProjection(
		 cam->proj,
		-cam->aspect_ratio * cam->zoom,
		 cam->aspect_ratio * cam->zoom,
		-cam->zoom, cam->zoom
	);
}

void LinceDeleteCamera(LinceCamera* cam){
	if(!cam) return;
	LinceFree(cam);
}