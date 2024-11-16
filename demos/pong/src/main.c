/*
PONG - a Lince minigame

To Do
-----
- Display score using text asset
- Reset scores when either reaches 10
- Check pad collision against wall

*/


#include <lince.h>
#include <time.h>

#define PAD_SPEED 0.002f
#define BALL_SPEED 0.025f
#define LPAD_POS (-1.25f)
#define RPAD_POS (1.25f)

#define WALL_TOP (1.0f)
#define WALL_BOT (-1.0f)
#define WALL_LEFT  (-1.5f)
#define WALL_RIGHT (1.5f)



typedef enum PadLocation {PadLeft, PadRight} PadLocation;

typedef struct Pad {
    LinceKey up, down; // Keys that make the pad go up or down
    PadLocation loc;
} Pad;

typedef struct Ball {
    float xbounds[2];
    float ybounds[2];
} Ball;

typedef struct ScoreBox {
    LinceTileset *tset;
    LinceRect uv;
} ScoreBox;

typedef struct GameData {
    LinceCamera cam;
    LinceECS ecs;
    LinceEntity lpad, rpad, ball;
    LinceEntity lscore_text, rscore_text;
    LinceBool started;
    LinceKey key_start;
    int lscore, rscore;
    LinceTileset score_nums;
} GameData;

typedef enum Components {
    Component_Sprite,
    Component_Transform,
    Component_Box2D,
    Component_Pad,
    Component_Ball,
    Component_ScoreBox
} Components;


void DrawEntities(LinceECS* ecs, float dt, array_t* entities){
    LINCE_UNUSED(dt);

    for(uint32_t* entity = entities->begin; entity != entities->end; ++entity){
        LinceSprite* sprite = LinceECSGetComponent(ecs, *entity, Component_Sprite);
        LinceTransform* transform = LinceECSGetComponent(ecs, *entity, Component_Transform);

        if (LinceECSHasComponent(ecs, *entity, Component_ScoreBox)){
            ScoreBox* score_box = LinceECSGetComponent(ecs, *entity, Component_ScoreBox);
            LinceDrawSpriteTile(sprite, transform, &score_box->uv, NULL);
        } else {
            LinceDrawSprite(sprite, transform, NULL);
        }
    }
}

void ProcessInput(LinceECS* ecs, float dt, array_t* entities){

    static const float speed = PAD_SPEED;

    for(LinceEntity* entity = entities->begin; entity != entities->end; ++entity){
        Pad* pad = LinceECSGetComponent(ecs, *entity, Component_Pad);
        LinceBox2D* box = LinceECSGetComponent(ecs, *entity, Component_Box2D);
        LinceTransform* trans = LinceECSGetComponent(ecs, *entity, Component_Transform);
        if(LinceIsKeyPressed(pad->up) && (trans->y + box->h/2.0f) < WALL_TOP){
            box->dy = speed * dt;
        } else if(LinceIsKeyPressed(pad->down) && (trans->y - box->h/2.0f) > WALL_BOT){
            box->dy = -speed * dt;
        } else{
            box->dy = 0.0f;
        }
    }
}

void ResolveCollisions(LinceECS* ecs, float dt, array_t* entities){
    LINCE_UNUSED(dt);

    for(LinceEntity* e1 = entities->begin; e1 != entities->end; ++e1){
        LinceTransform* t1 = LinceECSGetComponent(ecs, *e1, Component_Transform);
        LinceBox2D* b1 = LinceECSGetComponent(ecs, *e1, Component_Box2D);

        // Ignore unmoving boxes
        if( b1->flags & LinceBox2D_Static) continue;

        // Move by x and y separately, check if either collides with other entities
        LinceBox2D bx1 = {.x = b1->x + t1->x + b1->dx, .y = b1->y + t1->y, .w = b1->w, .h=b1->h};
        LinceBox2D by1 = {.x = b1->x + t1->x, .y = b1->y + t1->y + b1->dy, .w = b1->w, .h=b1->h};

        LinceBool impact_x = 0, impact_y = 0;
        
        for(LinceEntity* e2 = entities->begin; e2 != entities->end; ++e2){
            if(e1 == e2) continue; // Ignore collision with itself
            LinceTransform* t2 = LinceECSGetComponent(ecs, *e2, Component_Transform);
            LinceBox2D* b2_rel = LinceECSGetComponent(ecs, *e2, Component_Box2D);
            LinceBox2D b2 = {.x=b2_rel->x+t2->x, .y=b2_rel->y+t2->y, .w=b2_rel->w, .h=b2_rel->h};

            if(!impact_x) impact_x = LinceBox2DCollides(&bx1, &b2);
            if(!impact_y) impact_y = LinceBox2DCollides(&by1, &b2);
            if(impact_x && impact_y) break;
        }
        
        if(!impact_x){
            t1->x += b1->dx;
        } else if(b1->flags & LinceBox2D_Bounce){
            b1->dx = -b1->dx;     
        }       

        if(!impact_y){
            t1->y += b1->dy;
        } else if(b1->flags & LinceBox2D_Bounce){
            b1->dy = -b1->dy;     
        }

    }

}

