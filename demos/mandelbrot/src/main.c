#include <lince.h>

/*

TODO:
    - Use LinceTransform instead of xlim/ylim
    - Add LinceTransform functions to calculate bounds of rectangle
    - Add colormap to shader
*/

struct AppState {
    LinceCamera cam;
    LinceSprite canvas;
    LinceShader* canvas_shader;
    float xlim[2];
    float ylim[2];
} state;

void OnInit(){
    LinceInitCamera(&state.cam, LinceAppGetAspectRatio());
    state.canvas = (LinceSprite){
        .x = 0.0f, .y = 0.0f,
        .w = 2.0f*LinceAppGetAspectRatio(), .h = 2.0f,
        .color = {1,1,1,1}
    };
    state.xlim[0] = -2.0f*LinceAppGetAspectRatio();
    state.xlim[1] = 2.0f*LinceAppGetAspectRatio();
    state.ylim[0] = -2.0f;
    state.ylim[1] = 2.0f;
    
    LinceAppPushAssetDir("../../../demos/mandelbrot/assets");
    char vert_path[LINCE_PATH_MAX];
    char frag_path[LINCE_PATH_MAX];
    strcpy(vert_path, LinceAppFetchAssetPath("shaders/mandelbrot.vert.glsl"));
    strcpy(frag_path, LinceAppFetchAssetPath("shaders/mandelbrot.frag.glsl"));

    state.canvas_shader = LinceCreateShader(vert_path, frag_path);
    LincePoint screen = LinceAppGetScreenSize();

    LinceBindShader(state.canvas_shader);
    LinceSetShaderUniformVec2(state.canvas_shader, "uScreen", (float[]){screen.x,screen.y});
    LinceSetShaderUniformVec2(state.canvas_shader, "uXlim", state.xlim);
    LinceSetShaderUniformVec2(state.canvas_shader, "uYlim", state.ylim);
}

void OnUpdate(float dt){
    LinceUpdateCamera(&state.cam);

    LinceBindShader(state.canvas_shader);
    static float speed = 0.005f;
    const float zoom = 1.0f + 0.01f;

    // Move up/down
    if(LinceIsKeyPressed(LinceKey_w)){
        state.ylim[0] += speed * dt;
        state.ylim[1] += speed * dt;
        LinceSetShaderUniformVec2(state.canvas_shader, "uYlim", state.ylim);
    } else if(LinceIsKeyPressed(LinceKey_s)){
        state.ylim[0] -= speed * dt;
        state.ylim[1] -= speed * dt;
        LinceSetShaderUniformVec2(state.canvas_shader, "uYlim", state.ylim);
    }
    
    // Move left/right
    if(LinceIsKeyPressed(LinceKey_d)){
        state.xlim[0] += speed * dt;
        state.xlim[1] += speed * dt;
        LinceSetShaderUniformVec2(state.canvas_shader, "uXlim", state.xlim);
    } else if (LinceIsKeyPressed(LinceKey_a)){
        state.xlim[0] -= speed * dt;
        state.xlim[1] -= speed * dt;
        LinceSetShaderUniformVec2(state.canvas_shader, "uXlim", state.xlim);
    }

    // Zoom in/out
    if(LinceIsKeyPressed(LinceKey_q)){
        speed /= zoom;
        float wx = fabsf(state.xlim[1] - state.xlim[0]);
        float cx = (state.xlim[1] + state.xlim[0])/2.0f;
        wx /= zoom;
        state.xlim[0] = cx - wx/2.0f;
        state.xlim[1] = cx + wx/2.0f;
        LinceSetShaderUniformVec2(state.canvas_shader, "uXlim", state.xlim);

        float wy = fabsf(state.ylim[1] - state.ylim[0]);
        float cy = (state.ylim[1] + state.ylim[0])/2.0f;
        wy /= zoom;
        state.ylim[0] = cy - wy/2.0f;
        state.ylim[1] = cy + wy/2.0f;
        LinceSetShaderUniformVec2(state.canvas_shader, "uYlim", state.ylim);
    } else if(LinceIsKeyPressed(LinceKey_e)){
        speed *= zoom;
        float wx = fabsf(state.xlim[1] - state.xlim[0]);
        float cx = (state.xlim[1] + state.xlim[0])/2.0f;
        wx *= zoom;
        state.xlim[0] = cx - wx/2.0f;
        state.xlim[1] = cx + wx/2.0f;
        LinceSetShaderUniformVec2(state.canvas_shader, "uXlim", state.xlim);

        float wy = fabsf(state.ylim[1] - state.ylim[0]);
        float cy = (state.ylim[1] + state.ylim[0])/2.0f;
        wy *= zoom;
        state.ylim[0] = cy - wy/2.0f;
        state.ylim[1] = cy + wy/2.0f;
        LinceSetShaderUniformVec2(state.canvas_shader, "uYlim", state.ylim);
    }

    LinceBeginRender(&state.cam);
    LinceDrawSprite(&state.canvas, state.canvas_shader);
    LinceEndRender();
}

void OnEvent(LinceEvent* event){
    if(event->type == LinceEventType_WindowResize){
        LinceUpdateCameraProjection(&state.cam, LinceAppGetAspectRatio());
        LincePoint screen = LinceAppGetScreenSize();
        LinceBindShader(state.canvas_shader);
        LinceSetShaderUniformVec2(state.canvas_shader, "uScreen", (float[]){screen.x,screen.y});
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