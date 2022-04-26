#ifndef LINCE_APP_H
#define LINCE_APP_H

#include "core/window.h"
#include "event/event.h"
#include "event/key_event.h"
#include "event/mouse_event.h"
#include "event/window_event.h"

#include "core/layer.h"

/* Function pointer typedefs for user callbacks */
typedef void (*LinceGameInitFn)();
typedef void (*LinceGameOnUpdateFn)(float dt);
typedef void (*LinceGameOnEventFn)(LinceEvent*);
typedef void (*LinceGameTerminateFn)();

/* Holds program state */
typedef struct {
    LinceWindow     *window;
    LinceLayerStack *layer_stack;
    LinceLayerStack *overlay_stack;
    LinceBool        running;
    float time_ms;  // clock in milliseconds
    float dt;       // timestep in ms
    int current_layer; // index of layer baing updated/handled
    int current_overlay; // index of layer baing updated/handled

    /* User-defined data & functions */
    void* user_data;
    LinceGameInitFn      game_init;        /* Called on initialisation */
    LinceGameOnUpdateFn  game_on_update;   /* Called once per frame */
    LinceGameOnEventFn   game_on_event;    /* Events are passed to this */
    LinceGameTerminateFn game_terminate;   /* Called on program end */

} LinceApp;

/* Sets user defined callbacks to interact with program */
void LinceSetGameInitFn(LinceGameInitFn func); /* Called on initialisation */
void LinceSetGameOnUpdateFn(LinceGameOnUpdateFn func); /* Called on every frame */
void LinceSetGameOnEventFn(LinceGameOnEventFn func); /* Called on unhandled event */
void LinceSetGameTerminateFn(LinceGameTerminateFn func); /* Called on shutdown */

 /* Runs main application loop */
void LinceRun();

/* TEMPORARY */
void LinceInit(unsigned int width, unsigned int height, int flags);
void LinceOnUpdate();

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

/* IMPROVE THIS -
Returns time since initialisation in milliseconds */
double LinceGetTimeMillis();

/* Returns the current state of the running program */
LinceApp* LinceGetAppState();

/*
Returns current layer being handled or updated
Useful on specific event callbacks.
Returns NULL if no layer is being handled.
*/
LinceLayer* LinceGetCurrentLayer();

/*
Returns current overlay being handled or updated
Useful on specific event callbacks.
Returns NULL if no overlay is being handled.
*/
LinceLayer* LinceGetCurrentOverlay();


#endif // LINCE_APP_H