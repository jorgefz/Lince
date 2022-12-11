#ifndef MCOMMAND_MARKER_H
#define MCOMMAND_MARKER_H

#include <lince.h>
#include <cglm/affine.h>
#include <lince/containers/array.h>
#include "gameobject.h"

typedef struct Marker {
	float x, y;
    LinceQuadProps sprite;
} Marker;

void PlaceMarker(array_t* marker_list, vec2 position, LinceTexture* texture);


#endif /* MCOMMAND_MARKER_H */