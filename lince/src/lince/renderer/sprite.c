#include "lince/renderer/sprite.h"
#include "lince/renderer/transform.h"
#include "lince/renderer/shader.h"
#include "lince/renderer/renderer.h"

void LinceDrawSprite(LinceSprite* sprite, LinceShader* shader){
    static LinceQuadData data = { 0 };
    static const float default_uv[8] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f, 
    };

    data.zorder = sprite->zorder;
    data.rotation = sprite->rotation;
    memcpy(data.color, sprite->color, sizeof(float)*4);

    if(sprite->tile){
        memcpy(data.uv, sprite->tile->coords, sizeof(float)*8);
    } else {
        memcpy(data.uv, default_uv, sizeof(float)*8);
    }

    /*
    LinceTransform combined = {
        .x = transform->x + sprite->x,
        .y = transform->y + sprite->y,
        .w = transform->w * sprite->w,
        .h = transform->h * sprite->h,
        .coords = transform->coords
    };
    */

    LinceTransform transform = {.x=sprite->x, .y=sprite->y, .w=sprite->w, .h=sprite->h};

    LinceDrawQuad(&transform, &data, sprite->texture, shader);
}