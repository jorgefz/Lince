#include <lince.h>
#include "lince/tiles/tileset.h"

static LinceCamera camera;
static LinceTexture* texture;
static LinceSprite sprite = { .color = {1,1,1} }; // Red colour (RGB)
static LinceTransform transform = {
    .x = 0, .y = 0,
    .w = 1, .h = 1
};

static LinceTileset tileset;
static LinceTilemap map = {
    .width = 3, .height = 3,
    .pos = {-2, 0}, .scale = {0.5f, 0.5f}
};

static LinceTileAnim tile_anim = { .flags = LinceTileAnim_Repeat };

void OnInit() {
    LinceInitCamera(&camera, 800.0f / 500.0f);

    LinceAppPushAssetFolder(string_scoped_lit("../../../demos/tileset/assets"));
    texture = LinceAppGetAsset("outside.png", "texture");
    sprite.texture = texture;

    LinceTilesetInit(&tileset, texture, 16, 16);
    LinceTilemapInit(&map, (uint32_t[]){0, 1, 2, 3, 4, 5, 6, 7, 8});
    LinceTilemapUseTileset(&map, &tileset);

    LinceTileAnimInit(&tile_anim, &tileset, 200.0f);
    LinceTileAnimSetFrames(&tile_anim, 3, (uint32_t[]){1,2,3});
}

void OnUpdate(float dt) {
    
    const float speed = 0.005f; // units per millisec

    if(LinceIsKeyPressed(LinceKey_w)) camera.pos[1] += speed * dt; // move up
    if(LinceIsKeyPressed(LinceKey_s)) camera.pos[1] -= speed * dt; // move down
    if(LinceIsKeyPressed(LinceKey_d)) camera.pos[0] += speed * dt; // move right
    if(LinceIsKeyPressed(LinceKey_a)) camera.pos[0] -= speed * dt; // move left

    // LinceUpdateCameraProjection(&camera, LinceAppGetAspectRatio());
    LinceUpdateCamera(&camera);
    LinceTileAnimUpdate(&tile_anim, dt);
    
    LinceBeginRender(&camera);
    LinceDrawSprite(&sprite, &transform, NULL);

    LinceDrawSpriteTile(
        &(LinceSprite){.color = {1,1,1}, .texture=texture},
        &(LinceTransform){0, -1.5, 0.5, 0.5},
        LinceTileAnimGetFrameCoords(&tile_anim),
        NULL
    );

    // Draw tiles separately
    for(uint32_t x = 0; x != tileset.xcells; ++x){
        for(uint32_t y = 0; y != tileset.ycells; ++y){
            LinceTransform tile_transform = {
                .x = (float)(x+2)/2.0f, .y = (float)(y+2)/2.0f,
                .w = 0.4f, .h = 0.4f
            };
            LinceRect* uv = LinceTilesetGetTileCoords(&tileset, x, y);
            LinceDrawSpriteTile(&sprite, &tile_transform, uv, NULL);
        }
    }

    // Draw tilemap
    LinceDrawTilemap(&map, NULL);

    LinceEndRender();
}

void OnEvent(LinceEvent* event){
    if(event->type == LinceEventType_WindowResize){
        LinceUpdateCameraProjection(&camera, LinceAppGetAspectRatio());
    }
}

void OnTerminate(){
    LinceTilesetUninit(&tileset);
    LinceTilemapUninit(&map);
    LinceTileAnimUninit(&tile_anim);
}

int main() {

    LinceApp* app = LinceGetApp();
    app->screen_width = 800;
    app->screen_height = 500;
    LinceAppSetTitle("My Window", sizeof("My Window")-1);

    app->on_init = OnInit;
    app->on_update = OnUpdate;
    app->on_event = OnEvent;
    app->on_terminate = OnTerminate;

    LinceRun();

    return 0;
}
