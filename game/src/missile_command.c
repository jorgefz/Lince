#include "missile_command.h"
#include "math.h"
#include "time.h"

#include <cglm/affine.h>

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

#define BLAST_RADIUS 0.12f
#define BLAST_LIFETIME_MS 1000.0f
#define BLAST_LIFELOSS 0.5f
#define BLAST_COLOR_INIT {1.0f, 1.0f, 0.0, 1.0f}

#define MISSILE_WIDTH 0.04f
#define MISSILE_HEIGHT 0.10f
#define BOMB_WIDTH 0.1f
#define BOMB_HEIGHT 0.1f

#define BKG_WIDTH 3.5f
#define BKG_HEIGHT 2.0f

typedef struct Collider {
	float x, y, vx, vy, w, h, angle;
} Collider;

typedef struct Blast {
	float x, y;      // position
	float color[4];  // color of the blast
	float lifetime;  // time left
	float life_loss; // loss rate of lifetime
} Blast;

typedef struct Marker {
	float x, y;
} Marker;

typedef struct GameState{
	Collider *missiles;
	int missile_count;
	LinceTexture* missile_tex;

	Collider *bombs;
	int bomb_count;
	LinceTexture* bomb_tex;
	
	int score, hp;
	float cannon_x, cannon_y;
	float missile_vmax;
	float ymin, ymax;
	float xmin, xmax;
	float angle; // cannon angle
	float dt;    // delta time

	LinceBool missile_cooldown;
	float bomb_cooldown, bomb_cooldown_max;

	Blast* blasts;				// kill radius generated when missile detonates
	int blast_count;
	LinceTexture* blast_tex;	// texture of a blast - circle

	Marker* markers;
	int marker_count;
	LinceTexture* marker_tex;

	LinceTexture* bkg_city;

	LinceCamera* cam;
} GameState;


float GetRandomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

void ToWorldCoords(float *x, float *y, LinceCamera* cam){
	float sx = *x, sy = *y;
	const float w = (float)LinceGetAppState()->window->width;
	const float h = (float)LinceGetAppState()->window->height;
	// normalise screen coordinates to range (-1,1)
	sx = 2.0f*sx/w - 1.0f;
	sy = 1.0f - 2.0f*sy/h;
	vec4 svec = {sx, sy, 0.0f, 1.0f};
	mat4 vp_inv;
	vec4 wvec;
	glm_mat4_inv(cam->view_proj, vp_inv);
	glm_mat4_mulv(vp_inv, svec, wvec);
	*x = wvec[0] / wvec[3];
	*y = wvec[1] / wvec[3];
}

void GetMousePosWorld(float *x, float *y, LinceCamera* cam){
	LinceGetMousePos(x, y);
	ToWorldCoords(x, y, cam);
}

// Calculates the angle between the Y axis (centered on the screen)
// and the line connecting the cannon and the mouse pointer.
float CalculateCannonAngle(LinceCamera* cam){
	float mouse_x, mouse_y;
	GetMousePosWorld(&mouse_x, &mouse_y, cam);
	const float cannon_x = 0.0f, cannon_y = -1.0f;
	return 90.0f - atan2f(mouse_y-cannon_y, mouse_x-cannon_x) * 180.0f / M_PI;
}

void DeleteListItem(void** list_address, size_t* items, size_t item_size, size_t index){
	void* list = *list_address;

	if (*items == 0 || index >= *items) return;
	else if (*items == 1){
		*items = 0;
		free(list);
		*list_address = NULL;
		return;
	}

	if(index != *items - 1){
		void* dest = list + index * item_size;
		void* src  = list + (index + 1) * item_size;
		size_t move_bytes = item_size * (*items - index - 1);
		memmove(dest, src, move_bytes);
	}

	(*items)--;
	void* new_list = realloc(list, (*items) * item_size);
	LINCE_ASSERT_ALLOC(new_list, (*items) * item_size);
	*list_address = new_list;
}

float FindDistance2D(float x1, float y1, float x2, float y2){
	return sqrtf( powf(x2-x1,2) + powf(y2-y1,2) );
}

// ---------------------------

void CreateBlast(GameState* state, float x, float y){
	state->blast_count++;
	Blast* new_list = realloc(state->blasts, state->blast_count * sizeof(Blast));
	LINCE_ASSERT_ALLOC(new_list, state->blast_count * sizeof(Blast));
	state->blasts = new_list;

	Blast* b = &state->blasts[state->blast_count - 1];
	b->x = x;
	b->y = y;
	b->color[0] = 1.0f; b->color[1] = 1.0f;
	b->color[2] = 0.0f; b->color[3] = 1.0f;
	b->lifetime = BLAST_LIFETIME_MS;
	b->life_loss = BLAST_LIFELOSS;
}

