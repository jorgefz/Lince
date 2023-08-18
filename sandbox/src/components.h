#ifndef COMPS_H
#define COMPS_H

#include <lince.h>

typedef enum Component {
    Component_Sprite,
    Component_Camera,
    Component_Shader,
    Component_Tilemap,
    Component_TileAnim,
    Component_BoxCollider,

    Component_Count
} Component;

#define COMPONENT_SIZES             \
        sizeof(LinceSprite),        \
        sizeof(LinceCamera),        \
        sizeof(LinceShader),        \
        sizeof(LinceTilemap),       \
        sizeof(LinceTileAnim),      \
        sizeof(LinceBoxCollider)

#endif // COMPS_H