void ResetOnScore(GameData* data, PadLocation scored){
    LinceBox2D* box = LinceECSGetComponent(&data->ecs, data->ball, Component_Box2D);
    LinceBox2D* trans = LinceECSGetComponent(&data->ecs, data->ball, Component_Transform);
    trans->x = 0.0f;
    trans->y = 0.0f;
    box->dx = 0.0f;
    box->dy = 0.0f;
    data->started = LinceFalse;
    
    if (scored == PadLeft){
        data->lscore += 1;
    } else {
        data->rscore += 1;
    }

    printf("%d %d\n", data->lscore, data->rscore);

    ScoreBox* lscorebox = LinceECSGetComponent(&data->ecs, data->lscore_text, Component_ScoreBox);
    ScoreBox* rscorebox = LinceECSGetComponent(&data->ecs, data->rscore_text, Component_ScoreBox);
    lscorebox->uv = *LinceTilesetGetTileCoords(&data->score_nums, data->lscore % 9, 0);
    rscorebox->uv = *LinceTilesetGetTileCoords(&data->score_nums, data->rscore % 9, 0);
}


void CheckWallCollisions(LinceECS* ecs, float dt, array_t* entities){
    LINCE_UNUSED(dt);

    GameData* game_data = ecs->user_data;

    uint32_t ball_id = *(uint32_t*)array_get(entities, 0);
    Ball* ball = LinceECSGetComponent(ecs, ball_id, Component_Ball);
    LinceBox2D* box = LinceECSGetComponent(ecs, ball_id, Component_Box2D);
    LinceTransform* trans = LinceECSGetComponent(ecs, ball_id, Component_Transform);

    float xpos = box->x + trans->x;
    float ypos = box->y + trans->y;
    float xmin = ball->xbounds[0], xmax = ball->xbounds[1];
    float ymin = ball->ybounds[0], ymax = ball->ybounds[1];
    
    if (xpos <= xmin && box->dx < 0.0f) ResetOnScore(game_data, PadRight);
    if (xpos >= xmax && box->dx > 0.0f) ResetOnScore(game_data, PadLeft);
    if (ypos <= ymin && box->dy < 0.0f) box->dy = -box->dy;
    if (ypos >= ymax && box->dy > 0.0f) box->dy = -box->dy;

}


static GameData* GetGameData(){
    return LinceGetApp()->user_data;
}

