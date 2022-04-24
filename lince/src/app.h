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
typedef void (*LinceGame_OnEventFn)(LinceEvent*);
typedef void (*LinceGame_TerminateFn)();

/* LinceApp holds program state */
typedef struct {
    LinceWindow     *window;
    LinceLayerStack *layer_stack;
    LinceLayerStack *overlay_stack;
    LinceBool        running;

    /* User-defined data & functions */
    void* user_data;
    LinceGame_InitFn      game_init;        /* Called on initialisation */
    LinceGame_OnUpdateFn  game_on_update;   /* Called once per frame */
    LinceGame_OnEventFn   game_on_event;    /* Events are passed to this */
    LinceGame_TerminateFn game_terminate;   /* Called on program end */

} LinceApp;

/* Sets user defined functions to interact with program */
void Lince_SetGameInitFn(LinceGame_InitFn func);
void Lince_SetGameOnUpdateFn(LinceGame_OnUpdateFn func);
void Lince_SetGameOnEventFn(LinceGame_OnEventFn func);
void Lince_SetGameTerminateFn(LinceGame_TerminateFn func);

 /* Runs main application loop */
void LinceRun();

/* Adds a rendering layer to the program */
void LincePushLayer(LinceLayer* layer);

/* Adds a rendering overlay to the program.
Overlays are rendered after layers */
void LincePushOverlay(LinceLayer* overlay);

/* Store user data.
This is useful to pass custom data to runtime functions
such as OnEvent or OnUpdate */
void LinceSetUserData(void* data);

/* Retrieve user data during runtime */
void* LinceGetUserData();

/* Returns time since initialisation in milliseconds */
double LinceGetTimeMillis();

/* Returns the current state of the running program */
LinceApp* LinceGetAppState();


#endif // LINCE_APP_H