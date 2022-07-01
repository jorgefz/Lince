
#include <time.h>

#include "lince.h"
#include "cglm/types.h"
#include "cglm/vec4.h"
#include "cglm/affine.h"

#define NK_SHADER_VERSION "#version 450 core\n"

// TEMPORARY - exposes GLFW API
#include "gui/nuklear_wrapper.h"

#define nuklear_test

#ifdef nuklear_test

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

typedef struct NKLayer {
    struct nk_context *ctx;
    struct nk_glfw glfw;
} NKLayer;


void NKLayerOnAttach(LinceLayer* layer){
    NKLayer* data = LinceGetLayerData(layer);

    data->ctx = nk_glfw3_init(
        &data->glfw,
        LinceGetAppState()->window->handle,
        //NK_GLFW3_INSTALL_CALLBACKS
        0
    );

    // Initialise Font
    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&data->glfw, &atlas);
    struct nk_font *font = nk_font_atlas_add_from_file(atlas, "lince/assets/fonts/DroidSans.ttf", 13, 0);
    nk_glfw3_font_stash_end(&data->glfw);
    //nk_style_load_all_cursors(data->ctx, atlas->cursors);
    nk_style_set_font(data->ctx, &font->handle);

}

void NKLayerOnUpdate(LinceLayer* layer, float dt){
    NKLayer* data = LinceGetLayerData(layer);
    struct nk_context* ctx = data->ctx;
    struct nk_colorf bg = {0.1, 0.18, 0.24, 1.0};

    nk_glfw3_new_frame(&data->glfw);
    
    if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
        NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
    {
        enum {EASY, HARD};
        static int op = EASY;
        static int property = 20;
        nk_layout_row_static(ctx, 30, 80, 1);
        if (nk_button_label(ctx, "button"))
            fprintf(stdout, "button pressed\n");

        nk_layout_row_dynamic(ctx, 30, 2);
        if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
        if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;

        nk_layout_row_dynamic(ctx, 25, 1);
        nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "background:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(ctx, 25, 1);
        if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx),400))) {
            nk_layout_row_dynamic(ctx, 120, 1);
            bg = nk_color_picker(ctx, bg, NK_RGBA);
            nk_layout_row_dynamic(ctx, 25, 1);
            bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f,0.005f);
            bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f,0.005f);
            bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f,0.005f);
            bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f,0.005f);
            nk_combo_end(ctx);
        }
    }
    
    nk_end(data->ctx);

    LinceSetClearColor(bg.r, bg.g, bg.b, bg.a);
    nk_glfw3_render(&data->glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
    
}  

void NKLayerOnDetach(LinceLayer* layer){
    NKLayer* data = LinceGetLayerData(layer);
    nk_glfw3_shutdown(&data->glfw);
    free(layer->data);
}

LinceLayer* NKLayerInit(){
    NKLayer *data = calloc(1, sizeof(NKLayer));
    LINCE_ASSERT(data, "Failed to allocate %d bytes", (int)sizeof(NKLayer));

    LinceLayer* layer = LinceCreateLayer(data);
    layer->OnAttach = NKLayerOnAttach;
    layer->OnDetach = NKLayerOnDetach;
    //layer->OnEvent  = NKLayerOnEvent;
    layer->OnUpdate = NKLayerOnUpdate;

    return layer;
}

#endif


typedef struct MyLayer {
    char name[LINCE_NAME_MAX];
    float red, vel;
    float dt;
    float cam_speed, color_step;

    LinceVertexArray* va;
    LinceVertexBuffer vb;
    LinceIndexBuffer ib;
    LinceShader* shader;
    LinceTexture* tex_front;
    LinceTexture* tex_back;
    vec4 color;
    LinceCamera* cam;
} MyLayer;

void MyLayerOnAttach(LinceLayer* layer) {
    MyLayer* data = LinceGetLayerData(layer);
    LINCE_INFO(" Layer '%s' attached", data->name);

    data->cam = LinceCreateCamera(LinceGetAspectRatio());
    
    LinceInitRenderer(LinceGetAppState()->window);
    data->tex_front = LinceCreateTexture("Patrick", "lince/assets/front.png");
    data->tex_back  = LinceCreateTexture("Patrick", "lince/assets/back.png");

    data->red = 0.0f;
    data->vel = 5e-4f;
    data->cam_speed = 0.01f;
    data->color_step = 0.003f;
    
    LinceSetClearColor(0.0, 0.2, 0.5, 1.0);
}

void MyLayerOnDetach(LinceLayer* layer) {
    MyLayer* data = LinceGetLayerData(layer);
    LINCE_INFO(" Layer '%s' detached", data->name);

    LinceTerminateRenderer();
    LinceDeleteTexture(data->tex_front);
    LinceDeleteTexture(data->tex_back);
    LinceDeleteCamera(data->cam);

    free(data);
}


