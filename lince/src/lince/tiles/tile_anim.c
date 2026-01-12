#include "lince/tiles/tile_anim.h"
#include "lince/utils/memory.h"
#include "lince/core/logger.h"

/* Set default frame order -> cycle through all tiles in tileset */
static void LinceTileAnimSetDefaultFrames(LinceTileAnim* anim){
	uint32_t frame_count = anim->tileset->coords.size;
	array_resize(&anim->frames, frame_count);
	for(uint32_t i = 0; i != frame_count; ++i){
		array_set(&anim->frames, &i, i);
	}
}


LinceTileAnim* LinceTileAnimInit(LinceTileAnim* anim, LinceTileset* tileset, float frame_time){
	if(frame_time <= 0.0f){
		LINCE_WARN("TileAnim: invalid frame time (%f)\n", frame_time);
		return NULL;
	}

	anim->tileset = tileset;
	anim->frame_time = frame_time;
	array_init(&anim->frames, sizeof(uint32_t));
	LinceTileAnimSetDefaultFrames(anim); // Default frames -> cycle through all tiles
	LinceTileAnimReset(anim);
	return anim;
}

void LinceTileAnimUninit(LinceTileAnim* anim){
	array_uninit(&anim->frames);
}

void LinceTileAnimUpdate(LinceTileAnim* anim, float dt){
	if(anim->finished) return;

	anim->time += dt;
	if(anim->time < anim->frame_time) return;

	// Timer is finished - next frame
	anim->time = 0.0f;
	anim->index++;

	if(anim->index < anim->frames.size){
		anim->frame = *(uint32_t*)array_get(&anim->frames, anim->index);
		// if(anim->on_frame) anim->on_frame(anim, anim->callback_args);
		return;
	}

	// Animation finished - stop or loop back
	LinceTileAnimReset(anim);
	if(anim->on_finish){
		anim->on_finish(anim, anim->args);	
	}
	if(!(anim->flags & LinceTileAnim_Repeat)){
		anim->finished = LinceTrue;
	}
}

void LinceTileAnimSetFrames(LinceTileAnim* anim, uint32_t count, uint32_t* frames){
	if(!frames || count == 0){
		LinceTileAnimSetDefaultFrames(anim);
	}
	array_resize(&anim->frames, count);
	memcpy(anim->frames.data, frames, count * sizeof(uint32_t));
	LinceTileAnimReset(anim);
}

void LinceTileAnimSetCurrentFrame(LinceTileAnim* anim, uint32_t index){
	if(index >= anim->frames.size) return;
	anim->index = index;
}

LinceRect* LinceTileAnimGetFrameCoords(LinceTileAnim* anim){
	return array_get(&anim->tileset->coords, anim->frame);
}

void LinceTileAnimReset(LinceTileAnim* anim){
	anim->index = 0;
	anim->time  = 0;
	anim->finished = LinceFalse;
	anim->frame = *(uint32_t*)array_get(&anim->frames, 0);
}

LinceBool LinceTileAnimFinished(LinceTileAnim* anim){
	return anim->finished;
}

