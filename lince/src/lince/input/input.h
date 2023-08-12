#ifndef LINCE_INPUT_H
#define LINCE_INPUT_H

#include "lince/core/core.h"

/** @brief Returns true if a keyboard key is held.
*   @param key Enum value for a key. See `LinceKey`.
*/
LinceBool LinceIsKeyPressed(int key);

/** @brief Returns true if a mosue button is held
*   @brief button Enum value for a mouse button. See `LinceMouseButton`.
*/
LinceBool LinceIsMouseButtonPressed(int button);

/** @brief Provides 2D coordinates of the mouse in the screen.
*   The coordinates are the xy pixel positions with origin
*   on the top left of the window.
*   @param xpos location where x position of the mouse is written to.
*   @param ypos location where y position of the mouse is written to.
*   @todo Change arguments to vec2
*/
void LinceGetMousePos(float* xpos, float* ypos);

/** @brief Returns the x position of the mouse.
* This position is in pixel coordinates of the window, with origin on the right.
*/
float LinceGetMouseX();

/** @brief Returns the y position of the mouse.
* This position is in pixel coordinates of the window, with origin on the top.
*/
float LinceGetMouseY();

#endif // LINCE_INPUT_H