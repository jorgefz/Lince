#include "pong.h"

#include <miniaudio.h>


typedef struct GameObject {
	float w, h;   // size
	float x, y;   // position
	float vx, vy; // speed
} GameObject;

enum Collision {
	NO_COLLISION = 0,
	LEFT_COLLISION,
	RIGHT_COLLISION
};

typedef struct GameLayer{
	LinceCamera* cam; // camera
	LinceTexture *ball_tex, *pad_tex; // Textures
	GameObject ball, lpad, rpad;
	float pad_speed;

	/* Indicates whether ball just collided with left or right bounds */
	enum Collision ball_state;

	/* Points scored by the players */
	int lscore, rscore;

	/* Indicate whether game is paused or not */
	LinceBool paused, new_game;

	ma_engine audio_engine;

} GameLayer;


void PlayBallBounceSound(GameLayer* data){
	ma_result res = ma_engine_play_sound(&data->audio_engine,
		"minigames/pong/assets/ball-single-bounce-concrete.wav",
		NULL
	);
	if(res != MA_SUCCESS){
		printf("Failed to play sound\n");
	}
}

/* Displaces paddle by 'dy' distance only if it's within bounds (ymin,ymax) */
void MovePaddle(GameObject* pad, float dy, float ymin, float ymax){
	enum {UP, DOWN};
	int dir = (dy < 0.0f) ? DOWN : UP;

	if (dir == DOWN){
		if (pad->y - pad->h/2.0f + dy > ymin){
			pad->y += dy;
		}
	}
	else if (dir == UP){
		if (pad->y + pad->h/2.0f + dy < ymax){
			pad->y += dy;
		}
	}
}

void MoveBall(GameObject* ball, float dt, float xmin, float xmax, float ymin, float ymax){
	GameLayer* data = LinceGetLayerData(LinceGetCurrentLayer());
	
	// x direction
	enum {LEFT, RIGHT};
	float dx = ball->vx * dt;
	int xdir = (dx < 0.0f) ? LEFT : RIGHT;

	if (xdir == LEFT) {
		if (ball->x - ball->w/2.0f + dx < xmin){
			ball->vx *= -1.0f;
			data->ball_state = LEFT_COLLISION;
			PlayBallBounceSound(data);
		}
		ball->x += dx;
	}
	else if (xdir == RIGHT) {
		if (ball->x + ball->w/2.0f + dx > xmax){
			ball->vx *= -1.0f;
			data->ball_state = RIGHT_COLLISION;
			PlayBallBounceSound(data);
		}
		ball->x += dx;
	}

	// y direction
	enum {UP, DOWN};
	float dy = ball->vy * dt;
	int ydir = (dy < 0.0f) ? DOWN : UP;

	if (ydir == DOWN) {
		if (ball->y - ball->h/2.0f + dy < ymin){
			ball->vy *= -1.0f;
			PlayBallBounceSound(data);
		}
		ball->y += dy;
	}
	else if (ydir == UP) {
		if (ball->y + ball->h/2.0f + dy > ymax){
			ball->vy *= -1.0f;w
			PlayBallBounceSound(data);
		}
		ball->y += dy;
	}

}

void ResetGame(){
	GameLayer* data = LinceGetLayerData(LinceGetCurrentLayer());
	data->ball.x = 0.0f;
	data->ball.y = 0.0f;
	data->new_game = LinceTrue;
}

void CheckPaddleCollision(){
	GameLayer* data = LinceGetLayerData(LinceGetCurrentLayer());
	int contact;

	switch (data->ball_state) {
	case LEFT_COLLISION:
		contact =
			(data->ball.y - data->ball.h / 2.0f <= data->lpad.y + data->lpad.h / 2.0f) &&
			(data->ball.y + data->ball.h / 2.0f >= data->lpad.y - data->lpad.h / 2.0f);
		if(!contact){
			data->rscore += 1;
			ResetGame();
		}
		data->ball_state = NO_COLLISION;
		break;

	case RIGHT_COLLISION:
		contact =
			(data->ball.y - data->ball.h / 2.0f <= data->rpad.y + data->rpad.h / 2.0f) &&
			(data->ball.y + data->ball.h / 2.0f >= data->rpad.y - data->rpad.h / 2.0f);
		if(!contact){
			data->lscore += 1;
			ResetGame();
		}
		data->ball_state = NO_COLLISION;
		break;
	
	case NO_COLLISION:
		break;
	}
}

LinceLayer* GameLayerInit(){
	LinceLayer* layer = LinceCreateLayer(NULL);

	layer->OnAttach = GameLayerOnAttach;
	layer->OnUpdate = GameLayerOnUpdate;
	layer->OnEvent = GameLayerOnEvent;
	layer->OnDetach = GameLayerOnDetach;
	layer->data = calloc(1, sizeof(GameLayer));
	LINCE_ASSERT_ALLOC(layer->data, sizeof(GameLayer));

	GameLayer* data = layer->data;

	ma_engine_init(NULL, &data->audio_engine);

	data->ball_state = NO_COLLISION;
	data->lscore = 0;
	data->rscore = 0;
	data->paused = LinceFalse;
	data->new_game = LinceTrue;
	data->pad_speed = 2e-3f;

	static GameObject ball = {
		.w = 0.1f, .h = 0.1f,
		.x = 0.0f, .y = 0.0f,
		.vx = 0.0f, .vy = 0.0f
	};
	static GameObject lpad = {
		.w = 0.2f, .h = 0.5f,
		.x = -1.4f, .y = 0.0f,
		.vx = 0.0f, .vy = 0.0f
	};
	static GameObject rpad = {
		.w = 0.2f, .h = 0.5f,
		.x = 1.4f, .y = 0.0f,
		.vx = 0.0f, .vy = 0.0f
	};

	memmove(&data->ball, &ball, sizeof(GameObject));
	memmove(&data->rpad, &rpad, sizeof(GameObject));
	memmove(&data->lpad, &lpad, sizeof(GameObject));

	return layer;
}

