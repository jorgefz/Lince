#ifndef LINCE_TILE_ANIM_H
#define LINCE_TILE_ANIM_H

#include "lince/containers/array.h"
#include "lince/tiles/tileset.h"

/** @enum LinceTileAnimFlags
* @brief Settings for LinceTileAnim
*/
typedef enum LinceTileAnimFlags {
    // Defaults set to zero, useless but reassuring
    LinceTileAnimFlag_Repeat       	= 0x0,  ///< Default. Repeats tile animation.
	LinceTileAnimFlag_EndWithFirst 	= 0x0,  ///< Default. Last frame is the first one.
    // Non-defaults
    LinceTileAnimFlag_Once        	= 0x1,  ///< Run animation only once
	LinceTileAnimFlag_EndWithLast 	= 0x2,  ///< (unused) Ends animation on the last frame
    // LinceTileAnimFlag_OneFrame // Set frametime to one frame
} LinceTileAnimFlags;


struct LinceTileAnim; // forward declaration

/** @typedef LinceTileAnimFn
* Function signatures for LinceTileAnim callbacks.
*/
typedef void LinceTileAnimFn(struct LinceTileAnim* anim, void* params);

/** @struct LinceTileAnim
* @brief Stores the state of a tile animation
*/
typedef struct LinceTileAnim {
    array_t* frames;        ///< array<LinceTile>, list of tiles or subtexture coordinates. See tileset.h
    float frame_time;		///< Duration of each frame in millisec.

    uint32_t start;			///< Index of frame at which to start the animation.
    uint32_t repeats;		///< Number of repeats to animate. Repeats forever if zero.
	uint32_t* order;		///< Array of indices - order in which to render the tiles. If set, start index refers to an element of the order array.
    uint32_t order_count;	///< Number of indices in the order array.

    LinceTileAnimFn* on_finish;	  ///< Called when animation has finished
    LinceTileAnimFn* on_repeat;	  ///< Called when animation loops over
    // LinceTileAnimFn* on_frame  // Called when frame changes
    void* callback_args;		  ///< Generic data to pass onto callbacks

    LinceTileAnimFlags flags;     ///< Settings

    // Internal
    float time;					///< Countdown until next frame
    uint32_t current_frame;		///< Index of current frame
    LinceTile* current_tile;	///< Pointer to current tile in the buffer
	uint32_t repeat_count;		///< Number of times the animation has been looped
	LinceBool finished; 		///< Indicates whether animation has finished running

} LinceTileAnim;


/** @brief Initialises the animation.
* Settings and options should be pre-defined on the object
* before passing it as an argument.
*/
void LinceCreateTileAnim(LinceTileAnim* anim);

/** @brief Updates animation by one timestep */
void LinceUpdateTileAnim(LinceTileAnim* anim, float dt);

/** @brief Resets clock and frames to initial conditions */
void LinceResetTileAnim(LinceTileAnim* anim);

/** @brief Frees allocated data within the object,
* but the user is responsible for deallocating the object itself
* if it was allocated.
*/
void LinceDeleteTileAnim(LinceTileAnim* anim);



#endif /* LINCE_TILE_ANIM_H */