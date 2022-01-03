#ifndef LINCE_APP_H
#define LINCE_APP_H

#include "renderer/window.h"
#include "event/event.h"
#include "event/keyEvent.h"
#include "event/mouseEvent.h"
#include "event/windowEvent.h"

#include "layer.h"

typedef void (*LinceGame_InitFn)();
typedef void (*LinceGame_OnUpdateFn)();
typedef void (*LinceGame_OnEventFn)(Event*);
typedef void (*LinceGame_TerminateFn)();

typedef struct {
    LinceWindow* window;
    LinceLayerStack *layer_stack, *overlay_stack;
    unsigned int running;

    // User-defined game data & function pointers
    void* game_data;
    LinceGame_InitFn game_init;
    LinceGame_OnUpdateFn game_on_update;
    LinceGame_OnEventFn game_on_event;
    LinceGame_TerminateFn game_terminate;

} LinceApp;


void Lince_SetGameInitFn(LinceGame_InitFn func);
void Lince_SetGameOnUpdateFn(LinceGame_OnUpdateFn func);
void Lince_SetGameOnEventFn(LinceGame_OnEventFn func);
void Lince_SetGameTerminateFn(LinceGame_TerminateFn func);

void LinceApp_Run();

LinceApp* LinceApp_GetApplication();
void LinceApp_SetGameData(void* data);
void* LinceApp_GetGameData();

void LinceApp_PushLayer(LinceLayer* layer);
void LinceApp_PushOverlay(LinceLayer* overlay);


#endif // LINCE_APP_H