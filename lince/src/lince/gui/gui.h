#ifndef LINCE_UI_LAYER_H
#define LINCE_UI_LAYER_H

#include "lince/core/window.h"
#include "lince/app/asset_cache.h"
#include "lince/containers/hashmap.h"

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

/** @struct LinceUI
* @brief Private struct that holds Nuklear state allowing to draw GUI.
* @note It is only declared in the header to avoid propagating Nuklear headers
*/
typedef struct LinceUI LinceUI;

LinceBool LinceUILoadFont(LinceUI* ui, const char* name, const char* path, const uint32_t n, uint32_t* fontsizes);
void* LinceUIGetFontHandle(LinceUI* ui, const char* name);
void* LinceUIGetNkContext(LinceUI* ui);
LinceBool LinceUIUseFont(LinceUI* ui, const char* name);

/** @brief Initialise UI state and Nuklear rendering context
* @param window Application window handle.
* @param ac Asset cache to resolve font paths
* @todo Change input handle to LinceWindow.
*/
LinceUI* LinceInitUI(LinceWindow* window, LinceAssetCache* ac);


/** @brief Loads the predefined fonts using an initialised asset manager.
* @param ui initialised ui state
* @param am initialised asset manager to resolve paths to the fonts
*/
void LinceUILoadFonts(LinceUI* ui, LinceAssetCache* am);

/** @brief Initialise Nuklear's render queue.
* Any GUI draw calls must be bracketed between LinceBeginUIRender and LinceEndUIRender.
*/
void LinceBeginUIRender(LinceUI* ui);

/** @brief Close and submit Nuklear's render queue.
* Any GUI draw calls must be bracketed between LinceBeginUIRender and LinceEndUIRender.
*/
void LinceEndUIRender(LinceUI* ui);

/** @brief Passes key and mouse input events to Nuklear */
void LinceUIOnEvent(LinceUI* ui, LinceEvent* event);

/** @brief Destroy Nuklear rendering context */
void LinceTerminateUI(LinceUI* ui);


#endif