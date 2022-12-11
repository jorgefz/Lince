#include "marker.h"

void PlaceMarker(array_t* marker_list, vec2 position, LinceTexture* texture){
	Marker m = {
		.x = position[0],
		.y = position[1],
		.sprite = (LinceQuadProps){
			.x = position[0],
			.y = position[1],
			.w = 0.05f,
			.h = 0.05f,
			.color = {0.0f, 0.2f, 0.8f, 1.0f},
			.texture = texture
		},
	};
	array_push_back(marker_list, &m);
}

void DeleteMarker(array_t* marker_list, int index){
	array_remove(marker_list, index);
}

void DrawMarkers(array_t* marker_list){
	for(uint32_t i=0; i!=marker_list->size; ++i){
		Marker* m = array_get(marker_list, i);
		LinceDrawQuad(m->sprite);
	}
}
