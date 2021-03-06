#ifndef LINCE_APP_H
#define LINCE_APP_H

#include "core/window.h"
#include "event/event.h"
#include "event/key_event.h"
#include "event/mouse_event.h"
#include "event/window_event.h"

#include "core/layer.h"
#include "gui/ui_layer.h"

/* Function pointer typedefs for user callbacks */
typedef void (*LinceGameInitFn)();
typedef void (*LinceGameOnUpdateFn)(float dt);
typedef void (*LinceGameOnEventFn)(LinceEvent*);
typedef void (*LinceGameTerminateFn)();

/* Holds program state */
typedef struct LinceApp{
    /* User settings */
    void* user_data;
    uint32_t screen_width, screen_height;
    const char* title;
    // add more settings

    /* User callbacks */
    LinceGameInitFn      game_init;        /* Called on initialisation */
    LinceGameOnUpdateFn  game_on_update;   /* Called once per frame */
    LinceGameOnEventFn   game_on_event;    /* Events are passed to this */
    LinceGameTerminateFn game_terminate;   /* Called on program end */

    /* Internal state */
    LinceWindow     *window;
    LinceLayerStack *layer_stack;
    LinceLayerStack *overlay_stack;
    LinceBool        running;
    float time_ms;          // clock in milliseconds
    float dt;               // timestep in ms
    int current_layer;      // index of layer baing updated/handled
    int current_overlay;    // index of layer baing updated/handled

    /* UI */
    LinceUILayer* ui;

} LinceApp;


/* Runs main application loop */
void LinceRun();

/* Fetches any OpenGL errors and exits if sucessful */
void LinceCheckErrors();

/* Adds a rendering layer to the program */
void LincePushLayer(LinceLayer* layer);

/* Adds a rendering overlay to the program.
Overlays are rendered after layers */
void LincePushOverlay(LinceLayer* overlay);

/* IMPROVE THIS -
Returns time since initialisation in milliseconds */
double LinceGetTimeMillis();

/* Returns the current state of the running program */
LinceApp* LinceGetAppState();

/* Returns current aspect ratio of the window */
float LinceGetAspectRatio();

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