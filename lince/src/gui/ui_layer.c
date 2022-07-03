#include "gui/ui_layer.h"

#include "event/event.h"
#include "event/key_event.h"
#include "event/mouse_event.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "gui/nuklear_flags.h"
#include "gui/nuklear.h"
#include "gui/nuklear_glfw_gl3.h"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024


LinceUILayer* LinceInitUI(void* glfw_window){

	LinceUILayer* ui = calloc(1, sizeof(LinceUILayer));
	LINCE_ASSERT_ALLOC(ui, sizeof(LinceUILayer));

	ui->glfw_window = glfw_window;
	ui->glfw = calloc(1, sizeof(struct nk_glfw));
	LINCE_ASSERT_ALLOC(ui->glfw, sizeof(struct nk_glfw));

	ui->ctx = nk_glfw3_init(
        ui->glfw,
        ui->glfw_window,
        NK_GLFW3_DEFAULT
    );

	// Initialise Font
    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(ui->glfw, &atlas);

    ui->fonts[LinceFont_Droid15] = nk_font_atlas_add_from_file(atlas, "lince/assets/fonts/DroidSans.ttf", 15, 0);
    ui->fonts[LinceFont_Droid20] = nk_font_atlas_add_from_file(atlas, "lince/assets/fonts/DroidSans.ttf", 20, 0);
    ui->fonts[LinceFont_Droid30] = nk_font_atlas_add_from_file(atlas, "lince/assets/fonts/DroidSans.ttf", 30, 0);
    ui->fonts[LinceFont_Droid50] = nk_font_atlas_add_from_file(atlas, "lince/assets/fonts/DroidSans.ttf", 50, 0);
    
    nk_glfw3_font_stash_end(ui->glfw);
    
    //nk_style_load_all_cursors(data->ctx, atlas->cursors);
    //nk_style_set_font(ui->ctx, &droid_20->handle);

	LINCE_INFO(" UI Initialised");
	return ui;
}

void LinceBeginUIRender(LinceUILayer* ui){
	nk_glfw3_new_frame(ui->glfw);
}

void LinceEndUIRender(LinceUILayer* ui){
	nk_glfw3_render(ui->glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
}

void LinceUIOnEvent(LinceUILayer* ui, LinceEvent* event){
    void* win = ui->glfw_window;

    // Since the nuklear API makes use of the glfw user pointer,
    // we must temporarily change it, and then set it back to the LinceWindow object.
	void* lince_window = glfwGetWindowUserPointer(win);
    glfwSetWindowUserPointer(win, ui->glfw);
    switch (event->type) {
    case LinceEventType_KeyType:
        nk_glfw3_char_callback(win, event->data.KeyType->keycode);
        break;
    case LinceEventType_MouseScrolled:
        nk_gflw3_scroll_callback(win, event->data.MouseScrolled->xoff, event->data.MouseScrolled->yoff);
        break;
    case LinceEventType_MouseButtonPressed:
        nk_glfw3_mouse_button_callback(win, event->data.MouseButtonPressed->button, GLFW_PRESS, 0);
        break;
    case LinceEventType_MouseButtonReleased:
        nk_glfw3_mouse_button_callback(win, event->data.MouseButtonPressed->button, GLFW_RELEASE, 0);
        break;
    default:
        break;
    }
    glfwSetWindowUserPointer(win, lince_window);
}

void LinceTerminateUI(LinceUILayer* ui){
	if(!ui) return;
    nk_glfw3_shutdown(ui->glfw);
    free(ui->glfw);
	free(ui);
	LINCE_INFO(" UI Terminated");
}

void LinceUIText(
    LinceUILayer* ui,
    const char* window_name,
    float x, float y,
    LinceFonts font,
    size_t max_size,
    const char* text, ...
    ) {
    
    struct nk_context* ctx = ui->ctx;
    struct nk_style_item style_state = ctx->style.window.fixed_background;

    // SLOW - refactor this in the future
    char* formatted_text = malloc((max_size+1) * sizeof(char));
    memset(formatted_text, ' ', max_size-1);
    formatted_text[max_size-1] = (char)0;

    // format given text
    va_list args;
    va_start(args, text);
    size_t len = vsnprintf(formatted_text, max_size, text, args);
    va_end(args);

    // hide background
    ctx->style.window.fixed_background = nk_style_item_hide();
    
    nk_style_set_font(ui->ctx, &ui->fonts[font]->handle);
    if(nk_begin(ctx, window_name, nk_rect(x, y, 20*(float)len, 50), NK_WINDOW_NO_INPUT)){
        nk_layout_row_static(ctx, 30, 15*(float)len, 1);
        nk_text(ctx, formatted_text, len, NK_TEXT_CENTERED); 
    }
    nk_end(ctx);

    free(formatted_text);

    // restore previous style
    ctx->style.window.fixed_background = style_state;
}

