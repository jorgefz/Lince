
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "app.h"

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

LinceApp* LinceApp_GetApplication(){
    return &app;
}

void LinceApp_SetGameData(void* data) {
    app.game_data = data;
}

void* LinceApp_GetGameData() {
    return app.game_data;
}

void LinceApp_PushLayer(LinceLayer* layer) {
    LinceLayerStack_Push(app.layer_stack, layer);
}

void LinceApp_PushOverlay(LinceLayer* overlay) {
    LinceLayerStack_Push(app.overlay_stack, overlay);
}


// Private functions

static void LinceApp_OnEvent(Event* e);

static void LinceApp_Terminate();

static void LinceApp_Init(){
    // Create a windowed mode window and its OpenGL context
    app.window = LinceWindow_Create(1280, 720);
    LinceWindow_SetEventCallback(app.window, LinceApp_OnEvent);

    // init layer and overlay stacks
    app.layer_stack = LinceLayerStack_Create();
    app.overlay_stack = LinceLayerStack_Create();
    
    if (app.game_init) app.game_init(); // user may push layers onto stack
}

static void LinceApp_OnUpdate(){
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


static unsigned int LinceApp_OnWindowResize(Event* e){
    printf("Window resized to %d x %d\n", e->data.WindowResize->width, e->data.WindowResize->width);
    return 0; // allow other layers to receive event
}

static unsigned int LinceApp_OnWindowClose(Event* e) {
    app.running = 0;
    return 0; // allow other layers to receive event
}

static void LinceApp_OnEvent(Event* e){
    // Pre-defined event responses
    LinceEvent_Dispatch(e, EventType_WindowResize, LinceApp_OnWindowResize);
    LinceEvent_Dispatch(e, EventType_WindowClose, LinceApp_OnWindowClose);

    // pass event to layers and overlays
    // the ones in front receive it first
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

    // pass event to game app
    if (app.game_on_event && !e->handled ) app.game_on_event(e);
}

void LinceApp_Run(){
    
    LinceApp_Init();
    app.running = 1;

    float r = 0.0f, v = 0.0001f, b;
    while(app.running){
        r += v;
        if (r >= 1.0f) v = -v;
        else if (r <= 0.0f) v = -v;
        b = 1.0f - r;        
        glClearColor(r, 0.1f, b, 1.0f);

        LinceApp_OnUpdate();
    }

    LinceApp_Terminate();
}

static void LinceApp_Terminate(){
    LinceWindow_Destroy(app.window);
    app.window = NULL;
    app.running = 0;

    // destroy layer and overlay stacks
    LinceLayerStack_Destroy(app.layer_stack);
    LinceLayerStack_Destroy(app.overlay_stack);
    app.layer_stack = NULL;
    app.overlay_stack = NULL;

    if (app.game_terminate) app.game_terminate();
}



