
#include <time.h>

#include "lince.h"
#include "cglm/types.h"
#include "cglm/vec4.h"
#include "cglm/affine.h"

#include "gui/nuklear_flags.h"
#include "gui/nuklear.h"

#include "pong.h"
#include "missile_command.h"

void NKLayerOnUpdate(LinceLayer* layer, float dt){
    
    LinceUILayer* ui = LinceGetAppState()->ui;
    struct nk_context *ctx = ui->ctx;
    static struct nk_colorf bg = {0.1, 0.18, 0.24, 1.0};
    uint32_t window_width = LinceGetAppState()->window->width;
    uint32_t window_height = LinceGetAppState()->window->height;

    LinceUIText(ui, "Text1", 10, 10, LinceFont_Droid30, 10, "FPS %.0f", 1000.0/dt);
    LinceUIText(ui, "Text2", 10, 40, LinceFont_Droid30, 10, "%.2f ms", dt);
    LinceUIText(ui, "Title", (float)window_width/2-200, 50, LinceFont_Droid30, 30, "The Legend of Cheesus Christ");

    nk_style_set_font(ui->ctx, &ui->fonts[LinceFont_Droid15]->handle);
    if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
        NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE
    )) {
        enum {EASY, HARD};
        static int op = EASY;
        static int property = 20;
        nk_layout_row_static(ctx, 30, 80, 1);

        if (nk_button_label(ctx, "button")) { }

        static char buffer[1000] = {0};
        sprintf(buffer, "dt = %.2f ms", dt);
        nk_label(ctx, buffer, NK_TEXT_LEFT);
        sprintf(buffer, "fps = %.2f", 1000.0/dt);
        nk_label(ctx, buffer, NK_TEXT_LEFT);

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
    nk_end(ctx);

    LinceSetClearColor(bg.r, bg.g, bg.b, bg.a);
    
    LINCE_UNUSED(window_height);
    LINCE_UNUSED(layer);
}

LinceLayer* NKLayerInit(){
    LinceLayer* layer = LinceCreateLayer(NULL);
    layer->OnUpdate = NKLayerOnUpdate; 
    return layer;
}

#include "renderer/tileset.h"

enum TileNames {
    TILE_GRASS = 0,
    TILE_DIRT,
    TILE_TREE,
    TILE_CHICKEN,
    TILE_COUNT
};

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
    LinceTexture* tileset;

    LinceTile tiles[TILE_COUNT];

    vec4 color;
    LinceCamera* cam;
} MyLayer;

void MyLayerOnAttach(LinceLayer* layer) {
    MyLayer* data = LinceGetLayerData(layer);
    LINCE_INFO(" Layer '%s' attached", data->name);

    data->cam = LinceCreateCamera(LinceGetAspectRatio());
    
    data->tex_front = LinceCreateTexture("PatrickF", "lince/assets/front.png");
    data->tex_back  = LinceCreateTexture("PatrickB", "lince/assets/back.png");
    data->tileset = LinceCreateTexture("Tileset", "game/assets/textures/shubibubi-cozy-farm.png");

    data->tiles[TILE_GRASS]   = LinceGetTile(data->tileset, (vec2){1,8}, (vec2){16,16}, (vec2){1, 1});
    data->tiles[TILE_DIRT]    = LinceGetTile(data->tileset, (vec2){5,9}, (vec2){16,16}, (vec2){1, 1});
    data->tiles[TILE_TREE]    = LinceGetTile(data->tileset, (vec2){9,5}, (vec2){16,16}, (vec2){2, 2});
    data->tiles[TILE_CHICKEN] = LinceGetTile(data->tileset, (vec2){0,1}, (vec2){16,16}, (vec2){1, 1});

    data->red = 0.0f;
    data->vel = 5e-4f;
    data->cam_speed = 0.005f;
    data->color_step = 0.003f;
}

void MyLayerOnDetach(LinceLayer* layer) {
    MyLayer* data = LinceGetLayerData(layer);
    LINCE_INFO(" Layer '%s' detached", data->name);

    LinceDeleteTexture(data->tex_front);
    LinceDeleteTexture(data->tex_back);
    LinceDeleteCamera(data->cam);

    free(data);
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
    if (LinceIsKeyPressed(LinceKey_p)){
        int n = printf("dt: %7.3f ms, fps: %7.1f", dt, 1000.0/dt);
        fflush(stdout);
        for(int i=0; i!=n; ++i) printf("\b \b");
    }
    

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

    for(int i = 0; i != 10; ++i){
        for(int j = 0; j != 10; ++j){
            LinceDrawQuad((LinceQuadProps){
                .x=(float)i, .y=(float)j,
                .w=1.0f, .h=1.0f,
                .color={1,1,1,1},
                .tile = &data->tiles[TILE_GRASS],
                .zorder = 0.1
            });
        }
    }

    LinceDrawQuad((LinceQuadProps){
        .x=5.0f, .y=5.0f,
        .w=2.0f, .h=2.0f,
        .color={1,1,1,1},
        .tile = &data->tiles[TILE_TREE],
        .zorder = 0.5
    });

    LinceDrawQuad((LinceQuadProps){
        .x=5.0f, .y=3.0f,
        .w=1.0f, .h=1.0f,
        .color={1,1,1,1},
        .tile = &data->tiles[TILE_CHICKEN],
        .zorder = 0.5
    });

    
    LinceEndScene();
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
    //layer->OnEvent  = MyLayerOnEvent;
    layer->OnUpdate = MyLayerOnUpdate;

    return layer;
}


// =============================================================


void GameInit() {
	LINCE_INFO("\n User App Initialised");
    LincePushLayer(MyLayerInit("Test"));
    //LincePushLayer(NKLayerInit());
    //LincePushLayer(PongLayerInit());
    //LincePushLayer(MCommandLayerInit());
}

void GameOnUpdate(float dt) {
    LinceCheckErrors();
    LINCE_UNUSED(dt);
}

void GameTerminate() {
    LINCE_INFO(" User App Terminated");
}

int main(int argc, const char* argv[]) {

    #ifdef LINCE_DEBUG
    LINCE_INFO(" --- DEBUG MODE --- ");
    #endif

    LinceApp* app = LinceGetAppState();

    // app->user_data = NULL;
    app->screen_width = 900;
    app->screen_height = 600;
    app->title = "The Legend of Cheesus Christ";
    // app->options = LINCE_FULLSCREEN | LINCE_VSYNC | LINCE_RESIZEABLE | ...

    app->game_init = GameInit;
    app->game_on_update = GameOnUpdate;
    // app->game_on_event = GameOnEvent;
    app->game_terminate = GameTerminate;
    
    LinceRun();

    LINCE_UNUSED(argc);
    LINCE_UNUSED(argv);
    return 0;
}
