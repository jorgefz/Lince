#include "lince/physics/box2d.h"
#include <math.h>

LinceBool LinceBox2DContains(LinceBox2D* box1, LinceBox2D* box2) {
    return(
        box1->x - box1->w/2.0f <= box2->x - box2->w/2.0f &&
        box2->x + box2->w/2.0f <= box1->x + box1->w/2.0f &&
        box1->y + box1->h/2.0f <= box2->y + box2->h/2.0f &&
        box2->y - box2->h/2.0f <= box1->y - box1->h/2.0f
    );
}


LinceBool LinceBox2DCollides(LinceBox2D* box1, LinceBox2D* box2){
    return (
        box1->x - box1->w / 2.0f <= box2->x + box2->w / 2.0f &&
        box1->x + box1->w / 2.0f >= box2->x - box2->w / 2.0f &&
        box1->y - box1->h / 2.0f <= box2->y + box2->h / 2.0f &&
        box1->y + box1->h / 2.0f >= box2->y - box2->h/2.0f
    );
}


void LinceCalculateEntityCollisions(LinceEntityRegistry* reg, array_t* entities, int box_component_id){
    uint32_t query_num = entities->size;

    for(uint32_t i = 0; i != query_num; ++i){
        uint32_t id = *(uint32_t*)array_get(entities, i);
        LinceBox2D* box1 = LinceGetEntityComponent(reg, id, box_component_id);

        // Ignore zero-size boxes
        if( fabs(box1->dx) == 0.0f && fabs(box1->dy) == 0.0f ) continue;

        // Ignore unmoving boxes
        if( box1->flags & LinceBox2D_Static) continue;

        LinceBox2D xb = *box1, yb = *box1;
        xb.x += box1->dx;
        yb.y += box1->dy;
        LinceBool move_x = 1, move_y = 1;
        
        for(uint32_t j = 0; j != query_num; ++j){
            if(i == j) continue;
            uint32_t id2 = *(uint32_t*)array_get(entities, j);
            LinceBox2D* box2 = LinceGetEntityComponent(reg, id2, box_component_id);
            if(move_x) move_x = !LinceBox2DCollides(&xb, box2);
            if(move_y) move_y = !LinceBox2DCollides(&yb, box2);
            if(!move_x && !move_y) break;
        }
        
        if(move_x){
            box1->x = xb.x;
            box1->flags &= ~LinceBox2D_CollisionX;
        } else {
            box1->flags |= LinceBox2D_CollisionX;
            if(box1->flags & LinceBox2D_Bounce){
                box1->dx = -box1->dx;     
            }       
        }

        if(move_y){
            box1->y = yb.y;
            box1->flags &= ~LinceBox2D_CollisionY;
        } else {
            box1->flags |= LinceBox2D_CollisionY;
            if(box1->flags & LinceBox2D_Bounce){
                box1->dy = -box1->dy;
            }           
        }
    }

}