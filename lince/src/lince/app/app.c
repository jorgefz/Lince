
#include <stdio.h>

#include <glad/glad.h>

#include "app/app.h"
#include "renderer/renderer.h"
#include "gui/gui.h"
#include "input/input.h"
#include "core/profiler.h"
#include "utils/memory.h"


/* ==== Macros ==== */

/** Iterates over layers, calling the provided callback.
*   NOTE: requires __VA_ARGS__ to consume preceding comma!!
*   This is supported by (old) MSVC and GCC as an extension.
**/
#define LINCE_MAP_OVER_LAYERS(STACK, CURRENT, CALLBACK, ...)    \
    for(uint32_t i = 0; i != (STACK).size; ++i){                \
        CURRENT = i;                                            \
        LinceLayer* layer = array_get(&(STACK), i);             \
        if (layer && layer->CALLBACK){                          \
            layer->CALLBACK(layer, ##__VA_ARGS__);              \
        }                                                       \
    }                                                           \
    CURRENT = -1;                                               \


/* ==== Static variables ==== */

/** @brief Private application state - stack allocated */
static LinceApp app = {0};


/* ==== Static function declarations ==== */

/** Initialises OpenGL window and layer stacks */
static void LinceInit();

/** Called once per frame, updates window and renders layers */
static void LinceAppOnUpdate();

/** Shuts down application and frees allocated memory */
static void LinceAppTerminate();

/** Called when game event occurs,
propagates it to layers and user */
static void LinceAppOnEvent(LinceEvent* e);

/** Draws an UI panel showing debug information & stats */
static void LinceAppDrawDebugUIPanel(LinceLayer* overlay, float dt);

/** Window event callbacks */
static LinceBool LinceAppEventWindowResize(LinceEvent* e);
static LinceBool LinceAppEventWindowClose(LinceEvent* e);


/* ==== Public function definitions ==== */

/** @brief Runs main application loop. `LinceInit()` must have been called.
*/
void LinceRun(){

    LinceInit();
    
    LINCE_INFO("Running main loop...");
    while(app.running){
        LinceAppOnUpdate();
    }
    LinceAppTerminate();
}

/** @brief Returns the global state of the application. See `LinceApp`.
*/
LinceApp* LinceGetApp(){
    return &app;
}

/** @brief Set the window title. Only works before the window is initialised.
*/
void LinceAppSetTitle(const char* title) {
    size_t len = strlen(title) + 1;
    memcpy(app.title, title, (len > LINCE_TITLE_MAX) ? LINCE_TITLE_MAX : len);
    app.title[LINCE_TITLE_MAX - 1] = '\0';
}

/** @brief Save the location of an assets folder relative to the executable */
void LinceAppPushAssetDir(const char* dir){
    LinceAssetCachePushDir(&app.asset_cache, dir);
}

/** @brief Save the location of an assets folder relative to the executable */
char* LinceAppFetchAssetPath(const char* filename){
    return LinceAssetCacheFetchPath(&app.asset_cache, filename);
}

/** @brief Adds a rendering layer to the application.
* @param layer Rendering layer to push onto the application's layer stack.
*/
void LinceAppPushLayer(LinceLayer* layer) {
    if(layer->on_attach) layer->on_attach(layer);
    array_push_back(&app.layer_stack, layer);
}

/** @brief Adds a rendering overlay to the application.
* @param overlay Rendering overlays to push onto the application's overlay stack.
*                Overlays are rendered after layers.
*/
void LinceAppPushOverlay(LinceLayer* overlay) {
    if(overlay->on_attach) overlay->on_attach(overlay);
    array_push_back(&app.overlay_stack, overlay);
}

/** @brief Removes a rendering layer from the layer stack.
* @param layer Rendering layer to remove. It's 'on_detach' method will be called.
*/
void LinceAppPopLayer(LinceLayer* layer) {
    if(layer->on_detach) layer->on_detach(layer);
    for(uint32_t i = 0; i != app.layer_stack.size; ++i){
        if(layer == (LinceLayer*)array_get(&app.layer_stack, i)){
            array_remove(&app.layer_stack, i);
            return;
        }
    }
    LINCE_ASSERT(0, "Failed to find layer (0x%p) in stack", layer);
}

/** @brief Removes a rendering overlay from the layer stack.
* @param layer Rendering overlay to remove. It's 'on_detach' method will be called.
*/
void LinceAppPopOverlay(LinceLayer* overlay) {
    if(overlay->on_detach) overlay->on_detach(overlay);
    for(uint32_t i = 0; i != app.overlay_stack.size; ++i){
        if(overlay == (LinceLayer*)array_get(&app.overlay_stack, i)){
            array_remove(&app.overlay_stack, i);
            return;
        }
    }
    LINCE_ASSERT(0, "Failed to find overlay (0x%p) in stack", overlay);
}

/** @brief Returns current layer being handled or updated.
* Returns NULL if no layer is being handled.
* Should only be used within a layer's OnUpdate and OnEvent callbacks.
*/
LinceLayer* LinceAppGetCurrentLayer(){
    if (app.current_layer < 0) return NULL;
    return array_get(&app.layer_stack, app.current_layer);
}

/** @brief Returns current overlay being handled or updated.
* Returns NULL if no overlay is being handled.
* Should only be used within a overlay's OnUpdate and OnEvent callbacks.
*/
LinceLayer* LinceAppGetCurrentOverlay(){
    if (app.current_overlay < 0) return NULL;
    return array_get(&app.overlay_stack, app.current_overlay);
}

/** @brief Creates new scene in cache with defined callbacks. Will not call `on_init`.
* @param name Scene identifier
* @callbacks scene struct with callbacks defined
*/
void LinceAppRegisterScene(const char* name, LinceScene* callbacks) {
    hashmap_set(&app.scene_cache, name, LinceNewCopy(callbacks, sizeof(LinceScene)) );
}

/** @brief Sets a scene as the current scene. Calls its on_init method if uninitialised.
* Must have been registered with `LinceRegisterScene`.
* @param name Scene identifier to load
*/
void LinceAppLoadScene(const char* name) {
     LinceScene* next_scene = hashmap_get(&app.scene_cache, name);
     LINCE_ASSERT(next_scene, "Could not load scene '%s'", name);
     app.current_scene = next_scene;
     if (!app.current_scene->loaded){
         LinceInitScene(app.current_scene);
         LINCE_INFO("Initialised scene '%s'", name);
     }
     LINCE_INFO("Switched to scene '%s'", name);
}

/** @brief Return the scene with a given string identifier, or NULL if the scene has not been registered.
* @param name Scene identifier to load
* @returns Scene with matching identifier
*/
LinceScene* LinceAppGetScene(const char* name) {
    return hashmap_get(&app.scene_cache, name);
}

/** @brief Returns aspect ratio of the window.
*/
float LinceAppGetAspectRatio(){
    return (float)app.window->width / (float)app.window->height;
}

/** @brief Returns the current window width and height in pixels */
LincePoint LinceAppGetScreenSize(){
    return (LincePoint) {
        .x = (float)app.window->width,
        .y = (float)app.window->height
    };
}

/** @brief Returns the position of the mouse pointer in world coordinates.
* @param cam Camera used to determine the world position.
*/
LincePoint LinceGetMousePosWorld(LinceCamera* cam) {
    LincePoint scr = LinceAppGetScreenSize();
    LincePoint pos = LincePointPixelToScreen(LinceGetMousePos(), scr.x, scr.y);
    return LincePointScreenToWorld(pos, cam);
}



/* ==== Public function definitions ==== */


static void LinceInit(){
    // Open log file
    LinceOpenLogger(LINCE_DIR"log.txt");

    // Report platform and configuration
    LINCE_INFO("Lince version: "LINCE_VERSION);
    
#ifdef LINCE_WINDOWS
    LINCE_INFO("OS: Windows");
#elif defined(LINCE_LINUX)
    LINCE_INFO("OS: GNU/Linux");
#endif

#ifdef LINCE_DEBUG
    LINCE_INFO("Debug configuration");
#else
    LINCE_INFO("Release configuration");
#endif

    // Open profiling file
#ifdef LINCE_PROFILE
    LINCE_INFO("PROFILING ENABLED");
    LINCE_INFO("Saving profiling data to '%s'", LINCE_DIR"profiler.txt");
    LinceOpenProfiler(LINCE_DIR"profiler.txt");
#endif

    // Check user settings and set defaults
    if (app.screen_width == 0) app.screen_width = 500;
    if (app.screen_height == 0) app.screen_height = 500;
    
    // Create a windowed mode window and its OpenGL context
    app.window = LinceCreateWindow(app.screen_width, app.screen_height, app.title);
    LinceSetMainEventCallback(app.window, LinceAppOnEvent);

    // Create layer stacks
    array_init(&app.layer_stack, sizeof(LinceLayer));
    array_init(&app.overlay_stack, sizeof(LinceLayer));
    
    // Create scene stack
    // array_init(&app.scene_stack, sizeof(LinceScene));
    void* success = hashmap_init(&app.scene_cache, 5);
    LINCE_ASSERT(success, "Failed to create scene cache");

    // Create asset manager
    LinceInitAssetCache(&app.asset_cache);
    LinceAssetCachePushDir(&app.asset_cache, "../../../lince/assets");

    LinceInitRenderer(app.window);

    /// TODO: improve font handling
    app.ui = LinceInitUI(app.window->handle);
    
    app.clock = LinceNewClock();
    app.runtime = 0;

    app.running = LinceTrue;
    if (app.on_init) app.on_init(); // user may push layers onto stack

    // Delay loading fonts to give the user a chance to push custom asset paths on init
    LinceUILoadFonts(app.ui, &app.asset_cache);

    #ifdef LINCE_DEBUG
    // Default font
    nk_style_set_font(app.ui->ctx, &app.ui->fonts[LinceFont_Droid8]->handle);
    // Create panel with debug info
    LinceAppPushOverlay(&(LinceLayer){.on_update = LinceAppDrawDebugUIPanel});
    #endif
}

static void LinceAppOnUpdate(){
    LINCE_PROFILER_START(timer);
    LinceClear();

    // Calculate time elapsed between frames
    float runtime = (float)LinceReadClock(app.clock) * 1000.0f; // to millisecs
    app.dt = runtime - app.runtime;
    app.runtime = (float)LinceReadClock(app.clock) * 1000.0f;

    app.screen_width = app.window->width;
    app.screen_height = app.window->height;

    LinceBeginUIRender(app.ui);

    // Update layers
    LINCE_MAP_OVER_LAYERS(app.layer_stack, app.current_layer, on_update, app.dt);
    LINCE_MAP_OVER_LAYERS(app.overlay_stack, app.current_overlay, on_update, app.dt);

    // Update current scene
    if(app.current_scene){
        LinceUpdateScene(app.current_scene, app.dt);
    }

    // Update user application
    if (app.on_update) app.on_update(app.dt);

    LinceEndUIRender(app.ui);
    LinceUpdateWindow(app.window);
    LINCE_PROFILER_END(timer);
}

static void LinceAppTerminate(){

    if (app.on_terminate) app.on_terminate();

    LinceTerminateRenderer();
    
    // Destroy layer stacks
    LINCE_MAP_OVER_LAYERS(app.layer_stack, app.current_layer, on_detach);
    LINCE_MAP_OVER_LAYERS(app.overlay_stack, app.current_overlay, on_detach);
    array_uninit(&app.layer_stack);
    array_uninit(&app.overlay_stack);
    
    // Destroy scene cache
    char* key = NULL;
    while ((key = hashmap_iter(&app.scene_cache, key))) {
        LinceScene* scene = hashmap_get(&app.scene_cache, key);
        if (scene) {
            if (scene->loaded) {
                LinceUninitScene(scene);
            }
            LinceFree(scene);
        }
    }
    hashmap_uninit(&app.scene_cache);
    
    LinceTerminateUI(app.ui);

    LinceUninitAssetCache(&app.asset_cache);

    /* shutdown window last, as it destroys opengl context
    and all its functions */
    LinceDestroyWindow(app.window);
    app.window = NULL;
    app.running = 0;
    
    LinceCloseProfiler();
    LinceCloseLogger();
}

static void LinceAppOnEvent(LinceEvent* e){
    /* Pre-defined event responses:
    adapt viewport when window is resized,
    and shutdown program when window is closed */
    LinceDispatchEvent(
        e,
        LinceEventType_WindowResize,
        LinceAppEventWindowResize
    );
    LinceDispatchEvent(e,
        LinceEventType_WindowClose,
        LinceAppEventWindowClose
    );

    LinceUIOnEvent(app.ui, e);

    /* Propagate event to layers and overlays,
    the ones in front (rendered last) receive it first */
    LINCE_MAP_OVER_LAYERS(app.layer_stack, app.current_layer, on_event, e);
    LINCE_MAP_OVER_LAYERS(app.overlay_stack, app.current_overlay, on_event, e);

    // Propagate event to user
    if (app.on_event && !e->handled ) app.on_event(e);
}

static void LinceAppDrawDebugUIPanel(LinceLayer* overlay, float dt){
    LINCE_UNUSED(overlay);
    
    LinceUILayer* ui = LinceGetApp()->ui;
    struct nk_context *ctx = ui->ctx;
    nk_style_push_font(ctx, &ui->fonts[LinceFont_Droid20]->handle);

    if (nk_begin(ctx, "Debug", nk_rect(50, 50, 300, 250),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
        NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE
    )) {

        nk_layout_row_static(ctx, 30, 250, 1);
        
        nk_labelf(ctx, NK_TEXT_LEFT, "Window: %ux%u", app.screen_width, app.screen_height);
        nk_labelf(ctx, NK_TEXT_LEFT, "dt: %.2f ms", dt);
        nk_labelf(ctx, NK_TEXT_LEFT, "FPS: %.2f", 1000.0f/dt);
        nk_labelf(ctx, NK_TEXT_LEFT, "Runtime: %.2f s", app.runtime/1000.0f);
        
    }
    nk_end(ctx);
    nk_style_pop_font(ctx);
}

static LinceBool LinceAppEventWindowResize(LinceEvent* e){
    LINCE_INFO(" Window resized to %d x %d", 
        (int)e->data.window_resize->width,
        (int)e->data.window_resize->height
    );
    return LinceFalse; // allow other layers to receive event
}

static LinceBool LinceAppEventWindowClose(LinceEvent* e) {
    app.running = LinceFalse;
    return LinceFalse; // allow other layers to receive event
    LINCE_UNUSED(e);
}

