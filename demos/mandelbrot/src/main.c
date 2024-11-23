/*
    =============================
    === Mandelbrot Set Viewer ===
    =============================

    Author: Jorge Fernández
    Date: 09/03/2024

    Move up/down/left/right with WASD.
    Zoom in with Q and zoom out with E.

*/


#include <lince.h>

struct AppState {
    LinceCamera cam;
    LinceSprite canvas;
    LinceShader* canvas_shader;
    LinceTransform viewframe;
    LinceTransform transform;
} state;

void LinceTransformGetBounds(LinceTransform* t, LincePoint* xbound, LincePoint* ybound){
    if(xbound){
        xbound->x = t->x - t->w/2.0f;
        xbound->y = t->x + t->w/2.0f;
    }
    if(ybound){
        ybound->x = t->y - t->h/2.0f;
        ybound->y = t->y + t->h/2.0f;
    }
}

void OnInit(){
    LinceInitCamera(&state.cam, LinceAppGetAspectRatio());
    state.canvas = (LinceSprite){ .color = {1,1,1} };
    state.viewframe = (LinceTransform){
        .x = 0.0f, .y = 0.0f, .h = 2.0f,
        .w = 2.0f*LinceAppGetAspectRatio(),
    };
    state.transform = state.viewframe;
    
    LinceAppPushAssetFolder(string_scoped_lit("../../../demos/mandelbrot/assets"));
    char vert_path[LINCE_PATH_MAX];
    char frag_path[LINCE_PATH_MAX];
    strcpy(vert_path, LinceAssetCacheFetchPath(LinceAppGetAssetCache(), "shaders/mandelbrot.vert.glsl"));
    strcpy(frag_path, LinceAssetCacheFetchPath(LinceAppGetAssetCache(), "shaders/mandelbrot.frag.glsl"));

    state.canvas_shader = LinceCreateShader(vert_path, frag_path);
    LincePoint screen = LinceAppGetScreenSize();

    LinceBindShader(state.canvas_shader);
    LinceSetShaderUniformVec2(state.canvas_shader, string_scoped_lit("uScreen"), (float[]){screen.x,screen.y});
    
    LincePoint xlim, ylim;
    LinceTransformGetBounds(&state.viewframe, &xlim, &ylim);
    LinceSetShaderUniformVec2(state.canvas_shader, string_scoped_lit("uXlim"), &xlim.x);
    LinceSetShaderUniformVec2(state.canvas_shader, string_scoped_lit("uYlim"), &ylim.x);
}

void OnUpdate(float dt){
    LinceUpdateCamera(&state.cam);

    static float speed = 0.0025f;
    const float zoom = 1.0f + 0.01f;

    // Move up/down
    if(LinceIsKeyPressed(LinceKey_w)){
        state.viewframe.y += speed * dt;
    } else if(LinceIsKeyPressed(LinceKey_s)){
        state.viewframe.y -= speed * dt;
    }
    
    // Move left/right
    if(LinceIsKeyPressed(LinceKey_d)){
        state.viewframe.x += speed * dt;
    } else if (LinceIsKeyPressed(LinceKey_a)){
        state.viewframe.x -= speed * dt;
    }

    // Zoom in/out
    if(LinceIsKeyPressed(LinceKey_q)){
        speed /= zoom;
        state.viewframe.w /= zoom;
        state.viewframe.h /= zoom;

    } else if(LinceIsKeyPressed(LinceKey_e)){
        speed *= zoom;
        state.viewframe.w *= zoom;
        state.viewframe.h *= zoom;
    }

    LincePoint xlim, ylim;
    LinceTransformGetBounds(&state.viewframe, &xlim, &ylim);
    LinceBindShader(state.canvas_shader);
    LinceSetShaderUniformVec2(state.canvas_shader, string_scoped_lit("uXlim"), &xlim.x);
    LinceSetShaderUniformVec2(state.canvas_shader, string_scoped_lit("uYlim"), &ylim.x);

    LinceBeginRender(&state.cam);
    LinceDrawSprite(&state.canvas, &state.transform, state.canvas_shader);
    LinceEndRender();
}

void OnEvent(LinceEvent* event){
    if(event->type == LinceEventType_WindowResize){
        LinceUpdateCameraProjection(&state.cam, LinceAppGetAspectRatio());
        LincePoint screen = LinceAppGetScreenSize();
        LinceBindShader(state.canvas_shader);
        LinceSetShaderUniformVec2(state.canvas_shader, string_scoped_lit("uScreen"), &screen.x);
    }
}

void OnTerminate(){
    LinceDeleteShader(state.canvas_shader);
}

int main(){
    LinceApp* app = LinceGetApp();
    app->on_init = OnInit;
    app->on_update = OnUpdate;
    app->on_event = OnEvent;
    app->on_terminate = OnTerminate;
    app->screen_width = 1920;
    app->screen_height = 1080;

    LinceRun();
    
    return 0;
}