void DeleteBlast(GameState* state, int index){
	size_t count = state->blast_count;
	DeleteListItem((void**)&state->blasts, &count, sizeof(Blast), (size_t)index);
	state->blast_count = (int)count;
}

void UpdateBlasts(GameState* state){
	Blast* b;
	for(int i=0; i!=state->blast_count; ++i){
		b = &state->blasts[i];
		if(b->lifetime <= 0.0f) continue;
		b->lifetime -= b->life_loss * state->dt;
		b->color[1] -= BLAST_LIFELOSS/BLAST_LIFETIME_MS * state->dt; // make redder
		b->color[3] -= BLAST_LIFELOSS/BLAST_LIFETIME_MS * state->dt;
		// update alpha, color, etc
	}

	// delete one old one
	for(int i=0; i!=state->blast_count; ++i){
		b = &state->blasts[i];
		if (b->lifetime <= 0.0f){
			DeleteBlast(state, i);
			break;
		}
	}
	
}

void DrawBlasts(GameState* state){
	Blast* b;
	for(int i=0; i!=state->blast_count; ++i){
		b = &state->blasts[i];
		LinceDrawQuad((LinceQuadProps){
			.x = b->x,
			.y = b->y,
			.w = BLAST_RADIUS*2.0f,
			.h = BLAST_RADIUS*2.0f,
			.color = {b->color[0], b->color[1], b->color[2], b->color[3]},
			.zorder = 0.5f,
			.texture = state->blast_tex
		});
	}
}



void PlaceMarker(GameState* state){
	float x, y;
	GetMousePosWorld(&x, &y, state->cam);
	state->marker_count++;
	state->markers = realloc(state->markers, state->marker_count * sizeof(Marker));
	LINCE_ASSERT_ALLOC(state->markers, state->marker_count * sizeof(Marker));
	Marker* m = &state->markers[state->marker_count-1];
	m->x = x;
	m->y = y;
}

void DeleteMarker(GameState* state, int index){
	size_t count = state->marker_count;
	DeleteListItem((void**)&state->markers, &count, sizeof(Marker), (size_t)index);
	state->marker_count = (int)count;
}

void DrawMarkers(GameState* state){
	for(int i=0; i!=state->marker_count; ++i){
		Marker* m = &state->markers[i];
		LinceDrawQuad((LinceQuadProps){
			.x = m->x, .y = m->y,
			.w = 0.05f, .h = 0.05f,
			.color = {0.0f, 0.2f, 0.8f, 1.0f},
			.texture = state->marker_tex
		});
	}
}

// Check if two non-rotatde rectangles overlap
LinceBool CollidersOverlap(Collider* a, Collider* b){
	LinceBool overlap =
	a->x + a->w / 2.0f > b->x - b->w / 2.0f &&
    a->x - a->w / 2.0f < b->x + b->w / 2.0f &&
    a->y + a->h / 2.0f > b->y - b->h / 2.0f &&
    a->y - a->h / 2.0f < b->y + b->h / 2.0f;

	return overlap;
}


void LaunchMissile(GameState* state, float angle){
	state->missile_count++;
	Collider* new_list = realloc(state->missiles, state->missile_count * sizeof(Collider));
	LINCE_ASSERT_ALLOC(new_list, state->missile_count * sizeof(Collider));
	state->missiles = new_list;

	float vtot = state->missile_vmax;
	float vx = vtot * cosf((90.0f - angle) * (float)M_PI / 180.0f);
	float vy = vtot * sinf((90.0f - angle) * (float)M_PI / 180.0f);
	Collider new_missile = {
		.x = state->cannon_x,
		.y = state->cannon_y,
		.w = MISSILE_WIDTH, .h = MISSILE_HEIGHT,
		.vx = vx,
		.vy = vy,
		.angle = angle
	};
	memmove(&state->missiles[state->missile_count-1], &new_missile, sizeof(Collider));
}


void DeleteBomb(GameState* state, int index){
	size_t count = state->bomb_count;
	DeleteListItem((void**)&state->bombs, &count, sizeof(Collider), index);
	state->bomb_count = (int)count;
}

void DeleteMissile(GameState* state, int index){
	size_t count = state->missile_count;
	float x = state->missiles[index].x;
	float y = state->missiles[index].y;
	DeleteListItem((void**)&state->missiles, &count, sizeof(Collider), index);
	state->missile_count = (int)count;

	DeleteMarker(state, index);
	
	// search and delete bombs within radius
	float distance;
	for(int i=0; i!=state->bomb_count; ++i){
		Collider* bomb = &state->bombs[i];
		distance = FindDistance2D(x, y, bomb->x, bomb->y);
		if(distance < BLAST_RADIUS){
			CreateBlast(state, bomb->x, bomb->y);
			DeleteBomb(state, i);
			break;
		}
	}

	// create blast
	CreateBlast(state, x, y);

}


