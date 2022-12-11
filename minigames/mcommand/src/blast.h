#ifndef MCOMMAND_BLAST_H
#define MCOMMAND_BLAST_H

#include <lince.h>
#include <cglm/affine.h>
#include "timer.h"
#include "gameobject.h"

#define BLAST_RADIUS 0.12f
#define BLAST_LIFETIME_MS 1000.0f
#define BLAST_LIFELOSS 0.5f


void CreateBlast(array_t* blast_list, vec2 position, LinceTexture* texture);

void UpdateBlasts(array_t* blast_list, float dt);


#endif /* MCOMMAND_BLAST_H */