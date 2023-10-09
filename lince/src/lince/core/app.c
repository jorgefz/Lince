
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core/app.h"
#include "renderer/renderer.h"
#include "gui/ui_layer.h"
#include "input/input.h"
#include "core/profiler.h"
#include "core/memory.h"

/* Private application state - stack allocated */
static LinceApp app = {0};

/* Calls the program's main loop */
void LinceRun();


/* --- Static functions --- */

/* Initialises OpenGL window and layer stacks */
static void LinceInit();

/* Called once per frame, updates window and renders layers */
static void LinceOnUpdate();

/* Shuts down application and frees allocated memory */
static void LinceTerminate();

/* Called when game event occurs,
propagates it to layers and user */
static void LinceOnEvent(LinceEvent* e);

/* Window event callbacks */
static LinceBool LinceOnEventWindowResize(LinceEvent* e);
static LinceBool LinceOnEventWindowClose(LinceEvent* e);

static char const* GLGetErrorString(GLenum const err) {
  switch (err) {
    // opengl 2 errors (8)
    case GL_NO_ERROR:
      return "GL_NO_ERROR";

    case GL_INVALID_ENUM:
      return "GL_INVALID_ENUM";

    case GL_INVALID_VALUE:
      return "GL_INVALID_VALUE";

    case GL_INVALID_OPERATION:
      return "GL_INVALID_OPERATION";

    case GL_STACK_OVERFLOW:
      return "GL_STACK_OVERFLOW";

    case GL_STACK_UNDERFLOW:
      return "GL_STACK_UNDERFLOW";

    case GL_OUT_OF_MEMORY:
      return "GL_OUT_OF_MEMORY";

    // opengl 3 errors (1)
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      return "GL_INVALID_FRAMEBUFFER_OPERATION";

    // gles 2, 3 and gl 4 error are handled by the switch above
    default:
      return "Unknown Error";
  }
}

void LinceCheckErrors(){
    //return;
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        LINCE_ASSERT(LinceFalse, "GLerror %d: %s", err, GLGetErrorString(err));
    }
}


