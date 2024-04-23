
#include "cglm/mat4.h"
#include "transform.h"

void LinceRectGetBounds(LinceRect rect, LincePoint* lo, LincePoint* up);{
	if(lo) *lo = (LincePoint){rect.x - rect.w/2.0f, rect.y - rect.h/2.0f};
	if(up) *up = (LincePoint){rect.x + rect.w/2.0f, rect.y + rect.h/2.0f};
}

float* LinceRectGetVertices(LinceRect rect, LincePoint bounds[4]){

	LincePoint lower, upper;
	LinceRectGetBounds(rect, &lower, &upper);

	bounds[0] = (LincePoint){lower.x, lower.y };
	bounds[1] = (LincePoint){upper.x, lower.y };
	bounds[2] = (LincePoint){upper.x, upper.y };
	bounds[3] = (LincePoint){lower.x, upper.y };
	
	return (float*)(bounds);
}

/* Calculate screen coordinates from pixel location */
LincePoint LincePointPixelToScreen(const LincePoint p, const float sc_w, const float sc_h) {
	return (LincePoint) { .x = 2 * p.x / sc_w - 1, .y = 1 - 2 * p.y / sc_h };
}

/* Calculate pixel location from screen coordinates */
LincePoint LincePointScreenToPixel(const LincePoint p, const float sc_w, const float sc_h) {
	return (LincePoint) { .x = (p.x + 1) * sc_w / 2, .y = (1 - p.y) * sc_h / 2 };
}

/* Convert a point from screen to world coordinates */
LincePoint LincePointScreenToWorld(const LincePoint p, LinceCamera* cam) {
	vec4 wd, sc = { p.x, p.y, 0.0f, 1.0f };
	glm_mat4_mulv(cam->view_proj_inv, sc, wd);
	return (LincePoint) { .x = wd[0] / wd[3], .y = wd[1] / wd[3] };
}

/* Convert a point from world to screen coordinates */
LincePoint LincePointWorldToScreen(const LincePoint p, LinceCamera* cam) {
	vec4 sc, wd = { p.x, p.y, 0.0, 1.0 };
	glm_mat4_mulv(cam->view_proj, wd, sc);
	return (LincePoint) { .x = sc[0] / sc[3], .y = sc[1] / sc[3] };
}


/* Translates a transform to world coordinates */
void LinceTransformToWorld(LinceTransform* out, const LinceTransform* in, LinceCamera* cam, const float sc_w, const float sc_h) {
	LincePoint centre = { .x = in->x, .y = in->y };
	LincePoint corner = { .x = in->x + in->w, .y = in->y + in->h };
	out->coords = LinceCoordSystem_World;

	switch (in->coords) {
	
	default:
	case LinceCoordSystem_World:
		*out = *in;
		break;

	case LinceCoordSystem_Screen:
		centre = LincePointScreenToWorld(centre, cam);
		corner = LincePointScreenToWorld(corner, cam);
		break;

	case LinceCoordSystem_Pixel:
		centre = LincePointPixelToScreen(centre, sc_w, sc_h);
		corner = LincePointPixelToScreen(corner, sc_w, sc_h);
		centre = LincePointScreenToWorld(centre, cam);
		corner = LincePointScreenToWorld(corner, cam);
		break;
	}

	out->x = centre.x;
	out->y = centre.y;
	out->w = (float)fabs(corner.x - centre.x);
	out->h = (float)fabs(corner.y - centre.y);
}



/* Translates a transform to screen coordinates */
void LinceTransformToScreen(LinceTransform* out, const LinceTransform* in, LinceCamera* cam, const float sc_w, const float sc_h) {
	LincePoint centre = { .x = in->x, .y = in->y };
	LincePoint corner = { .x = in->x + in->w, .y = in->y + in->h };
	out->coords = LinceCoordSystem_Screen;

	switch (in->coords) {

	default:
	case LinceCoordSystem_Screen:
		*out = *in;
		break;

	case LinceCoordSystem_World:
		centre = LincePointWorldToScreen(centre, cam);
		corner = LincePointWorldToScreen(corner, cam);
		break;

	case LinceCoordSystem_Pixel:
		centre = LincePointPixelToScreen(centre, sc_w, sc_h);
		corner = LincePointPixelToScreen(corner, sc_w, sc_h);
		break;
	}

	out->x = centre.x;
	out->y = centre.y;
	out->w = (float)fabs(corner.x - centre.x);
	out->h = (float)fabs(corner.y - centre.y);
}


/* Translates a transform to pixel coordinates */
void LinceTransformToPixel(LinceTransform* out, const LinceTransform* in, LinceCamera* cam, const float sc_w, const float sc_h) {
	LincePoint centre = { .x = in->x, .y = in->y };
	LincePoint corner = { .x = in->x + in->w, .y = in->y + in->h };
	out->coords = LinceCoordSystem_Pixel;

	switch (in->coords) {

	default:
	case LinceCoordSystem_Pixel:
		*out = *in;
		break;

	case LinceCoordSystem_World:
		centre = LincePointWorldToScreen(centre, cam);
		corner = LincePointWorldToScreen(corner, cam);
		centre = LincePointScreenToPixel(centre, sc_w, sc_h);
		corner = LincePointScreenToPixel(corner, sc_w, sc_h);
		break;

	case LinceCoordSystem_Screen:
		centre = LincePointScreenToPixel(centre, sc_w, sc_h);
		corner = LincePointScreenToPixel(corner, sc_w, sc_h);
		break;
	}

	out->x = centre.x;
	out->y = centre.y;
	out->w = (float)fabs(corner.x - centre.x);
	out->h = (float)fabs(corner.y - centre.y);
}