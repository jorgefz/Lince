/* @file transform.h
* 
* This header handles transformations between coordinate systems,
* for both points (i.e. a 2D poisition) and rectangles (i.e. with width and height).
* 
* The LincePoint--- functions handle transforms for points,
* and the LinceTransformTo--- functions, for rectangles
* 
* OpenGL uses three coordinate systems:
* 
*	- World coordinates
* 		Location within the game. Defined by the MVP matrix.
* 
*	- Screen coordinates
*		Location on the screen normalised to the range (-1,1),
*		where (0,0) is the centre of the window,
*		right is positive X, and up is positive Y.
* 
*	- Pixel coordinates
*		Location on the screen in pixels,
*		where the origin is in the top-left.
*		NOTE: the Y axis is inverted, with down being positive Y.
*		
*/


#ifndef LINCE_TRANSFORM
#define LINCE_TRANSFORM

#include "renderer/camera.h"

/* Defines the location of a point in two dimensions */
typedef struct LincePoint {
	float x, y; ///> Cartesian coordinates of the point
} LincePoint;

/* Calculate screen coordinates from pixel location */
LincePoint LincePointPixelToScreen(const LincePoint p, const float sc_w, const float sc_h);

/* Calculate pixel location from screen coordinates */
LincePoint LincePointScreenToPixel(const LincePoint p, const float sc_w, const float sc_h);

/* Convert a point from screen to world coordinates */
LincePoint LincePointScreenToWorld(const LincePoint p, LinceCamera* cam);

/* Convert a point from world to screen coordinates */
LincePoint LincePointWorldToScreen(const LincePoint p, LinceCamera* cam);


/* Supported coordinate systems */
typedef enum LinceCoordSystem {
	LinceCoordSystem_World = 0, // default value with zero initialization
	LinceCoordSystem_Screen,
	LinceCoordSystem_Pixel,
} LinceCoordSystem;

/* Holds the position and scale of an object */
typedef struct LinceTransform {
	float x, y;				 ///> Position
	float w, h;				 ///> Scale
	LinceCoordSystem coords; ///> Coordinate system of the transform 
} LinceTransform;


/* Translates a transform to world coordinates */
void LinceTransformToWorld(const LinceTransform* in, LinceTransform* out, LinceCamera* cam, const float sc_w, const float sc_h);

/* Translates a transform to screen coordinates */
void LinceTransformToScreen(const LinceTransform* in, LinceTransform* out, LinceCamera* cam, const float sc_w, const float sc_h);

/* Translates a transform to pixel coordinates */
void LinceTransformToPixel(const LinceTransform* in, LinceTransform* out, LinceCamera* cam, const float sc_w, const float sc_h);


#endif /* LINCE_TRANSFORM */
