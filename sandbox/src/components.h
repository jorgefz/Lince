#ifndef COMPS_H
#define COMPS_H

#include <lince.h>

typedef enum Component {
    Component_Sprite,
    Component_Camera,
    Component_Shader,
} Component;

#define COMPONENT_SIZES   2, \
        sizeof(LinceSprite), \
        sizeof(LinceCamera), \
        sizeof(LinceShader)

#endif // COMPS_H