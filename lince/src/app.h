#ifndef LINCE_APP_H
#define LINCE_APP_H

#include "lince.h"

typedef struct {
    LinceWindow* window;
} LinceApp;

LinceApp* LinceApp_GetApplication();


// User-defined function pointers
void (*LinceGame_Init_ptr)(LinceWindow*);
void (*LinceGame_OnUpdate_ptr)(LinceWindow*, void* args);
void (*LinceGame_OnEvent_ptr)(LinceWindow*, void* args);
void (*LinceGame_Terminate_ptr)(LinceWindow*, void* args);

#endif // LINCE_APP_H