void MyLayerOnEvent(LinceLayer* layer, LinceEvent* e) {
    
}


void MyLayerOnUpdate(LinceLayer* layer, float dt) {
    MyLayer* data = LinceGetLayerData(layer);
    data->dt = dt;

    LinceResizeCameraView(data->cam, LinceGetAspectRatio());
	LinceUpdateCamera(data->cam);
    
    // User Input
    const float cam_speed  = data->cam_speed;
    const float zoom       = data->cam->zoom;
    const float dr = cam_speed * dt * zoom;

    // camera movement
    if (LinceIsKeyPressed(LinceKey_Up))    data->cam->pos[1] += dr;
    if (LinceIsKeyPressed(LinceKey_Down))  data->cam->pos[1] -= dr;
    if (LinceIsKeyPressed(LinceKey_Right)) data->cam->pos[0] += dr;
    if (LinceIsKeyPressed(LinceKey_Left))  data->cam->pos[0] -= dr;
    // camera zoom
    if (LinceIsKeyPressed(LinceKey_Period)) data->cam->zoom *= 0.99; // * 0.5 * dt;
    if (LinceIsKeyPressed(LinceKey_Comma))  data->cam->zoom /= 0.99; // * 0.5 * dt;
    // debug frame time
    if (LinceIsKeyPressed(LinceKey_p)) printf("dt: %7.3f ms, fps: %7.1f\n", dt, 1000.0/dt);
    
    LinceBeginScene(data->cam);

    int gridx = 10;
    int gridy = 10;

    float r, g, b;
    for(int i = 0; i != gridx; ++i){
        for(int j = 0; j != gridy; ++j){
            r = (float)i/(float)gridx;
            b = (float)j/(float)gridy;
            g = 0.8 - b - r;
            LinceDrawQuad( (LinceQuadProps){
                .x = (float)i - (float)gridx / 2.0,
                .y = (float)j - (float)gridy / 2.0,
                .w = 0.95, .h = 0.95,
                .color = {r, g, b, 1.0}
            });
        }
    }

    LinceDrawQuad( (LinceQuadProps){
        .x=-0.5, .y=0.0, .w=0.9, .h=0.9,
        .color={1,1,1,1},
        .rotation = 45,
        .texture=data->tex_front,
        .zorder = 0.1
    });
    LinceDrawQuad( (LinceQuadProps){
        .x=0.0, .y=0.0, .w=0.9, .h=0.9,
        .color={1,1,1,1},
        .rotation = 45,
        .texture=data->tex_back,
        .zorder = 0.9
    });
    LinceDrawQuad( (LinceQuadProps){
        .x=0.5, .y=0.0, .w=0.9, .h=0.9,
        .color={1,1,1,1},
        .rotation = 45,
        .texture=data->tex_front,
        .zorder = 0.1
    });
    
    LinceEndScene();

    // Update background color
    // data->red += data->vel * dt;
    // if (data->red >= 1.0f) data->vel = -data->vel;
    // else if (data->red <= 0.0f) data->vel = -data->vel;
    // float blue = 1.0f - data->red;
    // LinceSetClearColor(data->red, 0.1f, blue, 1.0f);
}

LinceLayer* MyLayerInit(char* name) {

    MyLayer* my_layer = calloc(1, sizeof(MyLayer));
    LINCE_ASSERT(my_layer, "Failed to allocate layer data");

    size_t len_orig = strlen(name);
    size_t len = len_orig < LINCE_NAME_MAX ? len_orig : LINCE_NAME_MAX;
    memcpy(my_layer->name, name, len);

    LinceLayer* layer = LinceCreateLayer(my_layer);
    layer->OnAttach = MyLayerOnAttach;
    layer->OnDetach = MyLayerOnDetach;
    layer->OnEvent  = MyLayerOnEvent;
    layer->OnUpdate = MyLayerOnUpdate;

    return layer;
}


// =============================================================


void GameInit() {
	LINCE_INFO("\n User App Initialised");
    LincePushLayer(MyLayerInit("Test"));
#ifdef nuklear_test
    LincePushLayer(NKLayerInit());
#endif
}

void GameOnUpdate(float dt) {
    LinceCheckErrors();
}

void GameOnEvent(LinceEvent* e) {

}

void GameTerminate() {
    LINCE_INFO(" User App Terminated");
}


int main(int argc, const char* argv[]) {

    #ifdef LINCE_DEBUG
    LINCE_INFO(" --- DEBUG MODE --- ");
    #endif

    LinceApp* app = LinceGetAppState();

    app->game_init = GameInit;
    app->game_on_update = GameOnUpdate;
    app->game_on_event = GameOnEvent;
    app->game_terminate = GameTerminate;
    
    LinceRun();

    return 0;
}
