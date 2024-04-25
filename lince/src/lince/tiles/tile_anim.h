#ifndef LINCE_TILE_ANIM_H
#define LINCE_TILE_ANIM_H

#include "lince/containers/array.h"
#include "lince/tiles/tileset.h"

/** @enum LinceTileAnimFlags
* @brief Settings for LinceTileAnim
*/
typedef enum LinceTileAnimFlags {
    LinceTileAnim_Repeat       	= 0x1,  ///< Repeats the animation on finish
} LinceTileAnimFlags;

struct LinceTileAnim; // forward declaration

/** @typedef LinceTileAnimFn
* Function signatures for LinceTileAnim callbacks.
*/
typedef void (*LinceTileAnimFn)(struct LinceTileAnim* anim, void* args);

/** @struct LinceTileAnim
* @brief Stores the state of a tile animation
*/
typedef struct LinceTileAnim {

    // User-defined parameters
    float frame_time; ///< Duration of each frame in ms. Must be greater than zero.
    int flags;        ///< Bitflags, LinceTileAnimFlags
    LinceTileAnimFn on_finish; ///< Callback called when animation ends or repeats.
    void* args;       ///< Arguments passed to callback

    // Internal data
    float     time;	    ///< Countdown until next frame
    uint32_t  frame;    ///< Index of current frame in tileset
    uint32_t  index;    ///< Index of current frame in frame array
    LinceBool finished; ///< Indicates whether animation has finished running

    LinceTileset* tileset;  ///< Tileset with tiles to animate
    array_t frames; ///< Order in which to display tiles,
                    /// defaults to all tiles in tileset in order.

} LinceTileAnim;


/** @brief Initialise a tile animation.
 * @param anim TileAnim with user-defined parameters.
 * @param tileset Tileset containing tiles to animate.
 * @param frame_time Duration of each frame in ms.
 * @returns Input anim if successful, and null otherwise.
*/
LinceTileAnim* LinceTileAnimInit(LinceTileAnim* anim, LinceTileset* tileset, float frame_time);

/** @brief Frees allocated memory */
void LinceTileAnimUninit(LinceTileAnim* anim);

/** @brief Updates the internal timer and advances the animation
 * @param anim Tile animation
 * @param dt Time elapsed in ms
*/
void LinceTileAnimUpdate(LinceTileAnim* anim, float dt);

/** @brief Sets which tileset frames are displayed and their order
 * @param anim Tile animation
 * @param count Number of frames
 * @param frames Indices of the tiles in the tileset to animate
*/
void LinceTileAnimSetFrames(LinceTileAnim* anim, uint32_t count, uint32_t* frames);

/** @brief Sets the current frame of the animation
 * @param anim Tile animation
 * @param index Index for the array of frames
*/
void LinceTileAnimSetCurrentFrame(LinceTileAnim* anim, uint32_t index);

/** @brief Returns the UV coordinates of the current tile in the animation.
 * Pass this to `LinceDrawSpriteTile` to render the animation.
 * @param anim Tile animation
 * @returns Bounds of current animation tile in the tileset
*/
LinceRect* LinceTileAnimGetFrameCoords(LinceTileAnim* anim);

/** @brief Resets the animation to the first frame */
void LinceTileAnimReset(LinceTileAnim* anim);

/** @brief Returns true if the animation has finished.
 * If the animation is set to repeat, it will
*/
LinceBool LinceTileAnimFinished(LinceTileAnim* anim);


#endif /* LINCE_TILE_ANIM_H */