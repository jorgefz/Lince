#include <lince.h>

LinceCamera camera;
LinceSprite sprite = { .color = {1,0,0} }; // Red colour (RGB)
LinceTransform transform = {
    .x = 0, .y = 0, // Position at the centre of the screen
    .w = 1, .h = 1  // Square of one unit in size (in world coordinates)
};

void OnInit() {
    LinceInitCamera(&camera, 800.0f / 500.0f);
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
    LinceDrawSprite(&sprite, &transform, NULL);
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
    LinceAppSetTitle("My Window", sizeof("My Window")-1);

    app->on_init = OnInit;
    app->on_update = OnUpdate;
    app->on_event = OnEvent;

    LinceRun();

    return 0;
}
