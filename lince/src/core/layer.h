#ifndef LINCE_LAYER_H
#define LINCE_LAYER_H

#include "event/event.h"


typedef void* LinceLayerData;

typedef struct LinceLayer {
	// custom user layer pointer
	LinceLayerData data;

	/// TODO: add debug name?

	// callbacks
	/* Called when layer is pushed onto stack */
	void (*OnAttach)(struct LinceLayer*);
	/* Called when layer is popped from stack,
	should free the user layer but not the LinceLayer itself */
	void (*OnDetach)(struct LinceLayer*);
	/* called on each game frame */
	void (*OnUpdate)(struct LinceLayer*, float dt);
	/* called only when an event takes place and hasn't been handled yet */
	void (*OnEvent)(struct LinceLayer*, LinceEvent*);
} LinceLayer;

/* Creates new layer using custom data passed to it */
LinceLayer* LinceCreateLayer(void* data);
/* Retrieves pointer to user-defined layer */
void* LinceGetLayerData(LinceLayer* layer);

typedef struct LinceLayerStack {
	LinceLayer** layers;
	unsigned int count;
} LinceLayerStack;

/*
	Since LayerStack stores layers as an array of pointers,
	you must pass a pointer to a previously allocated layer
	and not free it, as the layer will be destroyed
	when the stack is popped or destroyed.
*/

/* Initialises an empty layer stack */
LinceLayerStack* LinceCreateLayerStack();

/* Detaches and frees all layers */
void LinceDestroyLayerStack(LinceLayerStack*);

/* Adds a layer to the stack */
void LinceLayerStackPush(LinceLayerStack*, LinceLayer*);

/* Removes a layer from the top of the stack */
void LinceLayerStackPop(LinceLayerStack*, LinceLayer*);


#endif // LINCE_LAYER_H
