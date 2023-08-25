#ifndef GAME_DATA_H
#define GAME_DATA_H

#include <lince.h>
#include <lince/physics/boxcollider.h>

typedef struct GameData {
    hashmap_t scene_cache;
    LinceCamera camera;
    float camera_speed;
    LinceSprite player_sprite;
    LinceBoxCollider player_box;
} GameData;


typedef struct DoorLink {
    LinceBoxCollider box; // Collider box that triggers teleport
    char to_scene[LINCE_NAME_MAX]; // Which scene to load
    float to_x, to_y; // Set camera to this position when teleport
} DoorLink;


typedef enum Component {
    Component_Sprite,
    Component_Tilemap,
    Component_TileAnim,
    Component_BoxCollider,

    Component_Count
} Component;


#define COMPONENT_SIZES             \
        sizeof(LinceSprite),        \
        sizeof(LinceTilemap),       \
        sizeof(LinceTileAnim),      \
        sizeof(LinceBoxCollider)

#endif // GAME_DATA_H