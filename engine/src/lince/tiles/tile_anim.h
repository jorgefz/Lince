#ifndef LINCE_TILE_ANIM_H
#define LINCE_TILE_ANIM_H

#include "lince/tiles/tileset.h"

/*
=== LinceTileAnim ===

LinceTileAnim
    - frames:   array of LinceTile tiles that define each frame of the animation
    - frame_count: number of frames in buffer above
    - frametime:  length of each frame in ms
    (implement multiple frametimes?)

    - order: array of frame indices that indicates order in which to show frames
    - start_frame: initial frame, used only if order is not set. Defaults to 0.
    - repeats: number of repeats of the animation

    - on_finish: callback called when animation finished.
                Not called when it is set to repeat forever. 
    - on_repeat: callback called each time animation loops over.
                Not called when it is set to run only once.
    - callback_params: list of parameters to pass to callbacks

    - flags:
        0: once or repeat
        1: ... 

    // Internal
    - time
    - current_frame


Code Example:

*/


struct LinceTileAnim; // forward declaration

typedef enum LinceTileAnimFlags {
    // Defaults set to zero, useless but reassuring
    LinceTileAnimFlag_Repeat       	= 0x0,
	LinceTileAnimFlag_EndWithFirst 	= 0x0,
    // Non-defaults
    LinceTileAnimFlag_Once        	= 0x1,  // Run animation only once
	LinceTileAnimFlag_EndWithLast 	= 0x2,  // Ends animation on the last frame
    // LinceTileAnimFlag_OneFrame // Set frametime to one frame
} LinceTileAnimFlags;

typedef void LinceTileAnimFn(struct LinceTileAnim* anim, void* params);

typedef struct LinceTileAnim {
    LinceTile* frames;		// buffer than contains tile array
    uint32_t frame_count;	// number of tiles in the buffer
    float frame_time;		// time in ms between frames

    uint32_t start;			// tile index at which to start the animation
    uint32_t repeats;		// number of repeats to animate
							// if set to zero, it will repeat forever
	
	uint32_t* order;		// Array if indices - order in which to render the tiles
                            // If set, start index refers to an element of the order array
    uint32_t order_count;	// Number of indices in the order array

    LinceTileAnimFn* on_finish;		// Called when animation has finished
    LinceTileAnimFn* on_repeat;		// Called when animation loops over
    // LinceTileAnimFn* on_frame 	// Called when frame changes
    void* callback_args;			// Arguments to pass onto callbacks

    LinceTileAnimFlags flags;

    // Internal
    float time;					// countdown until next frame
    uint32_t current_frame;		// index of current frame
    LinceTile* current_tile;	// pointer to current tile in the buffer
	uint32_t repeat_count;		// number of times the animation has been looped
	LinceBool finished; 		// indicates whether animation has finished running

} LinceTileAnim;


// Sorts out the user-defined animation settings and initialises the animation
LinceTileAnim* LinceCreateTileAnim(const LinceTileAnim* anim);
// Animates by time dt
void LinceUpdateTileAnim(LinceTileAnim* anim, float dt);
// Resets clock and frames to initial conditions
void LinceResetTileAnim(LinceTileAnim* anim);
// Frees animation data
void LinceDeleteTileAnim(LinceTileAnim* anim);



#endif /* LINCE_TILE_ANIM_H */