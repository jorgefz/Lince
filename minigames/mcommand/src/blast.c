
#include <lince.h>
#include <lince/containers/array.h>
#include "blast.h"


void CreateBlast(array_t* blast_list, vec2 position, LinceTexture* texture){
	Timer timer = {
		.start = BLAST_LIFETIME_MS,
		.tick = -BLAST_LIFELOSS,
		.end = 0.0f
	};
	ResetTimer(&timer);
	Sprite sprite = {
		.x = position[0],
		.y = position[1],
		.w = BLAST_RADIUS * 2.0f,
		.h = BLAST_RADIUS * 2.0f,
		.zorder = 0.5f,
		.color = {1.0f, 1.0f, 0.0f, 1.0f},
		.texture = texture
	};
	GameObject entity = {
		.timer = LinceNewCopy(&timer, sizeof(Timer)),
		.sprite = LinceNewCopy(&sprite, sizeof(Sprite)),
		.collider = NULL
	};
	array_push_back(blast_list, &entity);
}


void UpdateBlasts(array_t* blast_list, float dt){
	GameObject* b;
	for(uint32_t i=0; i!=blast_list->size; ++i){
		b = array_get(blast_list, i);
		if( UpdateTimer(b->timer, dt) ) continue;
		b->sprite->color[1] -= BLAST_LIFELOSS / BLAST_LIFETIME_MS * dt; // make redder
		b->sprite->color[3] -= BLAST_LIFELOSS / BLAST_LIFETIME_MS * dt;
	}

	// Delete one old one
	for(uint32_t i=0; i!=blast_list->size; ++i){
		b = array_get(blast_list, i);
		if (b->timer->finished){
			DeleteEntityItem(blast_list, i);
			break;
		}
	}	
}


