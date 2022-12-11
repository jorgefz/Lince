
#include <lince.h>
#include <lince/containers/array.h>
#include "blast.h"


void CreateBlast(array_t* blast_list, float x, float y){
	array_push_back(blast_list, NULL);
	Blast *b = array_back(blast_list);
	
	b->x = x;
	b->y = y;
	b->color[0] = 1.0f; b->color[1] = 1.0f;
	b->color[2] = 0.0f; b->color[3] = 1.0f;
	b->lifetime = BLAST_LIFETIME_MS;
	b->life_loss = BLAST_LIFELOSS;
}

void DeleteBlast(array_t* blast_list, int index){
	array_remove(blast_list, index);
}

void UpdateBlasts(array_t* blast_list, float dt){
	Blast* b;
	for(uint32_t i=0; i!=blast_list->size; ++i){
		b = array_get(blast_list, i);
		if(b->lifetime <= 0.0f) continue;
		b->lifetime -= b->life_loss * dt;
		b->color[1] -= BLAST_LIFELOSS/BLAST_LIFETIME_MS * dt; // make redder
		b->color[3] -= BLAST_LIFELOSS/BLAST_LIFETIME_MS * dt;
		// update alpha, color, etc
	}

	// Delete one old one
	for(uint32_t i=0; i!=blast_list->size; ++i){
		b = array_get(blast_list, i);
		if (b->lifetime <= 0.0f){
			DeleteBlast(blast_list, i);
			break;
		}
	}	
}

void DrawBlasts(array_t* blast_list, LinceTexture* texture){
	Blast* b;
	for(uint32_t i=0; i!=blast_list->size; ++i){
		b = array_get(blast_list, i);
		LinceDrawQuad((LinceQuadProps){
			.x = b->x,
			.y = b->y,
			.w = BLAST_RADIUS*2.0f,
			.h = BLAST_RADIUS*2.0f,
			.color = {b->color[0], b->color[1], b->color[2], b->color[3]},
			.zorder = 0.5f,
			.texture = texture
		});
	}
}

