
#include <time.h>

#include "lince.h"
#include "cglm/types.h"
#include "cglm/vec4.h"
#include "cglm/affine.h"

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

    
    LinceInitRenderer(LinceGetAppState()->window);
    data->tex_front = LinceCreateTexture("Patrick", "lince/assets/front.png");
    data->tex_back  = LinceCreateTexture("Patrick", "lince/assets/back.png");

    data->red = 0.0f;
    data->vel = 5e-4f;
    data->cam_speed = 0.01f;
    data->color_step = 0.003f;
    
}

void MyLayerOnDetach(LinceLayer* layer) {
    MyLayer* data = LinceGetLayerData(layer);
    LINCE_INFO(" Layer '%s' detached", data->name);

    LinceTerminateRenderer();
    LinceDeleteTexture(data->tex_front);
    LinceDeleteTexture(data->tex_back);

    free(data);
}


void MyLayerOnEvent(LinceLayer* layer, LinceEvent* e) {
    
}

/*
Q, A: increase, decrease red
W, S: increase, decrease green
E, D: increase, decrease blue
*/
void MyLayerOnUpdate(LinceLayer* layer, float dt) {
    MyLayer* data = LinceGetLayerData(layer);
    data->dt = dt;
    
    /*
    // User Input
    const float color_step = data->color_step;
    const float cam_speed  = data->cam_speed;
    const float zoom       = data->cam->zoom;
    const float dr = cam_speed * dt * zoom;
    const float dc = color_step * dt;

    // change colors
    if (LinceIsKeyPressed(LinceKey_q)) data->color[0] += dc;
    if (LinceIsKeyPressed(LinceKey_a)) data->color[0] -= dc;
    if (LinceIsKeyPressed(LinceKey_w)) data->color[1] += dc;
    if (LinceIsKeyPressed(LinceKey_s)) data->color[1] -= dc;
    if (LinceIsKeyPressed(LinceKey_e)) data->color[2] += dc;
    if (LinceIsKeyPressed(LinceKey_d)) data->color[2] -= dc;
    // camera movement
    if (LinceIsKeyPressed(LinceKey_Up))    data->cam->pos[1] += dr;
    if (LinceIsKeyPressed(LinceKey_Down))  data->cam->pos[1] -= dr;
    if (LinceIsKeyPressed(LinceKey_Right)) data->cam->pos[0] += dr;
    if (LinceIsKeyPressed(LinceKey_Left))  data->cam->pos[0] -= dr;
    // camera zoom
    if (LinceIsKeyPressed(LinceKey_Period)) data->cam->zoom *= 0.99;
    if (LinceIsKeyPressed(LinceKey_Comma))  data->cam->zoom /= 0.99;
    */
    
    LinceBeginScene();
    LinceDrawQuad( (LinceQuadProps){
        .x=0.5, .y=0.5, .w=0.5, .h=0.5,
        .color={1.0,1.0,1.0,-0.5},
        .texture=data->tex_front
    });
    LinceDrawQuad( (LinceQuadProps){
        .x=0.4, .y=0.4, .w=0.5, .h=0.5,
        .color={1.0,1.0,1.0,-0.5},
        .texture=data->tex_back
    });
    LinceEndScene();

    /* Update background color */
    data->red += data->vel * dt;
    if (data->red >= 1.0f) data->vel = -data->vel;
    else if (data->red <= 0.0f) data->vel = -data->vel;
    float blue = 1.0f - data->red;
    LinceSetClearColor(data->red, 0.1f, blue, 1.0f);
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
