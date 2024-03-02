#include "gui/gui.h"
#include "core/memory.h"

#include "event/event.h"
#include "event/key_event.h"
#include "event/mouse_event.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "nuklear_flags.h"
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

#define MAX_VERTEX_BUFFER  (512 * 1024)
#define MAX_ELEMENT_BUFFER (128 * 1024)


LinceUILayer* LinceInitUI(void* glfw_window){

	LinceUILayer* ui = LinceCalloc(sizeof(LinceUILayer));
	ui->glfw_window = glfw_window;
	ui->glfw = LinceCalloc(sizeof(struct nk_glfw));

	ui->ctx = nk_glfw3_init(
        ui->glfw,
        ui->glfw_window,
        NK_GLFW3_DEFAULT
    );
    LINCE_ASSERT(ui->ctx, "Nuklear context failed to initialise");

	LINCE_INFO("Nuklear UI Initialised");
	return ui;
}


void LinceUILoadFonts(LinceUILayer* ui, LinceAssetManager* am){

    static const char* font_files[] = { "fonts/DroidSans.ttf" };
    const uint32_t font_num = sizeof(font_files) / sizeof(char**);

    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(ui->glfw, &atlas);
    
    for (uint32_t i = 0; i != font_num; ++i){
        const char* font_path = LinceFetchAssetPath(am, font_files[i]);
        LINCE_ASSERT(font_path, "Could not find location of default font '%s'", font_files[i]);

        ui->fonts[LinceFont_Droid8]  = nk_font_atlas_add_from_file(atlas, font_path, 8, 0);
        ui->fonts[LinceFont_Droid15] = nk_font_atlas_add_from_file(atlas, font_path, 15, 0);
        ui->fonts[LinceFont_Droid20] = nk_font_atlas_add_from_file(atlas, font_path, 20, 0);
        ui->fonts[LinceFont_Droid30] = nk_font_atlas_add_from_file(atlas, font_path, 30, 0);
        ui->fonts[LinceFont_Droid50] = nk_font_atlas_add_from_file(atlas, font_path, 50, 0);
        
        LINCE_ASSERT(ui->fonts[LinceFont_Droid15], "Failed to load font at '%s'", font_path);
    }

    nk_glfw3_font_stash_end(ui->glfw);
    
    //nk_style_load_all_cursors(data->ctx, atlas->cursors);
    //nk_style_set_font(ui->ctx, ui->fonts[LinceFont_Droid20]);
}

void LinceBeginUIRender(LinceUILayer* ui){
	nk_glfw3_new_frame(ui->glfw);
}

void LinceEndUIRender(LinceUILayer* ui){
	nk_glfw3_render(ui->glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
}

void LinceUIOnEvent(LinceUILayer* ui, LinceEvent* event){
    void* win = ui->glfw_window;
    if(event->handled == LinceTrue) return;

    event->handled = (LinceBool)nk_item_is_any_active(ui->ctx);
    if(!event->handled) return; // No widget active

    // Since the nuklear API makes use of the glfw user pointer,
    // we must temporarily change it, and then set it back to the LinceWindow object.
	void* lince_window = glfwGetWindowUserPointer(win);
    glfwSetWindowUserPointer(win, ui->glfw);
    switch (event->type) {
    case LinceEventType_KeyType:
        nk_glfw3_char_callback(win, event->data.key_type->keycode);
        break;
    case LinceEventType_MouseScroll:
        nk_gflw3_scroll_callback(win, event->data.mouse_scroll->xoff, event->data.mouse_scroll->yoff);
        break;
    case LinceEventType_MousePress:
        nk_glfw3_mouse_button_callback(win, event->data.mouse_press->button, GLFW_PRESS, 0);
        break;
    case LinceEventType_MouseRelease:
        nk_glfw3_mouse_button_callback(win, event->data.mouse_press->button, GLFW_RELEASE, 0);
        break;
    default:
        break;
    }
    glfwSetWindowUserPointer(win, lince_window);
}

void LinceTerminateUI(LinceUILayer* ui){
	if(!ui) return;
    nk_glfw3_shutdown(ui->glfw);
    LinceFree(ui->glfw);
	LinceFree(ui);
	LINCE_INFO("Nuklear UI Terminated");
}


