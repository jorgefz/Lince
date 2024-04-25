#ifndef GAME_DATA_H
#define GAME_DATA_H

#include <lince.h>

typedef struct GameData {
    LinceCamera camera;
    float camera_speed;
    LinceSprite player_sprite;
    LinceTransform player_transform;
    LinceBox2D player_box;
} GameData;


typedef struct DoorLink {
    LinceBox2D box; // Collider box that triggers teleport
    char to_scene[LINCE_NAME_MAX]; // Which scene to load
    float to_x, to_y; // Set camera to this position when teleport
} DoorLink;


typedef enum Component {
    Component_Sprite,
    Component_Transform,
    Component_Tilemap,
    Component_TileAnim,
    Component_Box2D,

    Component_Count
} Component;


#define COMPONENT_SIZES             \
        sizeof(LinceSprite),        \
        sizeof(LinceTransform),     \
        sizeof(LinceTilemap),       \
        sizeof(LinceTileAnim),      \
        sizeof(LinceBox2D)

#endif // GAME_DATA_H