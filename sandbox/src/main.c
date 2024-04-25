#include <stdio.h>
#include <time.h>
#include <lince.h>

#include "gamedata.h"
#include "scenes/scenes.h"

static GameData DATA = {0};

static const char* asset_dir = "../../../sandbox/assets";

void SandboxInit() {

    LinceApp* app = LinceGetApp();
    LinceAppPushAssetFolder(asset_dir);
    LinceInitCamera(&DATA.camera, LinceAppGetAspectRatio());
    DATA.camera.zoom = 3.0f;
    DATA.camera_speed = 0.003f;

    DATA.player_box = (LinceBox2D){.x=0, .y=0, .w=0.7f, .h=0.7f};
    DATA.player_sprite = (LinceSprite){ .color={0,0,1}, .zorder=1};
    DATA.player_transform = (LinceTransform){.x=0, .y=0, .w=0.7f, .h=0.7f};

    app->user_data = &DATA;

    LinceAppRegisterScene("MainMenu", &SCENE_CALLBACKS[Scene_MainMenu]);
    LinceAppRegisterScene("World", &SCENE_CALLBACKS[Scene_World]);
    LinceAppRegisterScene("House", &SCENE_CALLBACKS[Scene_House]);

    LinceAppLoadScene("MainMenu");
}

void SandboxUpdate(float dt){
    LINCE_UNUSED(dt);
    LinceUpdateCameraProjection(&DATA.camera, LinceAppGetAspectRatio());
    LinceUpdateCamera(&DATA.camera);
}

void SandboxTerminate(){

}

void SetupApplication(){
    LinceApp* app = LinceGetApp();
    app->screen_width = 1280;
    app->screen_height = 720;
    app->on_init      = SandboxInit;
    app->on_update    = SandboxUpdate;
    app->on_terminate = SandboxTerminate;
    app->show_debug_panel = LinceTrue;
    LinceAppSetTitle("Sandbox");
}


int main(void) {

    SetupApplication();
    LinceRun();

    return 0;
}

