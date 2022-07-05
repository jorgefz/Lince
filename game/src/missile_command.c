#include "missile_command.h"
#include "math.h"
#include "time.h"


/*

	- Add on click events for shooting
	- Click sets target
	- Missile explodes when reaching target
	- When missile explodes, bombs in a radius will explode.
	- Loose / win condition
	- Fix angles
	- Fix visual bug where quads persist after death

*/

#define BOMB_HP_DAMAGE 10

typedef struct Collider {
	float x, y, vx, vy, w, h, angle;
} Collider;

typedef struct GameState{
	Collider *missiles;
	Collider *bombs;
	int bomb_count, missile_count;
	int score, hp;
	float cannon_x, cannon_y;
	float missile_vmax;
	float ymin, ymax;
	float xmin, xmax;

	LinceBool missile_cooldown;
	float bomb_cooldown, bomb_cooldown_max;

	LinceCamera* cam;
} GameState;


float GetRandomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

// Check if two non-rotatde rectangles overlap
LinceBool CollidersOverlap(Collider* a, Collider* b){
	//float aright = a->x + a->w/2.0f;
	//float aleft  = a->x - a->w/2.0f;
	//float atop   = a->y + a->h/2.0f;
	//float abot   = a->y - a->h/2.0f;
	//float bright = b->x + b->w/2.0f;
	//float bleft  = b->x - b->w/2.0f;
	//float btop   = b->y + b->h/2.0f;
	//float bbot   = b->y - b->h/2.0f;
	//LinceBool ax_overlap = (aright <= bright && aright >= bleft) && (aleft <= bright && aright >= bleft);
	//LinceBool ay_overlap = (atop <= btop && atop >= bbot) && (abot <= bright && aright >= bleft);

	LinceBool overlap =
	a->x + a->w / 2.0f > b->x - b->w / 2.0f &&
    a->x - a->w / 2.0f < b->x + b->w / 2.0f &&
    a->y + a->h / 2.0f > b->y - b->h / 2.0f &&
    a->y - a->h / 2.0f < b->y + b->h / 2.0f;

	return overlap;
}

// Calculates the angle between the Y axis (centered on the screen)
// and the line connecting the cannon and the mouse pointer.
float CalculateCannonAngle(){
	const float width = (float)LinceGetAppState()->window->width;
	const float height = (float)LinceGetAppState()->window->height;
	float mouse_x, mouse_y;
	const float cannon_x = 0.0f, cannon_y = -1.0f;
	LinceGetMousePos(&mouse_x, &mouse_y);
	mouse_x = mouse_x/width - 0.5f;
	mouse_y = 1.0f - mouse_y/height;
	return atan2f(mouse_x, mouse_y) * 180.0f / M_PI;
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
		.w = 0.02f, .h = 0.05f,
		.vx = vx,
		.vy = vy,
		.angle = angle
	};
	memcpy(&state->missiles[state->missile_count-1], &new_missile, sizeof(Collider));
}

void DeleteMissile(GameState* state, int index){
	if (state->missile_count == 0 || index < 0) return;

	if (state->missile_count == 1){
		state->missile_count = 0;
		free(state->missiles);
		state->missiles = NULL;
		return;
	}

	for(int i = index; i != state->missile_count-1; ++i){
		memcpy(&state->missiles[i], &state->missiles[i+1], sizeof(Collider));
	}
	state->missile_count--;
	Collider* new_list = realloc(state->missiles, state->missile_count * sizeof(Collider));
	LINCE_ASSERT_ALLOC(new_list, state->missile_count * sizeof(Collider));
}

void DeleteBomb(GameState* state, int index){
	if (state->bomb_count == 0 || index < 0) return;

	if (state->bomb_count == 1){
		state->bomb_count = 0;
		free(state->bombs);
		state->bombs = NULL;
		return;
	}

	for(int i = index; i != state->bomb_count-1; ++i){
		memcpy(&state->bombs[i], &state->bombs[i+1], sizeof(Collider));
	}
	state->bomb_count--;
	Collider* new_list = realloc(state->bombs, state->bomb_count * sizeof(Collider));
	LINCE_ASSERT_ALLOC(new_list, state->bomb_count * sizeof(Collider));
}

