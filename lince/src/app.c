
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "app.h"
#include "renderer/renderer.h"

static LinceApp app = {0};


// Public API
void Lince_SetGameInitFn(LinceGame_InitFn func) {
    app.game_init = func;
}

void Lince_SetGameOnUpdateFn(LinceGame_OnUpdateFn func) {
    app.game_on_update = func;
}

void Lince_SetGameOnEventFn(LinceGame_OnEventFn func) {
    app.game_on_event = func;
}

void Lince_SetGameTerminateFn(LinceGame_TerminateFn func) {
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


/* --- Static functions --- */

/* Initialises OpenGL window and layer stacks */
static void LinceInit();

/* Calls the program's main loop */
void LinceRun();

/* Called once per frame, updates window and renders layers */
static void LinceOnUpdate();

/* Shuts down application and frees allocated memory */
static void LinceTerminate();

/* Called when game event occurs,
propagates it to layers and user */
static void LinceOnEvent(LinceEvent* e);

static LinceBool LinceOnEventWindowResize(LinceEvent* e);
static LinceBool LinceOnEventWindowClose(LinceEvent* e);


/* --- Implementations of static functions */

static void LinceInit(){
    // Create a windowed mode window and its OpenGL context
    app.window = LinceWindow_Create(1280, 720);
    LinceWindow_SetEventCallback(app.window, LinceOnEvent);

    // init layer and overlay stacks
    app.layer_stack = LinceCreateLayerStack();
    app.overlay_stack = LinceCreateLayerStack();
    
    if (app.game_init) app.game_init(); // user may push layers onto stack
}

void LinceRun(){

    LinceInit();
    app.running = LinceTrue;

    while(app.running){
        LinceOnUpdate();
    }
    LinceTerminate();
}

static void LinceOnUpdate(){
    glClear(GL_COLOR_BUFFER_BIT);
    LinceWindow_Update(app.window);

    // update layers
    unsigned int i;
    for (i = 0; i != app.layer_stack->count; ++i) {
        LinceLayer* layer = app.layer_stack->layers[i];
        if (layer && layer->OnUpdate) layer->OnUpdate(layer);
    }
    // update overlays
    for (i = 0; i != app.overlay_stack->count; ++i) {
        LinceLayer* overlay = app.overlay_stack->layers[i];
        if (overlay && overlay->OnUpdate) overlay->OnUpdate(overlay);
    }
    
    // update game app
    if (app.game_on_update) app.game_on_update();
}

static void LinceTerminate(){
    LinceWindow_Destroy(app.window); // shutdown opengl window
    app.window = NULL;
    app.running = 0;

    // free layer and overlay stacks
    LinceDestroyLayerStack(app.layer_stack);
    LinceDestroyLayerStack(app.overlay_stack);
    app.layer_stack = NULL;
    app.overlay_stack = NULL;

    if (app.game_terminate) app.game_terminate();
}

static void LinceOnEvent(LinceEvent* e){
    /* Pre-defined event responses:
    adapt viewport when window is resized,
    and shutdown program when window is closed */
    LinceEvent_Dispatch(
        e,
        LinceEventType_WindowResize,
        LinceOnEventWindowResize
    );
    LinceEvent_Dispatch(e,
        LinceEventType_WindowClose,
        LinceOnEventWindowClose
    );

    /* propagate event to layers and overlays,
    the ones in front (rendered last) receive it first */
    int i;
    for (i = (int)app.overlay_stack->count - 1; i >= 0; --i) {
        if (e->handled) break;
        LinceLayer* overlay = app.overlay_stack->layers[i];
        if (overlay && overlay->OnEvent) overlay->OnEvent(overlay, e);
    }
    for (i = (int)app.layer_stack->count - 1; i >= 0; --i) {
        if (e->handled) break;
        LinceLayer* layer = app.layer_stack->layers[i];
        if (layer && layer->OnEvent) layer->OnEvent(layer, e);
    }

    // propagate event to user
    if (app.game_on_event && !e->handled ) app.game_on_event(e);
}


static LinceBool LinceOnEventWindowResize(LinceEvent* e){
    printf("Window resized to %d x %d\n", 
        e->data.WindowResize->width,
        e->data.WindowResize->width
    );
    return LinceFalse; // allow other layers to receive event
}

static LinceBool LinceOnEventWindowClose(LinceEvent* e) {
    app.running = LinceFalse;
    return LinceFalse; // allow other layers to receive event
}



