#ifndef MCOMMAND_GAMEOBJECT_H
#define MCOMMAND_GAMEOBJECT_H

// include all components
#include <lince.h>
#include <lince/containers/array.h>
#include "collider.h"
#include "timer.h"

typedef LinceQuadProps Sprite;

typedef struct GameObject {
	Collider* collider;
	Sprite* sprite;
	Timer* timer;
} GameObject;

void DrawEntity(GameObject* entity);

void DrawEntityList(array_t* objects);

void DeleteEntity(GameObject* entity);

void DeleteEntityItem(array_t* items, uint32_t index);

void DeleteEntityList(array_t* list);

// to be included by systems and entities


#endif /* MCOMMAND_GAMEOBJECT_H */