void UpdateMissiles(GameState* state){
	// displace
	for(int i=0; i!=state->missile_count; ++i){
		Collider* missile = &state->missiles[i];
		missile->x += missile->vx;
		missile->y += missile->vy;
	}

	// Missile cleanup
	int stray_missile = -1;
	for(int i=0; i!=state->missile_count; ++i){
		Collider* ms = &state->missiles[i];
		float marker_y = state->markers[i].y;
		// out of bounds missiles
		if (ms->x > state->xmax || ms->x < state->xmin ||
			ms->y > state->ymax || ms->y < state->ymin
		){
			stray_missile = i;
			break;
		}
		// reached marker
		else if (ms->y > marker_y){
			stray_missile = i;
			break;
		}
	}
	if(stray_missile > -1){
		DeleteMissile(state, stray_missile);
	}
}

void DrawMissiles(GameState* state){
	if(state->missile_count == 0) return;
	for(int i=0; i!=state->missile_count; ++i){
		Collider* missile = &state->missiles[i];
		LinceDrawQuad((LinceQuadProps){
			.x = missile->x,
			.y = missile->y,
			.w = missile->w,
			.h = missile->h,
			//.color = {1.0f, 0.0f, 0.0f, 1.0f},
			.color = {1.0f, 1.0f, 1.0f, 1.0f},
			.rotation = missile->angle,
			.texture = state->missile_tex
		});
	}
}


void DropBomb(GameState* state){
	// expand bomb list
	state->bomb_count++;
	state->bombs = realloc(state->bombs, state->bomb_count * sizeof(Collider));
	LINCE_ASSERT_ALLOC(state->bombs, state->bomb_count * sizeof(Collider));

	Collider* new_bomb = &state->bombs[state->bomb_count-1];
	new_bomb->x     = GetRandomFloat(-1.3f, 1.3f);
	new_bomb->y     = 1.0f;
	new_bomb->vx    = 0.0f;
	new_bomb->vy    = -5e-4f;
	new_bomb->w     = BOMB_WIDTH;
	new_bomb->h     = BOMB_HEIGHT;
	new_bomb->angle = 0.0f;
}

void UpdateBombs(GameState* state){
	// displace
	for(int i=0; i!=state->bomb_count; ++i){
		Collider* bomb = &state->bombs[i];
		bomb->x += bomb->vx;
		bomb->y += bomb->vy;
	}

	int dead_bomb = -1;
	for(int i=0; i!=state->bomb_count; ++i){
		Collider* b = &state->bombs[i];
		// find crashed bomb
		if (b->y <= state->ymin){
			dead_bomb = i;
			state->hp -= BOMB_HP_DAMAGE;
			break;
		}
	}
	if(dead_bomb > -1){
		CreateBlast(state, state->bombs[dead_bomb].x, state->bombs[dead_bomb].y);
		DeleteBomb(state, dead_bomb);
	}
}


void CheckBombIntercept(GameState* state){
	// find and delete intercepted bombs

	int dead_bomb = -1;
	int dead_missile = -1;

	for(int i=0; i!=state->bomb_count; ++i){
		Collider* b = &state->bombs[i];
		for(int j=0; j!=state->missile_count; ++j){
			Collider* m = &state->missiles[j];
			if(CollidersOverlap(b, m)){
				dead_bomb = i;
				dead_missile = j;
				state->score += 1;
				break;
			}
		}
		if (dead_bomb != -1) break;
	}

	if(dead_bomb != -1){
			DeleteBomb(state, dead_bomb);
			DeleteMissile(state, dead_missile);
	}
}

void DrawBombs(GameState* state){
	if(state->bomb_count == 0) return;
	for(int i=0; i!=state->bomb_count; ++i){
		Collider* bomb = &state->bombs[i];
		LinceDrawQuad((LinceQuadProps){
			.x = bomb->x,
			.y = bomb->y,
			.w = bomb->w,
			.h = bomb->h,
			//.color = {0.6f, 0.0f, 0.8f, 1.0f},
			.color = {1.0f, 1.0f, 1.0f, 1.0f},
			.rotation = 0.0f,
			.texture = state->bomb_tex
		});
	}
}

