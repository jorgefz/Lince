#ifndef LINCE_LAYER_H
#define LINCE_LAYER_H

#include "event/event.h"


typedef union LinceLayerData {
	// other predefined layers here
	void* GenericLayer;
} LinceLayerData;

typedef struct LinceLayer {
	// data
	LinceLayerData data;
	// methods
	void (*OnAttach)(struct LinceLayer*); // called when layer is pushed on stack
	void (*OnDetach)(struct LinceLayer*); // called when layer is popped from stack (must be freed)
	void (*OnUpdate)(struct LinceLayer*); // called on each game tick
	void (*OnEvent)(struct LinceLayer*, Event*); // called on each game event
} LinceLayer;

typedef struct LinceLayerStack {
	LinceLayer** layers;
	unsigned int count, insert_index;
} LinceLayerStack;

/*
	Since LayerStack stores layers as an array of pointers,
	you must pass a pointer to a previously allocated layer
	and not free it, as the layer will be destroyed
	when the stack is popped or destroyed.
*/

LinceLayerStack* LinceLayerStack_Create();
void LinceLayerStack_Destroy(LinceLayerStack*);

void LinceLayerStack_Push(LinceLayerStack*, LinceLayer*);
void LinceLayerStack_Pop(LinceLayerStack*, LinceLayer*);


#endif // LINCE_LAYER_H
