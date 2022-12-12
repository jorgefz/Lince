#include "missile_command.h"
#include "math.h"
#include "time.h"

#include <lince/containers/array.h>

#include <cglm/affine.h>

#include "timer.h"
#include "collider.h"
#include "gameobject.h"
#include "blast.h"
#include "marker.h"

/*
TO-DO:
	- Fix bug where duplicates are rendered
	- Loose / win condition

	- For Colliders, add delete_flag and just iterate+remove at the end of each loop

Notes:
	- Only one bomb and one missile may be deleted per frame due to out of bounds behaviour.
	- Only one bomb/missile pair may be deleted per frame due to an intercept.
*/

#define BOMB_HP_DAMAGE 10

#define MISSILE_WIDTH 0.04f
#define MISSILE_HEIGHT 0.10f
#define MISSILE_COOLDOWN 100.0f // launch min every 1 sec

#define BOMB_WIDTH 0.1f
#define BOMB_HEIGHT 0.1f
#define BOMB_COOLDOWN 3000.0f // bomb drops every 3 sec
#define BOMB_SPEED 2e-3

#define BKG_WIDTH 3.5f
#define BKG_HEIGHT 2.0f



typedef struct GameState{

	int score, hp;
	float cannon_x, cannon_y;
	float missile_vmax;
	float ymin, ymax;
	float xmin, xmax;
	float angle; // cannon angle
	float dt;    // delta time
	LinceCamera* cam;

	Timer missile_timer; // how often player is able to launch
	Timer bomb_timer; // how often to drop bombs

	array_t missile_list;
	array_t bomb_list;
	array_t blast_list;  // kill radius generated when missile detonates
	array_t marker_list; // points where missile is directed

	LinceTexture* missile_tex;
	LinceTexture* bomb_tex;
	LinceTexture* blast_tex;
	LinceTexture* marker_tex;
	LinceTexture* bkg_city;
	
} GameState;


float GetRandomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

// Calculates the angle between the Y axis (centered on the screen)
// and the line connecting the cannon and the mouse pointer.
float CalculateCannonAngle(LinceCamera* cam){
	vec2 mouse;
	LinceGetMousePosWorld(mouse, cam);
	static const float cannon_x = 0.0f, cannon_y = -1.0f;
	return 90.0f - atan2f(mouse[1]-cannon_y, mouse[0]-cannon_x) * 180.0f / M_PI;
}


float GetDistance2D(float x1, float y1, float x2, float y2){
	return sqrtf( powf(x2-x1,2) + powf(y2-y1,2) );
}




// ---------------------------


void CreateBomb(array_t* bomb_list, LinceTexture* texture){
	Collider collider = {
		.x = GetRandomFloat(-1.3f, 1.3f),
		.y = 1.0f,
		.vx = 0.0f,
		.vy = BOMB_SPEED * (-1.0f),
		.w = BOMB_WIDTH,
		.h = BOMB_HEIGHT
	};
	Sprite sprite = {
		.x = collider.x,
		.y = collider.y,
		.w = collider.w,
		.h = collider.h,
		.color = {1.0f, 1.0f, 1.0f, 1.0f},
		.rotation = 0.0f,
		.texture = texture
	};
	GameObject bomb = {
		.collider = LinceNewCopy(&collider, sizeof(Collider)),
		.sprite   = LinceNewCopy(&sprite, sizeof(Sprite)),
	};
	array_push_back(bomb_list, &bomb);
}


void DeleteCrashedBomb(GameState* state){
	GameObject *obj;

	for(uint32_t i=0; i!=state->bomb_list.size; ++i){
		obj = array_get(&state->bomb_list, i);
		vec2 pos = {obj->collider->x, obj->collider->y};

		if(pos[1] > state->ymin){
			continue;
		}

		state->hp -= BOMB_HP_DAMAGE;
		CreateBlast(&state->blast_list, pos, state->blast_tex);
		DeleteEntityItem(&state->bomb_list, i);
		break;
	}
}


