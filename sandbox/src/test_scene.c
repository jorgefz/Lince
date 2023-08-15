#include "test_scene.h"


void InitBlueScene(LinceScene* scene){

    BlueScene* data = LinceMalloc(sizeof(BlueScene));
    LinceInitCamera(&data->camera, LinceGetAspectRatio());
    scene->data = data;

    LinceUILayer* ui = LinceGetApp()->ui;
    struct nk_context *ctx = ui->ctx;

    nk_style_push_color(
        ctx,
        &ctx->style.window.background,
        (struct nk_color){0,0,0,255}
    );

    nk_style_push_style_item(
        ctx, &ctx->style.window.fixed_background,
        nk_style_item_color((struct nk_color){0,0,0,255})
    );

}

void UninitBlueScene(LinceScene* scene){
    LinceFree(scene->data);
}

void UpdateBlueScene(LinceScene* scene, float dt){
    LINCE_UNUSED(dt);

    BlueScene* data = scene->data;
    LinceResizeCameraView(&data->camera, LinceGetAspectRatio());
	LinceUpdateCamera(&data->camera);

    // GUI
    LinceUILayer* ui = LinceGetApp()->ui;
    struct nk_context *ctx = ui->ctx;
    nk_style_set_font(ctx, &ui->fonts[LinceFont_Droid20]->handle);
    
    if (nk_begin(ctx, "Demo", nk_rect(20, 20, 200, 150), 0)) {
        struct nk_style_button button_style = {
            /* background */
            .normal = nk_style_item_color((struct nk_color){0,0,0,255}),
            .hover  = nk_style_item_color((struct nk_color){50,50,50,255}),
            .active = nk_style_item_color((struct nk_color){18,44,102,255}),
            // struct nk_color border_color;

            /* text */
            .text_background = {0,0,0,255}, // struct nk_color text_background;
            .text_normal     = {255,255,255,255}, // struct nk_color text_normal;
            .text_hover      = {255,255,255,255}, // struct nk_color text_hover;
            .text_active     = {255,255,255,255}, // struct nk_color text_active;
            .text_alignment  = NK_TEXT_CENTERED, // nk_flags text_alignment;

            /* properties */
            .border   = 0, // float border;
            .rounding = 0, // float rounding;
            .padding  = {0}, // struct nk_vec2 padding;
            .image_padding = {0}, // struct nk_vec2 image_padding;
            .touch_padding = {0}, // struct nk_vec2 touch_padding;
        };
        nk_layout_row_dynamic(ctx, 30, 1);
        if (nk_button_label_styled(ctx, &button_style, "Go to red room")){
            LincePushScene(&(LinceScene){
                .on_init = InitRedScene,
                .on_delete = UninitRedScene,
                .on_update = UpdateRedScene,
                .on_draw = DrawRedScene
            });
        }
        nk_layout_row_dynamic(ctx, 30, 1);
        if (nk_button_label_styled(ctx, &button_style, "Back to main menu")){
            LincePopScene();
        }
    }
    nk_end(ctx);
}

void DrawBlueScene(LinceScene* scene){

    BlueScene* data = scene->data;
    LinceBeginScene(&data->camera);
    LinceDrawSprite(&(LinceSprite){
        .w = 1, .h = 1,
        .x = 0.5, .y = 0.5,
        .color = {0,0,1,1}
    }, NULL);
    LinceEndScene();
}



void InitRedScene(LinceScene* scene){
    RedScene* data = LinceMalloc(sizeof(RedScene));
    LinceInitCamera(&data->camera, LinceGetAspectRatio());
    scene->data = data;
}

void UninitRedScene(LinceScene* scene){
    LinceFree(scene->data);
}

void UpdateRedScene(LinceScene* scene, float dt){
    LINCE_UNUSED(dt);

    RedScene* data = scene->data;
    LinceResizeCameraView(&data->camera, LinceGetAspectRatio());
	LinceUpdateCamera(&data->camera);

    // GUI
    LinceUILayer* ui = LinceGetApp()->ui;
    struct nk_context *ctx = ui->ctx;
    nk_style_set_font(ctx, &ui->fonts[LinceFont_Droid20]->handle);

    if (nk_begin(ctx, "Demo", nk_rect(20, 20, 150, 150), 0)) {
        struct nk_style_button button_style = {
            /* background */
            .normal = nk_style_item_color((struct nk_color){0,0,0,255}),
            .hover  = nk_style_item_color((struct nk_color){50,50,50,255}),
            .active = nk_style_item_color((struct nk_color){18,44,102,255}),
            // struct nk_color border_color;

            /* text */
            .text_background = {0,0,0,255}, // struct nk_color text_background;
            .text_normal     = {255,255,255,255}, // struct nk_color text_normal;
            .text_hover      = {255,255,255,255}, // struct nk_color text_hover;
            .text_active     = {255,255,255,255}, // struct nk_color text_active;
            .text_alignment  = NK_TEXT_CENTERED, // nk_flags text_alignment;

            /* properties */
            .border   = 0, // float border;
            .rounding = 0, // float rounding;
            .padding  = {0}, // struct nk_vec2 padding;
            .image_padding = {0}, // struct nk_vec2 image_padding;
            .touch_padding = {0}, // struct nk_vec2 touch_padding;
        };
        nk_layout_row_dynamic(ctx, 30, 1);
        if (nk_button_label_styled(ctx, &button_style, "Go to blue room")){
            LincePopScene();
        }
    }
    nk_end(ctx);

}

void DrawRedScene(LinceScene* scene){
    
    RedScene* data = scene->data;
    LinceBeginScene(&data->camera);
    LinceDrawSprite(&(LinceSprite){
        .w = 0.5, .h = 0.5,
        .x = -0.5, .y = -0.5,
        .color = {1,0,0,1}
    }, NULL);
    LinceEndScene();
}