// Iterates over layers, calling the provided callback.
// NOTE: requires __VA_ARGS__ to consume preceding comma!!
//  This is supported by (old) MSVC and GCC as an extension.
#define LINCE_MAP_OVER_LAYERS(STACK, CURRENT, CALLBACK, ...)    \
    for(uint32_t i = 0; i != (STACK).size; ++i){                \
        CURRENT = i;                                            \
        LinceLayer* layer = array_get(&(STACK), i);             \
        if (layer && layer->CALLBACK){                          \
            layer->CALLBACK(layer, ##__VA_ARGS__);              \
        }                                                       \
    }                                                           \
    CURRENT = -1;                                               \


void LinceRun(){

    LinceInit();
    
    LINCE_INFO("Running main loop...");
    while(app.running){
        LinceOnUpdate();
    }
    LinceTerminate();
}


LinceApp* LinceGetApp(){
    return &app;
}

void LincePushLayer(LinceLayer* layer) {
    if(layer->on_attach) layer->on_attach(layer);
    array_push_back(&app.layer_stack, layer);
}

void LincePushOverlay(LinceLayer* overlay) {
    if(overlay->on_attach) overlay->on_attach(overlay);
    array_push_back(&app.overlay_stack, overlay);
}

void LincePopLayer(LinceLayer* layer) {
    if(layer->on_detach) layer->on_detach(layer);
    for(uint32_t i = 0; i != app.layer_stack.size; ++i){
        if(layer == (LinceLayer*)array_get(&app.layer_stack, i)){
            array_remove(&app.layer_stack, i);
            return;
        }
    }
    LINCE_ASSERT(0, "Failed to find layer (0x%p) in stack", layer);
}

void LincePopOverlay(LinceLayer* overlay) {
    if(overlay->on_detach) overlay->on_detach(overlay);
    for(uint32_t i = 0; i != app.overlay_stack.size; ++i){
        if(overlay == (LinceLayer*)array_get(&app.overlay_stack, i)){
            array_remove(&app.overlay_stack, i);
            return;
        }
    }
    LINCE_ASSERT(0, "Failed to find overlay (0x%p) in stack", overlay);
}

void LincePushScene(LinceScene* scene){
    LINCE_ASSERT(scene, "Pushing NULL scene");
    array_push_back(&app.scene_stack, scene);
    app.current_scene = array_back(&app.scene_stack);
    LinceInitScene(app.current_scene);
}

void LincePopScene(){
    LINCE_ASSERT(app.scene_stack.size > 0, "No scenes in stack");
    LINCE_ASSERT(app.current_scene, "No scenes in stack");
    LinceUninitScene(app.current_scene);
    array_pop_back(&app.scene_stack);
    app.current_scene = array_back(&app.scene_stack);
}


double LinceGetTimeMillis(){
    return (glfwGetTime() * 1000.0);
}

float LinceGetAspectRatio(){
    return (float)app.window->width / (float)app.window->height;
}

void LinceGetScreenSize(vec2 size){
    size[0] = (float)LinceGetApp()->window->width;
    size[1] = (float)LinceGetApp()->window->height;
}

void LinceTransformToWorld(vec2 screen_coords, vec2 screen_size, mat4 vp_inv){
	float sx = screen_coords[0], sy = screen_coords[1];
	const float w = screen_size[0];
	const float h = screen_size[1];

	// normalise screen coordinates to range (-1,1)
	sx = 2.0f*sx/w - 1.0f;
	sy = 1.0f - 2.0f*sy/h;
	vec4 svec = {sx, sy, 0.0f, 1.0f};
	vec4 wvec;
    glm_mat4_mulv(vp_inv, svec, wvec);
	screen_coords[0] = wvec[0] / wvec[3];
	screen_coords[1] = wvec[1] / wvec[3];
}

void LinceGetMousePosWorld(vec2 pos, LinceCamera* cam){
    vec2 screen_size;
    LinceGetMousePos(&pos[0], &pos[1]);
    LinceGetScreenSize(screen_size);
    LinceTransformToWorld(pos, screen_size, cam->view_proj_inv);
}

LinceLayer* LinceGetCurrentLayer(){
    if (app.current_layer < 0) return NULL;
    return array_get(&app.layer_stack, app.current_layer);
}

LinceLayer* LinceGetCurrentOverlay(){
    if (app.current_overlay < 0) return NULL;
    return array_get(&app.overlay_stack, app.current_overlay);
}

void LincePushAssetDir(const char* dir){
    uint32_t length = strlen(dir);
    uint32_t exedir_length = strlen(app.exe_dir);
    
    // LINCE_ASSERT(app.running, "App is not running");
    LINCE_ASSERT(length < LINCE_PATH_MAX - exedir_length - 1,
        "Asset directory is too long. Length %u but max is %u",
        length, LINCE_PATH_MAX-exedir_length-1);
    
    array_push_front(&app.asset_dirs, NULL);
    char* p = array_front(&app.asset_dirs);

    // Prepend directory of executable
    memmove(p, app.exe_dir, exedir_length);
    if (p[exedir_length-1] != '\\' && p[exedir_length-1] != '/'){
        p[exedir_length] = '/';
        exedir_length++;
    }
    p += exedir_length;

    // Append relative directory to assets folder
    memmove(p, dir, length);
    if (p[length-1] != '\\' && p[length-1] != '/'){
        p[length] = '/';
        length++;
    }
    p[length] = '\0';

    if(LinceIsDir(array_front(&app.asset_dirs)) != 1){
        LINCE_WARN("Failed to add assets folder because it does not exist: '%s'",
            (char*)array_front(&app.asset_dirs));
        array_pop_front(&app.asset_dirs);
        return;
    }

    LINCE_INFO("Added assets folder '%s'", (char*)array_front(&app.asset_dirs));
}


LinceBool LinceFetchAssetPath(char* asset_path, const char* asset_filename){
    for(uint32_t i = 0; i != app.asset_dirs.size; ++i){
        char* dir = array_get(&app.asset_dirs, i);
        uint32_t dir_len = strlen(dir);
        
        if (dir_len + strlen(asset_filename) >= LINCE_PATH_MAX){
            LINCE_WARN("Skipping path, too long: '%s' + '%s'", dir, asset_filename);
            continue;
        }
        
        memmove(asset_path, dir, dir_len);
        memmove(asset_path + dir_len, asset_filename, strlen(asset_filename)+1);
        if (LinceIsFile(asset_path)){
            LINCE_INFO("Located asset '%s' at '%s'", asset_filename, asset_path);
            return LinceTrue;
        }
    }

    return LinceFalse;
}


/* --- Implementations of static functions --- */

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
    if (app.title == NULL) app.title = "Lince Window";
    app.title = LinceNewCopy(app.title, strlen(app.title)+1);

    // Create a windowed mode window and its OpenGL context
    app.window = LinceCreateWindow(app.screen_width, app.screen_height, app.title);
    LinceSetMainEventCallback(app.window, LinceOnEvent);

    // Create layer stacks
    array_init(&app.layer_stack, sizeof(LinceLayer));
    array_init(&app.overlay_stack, sizeof(LinceLayer));
    
    // Create scene stack
    array_init(&app.scene_stack, sizeof(LinceScene));

    app.exe_dir = LinceCalloc(LINCE_PATH_MAX * sizeof(char));
    LinceFetchExeDir(app.exe_dir, LINCE_PATH_MAX);
    array_init(&app.asset_dirs, LINCE_PATH_MAX * sizeof(char));
    LincePushAssetDir("../../../lince/assets");

    LinceInitRenderer(app.window);

    // TODO: improve
    // Load default font
    const char* font_fname = "fonts/DroidSans.ttf";
    char font_path[LINCE_PATH_MAX];
    LinceBool found = LinceFetchAssetPath(font_path, font_fname);
    LINCE_ASSERT(found, "Could not find location of default font '%s'", font_fname);
    app.ui = LinceInitUI(app.window->handle, font_path);
    
    app.running = LinceTrue;
    if (app.on_init) app.on_init(); // user may push layers onto stack
}