void MCommandOnAttach(LinceLayer* layer){

	GameState* data = LinceGetLayerData(layer);
	data->cam = LinceCreateCamera(LinceGetAspectRatio());
	data->score = 0;
	data->hp = 100;
		
	data->cannon_x = 0.0f;
	data->cannon_y = -1.0f;
	data->missile_cooldown = LinceFalse;
	data->missile_vmax = 7e-3f;
	data->ymin = -1.0f;
	data->ymax = 1.0f;
	data->xmin = -1.5f;
	data->xmax = 1.5f;
	data->dt = 0.0f;

	data->bombs = NULL;
	data->bomb_count = 0;
	data->bomb_tex = LinceCreateTexture("Bomb", "game/assets/textures/bomb.png");

	data->missiles = NULL;
	data->missile_count = 0;
	data->missile_tex = LinceCreateTexture("Missile", "game/assets/textures/missile.png");

	data->markers = NULL;
	data->marker_count = 0;
	data->marker_tex = LinceCreateTexture("Marker", "game/assets/textures/marker.png");

	data->bomb_cooldown_max = 3000.0f; // bomb every 3 seconds
	data->bomb_cooldown = data->bomb_cooldown_max;

	data->blast_count = 0;
	data->blasts = NULL;
	data->blast_tex = LinceCreateTexture("Blast", "game/assets/textures/pong_ball.png");

	data->bkg_city = LinceCreateTexture("City", "game/assets/textures/background-city.png");

	srand(time(NULL));
}

void MCommandOnUpdate(LinceLayer* layer, float dt){
	
	GameState* data = LinceGetLayerData(layer);
	LinceUILayer* ui = LinceGetAppState()->ui;
	const float width = (float)LinceGetAppState()->window->width;
	const float height = (float)LinceGetAppState()->window->height;
	data->dt = dt;

	// update view
	LinceResizeCameraView(data->cam, LinceGetAspectRatio());
	LinceUpdateCamera(data->cam);

	// handle missiles
	float angle = CalculateCannonAngle(data->cam);
	data->angle = angle;
	UpdateMissiles(data);

	// handle bombs
	data->bomb_cooldown -= dt;
	if(data->bomb_cooldown <= 0.0f){
		data->bomb_cooldown = data->bomb_cooldown_max;
		DropBomb(data);
	}
	UpdateBombs(data);
	CheckBombIntercept(data);
	UpdateBlasts(data);
	
	// draw UI
	LinceUIText(ui, "Angle",    40, 20,  LinceFont_Droid30, 20, "Angle: %.2f",  angle);
	LinceUIText(ui, "BombCool", 40, 40,  LinceFont_Droid30, 20, "Cooldown: %.2f",  data->bomb_cooldown);
	LinceUIText(ui, "HP",       40, 60,  LinceFont_Droid30, 20, "HP: %d",  data->hp);
	LinceUIText(ui, "Score",    40, 80,  LinceFont_Droid30, 20, "Score: %d",  data->score);

	LinceUIText(ui, "Missiles", 40, 120,  LinceFont_Droid30, 20, "Missiles: %d",  data->missile_count);
	LinceUIText(ui, "Bombs",    40, 140,  LinceFont_Droid30, 20, "Bombs: %d",  data->bomb_count);
	LinceUIText(ui, "Markers",  40, 160,  LinceFont_Droid30, 20, "Markers: %d",  data->marker_count);
	LinceUIText(ui, "Blasts",   40, 180,  LinceFont_Droid30, 20, "Blasts: %d",  data->blast_count);

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
		.rotation = angle
	});
	
	DrawMissiles(data);
	DrawBombs(data);
	DrawMarkers(data);
	DrawBlasts(data);
	LinceEndScene();
	LinceSetClearColor(0.0, 0.0, 0.0, 1.0);
}

void MCommandLayerOnEvent(LinceLayer* layer, LinceEvent* event){
	if(event->type != LinceEventType_MouseButtonPressed) return;
	GameState* state = LinceGetLayerData(layer);

	//float angle = CalculateCannonAngle(state->cam);
	LaunchMissile(state, state->angle);
	PlaceMarker(state);
}

void MCommandOnDetach(LinceLayer* layer){
	GameState* data = LinceGetLayerData(layer);

	if(data->missiles) free(data->missiles);
	if(data->bombs) free(data->bombs);
	if(data->markers) free(data->markers);
	if(data->blasts) free(data->blasts);

	LinceDeleteCamera(data->cam);
	LinceDeleteTexture(data->missile_tex);
	LinceDeleteTexture(data->bomb_tex);
	LinceDeleteTexture(data->bkg_city);
	LinceDeleteTexture(data->marker_tex);
	LinceDeleteTexture(data->blast_tex);
	LinceDeleteTexture(data->bkg_city);
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