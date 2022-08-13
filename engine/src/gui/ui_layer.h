#ifndef LINCE_UI_LAYER_H
#define LINCE_UI_LAYER_H

#include "core/layer.h"
#include "gui/nuklear_flags.h"
#include "gui/nuklear.h"

typedef enum LinceFonts {
    LinceFont_Droid15 = 0,
    LinceFont_Droid20,
    LinceFont_Droid30,
    LinceFont_Droid50,
    LinceFont_Count
} LinceFonts;

typedef struct LinceUILayer {
    struct nk_context *ctx;
    void *glfw;
	void *glfw_window;
    struct nk_font* fonts[LinceFont_Count];
} LinceUILayer;

/* Initialise UI state and Nuklear rendering context */
LinceUILayer* LinceInitUI(void* glfw_window);

/* Initialise Nuklear's render queue */
void LinceBeginUIRender(LinceUILayer* ui);

/* Close and submit render queue */
void LinceEndUIRender(LinceUILayer* ui);

/* Passes key and scroll events to Nuklear */
void LinceUIOnEvent(LinceUILayer* ui, LinceEvent* event);

/* Destroy Nuklear rendering context */
void LinceTerminateUI(LinceUILayer* ui);

/* Draws text to the screen */
void LinceUIText(
    LinceUILayer* ui,    /* UI state */
    const char* name,     /* Unique name for the Nuklear object */
    float x, float y,    /* position of the text in pixels */
    LinceFonts font,     /* font and fontsize of the text */
    size_t max_size,     /* max characters to render */
    const char* text,    /* string to render */
    ...                  /* varargs */
);

#endif