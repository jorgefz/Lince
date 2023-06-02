#include "test_scene.h"


void InitBlueScene(LinceSceneStack* stack, LinceScene* scene){
    BlueScene* data = LinceMalloc(sizeof(BlueScene));
    LinceInitCamera(&data->cam, LinceGetAspectRatio());
    scene->data = data;
}

void UninitBlueScene(LinceSceneStack* stack, LinceScene* scene){
    LinceFree(scene->data);
}

void UpdateBlueScene(LinceSceneStack* stack, LinceScene* scene, float dt){
    BlueScene* data = scene->data;
    LinceResizeCameraView(&data->cam, LinceGetAspectRatio());
	LinceUpdateCamera(&data->cam);

    // GUI
    LinceUILayer* ui = LinceGetAppState()->ui;
    struct nk_context *ctx = ui->ctx;
    nk_style_set_font(ctx, &ui->fonts[LinceFont_Droid15]->handle);
    if (nk_begin(ctx, "Demo", nk_rect(20, 20, 100, 100), 0)) {
        nk_layout_row_static(ctx, 25, 80, 1);
        if (nk_button_label(ctx, "Start")){
            LincePushScene(stack, &(LinceScene){
                .on_init = InitRedScene,
                .on_delete = UninitRedScene,
                .on_update = UpdateRedScene,
                .on_draw = DrawRedScene
            });
        }
    }
    nk_end(ctx);
}

void DrawBlueScene(LinceSceneStack* stack, LinceScene* scene){
    BlueScene* data = scene->data;
    LinceBeginScene(&data->cam);
    LinceDrawSprite(&(LinceSprite){
        .w = 1, .h = 1,
        .x = 0.5, .y = 0.5,
        .color = {0,0,1,1}
    }, NULL);
    LinceEndScene();
}



void InitRedScene(LinceSceneStack* stack, LinceScene* scene){
    RedScene* data = LinceMalloc(sizeof(RedScene));
    LinceInitCamera(&data->cam, LinceGetAspectRatio());
    scene->data = data;
}

void UninitRedScene(LinceSceneStack* stack, LinceScene* scene){
    LinceFree(scene->data);
}

void UpdateRedScene(LinceSceneStack* stack, LinceScene* scene, float dt){
    RedScene* data = scene->data;
    LinceResizeCameraView(&data->cam, LinceGetAspectRatio());
	LinceUpdateCamera(&data->cam);

    // GUI
    LinceUILayer* ui = LinceGetAppState()->ui;
    struct nk_context *ctx = ui->ctx;
    nk_style_set_font(ctx, &ui->fonts[LinceFont_Droid15]->handle);
    if (nk_begin(ctx, "Demo", nk_rect(20, 20, 100, 100), 0)) {
        nk_layout_row_static(ctx, 25, 80, 1);
        if (nk_button_label(ctx, "Start")){
            LincePopScene(stack);
        }
    }
    nk_end(ctx);
}

void DrawRedScene(LinceSceneStack* stack, LinceScene* scene){
    RedScene* data = scene->data;
    LinceBeginScene(&data->cam);
    LinceDrawSprite(&(LinceSprite){
        .w = 0.5, .h = 0.5,
        .x = -0.5, .y = -0.5,
        .color = {1,0,0,1}
    }, NULL);
    LinceEndScene();
}
