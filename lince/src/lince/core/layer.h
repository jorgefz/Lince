/** @file layer.h
* Example code:
* ```c
* void MyLayerOnAttach(LinceLayer* layer){
*     printf("MyLayer attached\n");
* }
*  
* void MyLayerOnDetach(LinceLayer* layer){
*     printf("MyLayer detached\n");
* }
*  
* void MyLayerOnUpdate(LinceLayer* layer, float dt){
*  
* }
*  
* void MyLayerOnEvent(LinceLayer* layer, LinceEvent* e){
* 
* }
*  
* void GameInit(){
*     LinceLayer* layer = LinceCreateLayer(NULL);
*  
*     layer->OnAttach = MyLayerOnAttach;
*     layer->OnDetach = MyLayerOnDetach;
*     layer->OnUpdate = MyLayerOnUpdate;
*     layer->OnEvent = MyLayerOnEvent;
*  
*     LincePushLayer(layer);
* }
*  
* int main(){
*     LinceApp* app = LinceGetAppState();
*     app->game_init = GameInit;
*     
*     LinceRun();
*  
*     return 0;
* }
* ```
*/

#ifndef LINCE_LAYER_H
#define LINCE_LAYER_H

#include "lince/event/event.h"

/** @struct LinceLayer
* @brief Stores geometry that is generally rendered together.
* 
* Layers are important to separate the rendering process into steps,
* with the geometry on each having different properties, roles,
* and being rendered in a different order.
* Some layers are rendered over others, such as UI over game world.
* @todo add debug name?
*/
typedef struct LinceLayer {
	
	void* data; ///< custom user layer pointer

	void (*OnAttach)(struct LinceLayer*);
	///< Called when layer is pushed onto stack
	
	void (*OnDetach)(struct LinceLayer*);
	///< Called when layer is popped from stack. Should only free user data
	
	void (*OnUpdate)(struct LinceLayer*, float dt);
	///< Called on each game frame
	
	void (*OnEvent)(struct LinceLayer*, LinceEvent*);
	///< called when an event propagates to the layer
} LinceLayer;

/** @struct LinceLayerStack
* @brief Data structure to store layers in a stack.
* @todo The layers are stored in an array of pointers. Not cache friendly!
* @todo Replace with array_t, contiguous memory - more cache friendly.
*
* Since LayerStack stores layers as an array of pointers,
* you must pass a pointer to a previously allocated layer
* and not free it, as the layer will be destroyed
* when the stack is popped or destroyed.
*/
typedef struct LinceLayerStack {
	LinceLayer** layers;  ///< Stack of layers
	unsigned int count;   ///< Number of stacked layers
} LinceLayerStack;


/** @brief Allocates a new layer.
* @param data User-defined layer data. This is not copied - only the pointer is stored.
*/
LinceLayer* LinceCreateLayer(void* data);

/** @brief Deallocates a given layer without freeing user data */
void LinceDeleteLayer(LinceLayer* layer);

/** @brief Initialises an empty layer stack */
LinceLayerStack* LinceCreateLayerStack();

/** @brief Detaches and frees all layers */
void LinceDestroyLayerStack(LinceLayerStack* stack);

/** @brief Adds a layer to the stack */
void LinceLayerStackPush(LinceLayerStack* stack, LinceLayer* layer);

/** @brief Removes a layer from the top of the stack */
void LinceLayerStackPop(LinceLayerStack* stack, LinceLayer* layer);


#endif // LINCE_LAYER_H
