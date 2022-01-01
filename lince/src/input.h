#ifndef LINCE_INPUT_H
#define LINCE_INPUT_H

unsigned int LinceInput_IsKeyPressed(int key);
unsigned int LinceInput_IsMouseButtonPressed(int button);
void LinceInput_GetMousePos(float* xpos, float* ypos);
float LinceInput_GetMouseX();
float LinceInput_GetMouseY();

#endif // LINCE_INPUT_H