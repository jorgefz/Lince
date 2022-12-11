#include "marker.h"

void PlaceMarker(array_t* marker_list, vec2 position, LinceTexture* texture){
	Sprite sprite = {
		.x = position[0],
		.y = position[1],
		.w = 0.05f,
		.h = 0.05f,
		.color = {0.0f, 0.2f, 0.8f, 1.0f},
		.texture = texture
	};
	GameObject marker = {0};
	marker.sprite = LinceNewCopy(&sprite, sizeof(Sprite));
	array_push_back(marker_list, &marker);
}



