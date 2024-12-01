#include <stdio.h>
#include <time.h>
#include <lince.h>

#include "gamedata.h"
#include "scenes/scenes.h"

#include <toml.h>


static GameData DATA = {0};

static const char asset_dir[] = "../../../sandbox/assets";

void SandboxInit() {

    LinceApp* app = LinceGetApp();
    LinceAppPushAssetFolder(string_scoped_lit(asset_dir));
    LinceInitCamera(&DATA.camera, LinceAppGetAspectRatio());
    DATA.camera.zoom = 3.0f;
    DATA.camera_speed = 0.003f;

    DATA.player_box = (LinceBox2D){.x=0, .y=0, .w=0.7f, .h=0.7f};
    DATA.player_sprite = (LinceSprite){ .color={0,0,1}, .zorder=1};
    DATA.player_transform = (LinceTransform){.x=0, .y=0, .w=0.7f, .h=0.7f};

    app->user_data = &DATA;

    LinceAppRegisterScene(string_scoped_lit("MainMenu"), &SCENE_CALLBACKS[Scene_MainMenu]);
    LinceAppRegisterScene(string_scoped_lit("World"), &SCENE_CALLBACKS[Scene_World]);
    LinceAppRegisterScene(string_scoped_lit("House"), &SCENE_CALLBACKS[Scene_House]);

    LinceAppLoadScene(string_scoped_lit("MainMenu"));
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
    LinceAppSetTitle("Sandbox", sizeof("Sandbox")-1);
}


void TestReadToml(){
    char errbuf[200];

    string_t content = LinceReadFile(string_scoped_lit("sandbox/assets/config/config.toml"));
    if(!content.str){
        printf("Could not read toml file\n");
        return;
    }

    toml_table_t* tab = toml_parse(content.str, errbuf, sizeof(errbuf));
    if(!tab){
        printf("Failed to parse toml file\n");
        printf("%s\n", errbuf);
        return;
    }
    printf("Success!\n");

    toml_datum_t number = toml_int_in(tab, "number");
    if(!number.ok) printf("Failed to parse number\n");
    else           printf("number = %d\n", (int)number.u.i);

    toml_datum_t name = toml_string_in(tab, "name");
    if(!name.ok) printf("Failed to parse name\n");
    else {
        printf("name = %s\n", name.u.s);
        free(name.u.s);
    }

    toml_free(tab);
}

int main(void) {

    TestReadToml();

    // SetupApplication();
    // LinceRun();

    return 0;
}

