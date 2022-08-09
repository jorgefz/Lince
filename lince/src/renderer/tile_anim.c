#include "renderer/tile_anim.h"

/*
Returns a pointer to the current tile in the animation
*/
static LinceTile* GetCurrentTile(LinceTileAnim* anim){
    return &anim->frames[anim->current_frame];
}


LinceTileAnim* LinceCreateTileAnim(const LinceTileAnim* props){
	
	LINCE_ASSERT(props && props->frames, "Animation is missing tile buffer");
	LINCE_ASSERT(props->frame_count > 0, "Animation cannot have zero frames");
	LINCE_ASSERT(props->frame_time > 0.0f, "Animation cannot have a frame time of zero ms");
	LINCE_ASSERT(props->start < props->frame_count,
		"Animation start frame is out of bounds"
		"(start at frame index %d but there are only %d frames)",
		(int)props->start, (int)props->frame_count
	);

	// Allocate & copy animation properties
	LinceTileAnim* anim = calloc(1, sizeof(LinceTileAnim));
	LINCE_ASSERT_ALLOC(anim, sizeof(LinceTileAnim));
	memmove(anim, props, sizeof(LinceTileAnim));

	// Copy tile buffer
	anim->frames = malloc(sizeof(LinceTile) * props->frame_count);
	LINCE_ASSERT_ALLOC(anim->frames, sizeof(LinceTile) * props->frame_count);
	memmove(anim->frames, props->frames, sizeof(LinceTile) * props->frame_count);
	
	// Setup initial conditions
	anim->time = anim->frame_time;
	anim->repeat_count = 0;
	anim->current_frame = anim->start;
	anim->current_tile = GetCurrentTile(anim);
	return anim;
}


void LinceUpdateTileAnim(LinceTileAnim* anim, float dt){
	anim->time -= dt;

	if(anim->time <= 0.0f){
		anim->current_frame++;
		if(anim->current_frame >= anim->frame_count){
			anim->current_frame = 0;
			anim->repeat_count++;
			if(anim->on_repeat){
				anim->on_repeat(anim, anim->callback_args);
			}
		}
		anim->time = anim->frame_time; // reset countdown
		anim->current_tile = GetCurrentTile(anim);
	}

}


void LinceResetTileAnim(LinceTileAnim* anim){
	anim->time = anim->frame_time;
	anim->current_frame = 0;
	anim->current_tile = GetCurrentTile(anim);
	anim->repeat_count = 0;
}


void LinceDeleteAnim(LinceTileAnim* anim){
	if(!anim) return;
	if(anim->frames) free(anim->frames);
	free(anim);
}
