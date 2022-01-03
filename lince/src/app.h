#ifndef LINCE_APP_H
#define LINCE_APP_H

#include "renderer/window.h"
#include "event/event.h"
#include "event/keyEvent.h"
#include "event/mouseEvent.h"
#include "event/windowEvent.h"

#include "layer.h"

typedef struct {
    LinceWindow* window;
    LinceLayerStack *layer_stack, *overlay_stack;
    unsigned int running;
} LinceApp;


void LinceApp_Run();

LinceApp* LinceApp_GetApplication();
void LinceApp_PushLayer(LinceLayer* layer);
void LinceApp_PushOverlay(LinceLayer* overlay);


// User-defined function pointers
extern void (*LinceGame_Init_ptr)();
extern void (*LinceGame_OnUpdate_ptr)();
extern void (*LinceGame_OnEvent_ptr)(Event*);
extern void (*LinceGame_Terminate_ptr)();

#endif // LINCE_APP_H