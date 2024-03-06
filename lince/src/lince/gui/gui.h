#ifndef LINCE_UI_LAYER_H
#define LINCE_UI_LAYER_H

#include "lince/app/layer.h"
#include "lince/app/asset_cache.h"

/** @enum LinceFonts
* @brief Indices of preloaded fonts by the engine,
* with their sizes in pt.
*/
typedef enum LinceFonts {
    LinceFont_Droid8 = 0,
    LinceFont_Droid15,
    LinceFont_Droid20,
    LinceFont_Droid30,
    LinceFont_Droid50,
    LinceFont_Count
} LinceFonts;

/** @struct LinceUILayer
* @brief Holds Nuklear state allowing to draw GUI.
* @note Nuklear pointers have been type erased to avoid propagating nuklear headers
*/
typedef struct LinceUILayer {
    void* ctx;              ///< Nuklear context
    void* glfw;             ///< GLFW backend for Nuklear
	void* glfw_window;      ///< GLFW window handle for Nuklear
    void* fonts[LinceFont_Count]; ///< Preloaded fonts
} LinceUILayer;

/** @brief Initialise UI state and Nuklear rendering context
* @param glfw_window GLFW window handle from LinceWindow.
* @todo Change input handle to LinceWindow.
*/
LinceUILayer* LinceInitUI(void* glfw_window);


// void LinceUILoadFont(LinceUI* ui, const char* name, const char* file);

// void* LinceUIGetFont(LinceUI* ui, const char* name, uint32_t fontsize);


/** @brief Loads the predefined fonts using an initialised asset manager.
* @param ui initialised ui state
* @param am initialised asset manager to resolve paths to the fonts
*/
void LinceUILoadFonts(LinceUILayer* ui, LinceAssetCache* am);

/** @brief Initialise Nuklear's render queue.
* Any GUI draw calls must be bracketed between LinceBeginUIRender and LinceEndUIRender.
*/
void LinceBeginUIRender(LinceUILayer* ui);

/** @brief Close and submit Nuklear's render queue.
* Any GUI draw calls must be bracketed between LinceBeginUIRender and LinceEndUIRender.
*/
void LinceEndUIRender(LinceUILayer* ui);

/** @brief Passes key and mouse input events to Nuklear */
void LinceUIOnEvent(LinceUILayer* ui, LinceEvent* event);

/** @brief Destroy Nuklear rendering context */
void LinceTerminateUI(LinceUILayer* ui);


#endif