void DeleteInterceptedBombs(array_t* bomb_list, vec2 pos){
	float distance;
	GameObject* obj;

	for(uint32_t i=0; i!=bomb_list->size; ++i){
		obj = array_get(bomb_list, i);
		vec2 bpos = {obj->collider->x, obj->collider->y};
		distance = GetDistance2D(pos[0], pos[1], bpos[0], bpos[1]);
		if(distance < BLAST_RADIUS){
			DeleteEntityItem(bomb_list, i);
			break;
		}
	}
}



void UpdateBombs(GameState* state){

	Collider *b;
	Sprite *s;
	GameObject *obj;

	// Updated collider and sprite locations
	for(uint32_t i=0; i!=state->bomb_list.size; ++i){
		obj = array_get(&state->bomb_list, i);
		b = obj->collider;
		s = obj->sprite;
		b->x += b->vx;
		b->y += b->vy;
		s->x = b->x;
		s->y = b->y;
	}

	DeleteCrashedBomb(state);
}


// -------------------------


void CreateMissile(GameState* state, float angle, LinceTexture* texture){
	float vtot = state->missile_vmax;
	Collider collider = {
		.x = state->cannon_x,
		.y = state->cannon_y,
		.w = MISSILE_WIDTH,
		.h = MISSILE_HEIGHT,
		.vx = vtot * cosf((90.0f - angle) * (float)M_PI / 180.0f),
		.vy = vtot * sinf((90.0f - angle) * (float)M_PI / 180.0f),
		.angle = angle
	};
	Sprite sprite = {
		.x = collider.x,
		.y = collider.y,
		.w = collider.w,
		.h = collider.h,
		.color = {1.0f, 1.0f, 1.0f, 1.0f},
		.rotation = angle,
		.texture = texture
	};
	GameObject missile = {
		.sprite = LinceNewCopy(&sprite, sizeof(Sprite)),
		.collider = LinceNewCopy(&collider, sizeof(Collider))
	};
	array_push_back(&state->missile_list, &missile);
}

void DeleteMissile(GameState* state, int index){
	
	GameObject* m = array_get(&state->missile_list, index);
	vec2 pos = {m->collider->x, m->collider->y};

	DeleteEntityItem(&state->missile_list, index);
	DeleteEntityItem(&state->marker_list, index);
	CreateBlast(&state->blast_list, pos, state->blast_tex);
	DeleteInterceptedBombs(&state->bomb_list, pos);
}


void DeleteStrayMissiles(GameState* state){
	
	for(uint32_t i = 0; i != state->missile_list.size; ++i){
		GameObject* obj = array_get(&state->missile_list, i);
		vec2 pos = {obj->collider->x, obj->collider->y};
		GameObject* marker = array_get(&state->marker_list, i);
		float marker_y = marker->sprite->y;

		// out of bounds missiles
		if (pos[0] > state->xmax || pos[0] < state->xmin ||
			pos[1] > state->ymax || pos[1] < state->ymin
		){
			DeleteMissile(state, i);
			break;
		}
		// reached marker
		else if (pos[1] > marker_y){
			DeleteMissile(state, i);
			break;
		}
	}
}


void CheckBombIntercept(GameState* state){
	// find and delete intercepted bombs

	int dead_bomb = -1;
	int dead_missile = -1;
	GameObject *bomb_obj, *ms_obj;
	Collider *b, *m;

	for(uint32_t i=0; i!=state->bomb_list.size; ++i){
		bomb_obj = array_get(&state->bomb_list, i);
		b = bomb_obj->collider;

		for(uint32_t j = 0; j != state->missile_list.size; ++j){
			ms_obj = array_get(&state->missile_list, j);
			m = ms_obj->collider;

			if(CollidersOverlap(b, m)){
				dead_bomb = (int)i;
				dead_missile = j;
				state->score += 1;
				break;
			}
		}
		if (dead_bomb != -1) break;
	}

	if(dead_bomb != -1){
		DeleteEntityItem(&state->bomb_list, dead_bomb);
		DeleteMissile(state, dead_missile);
	}
}


