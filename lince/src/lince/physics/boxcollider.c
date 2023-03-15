#include "boxcollider.h"
#include <math.h>

LinceBool LinceBoxColliderContains(LinceBoxCollider* box1, LinceBoxCollider* box2) {
    return(
        box1->x - box1->w/2.0f <= box2->x - box2->w/2.0f &&
        box2->x + box2->w/2.0f <= box1->x + box1->w/2.0f &&
        box1->y + box1->h/2.0f <= box2->y + box2->h/2.0f &&
        box2->y - box2->h/2.0f <= box1->y - box1->h/2.0f
    );
}

LinceBool LinceBoxColliderIntersects(LinceBoxCollider* box1, LinceBoxCollider* box2){
    return !(
        box1->x - box1->w/2.0f >= box2->x + box2->w/2.0f ||
        box1->x + box1->w/2.0f <= box2->x - box2->w/2.0f ||
        box1->y + box1->h/2.0f >= box2->y - box2->h/2.0f ||
        box1->y - box1->h/2.0f <= box2->y + box2->h/2.0f
    );
}

LinceBool LinceBoxCollides(LinceBoxCollider* rect1, LinceBoxCollider* rect2){
    return (
        rect1->x - rect1->w/2.0f <= rect2->x + rect2->w/2.0f &&
        rect1->x + rect1->w/2.0f >= rect2->x - rect2->w/2.0f &&
        rect1->y - rect1->h/2.0f <= rect2->y + rect2->h/2.0f &&
        rect1->y + rect1->h/2.0f >= rect2->y - rect2->h/2.0f
    );
}

// LinceUpdateBoxCollider
/// TODO: Improve neighbour search algorithm - use Quad Tree
void LinceCalculateEntityCollisions(LinceEntityRegistry* reg, array_t* entities, int box_component_id){
    uint32_t query_num = entities->size;

    for(uint32_t i = 0; i != query_num; ++i){
        uint32_t id = *(uint32_t*)array_get(entities, i);
        LinceBoxCollider* box1 = LinceGetEntityComponent(reg, id, box_component_id);

        // Ignore zero-size boxes
        if( fabs(box1->dx) == 0.0f && fabs(box1->dy) == 0.0f ) continue;

        // Ignore unmoving boxes
        if( box1->flags & LinceBoxCollider_Static) continue;

        LinceBoxCollider xb = *box1, yb = *box1;
        xb.x += box1->dx;
        yb.y += box1->dy;
        LinceBool move_x = 1, move_y = 1;
        
        for(uint32_t j = 0; j != query_num; ++j){
            if(i == j) continue;
            uint32_t id2 = *(uint32_t*)array_get(entities, j);
            LinceBoxCollider* box2 = LinceGetEntityComponent(reg, id2, box_component_id);
            if(move_x) move_x = !LinceBoxCollides(&xb, box2);
            if(move_y) move_y = !LinceBoxCollides(&yb, box2);
            if(!move_x && !move_y) break;
        }
        
        if(move_x){
            box1->x = xb.x;
            box1->flags &= ~LinceBoxCollider_CollisionX;
        } else {
            box1->flags |= LinceBoxCollider_CollisionX;
            if(box1->flags & LinceBoxCollider_Bounce){
                box1->dx = -box1->dx;     
            }       
        }

        if(move_y){
            box1->y = yb.y;
            box1->flags &= ~LinceBoxCollider_CollisionY;
        } else {
            box1->flags |= LinceBoxCollider_CollisionY;
            if(box1->flags & LinceBoxCollider_Bounce){
                box1->dy = -box1->dy;
            }           
        }
    }

}