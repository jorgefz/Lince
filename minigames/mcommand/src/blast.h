#ifndef MCOMMAND_BLAST_H
#define MCOMMAND_BLAST_H

#include <lince.h>

#define BLAST_RADIUS 0.12f
#define BLAST_LIFETIME_MS 1000.0f
#define BLAST_LIFELOSS 0.5f
#define BLAST_COLOR_INIT {1.0f, 1.0f, 0.0, 1.0f}

typedef struct Blast {
	float x, y;      // position
	float color[4];  // color of the blast
	float lifetime;  // time left
	float life_loss; // loss rate of lifetime
} Blast;

void CreateBlast(array_t* blast_list, float x, float y);

void UpdateBlasts(array_t* blast_list, float dt);

void DrawBlasts(array_t* blast_list, LinceTexture* texture);

void DeleteBlast(array_t* blast_list, int index);


#endif /* MCOMMAND_BLAST_H */