void UpdateMissiles(GameState* state){
	GameObject *ms_obj;
	Collider *ms;
	for(uint32_t i = 0; i != state->missile_list.size; ++i){
		ms_obj = array_get(&state->missile_list, i);
		ms = ms_obj->collider;
		ms->x += ms->vx;
		ms->y += ms->vy;
		ms_obj->sprite->x = ms->x;
		ms_obj->sprite->y = ms->y;
	}

	DeleteStrayMissiles(state);
	CheckBombIntercept(state);
}



// ----------------------


void DrawText(struct nk_context *ctx, nk_flags align, const char* text, ...){
	char buffer[100];
	va_list args;

	va_start(args, text);
	vsnprintf(buffer, 100, text, args);
	nk_label(ctx, buffer, align);
	va_end(args);
}

void DrawDebugUI(GameState* data){
	LinceUILayer* ui = LinceGetAppState()->ui;

	struct nk_rect size = {.x=50, .y=50, .w=200, .h=300};
	
	nk_style_set_font(ui->ctx, &ui->fonts[LinceFont_Droid20]->handle);
	if(nk_begin(ui->ctx, "Debug Info", size, 
		NK_WINDOW_BORDER | NK_WINDOW_MINIMIZABLE | NK_WINDOW_MOVABLE )){
		
		nk_layout_row_dynamic(ui->ctx, 20, 1);
		DrawText(ui->ctx, NK_TEXT_LEFT, "Angle: %.2f",    data->angle);
		DrawText(ui->ctx, NK_TEXT_LEFT, "Cooldown: %.2f", data->bomb_timer.counter);
		DrawText(ui->ctx, NK_TEXT_LEFT, "HP: %d",         data->hp);
		DrawText(ui->ctx, NK_TEXT_LEFT, "Score: %d",      data->score);
		DrawText(ui->ctx, NK_TEXT_LEFT, "Missiles: %d",   data->missile_list.size);
		DrawText(ui->ctx, NK_TEXT_LEFT, "Bombs: %d",      data->bomb_list.size);
		DrawText(ui->ctx, NK_TEXT_LEFT, "Markers: %d",    data->marker_list.size);
		DrawText(ui->ctx, NK_TEXT_LEFT, "Blasts: %d",     data->blast_list.size);
	}
	nk_end(ui->ctx);
}

// -----------------------

void MCommandOnAttach(LinceLayer* layer){

	GameState* data = LinceGetLayerData(layer);
	data->cam = LinceCreateCamera(LinceGetAspectRatio());
	data->score = 0;
	data->hp = 100;
		
	data->cannon_x = 0.0f;
	data->cannon_y = -1.0f;
	data->missile_vmax = 7e-3f;
	data->ymin = -1.0f;
	data->ymax = 1.0f;
	data->xmin = -1.5f;
	data->xmax = 1.5f;
	data->dt = 0.0f;

	data->missile_list = array_create(sizeof(GameObject));
	data->bomb_list    = array_create(sizeof(GameObject));
	data->marker_list  = array_create(sizeof(GameObject));
	data->blast_list   = array_create(sizeof(GameObject));

	data->bomb_timer = (Timer){.start = BOMB_COOLDOWN, .tick = -1.0f, .end = 0.0f};
	ResetTimer(&data->bomb_timer);

	data->missile_timer = (Timer){.start = MISSILE_COOLDOWN, .tick = -1.0f, .end = 0.0f};
	ResetTimer(&data->missile_timer);

	data->bomb_tex = LinceCreateTexture("Bomb",       "minigames/mcommand/assets/bomb.png");
	data->missile_tex = LinceCreateTexture("Missile", "minigames/mcommand/assets/missile.png");
	data->blast_tex = LinceCreateTexture("Blast",     "minigames/mcommand/assets/circle.png");
	data->marker_tex = LinceCreateTexture("Marker",   "minigames/mcommand/assets/marker.png");
	data->bkg_city = LinceCreateTexture("City",       "minigames/mcommand/assets/background-city.png");

	srand(time(NULL));
}