static void OnInit() {

    GameData* data = LinceCalloc(sizeof(GameData));
    LinceGetApp()->user_data = data;
    data->key_start = LinceKey_Space;

    LinceAppPushAssetFolder("../../../demos/pong/assets");

    LinceInitCamera(&data->cam, LinceAppGetAspectRatio());
    LinceECSInit(&data->ecs);
    data->ecs.user_data = data;

    LinceECSNewComponent(&data->ecs, sizeof(LinceSprite));    /* Component_Sprite    */
    LinceECSNewComponent(&data->ecs, sizeof(LinceTransform)); /* Component_Transform */
    LinceECSNewComponent(&data->ecs, sizeof(LinceBox2D));     /* Component_Box2D     */
    LinceECSNewComponent(&data->ecs, sizeof(Pad));            /* Component_Pad       */
    LinceECSNewComponent(&data->ecs, sizeof(Ball));           /* Component_Ball      */
    LinceECSNewComponent(&data->ecs, sizeof(ScoreBox));       /* Component_ScoreBox  */

    LinceECSAddSystem(&data->ecs, DrawEntities, 2, (uint32_t[]){Component_Sprite,Component_Transform});
    LinceECSAddSystem(&data->ecs, ProcessInput, 2, (uint32_t[]){Component_Pad, Component_Box2D});
    LinceECSAddSystem(&data->ecs, ResolveCollisions, 2, (uint32_t[]){Component_Box2D, Component_Transform});
    LinceECSAddSystem(&data->ecs, CheckWallCollisions, 3, (uint32_t[]){Component_Box2D, Component_Ball, Component_Transform});

    LinceEntity lpad = LinceECSNewEntity(&data->ecs);
    LinceEntity rpad = LinceECSNewEntity(&data->ecs);
    LinceEntity ball = LinceECSNewEntity(&data->ecs);
    LinceEntity lscore = LinceECSNewEntity(&data->ecs);
    LinceEntity rscore = LinceECSNewEntity(&data->ecs);
    
    data->lpad = lpad;
    data->rpad = rpad;
    data->ball = ball;
    data->lscore_text = lscore;
    data->rscore_text = rscore;

    /* Left pad */
    LinceECSAddComponents(&data->ecs, lpad, 4, (uint32_t[]){
        Component_Sprite, Component_Transform, Component_Box2D, Component_Pad});
    LinceECSSetComponent(&data->ecs, lpad, Component_Sprite, &(LinceSprite){.color = {1,0,0}});
    LinceECSSetComponent(&data->ecs, lpad, Component_Transform, &(LinceTransform){.w=0.1, .h=0.3, .x=LPAD_POS});
    LinceECSSetComponent(&data->ecs, lpad, Component_Box2D, &(LinceBox2D){.w=0.1, .h=0.3});
    LinceECSSetComponent(&data->ecs, lpad, Component_Pad, &(Pad){.up=LinceKey_w, .down=LinceKey_s});

    /* Right pad */
    LinceECSAddComponents(&data->ecs, rpad, 4, (uint32_t[]){
        Component_Sprite, Component_Transform, Component_Box2D, Component_Pad});
    LinceECSSetComponent(&data->ecs, rpad, Component_Sprite, &(LinceSprite){.color = {0,0,1}});
    LinceECSSetComponent(&data->ecs, rpad, Component_Transform, &(LinceTransform){.w=0.1, .h=0.3, .x=RPAD_POS});
    LinceECSSetComponent(&data->ecs, rpad, Component_Box2D, &(LinceBox2D){.w=0.1, .h=0.3});
    LinceECSSetComponent(&data->ecs, rpad, Component_Pad, &(Pad){.up=LinceKey_Up, .down=LinceKey_Down});

    /* Ball */
    LinceTexture* ball_texture = LinceAppGetAsset("ball.png", "texture");
    LinceECSAddComponents(&data->ecs, ball, 4, (uint32_t[]){
        Component_Sprite, Component_Transform, Component_Box2D, Component_Ball});
    LinceECSSetComponent(&data->ecs, ball, Component_Sprite, &(LinceSprite){.color = {1,1,0}, .texture=ball_texture});
    LinceECSSetComponent(&data->ecs, ball, Component_Transform, &(LinceTransform){.w=0.1f, .h=0.1f});
    LinceECSSetComponent(&data->ecs, ball, Component_Box2D, &(LinceBox2D){.w=0.1f, .h=0.1f, .flags=LinceBox2D_Bounce});
    LinceECSSetComponent(&data->ecs, ball, Component_Ball, &(Ball){.xbounds={WALL_LEFT,WALL_RIGHT}, .ybounds={WALL_BOT, WALL_TOP}});

    /* Score indicators */
    LinceTexture* score_numbers = LinceAppGetAsset("text.png", "texture");
    LinceTilesetInit(&data->score_nums, score_numbers, 5, 5);
    LinceRect* score_zero =  LinceTilesetGetTileCoords(&data->score_nums, 0, 0);

    LinceECSAddComponents(&data->ecs, lscore, 3, (uint32_t[]){
        Component_Sprite, Component_Transform, Component_ScoreBox});
    LinceECSSetComponent(&data->ecs, lscore, Component_Sprite,    &(LinceSprite){.color = {1,1,1}, .texture=score_numbers});
    LinceECSSetComponent(&data->ecs, lscore, Component_Transform, &(LinceTransform){.w=0.2f, .h=0.2f, .x=-1.4f, .y=0.85f});
    LinceECSSetComponent(&data->ecs, lscore, Component_ScoreBox,  &(ScoreBox){.tset=&data->score_nums, .uv = *score_zero});
    
    LinceECSAddComponents(&data->ecs, rscore, 3, (uint32_t[]){
        Component_Sprite, Component_Transform, Component_ScoreBox});
    LinceECSSetComponent(&data->ecs, rscore, Component_Sprite,    &(LinceSprite){.color = {1,1,1}, .texture=score_numbers});
    LinceECSSetComponent(&data->ecs, rscore, Component_Transform, &(LinceTransform){.w=0.2f, .h=0.2f, .x=1.4f, .y=0.85f});
    LinceECSSetComponent(&data->ecs, rscore, Component_ScoreBox,  &(ScoreBox){.tset=&data->score_nums, .uv = *score_zero});

}

static void OnTerminate() {
    GameData* data = GetGameData();

    LinceTilesetUninit(&data->score_nums);
    LinceECSUninit(&data->ecs);
    LinceFree(data);
    LinceGetApp()->user_data = NULL;
}

static void OnUpdate(float dt) {
    GameData* data = GetGameData();

    if(LinceIsKeyPressed(data->key_start) && data->started == 0){
        data->started = 1;
        LinceBox2D* ball = LinceECSGetComponent(&data->ecs, data->ball, Component_Box2D);
        int signx = (rand() > RAND_MAX/2.0f) ? 1.0f : -1.0f;
        int signy = (rand() > RAND_MAX/2.0f) ? 1.0f : -1.0f;
        ball->dx = signx * BALL_SPEED * (rand()/(RAND_MAX+1.0f)/2.0f + 0.5f);
        ball->dy = signy * BALL_SPEED * (rand()/(RAND_MAX+1.0f)/2.0f + 0.5f);
    }

    LinceBeginRender(&data->cam);
    LinceECSUpdate(&data->ecs, dt);
    LinceEndRender();
}

static void OnEvent(LinceEvent* event){
    GameData* data = GetGameData();

    switch(event->type){
        case LinceEventType_WindowResize:
            LinceUpdateCameraProjection(&data->cam, LinceAppGetAspectRatio());
            LinceUpdateCamera(&data->cam);
            break;
    }
}

int main() {

    srand(time(NULL));

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
