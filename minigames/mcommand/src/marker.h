#ifndef MCOMMAND_MARKER_H
#define MCOMMAND_MARKER_H

#include <lince.h>
#include <cglm/affine.h>
#include <lince/containers/array.h>

typedef struct Marker {
	float x, y;
    LinceQuadProps sprite;
} Marker;

void PlaceMarker(array_t* marker_list, vec2 position, LinceTexture* texture);

void DeleteMarker(array_t* marker_list, int index);

void DrawMarkers(array_t* marker_list);

#endif /* MCOMMAND_MARKER_H */