void GameLayerOnAttach(LinceLayer* layer){
	GameLayer* data = LinceGetLayerData(layer);
	data->cam = LinceCreateCamera(LinceGetAspectRatio());
	data->ball_tex = LinceCreateTexture("PongBall", "minigames/pong/assets/pong_ball.png");
	data->pad_tex  = LinceCreateTexture("PongBall", "minigames/pong/assets/pong_pad.png");

	data->ball.vx = 1e-3f; // randomize initial speed and direction
	data->ball.vy = 2e-3f;
}

void GameLayerOnUpdate(LinceLayer* layer, float dt){
	GameLayer* data = LinceGetLayerData(layer);
	LinceUILayer* ui = LinceGetAppState()->ui;
	const float width = (float)LinceGetAppState()->window->width;
	const float height = (float)LinceGetAppState()->window->height;

	// update view
	LinceResizeCameraView(data->cam, LinceGetAspectRatio());
	LinceUpdateCamera(data->cam);

	// update pause state
	if(data->new_game){
		LinceUIText(ui, "NewGame", 
			(float)width/2.0f-170,
			(float)height/2.0f+60,
			LinceFont_Droid30, 50,
			" PRESS SPACE TO START "
		);
		if(LinceIsKeyPressed(LinceKey_Space)){
			data->new_game = LinceFalse;
		}
	} else if (data->paused) {
		LinceUIText(ui, "Paused",
			(float)width/2.0f-160,
			(float)height/2.0f+60,
			LinceFont_Droid30, 50,
			" PRESS SPACE TO RESUME "
		);
		if(LinceIsKeyPressed(LinceKey_Space)){
			data->paused = LinceFalse;
		}
	} else {
		if(LinceIsKeyPressed(LinceKey_p)){
			data->paused = LinceTrue;
		}
	}

	if (!data->new_game && !data->paused){
		// update paddle movement
		// static const float pad_speed = 2e-3f; //  MOVE TO
		const float pad_speed = data->pad_speed;
		// -- left paddle
		if (LinceIsKeyPressed(LinceKey_w)) MovePaddle(&data->lpad, pad_speed*dt, -1.0, 1.0); 
		if (LinceIsKeyPressed(LinceKey_s)) MovePaddle(&data->lpad, -pad_speed*dt, -1.0, 1.0);
		// -- right paddle
		if (LinceIsKeyPressed(LinceKey_Up))   MovePaddle(&data->rpad, pad_speed*dt, -1.0, 1.0); 
		if (LinceIsKeyPressed(LinceKey_Down)) MovePaddle(&data->rpad, -pad_speed*dt, -1.0, 1.0); 

		// update ball movement
		MoveBall(&data->ball, dt, data->lpad.x + data->lpad.w/2.0f, data->rpad.x - data->rpad.w/2.0f, -1.0, 1.0);
		CheckPaddleCollision();
	}

	// print debug FPS
	LinceUIText(ui, "LScore",  60       , 20, LinceFont_Droid50, 10, "  %d  ",  data->lscore);
	LinceUIText(ui, "RScore",  width-130, 20, LinceFont_Droid50, 10, "  %d  ",  data->rscore);
    LinceUIText(ui, "FPS_text", 10, height - 70, LinceFont_Droid30, 10, "FPS %.0f", 1000.0/dt);
    LinceUIText(ui, "dt_text",  10, height - 40, LinceFont_Droid30, 10, "%.2f ms",  dt);

	// draw paddles and ball
	LinceBeginScene(data->cam);

	LinceDrawQuad((LinceQuadProps){
		.x = data->ball.x, .y = data->ball.y,
		.w = data->ball.w, .h = data->ball.h,
		.color = {0.0, 1.0, 0.0, 1.0},
		.texture = data->ball_tex
	});

	LinceDrawQuad((LinceQuadProps){
		.x = data->lpad.x, .y = data->lpad.y,
		.w = data->lpad.w, .h = data->lpad.h,
		.color = {0.0, 0.0, 1.0, 1.0},
		.texture = data->pad_tex
	});

	LinceDrawQuad((LinceQuadProps){
		.x = data->rpad.x, .y = data->rpad.y,
		.w = data->rpad.w, .h = data->rpad.h,
		.color = {1.0, 0.0, 0.0, 1.0},
		.texture = data->pad_tex
	});

	LinceEndScene();
}

void GameLayerOnEvent(LinceLayer* layer, LinceEvent* event){
	LINCE_UNUSED(layer);
	LINCE_UNUSED(event);
}

void GameLayerOnDetach(LinceLayer* layer){
	GameLayer* data = LinceGetLayerData(layer);
	LinceDeleteTexture(data->ball_tex);
	LinceDeleteTexture(data->pad_tex);
	LinceDeleteCamera(data->cam);
	ma_engine_uninit(&data->audio_engine);
	free(data);
}