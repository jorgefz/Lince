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


/// TODO: rename
typedef struct DoorLink {
    LinceBox2D box; // Collider box that triggers teleport
    float to_x, to_y; // Set camera to this position when teleport

    /// TODO: replace with string_t
    char to_scene[LINCE_NAME_MAX]; // Name of scene to load
    size_t to_scene_len; // Number of characters in the scene name
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