#ifndef LINCE_INPUT_H
#define LINCE_INPUT_H

/* Returns true if given LinceKey is held */
unsigned int LinceIsKeyPressed(int key);

/* Returns true if given LinceMouseButton is held */
unsigned int LinceIsMouseButtonPressed(int button);

/*
Provides 2D coordinates of the mouse in the screen,
these are the xy pixel positions with origin
on the top left of the window.
*/
void LinceGetMousePos(float* xpos, float* ypos);

/*
Returns X position of the mouse,
in pixel coordinates of the window, with origin on the right
*/
float LinceGetMouseX();

/*
Returns Y position of the mouse,
in pixel coordinates of the window, with origin on the top
*/
float LinceGetMouseY();

#endif // LINCE_INPUT_H