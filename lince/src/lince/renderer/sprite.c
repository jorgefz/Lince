#include "lince/renderer/sprite.h"
#include "lince/renderer/transform.h"
#include "lince/renderer/shader.h"
#include "lince/renderer/renderer.h"

// Draws a sprite to the screen 
void LinceDrawSprite(LinceSprite* sprite, LinceTransform* transform, LinceShader* shader){
    static LinceRect default_uv = {.x=0.0f, .y=0.0f, .w=1.0f, .h=1.0f}; 
    LinceDrawSpriteTile(sprite, transform, &default_uv, shader);
}

// Draws a region of a sprite to the screen
void LinceDrawSpriteTile(LinceSprite* sprite, LinceTransform* transform, LinceRect* uv, LinceShader* shader){
    static LinceQuadData props = { 0 };

    // Sprite properties
    props.zorder = sprite->zorder;
    props.rotation = sprite->rotation;
    props.color[0] = sprite->color.r;
    props.color[1] = sprite->color.g;
    props.color[2] = sprite->color.b;
    props.color[3] = (sprite->flags & LinceSprite_UseAlpha) ? sprite->alpha : 1.0f;

    // Texture coordinates
    LincePoint bounds[4];
    float* coords = LinceRectGetVerticesLL(uv, bounds);
    memcpy(props.uv, coords, sizeof(float)*8);

    LinceDrawQuad(transform, &props, sprite->texture, shader);
}
