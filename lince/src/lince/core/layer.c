
#include "core/layer.h"
#include "core/memory.h"

/* Layer */

LinceLayer* LinceCreateLayer(void* data) {
	LinceLayer* layer = LinceCalloc(sizeof(LinceLayer));
	LINCE_ASSERT(layer, "Failed to allocate layer");
	layer->data = data;
	return layer;
}

void LinceDeleteLayer(LinceLayer* layer){
	if(!layer) return;
	LinceFree(layer);
}

LinceLayerStack* LinceCreateLayerStack() {
	LinceLayerStack* stack;
	stack = LinceCalloc(sizeof(LinceLayerStack));
	return stack;
}

void LinceDestroyLayerStack(LinceLayerStack* stack) {
	if (!stack) return;
	if (!stack->layers) {
		LinceFree(stack);
		return;
	}
	unsigned int i;
	for (i = 0; i != stack->count; ++i) {
		if (stack->layers[i] && stack->layers[i]->on_detach) {
			if (stack->layers[i]->on_detach) {
				stack->layers[i]->on_detach(stack->layers[i]);
			}
			LinceFree( stack->layers[i] );
		}
	}
	LinceFree(stack->layers);
	LinceFree(stack);
}

void LinceLayerStackPush(LinceLayerStack* stack, LinceLayer* layer) {
	if (!stack || !layer) return;
	
	// resize stack
	LinceLayer** ret = LinceRealloc(stack->layers, ((size_t)stack->count+1) * sizeof(LinceLayer*));
	if (!ret) {
		// Failed to allocate more space for additional pointer
		LinceDestroyLayerStack(stack);
		LINCE_ASSERT(0, "Failed to allocate memory");
	}
	stack->count++;
	stack->layers = ret;
	
	// attach layer
	stack->layers[stack->count - 1] = layer;
	if (layer->on_attach) layer->on_attach(layer);
}

void LinceLayerStackPop(LinceLayerStack* stack, LinceLayer* layer) {
	if (!stack || !layer) return;
	
	// find input layer
	unsigned int i;
	int which = -1;
	for (i = 0; i != stack->count; ++i) {
		if (layer == stack->layers[i]) {
			which = (int)i;
			break;
		}
	}
	if (stack->count == 0 || which < 0) return;

	// detach layer
	if (layer->on_detach) layer->on_detach(layer);

	// resize layer stack
	if (stack->count == 1) {
		LinceFree(stack->layers);
		stack->layers = NULL;
		stack->count = 0;
		return;
	}

	for (i = which; i != stack->count - 1; ++i) {
		stack->layers[i] = stack->layers[i + 1];
	}

	LinceLayer** ret = LinceRealloc(stack->layers, (stack->count - 1) * sizeof(LinceLayer*));
	if (!ret) {
		LinceDestroyLayerStack(stack);
		LINCE_ASSERT(0, "Failed to allocate memory");
	}
	stack->layers = ret;
	stack->count--;
}