static void LinceOnUpdate(){
    LINCE_PROFILER_START(timer);
    LinceClear();

    // Calculate delta time
    float new_time_ms = (float)(glfwGetTime() * 1000.0);
    app.dt = new_time_ms - app.time_ms;
    app.time_ms = new_time_ms;
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

static void LinceTerminate(){

    LinceTerminateRenderer();
    
    // Destroy layer stacks
    LINCE_MAP_OVER_LAYERS(app.layer_stack, app.current_layer, on_detach);
    LINCE_MAP_OVER_LAYERS(app.overlay_stack, app.current_overlay, on_detach);
    array_uninit(&app.layer_stack);
    array_uninit(&app.overlay_stack);
    
    // Destroy scene stack
    while(app.scene_stack.size > 0){
        LincePopScene();
    }
    array_uninit(&app.scene_stack);
    
    if (app.on_terminate) app.on_terminate();

    LinceTerminateUI(app.ui);

    /* shutdown window last, as it destroys opengl context
    and all its functions */
    LinceDestroyWindow(app.window);
    app.window = NULL;
    app.running = 0;
    LinceFree(app.title);
    LinceFree(app.exe_dir);
    array_uninit(&app.asset_dirs);

    LinceCloseProfiler();
    LinceCloseLogger();
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

    LinceUIOnEvent(app.ui, e);

    /* Propagate event to layers and overlays,
    the ones in front (rendered last) receive it first */
    LINCE_MAP_OVER_LAYERS(app.layer_stack, app.current_layer, on_event, e);
    LINCE_MAP_OVER_LAYERS(app.overlay_stack, app.current_overlay, on_event, e);

    // Propagate event to user
    if (app.on_event && !e->handled ) app.on_event(e);
}


static LinceBool LinceOnEventWindowResize(LinceEvent* e){
    LINCE_INFO(" Window resized to %d x %d", 
        (int)e->data.window_resize->width,
        (int)e->data.window_resize->height
    );
    return LinceFalse; // allow other layers to receive event
}

static LinceBool LinceOnEventWindowClose(LinceEvent* e) {
    app.running = LinceFalse;
    return LinceFalse; // allow other layers to receive event
    LINCE_UNUSED(e);
}



