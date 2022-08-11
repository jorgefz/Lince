# Tiles

## LinceTile
- `LinceTexture* texture`
	- Preloaded texture object (see [Textures](./Textures.md))
- `vec2 pos`
	- Location of the tile in units of cells
- `vec2 cellsize`
	- Size of a cell in pixels
- `vec2 tilesize`
	- Size of tile in units of cells
- `float coords[8]`
	- Texture coordinates

## LinceGetTile
```c
LinceTile LinceGetTile(LinceTexture* texture, vec2 pos, vec2 cellsize, vec2 tilesize)
```
This function calculates the texture coordinates of a tile within a texture,
and returns the results packaged in a `LinceTile` structure along with the input parameters used to calculate it (see above).
This object may later be passed to the `tile` argument of `LinceQuadProps` on the 2D renderer to render the desired tile only (see [Renderer](./Renderer.md)).

Example code:
```c

LinceTexture* atlas = LinceCreateTexture("atlas", "outdoors.png");

LinceTile tile_grass = LinceGetTile(
	atlas,
	(vec2){0,0},	// location 0,0 from lower left corner
	(vec2){16,16},	// a tile/cell is 16x16 pixels
	(vec2){1,1}		// the tile is 1x1 cells in size (16x16)
)
```


## LinceTileAnim
Stores the data to run tile-based sprite animations.

Required input data
- `LinceTile* frames`
	- Buffer than contains tile array
- `uint32_t frame_count`
	- Number of tiles in the buffer
- `float frame_time`
	- Time in ms between frames

Optional parameters
- `uint32_t start`
	- Tile index at which to start the animation
- `uint32_t repeats`
	- Number of repeats to animate. If set to zero, it will repeat forever
- `uint32_t* order`
	- Array if indices - order in which to render the tiles. If set, `start` index refers to an element of the order array
- `uint32_t order_count`
	- Number of indices in the order array
- `LinceTileAnimFn* on_finish`
	- Called when animation has finished. Signature `void callback(LinceTileAnim* anim, void* args)`.
- `LinceTileAnimFn* on_repeat`
	- Called when animation loops over. Signature `void callback(LinceTileAnim* anim, void* args)`.
- `void* callback_args`
	- Arguments to pass onto callbacks
- `LinceTileAnimFlags flags`
	- Further settings as bit flags. Unused parameter.

Internal data
- `float time`
	- Countdown until next frame
- `uint32_t current_frame`
	- Index of current frame
- `LinceTile* current_tile`
	- Pointer to current tile in the buffer
- `uint32_t repeat_count`
	- Number of times the animation has been looped
- `LinceBool finished`
	- Indicates whether animation has finished running


// Sorts out the user-defined animation settings and initialises the animation

// Animates by time dt

// Resets clock and frames to initial conditions

// Frees animation data


## LinceCreateTileAnim
```c
LinceTileAnim* LinceCreateTileAnim(const LinceTileAnim* anim)
```
Initialises the animation. The user-defined settings and data are passed as a `LinceTileAnim` struct itself, which is then copied over to heap-allocated memory and returned to the user.

## LinceUpdateTileAnim
```c
void LinceUpdateTileAnim(LinceTileAnim* anim, float dt)
```
Advaces the animation by time `dt` in milliseconds.


## LinceResetTileAnim
```c
void LinceResetTileAnim(LinceTileAnim* anim)
```
Resets the animation back to its original point.

## LinceDeleteTileAnim
```c
void LinceDeleteTileAnim(LinceTileAnim* anim)
```
Frees allocated animation data.

Code example:
```c

/* OnLayerCreate */
uint32_t count;
LinceTile* tiles = LoadTilesFromTexture("player-movement.png", &count);
// the function above is an example

LinceTileAnim* anim = LinceCreateTileAnim(&(LinceTileAnim){
	.frames = tiles,
	.frame_count = count,
	.frame_time = 100.0f // milliseconds
});


/* OnLayerUpdate */
LinceBeginScene(camera);
LinceUpdateAnim(anim);
LinceDrawQuad((LinceQuadProps){
	.w = 1.0, .h = 1.0,
	.color = {0.0, 0.0, 0.0, 1.0},
	.tile = anim->current_tile;
});
LinceEndScene();

/* OnLayerDestroy */
LinceDeleteAnim(anim);

```