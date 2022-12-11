#include "gameobject.h"


void DrawEntity(GameObject* entity){
    if(entity && entity->sprite){
        LinceDrawQuad(*entity->sprite);
    }
}

void DrawEntityList(array_t* objects){
	GameObject* item;
	for(uint32_t i = 0; i != objects->size; ++i){
		item = array_get(objects, i);
		DrawEntity(item);
	}
}

void DeleteEntity(GameObject* entity){
    if(!entity) return;
    if(entity->sprite)   LinceFree(entity->sprite);
    if(entity->timer)    LinceFree(entity->timer);
    if(entity->collider) LinceFree(entity->collider);
}

void DeleteEntityItem(array_t* items, uint32_t index){
    GameObject* item = array_get(items, index);
    DeleteEntity(item);
    array_remove(items, index);
}

void DeleteEntityList(array_t* list){
	GameObject* item;
    for(uint32_t i = 0; i != list->size; ++i){
		item = array_get(list, i);
        DeleteEntity(item);
	}
	array_destroy(list);
}
