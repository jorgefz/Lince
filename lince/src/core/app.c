
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core/app.h"
#include "renderer/renderer.h"

/* Private application state - stack allocated */
static LinceApp app = {0};

/* --- Static functions --- */

/* Initialises OpenGL window and layer stacks */
/* static void LinceInit(); */

/* Calls the program's main loop */
void LinceRun();

/* Called once per frame, updates window and renders layers */
/* static void LinceOnUpdate(); */

/* Shuts down application and frees allocated memory */
static void LinceTerminate();

/* Called when game event occurs,
propagates it to layers and user */
static void LinceOnEvent(LinceEvent* e);

static LinceBool LinceOnEventWindowResize(LinceEvent* e);
static LinceBool LinceOnEventWindowClose(LinceEvent* e);


/* --- Public API --- */

void LinceRun(){

    LinceInit(530, 300, 0); // width, height, flags
    app.running = LinceTrue;

    while(app.running){
        LinceOnUpdate();
    }
    LinceTerminate();
}

void LinceSetGameInitFn(LinceGameInitFn func) {
    app.game_init = func;
}

void LinceSetGameOnUpdateFn(LinceGameOnUpdateFn func) {
    app.game_on_update = func;
}

void LinceSetGameOnEventFn(LinceGameOnEventFn func) {
    app.game_on_event = func;
}

void LinceSetGameTerminateFn(LinceGameTerminateFn func) {
    app.game_terminate = func;
}


LinceApp* LinceGetAppState(){
    return &app;
}

void LinceSetUserData(void* data) {
    app.user_data = data;
}

void* LinceGetUserData() {
    return app.user_data;
}

void LincePushLayer(LinceLayer* layer) {
    LinceLayerStackPush(app.layer_stack, layer);
}

void LincePushOverlay(LinceLayer* overlay) {
    LinceLayerStackPush(app.overlay_stack, overlay);
}

double LinceGetTimeMillis(){
    return (glfwGetTime() * 1000.0);
}

LinceLayer* LinceGetCurrentLayer(){
    if (app.current_layer < 0) return NULL;
    return app.layer_stack->layers[app.current_layer];
}

LinceLayer* LinceGetCurrentOverlay(){
    if (app.current_overlay < 0) return NULL;
    return app.overlay_stack->layers[app.current_overlay];
}


/* --- Implementations of static functions --- */

/*static*/ void LinceInit(unsigned int width, unsigned int height, int flags){
    // Create a windowed mode window and its OpenGL context
    app.window = LinceCreateWindow(width, height);
    LinceSetMainEventCallback(app.window, LinceOnEvent);

    // init layer and overlay stacks
    app.layer_stack = LinceCreateLayerStack();
    app.overlay_stack = LinceCreateLayerStack();
    
    if (app.game_init) app.game_init(); // user may push layers onto stack
}


/*static*/ void LinceOnUpdate(){
    LinceRender_Clear();

    // Calculate delta time
    float new_time_ms = (float)(glfwGetTime() * 1000.0);
    app.dt = new_time_ms - app.time_ms;
    app.time_ms = new_time_ms;

    // update layers
    unsigned int i;
    for (i = 0; i != app.layer_stack->count; ++i) {
        LinceLayer* layer = app.layer_stack->layers[i];
        app.current_layer = i;
        if (layer && layer->OnUpdate) layer->OnUpdate(layer, app.dt);
    }
    app.current_layer = -1;

    // update overlays
    for (i = 0; i != app.overlay_stack->count; ++i) {
        LinceLayer* overlay = app.overlay_stack->layers[i];
        app.current_overlay = i;
        if (overlay && overlay->OnUpdate) overlay->OnUpdate(overlay, app.dt);
    }
    app.current_overlay = -1;

    // update user application
    if (app.game_on_update) app.game_on_update(app.dt);

    LinceUpdateWindow(app.window);
}

static void LinceTerminate(){

    // free layer and overlay stacks
    LinceDestroyLayerStack(app.layer_stack);
    LinceDestroyLayerStack(app.overlay_stack);
    app.layer_stack = NULL;
    app.overlay_stack = NULL;

    if (app.game_terminate) app.game_terminate();

    /* shutdown window last, as it destroys opengl context
    and all its functions */
    LinceDestroyWindow(app.window);
    app.window = NULL;
    app.running = 0;
}

static void LinceOnEvent(LinceEvent* e){
    /* Pre-defined event responses:
    adapt viewport when window is resized,
    and shutdown program when window is closed */
    LinceDispatchEvent(
        e,
        LinceEventType_WindowResize,
        LinceOnEventWindowResize
    );
    LinceDispatchEvent(e,
        LinceEventType_WindowClose,
        LinceOnEventWindowClose
    );

    /* propagate event to layers and overlays,
    the ones in front (rendered last) receive it first */
    int i;
    for (i = (int)app.overlay_stack->count - 1; i >= 0; --i) {
        if (e->handled) break;
        LinceLayer* overlay = app.overlay_stack->layers[i];
        app.current_overlay = i;
        if (overlay && overlay->OnEvent) overlay->OnEvent(overlay, e);
    }
    app.current_overlay = -1;

    for (i = (int)app.layer_stack->count - 1; i >= 0; --i) {
        if (e->handled) break;
        LinceLayer* layer = app.layer_stack->layers[i];
        app.current_layer = i;
        if (layer && layer->OnEvent) layer->OnEvent(layer, e);
    }
    app.current_layer = -1;

    // propagate event to user
    if (app.game_on_event && !e->handled ) app.game_on_event(e);
}


static LinceBool LinceOnEventWindowResize(LinceEvent* e){
    LINCE_INFO(" Window resized to %d x %d", 
        (int)e->data.WindowResize->width,
        (int)e->data.WindowResize->height
    );
    return LinceFalse; // allow other layers to receive event
}

static LinceBool LinceOnEventWindowClose(LinceEvent* e) {
    app.running = LinceFalse;
    return LinceFalse; // allow other layers to receive event
}



