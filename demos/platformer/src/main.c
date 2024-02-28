#include <lince.h>

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

	LinceRun();

	return 0;
}