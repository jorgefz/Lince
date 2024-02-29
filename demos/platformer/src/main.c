#include <lince.h>

typedef struct GameState {
    LinceCamera camera;
} GameState;

LinceCamera camera;
LinceSprite square = {
    .x = 0, .y = 0,     // position
    .w = 1, .h = 1,     // size
    .color = {1, 0, 0, 1} // rgba
};

void OnInit() {
	LinceInitCamera(&camera, LinceGetAspectRatio());
    // LincePushAssetDir(&LinceGetApp()->asset_manager, "../../../lince/lince/assets");
}

void OnUpdate(float dt) {
    LinceResizeCameraView(&camera, LinceGetAspectRatio());
    LinceUpdateCamera(&camera);

    if(LinceIsKeyPressed(LinceKey_Right)) square.x += 0.02f;
    if(LinceIsKeyPressed(LinceKey_Left))  square.x -= 0.02f;
    if(LinceIsKeyPressed(LinceKey_Up))    square.y += 0.02f;
    if(LinceIsKeyPressed(LinceKey_Down))  square.y -= 0.02f;

    LinceBeginScene(&camera);
    LinceDrawSprite(&square, NULL);
    LinceEndScene();
}

void OnTerminate() {
	
}


int main() {

    LinceApp* app = LinceGetApp();
    app->on_init = OnInit;
    app->on_update = OnUpdate;
    app->on_terminate = OnTerminate;
    LinceAppSetTitle("Platformer Demo");
    
	LinceRun();

	return 0;
}