#ifndef LINCE_INPUT_H
#define LINCE_INPUT_H

#include "lince/core/core.h"
#include "lince/renderer/transform.h"

/** @brief Returns true if a keyboard key is held.
*   @param key Enum value for a key. See `LinceKey`.
*/
LinceBool LinceIsKeyPressed(int key);

/** @brief Returns true if a mosue button is held
*   @brief button Enum value for a mouse button. See `LinceMouseButton`.
*/
LinceBool LinceIsMouseButtonPressed(int button);

/** @brief Returns the pixel coordinates of the mouse pointer on the screen.
*/
LincePoint LinceGetMousePos(void);

/** @brief Returns the x position of the mouse.
* This position is in pixel coordinates of the window, with origin on the right.
*/
float LinceGetMouseX();

/** @brief Returns the y position of the mouse.
* This position is in pixel coordinates of the window, with origin on the top.
*/
float LinceGetMouseY();

#endif // LINCE_INPUT_H