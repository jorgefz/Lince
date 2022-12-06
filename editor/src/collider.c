
#include "collider.h"

int LinceCollideBox(LinceBoxCollider* a, LinceBoxCollider* b){
	int result =
    a->pos[0] + a->size[0]/2.0f > b->pos[0] - b->size[0]/2.0f &&
    a->pos[0] - a->size[0]/2.0f < b->pos[0] + b->size[0]/2.0f &&
    a->pos[1] + a->size[1]/2.0f > b->pos[1] - b->size[1]/2.0f &&
    a->pos[1] - a->size[1]/2.0f < b->pos[1] + b->size[1]/2.0f;

    if(result == 0) return LinceBoxCollision_None;

    // min, max
    vec2 a_xbounds = {a->pos[0] - a->size[0]/2.0f, a->pos[0] + a->size[0]/2.0f};
    vec2 a_ybounds = {a->pos[1] - a->size[1]/2.0f, a->pos[1] + a->size[1]/2.0f};

    vec2 b_xbounds = {b->pos[0] - b->size[0]/2.0f, b->pos[0] + b->size[0]/2.0f};
    vec2 b_ybounds = {b->pos[1] - b->size[1]/2.0f, b->pos[1] + b->size[1]/2.0f};
    
    if (a_ybounds[0] >= b_ybounds[1]){
        result |= LinceBoxCollision_Top;
    } else if (a_ybounds[0] <= b_ybounds[1]){
        result |= LinceBoxCollision_Bot;
    }
    if (a_xbounds[0] >= b_xbounds[1]){
        result |= LinceBoxCollision_Right;
    } else if (a_xbounds[0] <= b_xbounds[1]){
        result |= LinceBoxCollision_Left;
    }
    return result;
}

