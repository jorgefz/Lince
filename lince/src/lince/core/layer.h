/** @file layer.h
* Example code:
* ```c
*	void MyLayerOnAttach(LinceLayer* layer){
*	    printf("MyLayer attached\n");
*	}
* 
*	void MyLayerOnDetach(LinceLayer* layer){
*	    printf("MyLayer detached\n");
*	}
*
*	void MyLayerOnUpdate(LinceLayer* layer, float dt){
*	 
*	}
* 
*	void MyLayerOnEvent(LinceLayer* layer, LinceEvent* e){
*	
*	}
*  
*	void GameInit(){
*		LinceLayer layer = {
*			.on_attach = MyLayerOnAttach,
*			.on_detach = MyLayerOnDetach,
*			.on_update = MyLayerOnUpdate,
*			.on_event  = MyLayerOnEvent
*		}
*		LincePushLayer(&layer);
*	}
* 
*	int main(){
*		LinceApp* app = LinceGetApp();
*		app->on_init = GameInit;
*		LinceRun();
*		return 0;
*	}
* ```
*/

#ifndef LINCE_LAYER_H
#define LINCE_LAYER_H

#include "lince/event/event.h"
#include "lince/core/uuid.h"

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

	void (*on_attach)(struct LinceLayer*);
	///< Called when layer is pushed onto stack
	
	void (*on_detach)(struct LinceLayer*);
	///< Called when layer is popped from stack. Should only free user data
	
	void (*on_update)(struct LinceLayer*, float dt);
	///< Called on each game frame
	
	void (*on_event)(struct LinceLayer*, LinceEvent*);
	///< called when an event propagates to the layer
} LinceLayer;


#endif // LINCE_LAYER_H
