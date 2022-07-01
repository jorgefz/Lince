#ifndef LINCE_UI_LAYER_H
#define LINCE_UI_LAYER_H

#include "core/layer.h"
#include "gui/nuklear_flags.h"
#include "gui/nuklear.h"

typedef struct LinceUILayer {
    struct nk_context *ctx;
    void *glfw;
	void *glfw_window;
} LinceUILayer;


LinceUILayer* LinceInitUI(void* glfw_window);
void LinceUIBegin(LinceUILayer* ui);
void LinceUIEnd(LinceUILayer* ui);
void LinceUIOnEvent(LinceUILayer* ui, LinceEvent* event);
void LinceTerminateUI(LinceUILayer* ui);

#endif