void UpdateMissiles(GameState* state){
	// displace
	for(int i=0; i!=state->missile_count; ++i){
		Collider* missile = &state->missiles[i];
		missile->x += missile->vx;
		missile->y += missile->vy;
	}

	// find and delete out-of-bounds missiles
	LinceBool cleanup = LinceTrue;
	while(cleanup){
		int stray_missile = -1;
		// find out of bounds missile
		for(int i=0; i!=state->missile_count; ++i){
			Collider* ms = &state->missiles[i];
			if (ms->x > state->xmax || ms->x < state->xmin ||
				ms->y > state->ymax || ms->y < state->ymin
			){
				stray_missile = i;
				break;
			}
		}
		if(stray_missile == -1){
			// cleanup done - no more stray missiles
			cleanup = LinceFalse;
			break;
		}
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
			.color = {1.0f, 0.0f, 0.0f, 1.0f},
			.rotation = missile->angle
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
	new_bomb->w     = 0.05f;
	new_bomb->h     = 0.05f;
	new_bomb->angle = 0.0f;
}

void UpdateBombs(GameState* state){
	// displace
	for(int i=0; i!=state->bomb_count; ++i){
		Collider* bomb = &state->bombs[i];
		bomb->x += bomb->vx;
		bomb->y += bomb->vy;
	}

	// find and delete crashed bombs
	LinceBool cleanup = LinceTrue;
	while(cleanup){
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
		if(dead_bomb == -1){
			// cleanup done - no more stray missiles
			cleanup = LinceFalse;
			break;
		}
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
				printf("COLLISION!\n");
				break;
			}
		}
		if (dead_bomb != -1) break;
	}

	if(dead_bomb != -1){
			DeleteBomb(state, dead_bomb);
			DeleteMissile(state, dead_missile);
	}

	return;

	LinceBool cleanup = LinceTrue;
	while(cleanup){
		int dead_bomb = -1;

		for(int i=0; i!=state->bomb_count; ++i){
			Collider* b = &state->bombs[i];
			for(int j=0; j!=state->missile_count; ++i){
				Collider* m = &state->missiles[j];
				if(CollidersOverlap(b, m)){
					dead_bomb = i;
					state->score += 1;
					printf("COLLISION!\n");
					break;
				}
			}
			if (dead_bomb > -1) break;
		}

		if(dead_bomb == -1){
			// cleanup done - no more intercepted bombs
			cleanup = LinceFalse;
			break;
		}
		DeleteBomb(state, dead_bomb);
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
			.color = {0.6f, 0.0f, 0.8f, 1.0f},
			.rotation = 0.0f
		});
	}
}

void MCommandOnAttach(LinceLayer* layer){

	GameState* data = LinceGetLayerData(layer);
	data->cam = LinceCreateCamera(LinceGetAspectRatio());
	data->score = 0;
	data->hp = 100;
	data->bomb_count = 0;
	data->missile_count = 0;
	data->bombs = NULL;
	data->missiles = NULL;
	data->cannon_x = 0.0f;
	data->cannon_y = -1.0f;
	data->missile_cooldown = LinceFalse;
	data->missile_vmax = 7e-3f;
	data->ymin = -1.0f;
	data->ymax = 1.0f;
	data->xmin = -1.5f;
	data->xmax = 1.5f;

	data->bomb_cooldown_max = 3000.0f; // bomb every 3 seconds
	data->bomb_cooldown = data->bomb_cooldown_max;

	srand(time(NULL));
}

void MCommandOnUpdate(LinceLayer* layer, float dt){
	
	GameState* data = LinceGetLayerData(layer);
	LinceUILayer* ui = LinceGetAppState()->ui;
	const float width = (float)LinceGetAppState()->window->width;
	const float height = (float)LinceGetAppState()->window->height;

	// update view
	LinceResizeCameraView(data->cam, LinceGetAspectRatio());
	LinceUpdateCamera(data->cam);

	// handle missiles
	float angle = CalculateCannonAngle();
	UpdateMissiles(data);
	if(LinceIsKeyPressed(LinceKey_Space)){
		if (!data->missile_cooldown){
			LaunchMissile(data, angle);
			data->missile_cooldown = LinceTrue;
		}
	} else {
		data->missile_cooldown = LinceFalse;
	}

	// handle bombs
	data->bomb_cooldown -= dt;
	if(data->bomb_cooldown <= 0.0f){
		data->bomb_cooldown = data->bomb_cooldown_max;
		DropBomb(data);
	}
	UpdateBombs(data);
	CheckBombIntercept(data);
	
	// draw UI
	LinceUIText(ui, "Angle",    40, 20,  LinceFont_Droid30, 20, "Angle: %.2f",  90.0f - angle);
	LinceUIText(ui, "Missiles", 40, 40,  LinceFont_Droid30, 20, "Missiles: %d",  data->missile_count);
	LinceUIText(ui, "Bombs",    40, 60,  LinceFont_Droid30, 20, "Bombs: %d",  data->bomb_count);
	LinceUIText(ui, "BombCool", 40, 80,  LinceFont_Droid30, 20, "Cooldown: %.2f",  data->bomb_cooldown);
	LinceUIText(ui, "HP",       40, 100, LinceFont_Droid30, 20, "HP: %d",  data->hp);
	LinceUIText(ui, "Score",    40, 120, LinceFont_Droid30, 20, "Score: %d",  data->score);

	// draw objects
	LinceBeginScene(data->cam);
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
	LinceEndScene();
	LinceSetClearColor(0.7, 0.8, 0.9, 1.0);
}

void MCommandOnDetach(LinceLayer* layer){
	GameState* data = LinceGetLayerData(layer);
	LinceDeleteCamera(data->cam);
	free(data);
}

LinceLayer* MCommandLayerInit(){
	LinceLayer* layer = LinceCreateLayer(NULL);

	layer->OnAttach = MCommandOnAttach;
	layer->OnUpdate = MCommandOnUpdate;
	//layer->OnEvent = MCommandLayerOnEvent;
	layer->OnDetach = MCommandOnDetach;
	layer->data = calloc(1, sizeof(GameState));
	LINCE_ASSERT_ALLOC(layer->data, sizeof(GameState));

	return layer;
}