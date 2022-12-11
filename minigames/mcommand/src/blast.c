
#include <lince.h>
#include <lince/containers/array.h>
#include "blast.h"


void CreateBlast(array_t* blast_list, vec2 position, LinceTexture* texture){
	Blast b = {
		.timer = {
			.start = BLAST_LIFETIME_MS,
			.tick = (-1.0f) * BLAST_LIFELOSS,
			.end = 0.0f,
		},
		.sprite = (LinceQuadProps){
			.x = position[0],
			.y = position[1],
			.w = BLAST_RADIUS*2.0f,
			.h = BLAST_RADIUS*2.0f,
			.zorder = 0.5f,
			.color = {1.0f, 1.0f, 0.0f, 1.0f},
			.texture = texture
		}
	};
	ResetTimer(&b.timer);
	array_push_back(blast_list, &b);
}

void DeleteBlast(array_t* blast_list, int index){
	array_remove(blast_list, index);
}

void UpdateBlasts(array_t* blast_list, float dt){
	Blast* b;
	for(uint32_t i=0; i!=blast_list->size; ++i){
		b = array_get(blast_list, i);
		if( UpdateTimer(&b->timer, dt) ) continue;
		b->sprite.color[1] -= BLAST_LIFELOSS/BLAST_LIFETIME_MS * dt; // make redder
		b->sprite.color[3] -= BLAST_LIFELOSS/BLAST_LIFETIME_MS * dt;
		// update alpha, color, etc
	}

	// Delete one old one
	for(uint32_t i=0; i!=blast_list->size; ++i){
		b = array_get(blast_list, i);
		if (b->timer.finished){
			DeleteBlast(blast_list, i);
			break;
		}
	}	
}

void DrawBlasts(array_t* blast_list){
	Blast* b;
	for(uint32_t i=0; i!=blast_list->size; ++i){
		b = array_get(blast_list, i);
		LinceDrawQuad(b->sprite);
	}
}

