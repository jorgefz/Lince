#include <lince.h>

LinceCamera camera;
LinceSprite square = {
    .x = 0, .y = 0,       // position at the centre of the screen
    .w = 1, .h = 1,       // size in "world coordinates"
    .color = {1, 0, 0, 1} // rgba, red and fully opaque
};

void OnInit() {
    LinceInitCamera(&camera, 800.0 / 500.0);
}

void OnUpdate(float dt) {
    
    const float speed = 0.005f; // units per millisec

    if(LinceIsKeyPressed(LinceKey_w)) camera.pos[1] += speed * dt; // move up
    if(LinceIsKeyPressed(LinceKey_s)) camera.pos[1] -= speed * dt; // move down
    if(LinceIsKeyPressed(LinceKey_d)) camera.pos[0] += speed * dt; // move right
    if(LinceIsKeyPressed(LinceKey_a)) camera.pos[0] -= speed * dt; // move left

    // LinceUpdateCameraProjection(&camera, LinceAppGetAspectRatio());
    LinceUpdateCamera(&camera);
    
    LinceBeginRender(&camera);
    LinceDrawSprite(&square, NULL);
    LinceEndRender();
}

void OnEvent(LinceEvent* event){
    if(event->type == LinceEventType_WindowResize){
        LinceUpdateCameraProjection(&camera, LinceAppGetAspectRatio());
    }
}

int main() {

    LinceApp* app = LinceGetApp();
    app->screen_width = 800;
    app->screen_height = 500;
    LinceAppSetTitle("My Window");

    app->on_init = OnInit;
    app->on_update = OnUpdate;
    app->on_event = OnEvent;

    LinceRun();

    return 0;
}
