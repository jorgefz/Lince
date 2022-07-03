#include "pong.h"

typedef struct PongLayer{
	LinceCamera* cam;
	LinceTexture *ball_tex, *pad_tex;
} PongLayer;

enum Collision {
	NO_COLLISION,
	LEFT_COLLISION,
	RIGHT_COLLISION
};

/* Indicates whether ball just collided with left or right bounds */
static enum Collision ball_state = NO_COLLISION;

/* Indicate whether game is paused or not */
static LinceBool paused = LinceFalse, new_game = LinceTrue;

/* Points won by the players */
static int lscore = 0, rscore = 0;

static PongObject ball = {
	.w = 0.1f, .h = 0.1f,
	.x = 0.0f, .y = 0.0f,
	.vx = 0.0f, .vy = 0.0f
};

static PongObject lpad = {
	.w = 0.2f, .h = 0.5f,
	.x = -1.0f, .y = 0.0f,
	.vx = 0.0f, .vy = 0.0f
};

static PongObject rpad = {
	.w = 0.2f, .h = 0.5f,
	.x = 1.0f, .y = 0.0f,
	.vx = 0.0f, .vy = 0.0f
};

/* Displaces paddle by 'dy' distance only if it's within bounds (ymin,ymax) */
void MovePaddle(PongObject* pad, float dy, float ymin, float ymax){
	enum {UP, DOWN};
	int dir = (dy < 0.0f) ? DOWN : UP;

	if (dir == DOWN){
		if (pad->y - pad->h/2.0f + dy > ymin) pad->y += dy;
	}
	else if (dir == UP){
		if (pad->y + pad->h/2.0f + dy < ymax) pad->y += dy;
	}
}

void MoveBall(PongObject* ball, float dt, float xmin, float xmax, float ymin, float ymax){
	// x direction
	enum {LEFT, RIGHT};
	float dx = ball->vx * dt;
	int xdir = (dx < 0.0f) ? LEFT : RIGHT;

	if (xdir == LEFT) {
		if (ball->x - ball->w/2.0f + dx < xmin){
			ball->vx *= -1.0f;
			ball_state = LEFT_COLLISION;
		}
		ball->x += dx;
	}
	else if (xdir == RIGHT) {
		if (ball->x + ball->w/2.0f + dx > xmax){
			ball->vx *= -1.0f;
			ball_state = RIGHT_COLLISION;
		}
		ball->x += dx;
	}

	// y direction
	enum {UP, DOWN};
	float dy = ball->vy * dt;
	int ydir = (dy < 0.0f) ? DOWN : UP;

	if (ydir == DOWN) {
		if (ball->y - ball->h/2.0f + dy < ymin) ball->vy *= -1.0f;
		ball->y += dy;
	}
	else if (ydir == UP) {
		if (ball->y + ball->h/2.0f + dy > ymax) ball->vy *= -1.0f;
		ball->y += dy;
	}

}

void ResetGame(){
	ball.x = 0.0f;
	ball.y = 0.0f;
	new_game = LinceTrue;
}

void CheckPaddleCollision(){
	int contact;
	switch (ball_state) {
	case LEFT_COLLISION:
		contact = (ball.y-ball.h/2.0f <= lpad.y+lpad.h/2.0f) &&
			(ball.y+ball.h/2.0f >= lpad.y-lpad.h/2.0f);
		if(!contact){
			rscore += 1;
			ResetGame();
		}
		ball_state = NO_COLLISION;
		break;

	case RIGHT_COLLISION:
		contact = (ball.y-ball.h/2.0f <= rpad.y+rpad.h/2.0f) &&
			(ball.y+ball.h/2.0f >= rpad.y-rpad.h/2.0f);
		if(!contact){
			lscore += 1;
			ResetGame();
		}
		ball_state = NO_COLLISION;
		break;
	
	case NO_COLLISION:
		break;
	}
}

LinceLayer* PongLayerInit(){
	LinceLayer* layer = LinceCreateLayer(NULL);

	layer->OnAttach = PongLayerOnAttach;
	layer->OnUpdate = PongLayerOnUpdate;
	layer->OnEvent = PongLayerOnEvent;
	layer->OnDetach = PongLayerOnDetach;
	layer->data = calloc(1, sizeof(PongLayer));
	LINCE_ASSERT_ALLOC(layer->data, sizeof(PongLayer));

	return layer;
}

