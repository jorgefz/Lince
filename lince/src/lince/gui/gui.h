#ifndef LINCE_UI_H
#define LINCE_UI_H

#include "lince/core/window.h"
#include "lince/app/asset_cache.h"
#include "lince/containers/str.h"
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

/** @brief Initialise UI state and Nuklear rendering context
* @param window Application window handle.
* @param ac Asset cache to resolve font paths
* @todo Change input handle to LinceWindow.
*/
LinceUI* LinceInitUI(LinceWindow* window, LinceAssetCache* ac);

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

/** @brief Loads a TTF font file with a pre-defined set of fontsizes
 * @param ui UI state
 * @param name Unique name for the font
 * @param path Path to TTF file
 * @param n Number of fontsizes to generate
 * @param fontsizes Array of fontsizes to generate
 * @returns LinceTrue if successful, and LinceFalse otherwise.
*/
LinceBool LinceUILoadFont(LinceUI* ui, string_t name, string_t path, const uint32_t n, uint32_t* fontsizes);

/** @brief Sets the current font.
 * @param ui UI state
 * @param name Unique name of the font followed by its fontsize, e.g. "droid15".
 * @param len Length of the unique font name
 * @returns LinceTrue if successful, and LinceFalse otherwise.
*/
LinceBool LinceUIUseFont(LinceUI* ui, string_t name);

/** @brief Retrieve the Nuklear handle for a font
 * @param ui UI state
 * @param name Unique name of the font followed by its fontsize, e.g. "droid15".
 * @param len Length of the unique font name
 * @returns the font handle
*/
void* LinceUIGetFontHandle(LinceUI* ui, string_t name);

/** @brief Retrieve the Nuklear context
 * @param ui UI state
 * @returns Nuklear context
*/
void* LinceUIGetNkContext(LinceUI* ui);


#endif /* LINCE_UI_H */