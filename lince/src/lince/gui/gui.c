#include "lince/gui/gui.h"
#include "lince/utils/memory.h"
#include "lince/core/logger.h"

#include "event/event.h"
#include "event/key_event.h"
#include "event/mouse_event.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <nuklear_flags.h>
#include <nuklear.h>
#include <nuklear_glfw_gl3.h>

#define MAX_VERTEX_BUFFER  (512 * 1024)
#define MAX_ELEMENT_BUFFER (128 * 1024)

/** @struct LinceUI
* @brief Holds Nuklear state allowing to draw GUI.
*/
typedef struct LinceUI {
    struct nk_context* ctx;  ///< Nuklear context
    struct nk_glfw backend;  ///< GLFW backend for Nuklear
	LinceWindow* window;     ///< Application window handle
    LinceAssetCache* asset_cache;
    hashmap_t font_cache;    ///< Cached fonts
    
    void* fonts[LinceFont_Count]; ///< Preloaded fonts
} LinceUI;


LinceUI* LinceInitUI(LinceWindow* window, LinceAssetCache* ac){

	LinceUI* ui = LinceCalloc(sizeof(LinceUI));
	ui->window = window;
    ui->asset_cache = ac;
    hashmap_init(&ui->font_cache, 10);

	ui->ctx = nk_glfw3_init(
        &ui->backend,
        ui->window->handle,
        NK_GLFW3_DEFAULT
    );

    LINCE_ASSERT(ui->ctx, "UI context failed to initialise");
    if(!ui->ctx){
        hashmap_uninit(&ui->font_cache);
        LinceFree(ui);
        return NULL;
    }

	LINCE_INFO("UI initialised");
	return ui;
}


void LinceUILoadFonts(LinceUI* ui, LinceAssetCache* am){

    static const char* font_files[] = { "fonts/DroidSans.ttf" };
    const uint32_t font_num = sizeof(font_files) / sizeof(char**);

    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&ui->backend, &atlas);
    
    for (uint32_t i = 0; i != font_num; ++i){
        const char* font_path = LinceAssetCacheFetchPath(am, font_files[i]);
        LINCE_ASSERT(font_path, "Could not find location of default font '%s'", font_files[i]);

        ui->fonts[LinceFont_Droid8]  = nk_font_atlas_add_from_file(atlas, font_path, 8, 0);
        ui->fonts[LinceFont_Droid15] = nk_font_atlas_add_from_file(atlas, font_path, 15, 0);
        ui->fonts[LinceFont_Droid20] = nk_font_atlas_add_from_file(atlas, font_path, 20, 0);
        ui->fonts[LinceFont_Droid30] = nk_font_atlas_add_from_file(atlas, font_path, 30, 0);
        ui->fonts[LinceFont_Droid50] = nk_font_atlas_add_from_file(atlas, font_path, 50, 0);
        
        LINCE_ASSERT(ui->fonts[LinceFont_Droid15], "Failed to load font at '%s'", font_path);
    }

    nk_glfw3_font_stash_end(&ui->backend);
    
    //nk_style_load_all_cursors(data->ctx, atlas->cursors);
    //nk_style_set_font(ui->ctx, ui->fonts[LinceFont_Droid20]);
}

void LinceBeginUIRender(LinceUI* ui){
	nk_glfw3_new_frame(&ui->backend);
}

void LinceEndUIRender(LinceUI* ui){
	nk_glfw3_render(&ui->backend, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
}

void LinceUIOnEvent(LinceUI* ui, LinceEvent* event){
    void* win = ui->window->handle;
    if(event->handled == LinceTrue) return;

    event->handled = (LinceBool)nk_item_is_any_active(ui->ctx);
    if(!event->handled) return; // No widget active

    // Since the nuklear API makes use of the glfw user pointer,
    // we must temporarily change it, and then set it back to the LinceWindow object.
	void* lince_window = glfwGetWindowUserPointer(win);
    glfwSetWindowUserPointer(win, &ui->backend);
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

void LinceTerminateUI(LinceUI* ui){
	if(!ui) return;
    nk_glfw3_shutdown(&ui->backend);
    hashmap_uninit(&ui->font_cache);
	LinceFree(ui);
	LINCE_INFO("Nuklear UI Terminated");
}

LinceBool LinceUILoadFont(LinceUI* ui, const char* name, const char* path, const uint32_t n, uint32_t* fontsizes){
    struct nk_font_atlas *atlas;
    struct nk_font* font;
    static char key[LINCE_NAME_MAX] = {0};
    
    const char* full_path = LinceAssetCacheFetchPath(ui->asset_cache, path);
    LINCE_ASSERT(full_path, "Could not find font '%s' from '%s'", name, full_path);
    if(!full_path) return LinceFalse;

    nk_glfw3_font_stash_begin(&ui->backend, &atlas);
    for(uint32_t i = 0; i != n; ++i){
        font = nk_font_atlas_add_from_file(atlas, full_path, fontsizes[i], NULL);
        LINCE_ASSERT(font, "Failed to load font '%s' from '%s'", name, full_path);
        if(!font) continue;
        snprintf(key, LINCE_NAME_MAX, "%s%u", name, fontsizes[i]);
        hashmap_set(&ui->font_cache, key, font);
    }
    nk_glfw3_font_stash_end(&ui->backend);
}

void* LinceUIGetFontHandle(LinceUI* ui, const char* name){
    struct nk_font* font = hashmap_get(&ui->font_cache, name);
    if(!font) return LinceFalse;
    return &font->handle;
}

void* LinceUIGetNkContext(LinceUI* ui){
    return ui->ctx;
}

LinceBool LinceUIUseFont(LinceUI* ui, const char* name){
    void* font_handle = LinceUIGetFontHandle(ui, name);
    nk_style_set_font(ui->ctx, font_handle);
    return LinceTrue;
}



