
#ifndef LINCE_H
#define LINCE_H

#ifdef __cplusplus
    extern "C" {
#endif

/* Core */
#include "lince/core/core.h"
#include "lince/core/window.h"
#include "lince/app/layer.h"
#include "lince/app/app.h"
#include "lince/utils/memory.h"
#include "lince/utils/uuid.h"
#include "lince/utils/fileio.h"
#include "lince/utils/clock.h"
#include "lince/utils/image.h"

/* Input */
#include "lince/input/input.h"
#include "lince/input/keycodes.h"
#include "lince/input/mousecodes.h"

/* Events */
#include "lince/event/event.h"
#include "lince/event/key_event.h"
#include "lince/event/mouse_event.h"
#include "lince/event/window_event.h"

/* Renderer */
#include "lince/renderer/renderer.h"
#include "lince/renderer/buffer.h"
#include "lince/renderer/vertex_array.h"
#include "lince/renderer/shader.h"
#include "lince/renderer/texture.h"
#include "lince/renderer/camera.h"
#include "lince/renderer/transform.h"
#include "lince/renderer/sprite.h"

/* Tilesets & tilemaps */
#include "lince/tiles/tileset.h"
#include "lince/tiles/tile_anim.h"
#include "lince/tiles/tilemap.h"

/* Audio */
#include "lince/audio/audio.h"

/* ECS */
#include "lince/ecs/ecs.h"

/* Scene */
#include "lince/scene/scene.h"

/* Physics */
#include "lince/physics/box2d.h"


#ifdef __cplusplus
}
#endif

#endif //LINCE_H
