#include "renderer/tile_anim.h"

/*
Returns a pointer to the current tile in the animation
*/
static LinceTile* GetCurrentTile(LinceTileAnim* anim){
 	return &anim->frames[anim->order[anim->current_frame]];
}

LinceTileAnim* LinceCreateTileAnim(const LinceTileAnim* props){
	
	LINCE_ASSERT(props && props->frames, "Animation is missing tile buffer");
	LINCE_ASSERT(props->frame_count > 0, "Animation cannot have zero frames");
	LINCE_ASSERT(props->frame_time > 0.0f,
		"Animation cannot have a frame time of zero ms");
	LINCE_ASSERT(props->start < props->frame_count,
		"Animation start frame is out of bounds"
		"(start at frame index %u but there are only %u frames)",
		props->start, props->frame_count
	);

	// Allocate & copy animation properties
	LinceTileAnim* anim = calloc(1, sizeof(LinceTileAnim));
	LINCE_ASSERT_ALLOC(anim, sizeof(LinceTileAnim));
	memmove(anim, props, sizeof(LinceTileAnim));

	// Copy tile buffer
	anim->frames = malloc(sizeof(LinceTile) * props->frame_count);
	LINCE_ASSERT_ALLOC(anim->frames, sizeof(LinceTile) * props->frame_count);
	memmove(anim->frames, props->frames, sizeof(LinceTile) * props->frame_count);

	// Setup tile order
	if(props->order){
		// Order provided
		LINCE_ASSERT(props->order_count > 0,
			"Order indices length must be greater than zero");

		// check all indices are allowed
		for(uint32_t i = 0; i != anim->order_count; ++i){
			LINCE_ASSERT(anim->order[i] < anim->frame_count,
				"Order index out of bounds"
				"(%uth index is %u but there are only %u frames)",
				i, anim->order[i], anim->frame_count);
		}

		// copy indices over	
		anim->order = malloc(sizeof(uint32_t) * anim->order_count);
		LINCE_ASSERT_ALLOC(anim->order, sizeof(uint32_t) * anim->order_count);
		memmove(anim->order, props->order, sizeof(uint32_t)*anim->order_count);

	}
	else {
		// setup default order: 0 to frame_count
		anim->order_count = anim->frame_count;
		anim->order = malloc(sizeof(uint32_t) * anim->order_count);
		LINCE_ASSERT_ALLOC(anim->order, sizeof(uint32_t) * anim->order_count);
		for(uint32_t i = 0; i != anim->order_count; ++i){
			anim->order[i] = i;
		}
	}
	
	// Setup initial conditions
	LinceResetTileAnim(anim);
	return anim;
}


void LinceUpdateTileAnim(LinceTileAnim* anim, float dt){
	if(anim->finished) return;

	anim->time -= dt;

	// Time is still ongoing - continue with current frame
	if(anim->time > 0.0f) return;

	// Timer is finished - change of frame
	anim->current_frame++;
	
	// Reached end of frame list
	// if(anim->current_frame >= anim->frame_count){
	if(anim->current_frame >= anim->order_count){
		anim->current_frame = 0;
		anim->repeat_count++;

		// Reached max number of repeats - end animation
		if(anim->repeats > 0 && anim->repeats == anim->repeat_count){
			anim->finished = LinceTrue;
			if(anim->on_finish){
				anim->on_finish(anim, anim->callback_args);
			}
		}
		// Or just loop over once more
		else if(anim->on_repeat){
			anim->on_repeat(anim, anim->callback_args);
		}
	}

	anim->time = anim->frame_time; // reset countdown
	anim->current_tile = GetCurrentTile(anim);
	
}


void LinceResetTileAnim(LinceTileAnim* anim){
	anim->time = anim->frame_time;
	
	// anim->current_frame = 0;
	anim->current_frame = anim->start;
	
	anim->current_tile = GetCurrentTile(anim);
	anim->repeat_count = 0;
	anim->finished = LinceFalse;
}


void LinceDeleteTileAnim(LinceTileAnim* anim){
	if(!anim) return;
	if(anim->frames) free(anim->frames);
	if(anim->order)  free(anim->order);
	free(anim);
}
