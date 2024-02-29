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
    LinceBeginScene(&camera);
    LinceDrawSprite(&square, NULL);
    LinceEndScene();
}

int main() {

    LinceApp* app = LinceGetApp();
    app->screen_width = 800;
    app->screen_height = 500;
    LinceAppSetTitle("My Window");

    app->on_init = OnInit;
    app->on_update = OnUpdate;

    LinceRun();

    return 0;
}