void PongLayerOnAttach(LinceLayer* layer){
	PongLayer* data = LinceGetLayerData(layer);
	data->cam = LinceCreateCamera(LinceGetAspectRatio());
	data->ball_tex = LinceCreateTexture("PongBall", "game/assets/textures/pong_ball.png");
	data->pad_tex  = LinceCreateTexture("PongBall", "game/assets/textures/pong_pad.png");

	ball.vx = 1e-3f;
	ball.vy = 2e-3f;
}

void PongLayerOnUpdate(LinceLayer* layer, float dt){
	PongLayer* data = LinceGetLayerData(layer);
	LinceUILayer* ui = LinceGetAppState()->ui;
	const float width = (float)LinceGetAppState()->window->width;
	const float height = (float)LinceGetAppState()->window->height;

	// update view
	LinceResizeCameraView(data->cam, LinceGetAspectRatio());
	LinceUpdateCamera(data->cam);

	// update pause state
	if(new_game && LinceIsKeyPressed(LinceKey_Space)) new_game = LinceFalse;
	if (!new_game && LinceIsKeyPressed(LinceKey_p)) paused = LinceTrue;
	if (!new_game && paused && LinceIsKeyPressed(LinceKey_Space)) paused = LinceFalse;

	if (new_game){
		LinceUIText(ui, "NewGame",  (float)width/2.0f-170, (float)height/2.0f+60, LinceFont_Droid30, 50, " PRESS SPACE TO START ");
	}
	if (paused){
		LinceUIText(ui, "Paused",  (float)width/2.0f-160, (float)height/2.0f+60, LinceFont_Droid30, 50, " PRESS SPACE TO RESUME ");
	}

	if (!new_game && !paused){
		// update paddle movement
		static const float pad_speed = 2e-3f;
		// -- left paddle
		if (LinceIsKeyPressed(LinceKey_w)) MovePaddle(&lpad, pad_speed*dt, -1.0, 1.0); 
		if (LinceIsKeyPressed(LinceKey_s)) MovePaddle(&lpad, -pad_speed*dt, -1.0, 1.0);
		// -- right paddle
		if (LinceIsKeyPressed(LinceKey_Up))   MovePaddle(&rpad, pad_speed*dt, -1.0, 1.0); 
		if (LinceIsKeyPressed(LinceKey_Down)) MovePaddle(&rpad, -pad_speed*dt, -1.0, 1.0); 

		// update ball movement
		MoveBall(&ball, dt, lpad.x + lpad.w/2.0f, rpad.x - rpad.w/2.0f, -1.0, 1.0);
		CheckPaddleCollision();
	}

	// print debug FPS
	LinceUIText(ui, "LScore",  60       , 20, LinceFont_Droid50, 10, "  %d  ",  lscore);
	LinceUIText(ui, "RScore",  width-130, 20, LinceFont_Droid50, 10, "  %d  ",  rscore);
    LinceUIText(ui, "FPS_text", 10, height - 70, LinceFont_Droid30, 10, "FPS %.0f", 1000.0/dt);
    LinceUIText(ui, "dt_text",  10, height - 40, LinceFont_Droid30, 10, "%.2f ms",  dt);

	// draw paddles and ball
	LinceBeginScene(data->cam);

	LinceDrawQuad((LinceQuadProps){
		.x = ball.x, .y = ball.y,
		.w = ball.w, .h = ball.h,
		.color = {0.0, 1.0, 0.0, 1.0},
		.texture = data->ball_tex
	});

	LinceDrawQuad((LinceQuadProps){
		.x = lpad.x, .y = lpad.y,
		.w = lpad.w, .h = lpad.h,
		.color = {0.0, 0.0, 1.0, 1.0},
		.texture = data->pad_tex
	});

	LinceDrawQuad((LinceQuadProps){
		.x = rpad.x, .y = rpad.y,
		.w = rpad.w, .h = rpad.h,
		.color = {1.0, 0.0, 0.0, 1.0},
		.texture = data->pad_tex
	});

	LinceEndScene();
}

void PongLayerOnEvent(LinceLayer* layer, LinceEvent* event){
	LINCE_UNUSED(layer);
	LINCE_UNUSED(event);
}

void PongLayerOnDetach(LinceLayer* layer){
	PongLayer* data = LinceGetLayerData(layer);
	LinceDeleteTexture(data->ball_tex);
	LinceDeleteTexture(data->pad_tex);
	LinceDeleteCamera(data->cam);
	free(data);
}