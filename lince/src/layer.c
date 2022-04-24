
#include "layer.h"

/* Layer */

LinceLayer* LinceCreateLayer(void* data) {
	LinceLayer* layer = calloc(1, sizeof(LinceLayer));
	LINCE_ASSERT(layer, "Failed to allocate layer");
	layer->data = data;
	return layer;
}

void* LinceGetLayerData(LinceLayer* layer) {
	return layer->data;
}

/* LayerStack */

LinceLayerStack* LinceCreateLayerStack() {
	LinceLayerStack* stack;
	stack = calloc(1, sizeof(LinceLayerStack));
	return stack;
}

void LinceDestroyLayerStack(LinceLayerStack* stack) {
	if (!stack) return;
	if (!stack->layers) {
		free(stack);
		return;
	}
	unsigned int i;
	for (i = 0; i != stack->count; ++i) {
		if (stack->layers[i] && stack->layers[i]->OnDetach) {
			if (stack->layers[i]->OnDetach) {
				stack->layers[i]->OnDetach(stack->layers[i]);
			}
			free( stack->layers[i] );
		}
	}
	free(stack->layers);
	free(stack);
}

void LinceLayerStackPush(LinceLayerStack* stack, LinceLayer* layer) {
	if (!stack || !layer) return;
	
	// resize stack
	LinceLayer** ret = realloc(stack->layers, ((size_t)stack->count+1) * sizeof(LinceLayer*));
	if (!ret) {
		// Failed to allocate more space for additional pointer
		LinceDestroyLayerStack(stack);
		LINCE_ASSERT(0, "Failed to allocate memory");
	}
	stack->count++;
	stack->layers = ret;
	
	// attach layer
	stack->layers[stack->count - 1] = layer;
	if (layer->OnAttach) layer->OnAttach(layer);
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
	if (layer->OnDetach) layer->OnDetach(layer);

	// resize layer stack
	if (stack->count == 1) {
		free(stack->layers);
		stack->layers = NULL;
		stack->count = 0;
		return;
	}

	for (i = which; i != stack->count - 1; ++i) {
		stack->layers[i] = stack->layers[i + 1];
	}

	LinceLayer** ret = realloc(stack->layers, (stack->count - 1) * sizeof(LinceLayer*));
	if (!ret) {
		LinceDestroyLayerStack(stack);
		LINCE_ASSERT(0, "Failed to allocate memory");
	}
	stack->layers = ret;
	stack->count--;
}



