/*
    Entities:
        - Pads  : Sprite, Transform, Box2D, Pad (for input)
        - Ball  : Sprite, Transform, Box2D, ???
        - Walls : Box2D

    How to track when a point has been scored?

*/


#include <lince.h>


typedef enum PadLocation {PadLeft, PadRight} PadLocation;

typedef struct Pad {
    LinceKey up, down; // Keys that make the pad go up or down
    PadLocation loc;
    uint32_t score;
} Pad;

typedef struct Ball {
    int _;
} Ball;


typedef enum Components {
    Component_Sprite,
    Component_Transform,
    Component_Box2D,
    Component_Pad,

} Components;


void DrawEntities(LinceECS* ecs, float dt, array_t* entities){
    for(uint32_t* entity = entities->begin; entity != entities->end; ++entity){
        LinceSprite* sprite = LinceECSGetComponent(ecs, *entity, Component_Sprite);
        LinceTransform* transform = LinceECSGetComponent(ecs, *entity, Component_Transform);
        LinceDrawSprite(sprite, transform, NULL);
    }
}

void MovePads(LinceECS* ecs, float dt, array_t* entities){

    static const float speed = 1e-3f;

    for(uint32_t* entity = entities->begin; entity != entities->end; ++entity){
        Pad* pad = LinceECSGetComponent(ecs, *entity, Component_Pad);
        LinceTransform* transform = LinceECSGetComponent(ecs, *entity, Component_Transform);
        
        if(LinceIsKeyPressed(pad->up))   transform->y += speed * dt;
        else if(LinceIsKeyPressed(pad->down)) transform->y -= speed * dt;

    }
}


typedef struct GameData {
    LinceCamera cam;
    LinceECS ecs;

    LinceEntity lpad, rpad, ball; // Main entities

} GameData;


static GameData* GetGameData(){
    return LinceGetApp()->user_data;
}

static void OnInit() {

    GameData* data = LinceCalloc(sizeof(GameData));
    LinceGetApp()->user_data = data;

    LinceAppPushAssetFolder("../../../demos/pong/assets");

    LinceInitCamera(&data->cam, LinceAppGetAspectRatio());
    LinceECSInit(&data->ecs);
    data->ecs.user_data = data;

    LinceECSNewComponent(&data->ecs, sizeof(LinceSprite));
    LinceECSNewComponent(&data->ecs, sizeof(LinceTransform));
    LinceECSNewComponent(&data->ecs, sizeof(LinceBox2D));
    LinceECSNewComponent(&data->ecs, sizeof(Pad));

    LinceECSAddSystem(&data->ecs, DrawEntities, 2, (uint32_t[]){Component_Sprite,Component_Transform});
    LinceECSAddSystem(&data->ecs, MovePads, 2, (uint32_t[]){Component_Pad,Component_Transform});

    LinceEntity lpad = LinceECSNewEntity(&data->ecs);
    LinceEntity rpad = LinceECSNewEntity(&data->ecs);
    LinceEntity ball = LinceECSNewEntity(&data->ecs);

    LinceECSAddComponents(&data->ecs, lpad, 4, (uint32_t[]){
        Component_Sprite, Component_Transform, Component_Box2D, Component_Pad});
    LinceECSSetComponent(&data->ecs, lpad, Component_Sprite, &(LinceSprite){.color = {1,0,0}});
    LinceECSSetComponent(&data->ecs, lpad, Component_Transform, &(LinceTransform){.w=0.1, .h=0.3, .x=-0.7});
    LinceECSSetComponent(&data->ecs, lpad, Component_Pad, &(Pad){.up=LinceKey_w, .down=LinceKey_s});

    LinceECSAddComponents(&data->ecs, rpad, 4, (uint32_t[]){
        Component_Sprite, Component_Transform, Component_Box2D, Component_Pad});
    LinceECSSetComponent(&data->ecs, rpad, Component_Sprite, &(LinceSprite){.color = {0,0,1}});
    LinceECSSetComponent(&data->ecs, rpad, Component_Transform, &(LinceTransform){.w=0.1, .h=0.3, .x=0.7});
    LinceECSSetComponent(&data->ecs, rpad, Component_Pad, &(Pad){.up=LinceKey_Up, .down=LinceKey_Down});

    LinceTexture* ball_texture = LinceAppGetAsset("ball.png", "texture");
    LinceECSAddComponents(&data->ecs, ball, 3, (uint32_t[]){
        Component_Sprite, Component_Transform, Component_Box2D});
    LinceECSSetComponent(&data->ecs, ball, Component_Sprite, &(LinceSprite){.color = {1,1,0}, .texture=ball_texture});
    LinceECSSetComponent(&data->ecs, ball, Component_Transform, &(LinceTransform){.w=0.1, .h=0.1});

    data->lpad = lpad;
    data->rpad = rpad;
    data->ball = ball;

}

static void OnTerminate() {
    GameData* data = GetGameData();

    LinceECSUninit(&data->ecs);
    LinceFree(data);
    LinceGetApp()->user_data = NULL;
}

static void OnUpdate(float dt) {
    GameData* data = GetGameData();

    LinceBeginRender(&data->cam);
    LinceECSUpdate(&data->ecs, dt);
    LinceEndRender();
}

static void OnEvent(LinceEvent* event){
    GameData* data = GetGameData();

    switch(event->type){
        case LinceEventType_WindowResize:
            LinceUpdateCameraProjection(&data->cam, LinceAppGetAspectRatio());
            break;
    }
}

int main() {

    LinceApp* app = LinceGetApp();
    LinceAppSetTitle("Pong");
    app->screen_width = 800;
    app->screen_height = 500;
    app->on_init   = OnInit;
    app->on_update = OnUpdate;
    app->on_event  = OnEvent;
    app->on_terminate = OnTerminate;

    LinceRun();

    return 0;
}
