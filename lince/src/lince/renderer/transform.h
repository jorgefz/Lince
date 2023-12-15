/* @file transform.h
* 
* This header handles transformations between coordinate systems,
* for both points (i.e. a 2D poisition) and rectangles (i.e. with width and height).
* 
* The LincePoint___ functions handle transforms for points,
* and the LinceTransformTo___ functions, for rectangles.
* 
* OpenGL uses three coordinate systems:
* 
*	- World coordinates
* 		Location within the game. Defined by the MVP matrix.
*		Commonly used by entities in the game.
* 
*	- Screen coordinates
*		Location on the screen normalised to the range (-1,1),
*		where (0,0) is the centre of the window,
*		right is positive X, and up is positive Y.
*		Useful for UI elements.
* 
*	- Pixel coordinates
*		Location on the screen in pixels,
*		where the origin is in the top-left.
*		NOTE: the Y axis is inverted, with down being positive Y.
*		Useful for UI elements and mouse position.
*/


#ifndef LINCE_TRANSFORM
#define LINCE_TRANSFORM

#include "lince/renderer/camera.h"

/** @brief Defines the location of a point in two dimensions */
typedef struct LincePoint {
	float x, y; ///> Cartesian coordinates of the point
} LincePoint;

/** @brief Calculate screen coordinates from pixel location
* @param p point whose position is defined in pixel coordinates
* @param sc_w screen width in pixels
* @param sc_h screen height in pixels
* @returns point with position in screen coordinates
*/
LincePoint LincePointPixelToScreen(const LincePoint p, const float sc_w, const float sc_h);

/** @brief Calculate pixel location from screen coordinates
* @param p point whose position is defined in screen coordinates
* @param sc_w screen width in pixels
* @param sc_h screen height in pixels
* @returns point with position in pixel coordinates
*/
LincePoint LincePointScreenToPixel(const LincePoint p, const float sc_w, const float sc_h);

/* Convert a point from screen to world coordinates
* @param p point whose position is defined in screen coordinates
* @param cam Camera with an up-to-date inverse view projection matrix
* @note this operation is somewhat expensive - it involves a 4x4 matrix multiplication with a vector
* @returns point with position in world coordinates
*/
LincePoint LincePointScreenToWorld(const LincePoint p, LinceCamera* cam);

/* Convert a point from world to screen coordinates
* @param p point whose position is defined in world coordinates
* @param cam Camera with an up-to-date inverse view projection matrix
* @note this operation is somewhat expensive - it involves a 4x4 matrix multiplication with a vector
* @returns point with position in screen coordinates
*/
LincePoint LincePointWorldToScreen(const LincePoint p, LinceCamera* cam);


/** @brief Supported coordinate systems */
typedef enum LinceCoordSystem {
	LinceCoordSystem_World = 0, ///> Default value using zero initialization
	LinceCoordSystem_Screen,
	LinceCoordSystem_Pixel,
} LinceCoordSystem;

/** @brief Holds the position and scale of an object */
typedef struct LinceTransform {
	float x, y;				 ///> Position
	float w, h;				 ///> Scale
	LinceCoordSystem coords; ///> Coordinate system of the transform 
} LinceTransform;


/** @brief Converts a transform to world coordinates
* @param in Input transform in any coordinate system
* @param out Output transform in world coordinates
* @param cam Camera with an up-to-date inverse view projection matrix
* @param sc_w screen width in pixels
* @param sc_h screen height in pixels
*/
void LinceTransformToWorld(LinceTransform* out, const LinceTransform* in, LinceCamera* cam, const float sc_w, const float sc_h);

/* Converts a transform to screen coordinates
* @param in Input transform in any coordinate system
* @param out Output transform in screen coordinates
* @param cam Camera with an up-to-date inverse view projection matrix
* @param sc_w screen width in pixels
* @param sc_h screen height in pixels
*/
void LinceTransformToScreen(LinceTransform* out, const LinceTransform* in, LinceCamera* cam, const float sc_w, const float sc_h);

/* Converts a transform to pixel coordinates 
* @param in Input transform in any coordinate system
* @param out Output transform in pixel coordinates
* @param cam Camera with an up-to-date inverse view projection matrix
* @param sc_w screen width in pixels
* @param sc_h screen height in pixels
*/
void LinceTransformToPixel(LinceTransform* out, const LinceTransform* in, LinceCamera* cam, const float sc_w, const float sc_h);


#endif /* LINCE_TRANSFORM */