void MCommandOnUpdate(LinceLayer* layer, float dt){
	
	GameState* data = LinceGetLayerData(layer);
	// LinceUILayer* ui = LinceGetAppState()->ui;
	data->dt = dt;

	// update view
	LinceResizeCameraView(data->cam, LinceGetAspectRatio());
	LinceUpdateCamera(data->cam);

	// missile cooldown will be reset when next is launched
	UpdateTimer(&data->missile_timer, dt);
	UpdateTimer(&data->bomb_timer, dt);
	if(data->bomb_timer.finished){
		ResetTimer(&data->bomb_timer);
		CreateBomb(&data->bomb_list, data->bomb_tex);
	}
	data->angle = CalculateCannonAngle(data->cam);
	
	UpdateMissiles(data);
	UpdateBombs(data);
	UpdateBlasts(&data->blast_list, dt);
	
	DrawDebugUI(data);

	// draw objects
	LinceBeginScene(data->cam);

	// Background
	LinceDrawQuad((LinceQuadProps){
		.x = 0.0f,
		.y = 0.0f,
		.w = BKG_WIDTH,
		.h = BKG_HEIGHT,
		.color = {1.0, 1.0, 1.0, 1.0},
		.texture = data->bkg_city,
		.zorder = -0.1f
	});

	// Cannon
	LinceDrawQuad((LinceQuadProps){
		.x = data->cannon_x,
		.y = data->cannon_y,
		.w = 0.05f,
		.h = 0.2f,
		.color = {0.5, 0.5, 0.5, 1.0},
		.rotation = data->angle
	});
	
	DrawEntityList(&data->missile_list);
	DrawEntityList(&data->bomb_list);
	DrawEntityList(&data->marker_list);
	DrawEntityList(&data->blast_list);

	LinceEndScene();
	LinceSetClearColor(0.0, 0.0, 0.0, 1.0);
}

void MCommandLayerOnEvent(LinceLayer* layer, LinceEvent* event){
	if(event->type != LinceEventType_MouseButtonPressed) return;
	GameState* state = LinceGetLayerData(layer);

	if(state->missile_timer.finished){
		CreateMissile(state, state->angle, state->missile_tex);
		ResetTimer(&state->missile_timer);

		vec2 mouse;
		LinceGetMousePosWorld(mouse, state->cam);
		PlaceMarker(&state->marker_list, mouse, state->marker_tex);
	}
}

void MCommandOnDetach(LinceLayer* layer){
	GameState* data = LinceGetLayerData(layer);

	DeleteEntityList(&data->missile_list);
	DeleteEntityList(&data->bomb_list);
	DeleteEntityList(&data->marker_list);
	DeleteEntityList(&data->blast_list);
	
	LinceDeleteCamera(data->cam);
	LinceDeleteTexture(data->missile_tex);
	LinceDeleteTexture(data->bomb_tex);
	LinceDeleteTexture(data->bkg_city);
	LinceDeleteTexture(data->marker_tex);
	LinceDeleteTexture(data->blast_tex);
	free(data);
}

LinceLayer* MCommandLayerInit(){
	LinceLayer* layer = LinceCreateLayer(NULL);

	layer->OnAttach = MCommandOnAttach;
	layer->OnUpdate = MCommandOnUpdate;
	layer->OnEvent  = MCommandLayerOnEvent;
	layer->OnDetach = MCommandOnDetach;
	layer->data = calloc(1, sizeof(GameState));
	LINCE_ASSERT_ALLOC(layer->data, sizeof(GameState));

	return layer;
}